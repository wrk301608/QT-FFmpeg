#include "tray.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Tray w;
    w.setWindowTitle("MinTray");
    w.show();
    return a.exec();
}
