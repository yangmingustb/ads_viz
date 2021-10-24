#include "utils.h"
#include "viewer.h"
#include <qt5/QtWidgets/QApplication>
#include <qt5/QtWidgets/QMainWindow>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMainWindow w;
    w.resize(640, 480);
    w.show();
    return a.exec();
}
