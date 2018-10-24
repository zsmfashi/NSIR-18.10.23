#include "mainwindow.h"
#include "settingwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SettingWindow test;
    test.show();

   // MainWindow w;
   // w.show();



    return a.exec();
}
