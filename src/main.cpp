#include <QApplication>
#include <QPushButton>
#include <spdlog/spdlog.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    spdlog::info("gomoku started");

    QPushButton button("Click me");
    button.resize(200, 60);
    button.show();

    QObject::connect(&button, &QPushButton::clicked, &app, &QApplication::quit);

    return app.exec();
}
