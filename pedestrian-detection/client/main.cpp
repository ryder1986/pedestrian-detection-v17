#include "mainwindow.h"
#include <QApplication>
#include "tool1.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    prt(info,"client start");
    MainWindow w;
    w.show();
    return a.exec();
}
