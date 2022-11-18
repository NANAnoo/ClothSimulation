#include "MainWindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    render = new RenderWidget(this);
}

MainWindow::~MainWindow()
{

}


void MainWindow::loadContent()
{
    // initialize
    render->setGeometry(10, 10, width() / 4 * 3 - 20, height() - 20);
}
