#pragma once

#include <array>
#include <bitset>
#include <cstdint>
#include <functional>
#include <utility>
#include <vector>

#include <spdlog/logger.h>

namespace gomoku {

enum class Player : std::uint8_t {
  kNone = 0,
  kBlack = 1,
  kWhite = 2,
};

// 19x19 Gomoku board with Ninuki-renju style captures and the no-double-three
// restriction, stored as one bitboard per player.
class Game {
public:
  static constexpr int kSize = 19;
  static constexpr int kCells = kSize * kSize;
  // "capture ten of your opponent's stones" -> 10 stones, i.e. 5 pairs.
  static constexpr int kCapturesToWin = 10;
  static constexpr int kWinLength = 5;

  explicit Game(spdlog::logger &logger);

  // Places a stone for `player` at (col, row). Rejects out-of-bounds and
  // occupied cells, and forbidden double-three moves unless the move also
  // captures a pair. Applies any resulting captures. Returns false if the
  // move is illegal.
  bool add_stone(int col, int row, Player player);

  // Removes whatever stone sits at (col, row). Returns false if the cell
  // was already empty.
  bool rm_stone(int col, int row);

  [[nodiscard]] Player get_grid(int col, int row) const;
  [[nodiscard]] std::array<std::array<Player, kSize>, kSize> get_grid() const;

  void print_grid() const;

  // True once `player` has captured 10 opponent stones, or holds an
  // alignment of 5+ stones that the opponent cannot break by capturing a pair.
  [[nodiscard]] bool check_win(Player player) const;

  // True if placing a stone for `player` at (col, row) would introduce two or
  // more simultaneous free-three alignments. Pure query, does not mutate
  // the board.
  [[nodiscard]] bool check_double_three(int col, int row, Player player) const;

  [[nodiscard]] int get_capture_count(Player player) const;

private:
  static int index(int col, int row);
  static bool in_bounds(int col, int row);
  static Player opponent(Player player);
  static int player_index(Player player);

  void set_cell(int col, int row, Player player);
  void clear_cell(int col, int row);

  // Opponent pairs that would be captured if `player` played at (col, row).
  // Pure query: (col, row) does not need to already hold a stone.
  [[nodiscard]] std::vector<std::pair<int, int>> find_captures(int col, int row,
                                                               Player player) const;

  [[nodiscard]] bool is_free_three(int col, int row, Player player, int dcol, int drow) const;

  [[nodiscard]] std::vector<std::vector<std::pair<int, int>>> find_alignments(Player player) const;
  [[nodiscard]] bool is_line_breakable(const std::vector<std::pair<int, int>> &alignment,
                                       Player owner) const;

  std::array<std::bitset<kCells>, 2> stones_;
  std::array<int, 2> capture_count_ = {0, 0};
  std::reference_wrapper<spdlog::logger> logger_;
};

} // namespace gomoku
