﻿#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow2d w;
    w.setTitle("OpenGL Hello World!");
    w.resize(640, 480);
    w.show();
    return a.exec();
}
