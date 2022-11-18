#include "MainWindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    // get size of screen

    int height = a.screens()[0]->size().height();
    int scale = std::max(height / 800, 1);
    w.setFixedSize(1200 * scale, 800 * scale);
    w.loadContent();
    w.show();

    return a.exec();
}
