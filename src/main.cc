﻿#include <viz.h>
#include <iostream>
#include <qt5/QtWidgets/QApplication>
#include <qt5/QtWidgets/QMainWindow>
#include <map.h>

int main(int argc, char **argv)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    QApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
#else
    qputenv("QT_DEVICE_PIXEL_RATIO", QByteArray("1"));
#endif

    QApplication qapp(argc, argv);
    QMainWindow window;
    window.setMinimumSize(900, 600);
    cat::QtOSGWidget *widget = new cat::QtOSGWidget(&window);

    cat::HDMap hd_map;
    hd_map.init();

    widget->loadHDMap(&hd_map);
    widget->drawLanelet();

    window.setCentralWidget(widget);
    window.show();

    return qapp.exec();
}