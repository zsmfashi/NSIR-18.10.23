#include "mainwindow.h"
#include "settingwindow.h"
#include "network.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //SettingWindow test;
    //test.show();

   // MainW4indow w;
   // w.show();

    Network n;
    n.init("210.30.97.233","5000");
    //qDebug() << n.request_decet("data/dog.jpg","123");
    //n.send_file(":/../../../horses.jpg","data/","name.jpg");
    n.download_file("123","aaa.jpg");
    return a.exec();
}
