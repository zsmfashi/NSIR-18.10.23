#include "mainwindow.h"
#include "settingwindow.h"
#include "network.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    //Network ne;
    //ne.send_file("C:/Users/Administrator/Desktop/test","data","horses.jpg");
    //ne.init("210.30.97.233","5000");
    //ne.request_decet("data/horses.jpg","123");
    //ne.download_file("123","horese.jpg");


    return a.exec();



}
//不能使用qt自带的..会导致内存
