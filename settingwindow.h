#ifndef SETTINGWINDOW_H
#define SETTINGWINDOW_H

#include <QWidget>
#include<QString>
#include<QFile>
#include<QDebug>
#include<QMessageBox>
#include<QCoreApplication>


namespace Ui {
class SettingWindow;
}

class SettingWindow : public QWidget
{
    Q_OBJECT

public:
    QString IP_addr;
    QString UsrName;
    QString Passwd;
    QString Upload_addr;
    QString Process_addr;
    QString Command;

public:
    explicit SettingWindow(QWidget *parent = nullptr);
    ~SettingWindow();

    int LoadSetting();

    void SaveSetting();

    void InitSetting();

    void ErrorWindow(QString Error_Message);

    void CheckSetting();

private slots:
    void on_Confirm_PushButton_clicked();

private:
    Ui::SettingWindow *ui;
};

#endif // SETTINGWINDOW_H
