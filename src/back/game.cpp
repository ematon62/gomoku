#include "game.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>

namespace gomoku {

namespace {

constexpr std::array<std::pair<int, int>, 4> kAxes = {{{1, 0}, {0, 1}, {1, 1}, {1, -1}}};
constexpr std::array<std::pair<int, int>, 8> kDirections = {
    {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}}};

// Length of the "four" a free-three would create, and the radius of the line
// window needed to see both of that four's unobstructed extremities.
constexpr int kFourLength = 4;
constexpr int kRadius = kFourLength + 1;
constexpr int kWindowSize = (2 * kRadius) + 1;

// Classification of a cell relative to the move being tested for `player`,
// used only for free-three detection.
enum class LineCell : std::uint8_t { kPlayer, kEmpty, kBlocked };

} // namespace

Game::Game(spdlog::logger &logger) : logger_(logger) {}

int Game::index(int col, int row) {
  return (row * kSize) + col;
}

bool Game::in_bounds(int col, int row) {
  return col >= 0 && col < kSize && row >= 0 && row < kSize;
}

Player Game::opponent(Player player) {
  return player == Player::kBlack ? Player::kWhite : Player::kBlack;
}

int Game::player_index(Player player) {
  return player == Player::kBlack ? 0 : 1;
}

void Game::set_cell(int col, int row, Player player) {
  stones_.at(player_index(player)).set(index(col, row));
}

void Game::clear_cell(int col, int row) {
  stones_.at(0).reset(index(col, row));
  stones_.at(1).reset(index(col, row));
}

Player Game::get_grid(int col, int row) const {
  const int cell = index(col, row);
  if (stones_.at(0).test(cell)) {
    return Player::kBlack;
  }
  if (stones_.at(1).test(cell)) {
    return Player::kWhite;
  }
  return Player::kNone;
}

std::array<std::array<Player, Game::kSize>, Game::kSize> Game::get_grid() const {
  std::array<std::array<Player, kSize>, kSize> grid{};
  for (int row = 0; row < kSize; ++row) {
    for (int col = 0; col < kSize; ++col) {
      grid.at(row).at(col) = get_grid(col, row);
    }
  }
  return grid;
}

void Game::print_grid() const {
  std::ostringstream out;
  out << "\n    ";
  for (int col = 0; col < kSize; ++col) {
    out << std::setw(3) << col;
  }
  out << '\n';
  for (int row = 0; row < kSize; ++row) {
    out << std::setw(3) << row << ' ';
    for (int col = 0; col < kSize; ++col) {
      char symbol = '.';
      switch (get_grid(col, row)) {
      case Player::kBlack:
        symbol = 'X';
        break;
      case Player::kWhite:
        symbol = 'O';
        break;
      case Player::kNone:
        break;
      }
      out << std::setw(3) << symbol;
    }
    out << '\n';
  }
  logger_.get().info(out.str());
}

int Game::get_capture_count(Player player) const {
  return capture_count_.at(player_index(player));
}

std::vector<std::pair<int, int>> Game::find_captures(int col, int row, Player player) const {
  std::vector<std::pair<int, int>> captures;
  const Player opp = opponent(player);
  for (const auto &[dcol, drow] : kDirections) {
    const int near_col = col + dcol;
    const int near_row = row + drow;
    const int far_col = col + (2 * dcol);
    const int far_row = row + (2 * drow);
    const int anchor_col = col + (3 * dcol);
    const int anchor_row = row + (3 * drow);
    if (!in_bounds(anchor_col, anchor_row)) {
      continue;
    }
    if (get_grid(near_col, near_row) == opp && get_grid(far_col, far_row) == opp &&
        get_grid(anchor_col, anchor_row) == player) {
      captures.emplace_back(near_col, near_row);
      captures.emplace_back(far_col, far_row);
    }
  }
  return captures;
}

bool Game::add_stone(int col, int row, Player player) {
  if (!in_bounds(col, row)) {
    logger_.get().warn("Rejected move ({}, {}): out of bounds", col, row);
    return false;
  }
  if (get_grid(col, row) != Player::kNone) {
    logger_.get().warn("Rejected move ({}, {}): cell occupied", col, row);
    return false;
  }

  const auto captures = find_captures(col, row, player);
  if (captures.empty() && check_double_three(col, row, player)) {
    logger_.get().warn("Rejected move ({}, {}): double-three", col, row);
    return false;
  }

  set_cell(col, row, player);
  for (const auto &[captured_col, captured_row] : captures) {
    clear_cell(captured_col, captured_row);
  }
  capture_count_.at(player_index(player)) += static_cast<int>(captures.size());

  logger_.get().info("Placed stone at ({}, {}), captured {} stone(s)", col, row, captures.size());
  return true;
}

bool Game::rm_stone(int col, int row) {
  if (!in_bounds(col, row) || get_grid(col, row) == Player::kNone) {
    return false;
  }
  clear_cell(col, row);
  logger_.get().info("Removed stone at ({}, {})", col, row);
  return true;
}

bool Game::is_free_three(int col, int row, Player player, int dcol, int drow) const {
  std::array<LineCell, kWindowSize> line{};
  for (int i = -kRadius; i <= kRadius; ++i) {
    const int cell_col = col + (i * dcol);
    const int cell_row = row + (i * drow);
    if (i == 0) {
      line.at(i + kRadius) = LineCell::kPlayer;
    } else if (!in_bounds(cell_col, cell_row)) {
      line.at(i + kRadius) = LineCell::kBlocked;
    } else {
      const Player occ = get_grid(cell_col, cell_row);
      if (occ == player) {
        line.at(i + kRadius) = LineCell::kPlayer;
      } else if (occ == Player::kNone) {
        line.at(i + kRadius) = LineCell::kEmpty;
      } else {
        line.at(i + kRadius) = LineCell::kBlocked;
      }
    }
  }

  // Slide a window covering the four cells that would become an open four
  // plus their two flanking cells; offset in [-kFourLength, -1] guarantees
  // the tested move (relative position 0) always falls among the middle four.
  for (int offset = -kFourLength; offset <= -1; ++offset) {
    const int base = offset + kRadius;
    if (line.at(base) != LineCell::kEmpty || line.at(base + kRadius) != LineCell::kEmpty) {
      continue;
    }
    int player_count = 0;
    int empty_count = 0;
    for (int k = 1; k <= kFourLength; ++k) {
      if (line.at(base + k) == LineCell::kPlayer) {
        ++player_count;
      } else if (line.at(base + k) == LineCell::kEmpty) {
        ++empty_count;
      }
    }
    if (player_count == kFourLength - 1 && empty_count == 1) {
      return true;
    }
  }
  return false;
}

bool Game::check_double_three(int col, int row, Player player) const {
  int free_three_axes = 0;
  for (const auto &[dcol, drow] : kAxes) {
    if (is_free_three(col, row, player, dcol, drow)) {
      ++free_three_axes;
    }
  }
  return free_three_axes >= 2;
}

std::vector<std::vector<std::pair<int, int>>> Game::find_alignments(Player player) const {
  std::vector<std::vector<std::pair<int, int>>> alignments;
  for (int row = 0; row < kSize; ++row) {
    for (int col = 0; col < kSize; ++col) {
      if (get_grid(col, row) != player) {
        continue;
      }
      for (const auto &[dcol, drow] : kAxes) {
        const int prev_col = col - dcol;
        const int prev_row = row - drow;
        if (in_bounds(prev_col, prev_row) && get_grid(prev_col, prev_row) == player) {
          continue; // Not the start of a run.
        }
        std::vector<std::pair<int, int>> run;
        int cur_col = col;
        int cur_row = row;
        while (in_bounds(cur_col, cur_row) && get_grid(cur_col, cur_row) == player) {
          run.emplace_back(cur_col, cur_row);
          cur_col += dcol;
          cur_row += drow;
        }
        if (run.size() >= kWinLength) {
          alignments.push_back(std::move(run));
        }
      }
    }
  }
  return alignments;
}

bool Game::is_line_breakable(const std::vector<std::pair<int, int>> &alignment,
                             Player owner) const {
  const Player opp = opponent(owner);
  for (int row = 0; row < kSize; ++row) {
    for (int col = 0; col < kSize; ++col) {
      if (get_grid(col, row) != Player::kNone) {
        continue;
      }
      const auto captures = find_captures(col, row, opp);
      for (const auto &captured : captures) {
        for (const auto &stone : alignment) {
          if (captured == stone) {
            return true;
          }
        }
      }
    }
  }
  return false;
}

bool Game::check_win(Player player) const {
  if (capture_count_.at(player_index(player)) >= kCapturesToWin) {
    return true;
  }
  const auto alignments = find_alignments(player);
  return std::ranges::any_of(alignments, [this, player](const auto &alignment) {
    return !is_line_breakable(alignment, player);
  });
}

} // namespace gomoku
