#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include<settingwindow.h>
#include<network.h>

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

    void set_setting(QString ip,QString port);
    void delaymsec(int msec);

    QString IP;
    QString port;

    SettingWindow setting_window;
    Network* connection;
private slots:
    void on_Test_Button_clicked(bool checked);

    void on_Continue_Button_clicked();

    void on_Setting_Button_clicked();

private:
    Ui::LoginWindow *ui;
};

#endif // LOGINWINDOW_H
