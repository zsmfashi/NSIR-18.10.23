#include "mainwindow.h"
#include "settingwindow.h"
#include "network.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //SettingWindow test;
    //test.show();

   // MainWindow w;
   // w.show();

    Network n;
    n.test();

    return a.exec();
}
