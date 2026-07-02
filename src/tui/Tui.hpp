#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

namespace tui {

class Tui {

  static void startLoop(ftxui::Component &renderer) {
    auto screen = ftxui::ScreenInteractive::Fullscreen();

    screen.Loop(renderer);
  }
};

} // namespace tui
