#include "loginwindow.h"
#include "ui_loginwindow.h"
#include<QTime>

LoginWindow::LoginWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginWindow)
{
    if(setting_window.LoadSetting() == -1)
    {
        setting_window.show();
    }
    while(!setting_window.isHidden())
    {
        delaymsec(1000);
    }
    this->IP = setting_window.IP_addr;
    this->port = setting_window.Port;
    ui->setupUi(this);
    setFixedSize(this->width(), this->height());
    ui->IP_lineEdit->setText(this->IP);
    ui->Port_lineEdit->setText(this->port);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::set_setting(QString ip,QString port)
{
    this->IP = ip;
    this->port = port;
}

void LoginWindow::delaymsec(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);

    while( QTime::currentTime() < dieTime )

    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void LoginWindow::on_Test_Button_clicked(bool checked)
{
    this->connection = new Network;
    if(this->connection->init(this->setting_window.IP_addr,this->setting_window.Port) == -1)
    {
        this->setting_window.ErrorWindow("无法连接服务器！");
    }
    else
    {
        if(this->connection->testing() == -1)
        {
            this->setting_window.ErrorWindow("服务端不在线！");
        }
        else
        {
            QMessageBox::information(NULL, "测试连接", "连接成功！", QMessageBox::Yes, QMessageBox::Yes);
            ui->Continue_Button->setEnabled(true);
            //ui->Test_Button->setEnabled(false);
            ui->Setting_Button->setEnabled(false);
        }
    }
    this->connection->closes();
    delete(connection);
}

void LoginWindow::on_Continue_Button_clicked()
{
    this->hide();
}

void LoginWindow::on_Setting_Button_clicked()
{
check:setting_window.show();
    delaymsec(500);
    while(!setting_window.isHidden())
    {
        delaymsec(1000);
    }
    if(setting_window.LoadSetting() == -1)
    {
        setting_window.ErrorWindow("配置错误！");
        goto check;
    }
    delaymsec(100);
    this->IP = setting_window.IP_addr;
    this->port = setting_window.Port;
    ui->IP_lineEdit->setText(this->IP);
    ui->Port_lineEdit->setText(this->port);
}
