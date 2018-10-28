#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<network.h>
#include<loginwindow.h>
#include<settingwindow.h>
#include<windows.h>
#include<QCoreApplication>
#include<thread.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void delaymsec(int msec);


    QString cur_pic_dir;

    Network ne;
    LoginWindow login_window;



private slots:
    void on_Pic_Button_clicked();

    void on_Start_Button_clicked();

    void on_Check_Button_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
