#include "settingwindow.h"
#include "ui_settingwindow.h"

SettingWindow::SettingWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWindow)
{
    ui->setupUi(this);
    this->LoadSetting();
    ui->IP_Edit->setText(this->IP_addr);
    ui->Usr_Edit->setText(this->UsrName);
    ui->Upload_Edit->setText(this->Upload_addr);
    ui->Psd_Edit->setText(this->Passwd);
    ui->Process_Edit->setText(this->Process_addr);
    ui->Command_Edit->setText(this->Command);
}

SettingWindow::~SettingWindow()
{
    delete ui;
}

int SettingWindow::LoadSetting()
{
    QString path = qApp->applicationDirPath();
    QFile file(path + "/setting.ini");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Can't open Setting.ini"<<endl;
        this->ErrorWindow("无法打开配置文件！");
        return 1;
    }
    for(int i = 0; i < 6; i++)
    {
        QByteArray line = file.readLine();
        QString str(line);
        if (str.startsWith("IP_address="))
        {
            int last = str.indexOf('\n');
            QString ip_temp(str.mid(11,last-11));
            this->IP_addr = ip_temp;
            qDebug() << ip_temp;
        }
        else if (str.startsWith("User_Name="))
        {
            int last = str.indexOf('\n');
            QString name_temp(str.mid(10,last-10));
            this->UsrName = name_temp;
            qDebug() << name_temp;
        }
        else if (str.startsWith("Password="))
        {
            int last = str.indexOf('\n');
            QString passwd_temp(str.mid(9,last-9));
            this->Passwd = passwd_temp;
            qDebug() << passwd_temp;
        }
        else if (str.startsWith("Upload_address="))
        {
            int last = str.indexOf('\n');
            QString up_temp(str.mid(15,last-15));
            this->Upload_addr = up_temp;
            qDebug() << up_temp;
        }
        else if (str.startsWith("Process_address="))
        {
            int last = str.indexOf('\n');
            QString pro_temp(str.mid(16,last-16));
            this->Process_addr = pro_temp;
            qDebug() << pro_temp;
        }
        else if (str.startsWith("Command="))
        {
            int last = str.indexOf('\n');
            QString com_temp(str.mid(8,last-8));
            this->Command = com_temp;
            qDebug() << com_temp;
        }
    }
    if(this->IP_addr.isEmpty()||
       this->UsrName.isEmpty()||
       this->Passwd.isEmpty()||
       this->Upload_addr.isEmpty()||
       this->Process_addr.isEmpty()||
       this->Command.isEmpty()
       )
    {
        //ErrorWindow("配置文件错误！");
        return 1;
    }
    file.close();
    return 0;
}

void SettingWindow::SaveSetting()
{
    this->InitSetting();
    this->IP_addr = ui->IP_Edit->text();
    this->UsrName = ui->Usr_Edit->text();
    this->Passwd = ui->Psd_Edit->text();
    this->Upload_addr = ui->Upload_Edit->text();
    this->Process_addr = ui->Process_Edit->text();
    this->Command = ui->Command_Edit->text();

    QString path = qApp->applicationDirPath();
    qDebug() << path;
    QFile file(path + "/setting.ini");
    if(!file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate))
    {
        qDebug()<< "Open failed." << endl;
    }

    QTextStream txtOutput(&file);
    QString sip("IP_address=" + ui->IP_Edit->text());
    QString sname("User_Name=" + ui->Usr_Edit->text());
    QString spasswd("Password=" + ui->Psd_Edit->text());
    QString sup("Upload_address=" + ui->Upload_Edit->text());
    QString spr("Process_address=" + ui->Process_Edit->text());
    QString scom("Command=" + ui->Command_Edit->text());
    txtOutput << sip << endl;
    txtOutput << sname << endl;
    txtOutput << spasswd << endl;
    txtOutput << sup << endl;
    txtOutput << spr << endl;
    txtOutput << scom << endl;

    file.close();
    QMessageBox::information(NULL, "保存设置", "保存完成", QMessageBox::Yes, QMessageBox::Yes);
}

void SettingWindow::InitSetting()
{
    this->IP_addr = "";
    this->UsrName = "";
    this->Upload_addr = "";
    this->Process_addr = "";
    this->Command = "";
    this->Passwd = "";
}

void SettingWindow::ErrorWindow(QString message)
{
    QMessageBox::critical(0,
    "critical message" , message,
    QMessageBox::Ok | QMessageBox::Default ,
    0,0);
}

void SettingWindow::on_Confirm_PushButton_clicked()
{
    this->SaveSetting();
    if(this->LoadSetting() == 1)
    {
        this->ErrorWindow("参数错误！请重新填写！");
    }
    else
    {
        this->setWindowModality(Qt::NonModal);
        this->hide();
    }
}

void SettingWindow::CheckSetting()
{
    if (this->LoadSetting() == 1)
    {
        this->setWindowModality(Qt::ApplicationModal);
        this->show();
    }
}
