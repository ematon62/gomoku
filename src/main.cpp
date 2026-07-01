#include "gomoku.hpp"

int main() {
    auto logger = get_logger();
    logger.info("Logger spawned.");

    auto screen = ftxui::ScreenInteractive::Fullscreen();
    ftxui::ButtonOption opt;
    opt.transform = [](const ftxui::EntryState &s) {
        auto label = ftxui::text(s.label) | ftxui::center;
        if (s.focused)
            label = label | ftxui::bold;
        return label | ftxui::borderRounded | ftxui::bgcolor(ftxui::Color::Default);
    };
    auto button = ftxui::Button("Quitter", screen.ExitLoopClosure(), opt);

    auto renderer = ftxui::Renderer(button, [&] {
        return ftxui::hbox({
                   ftxui::filler(),
                   ftxui::vbox({
                       ftxui::filler(),
                       button->Render(),
                       ftxui::filler(),
                   }),
                   ftxui::filler(),
               }) |
               ftxui::bgcolor(ftxui::Color::Default);
    });

    screen.Loop(renderer);

    return 0;
}
