#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTime>
#include<QFileDialog>
#include<QIcon>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    login_window.show();
    while(!login_window.isHidden())
    {
        delaymsec(1000);
    }

    ui->setupUi(this);
    setFixedSize(this->width(), this->height());

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::delaymsec(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);

    while( QTime::currentTime() < dieTime )

    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void MainWindow::on_Pic_Button_clicked()
{
    cur_pic_dir = QFileDialog::getOpenFileName(this, tr("打开图片"), " ",  tr("图片文件(*.jpg *.png);;Allfile(*.*)"));
    if(cur_pic_dir != "")
    {
        QImage Image;
        Image.load(cur_pic_dir);
        QPixmap pixmap = QPixmap::fromImage(Image);
        int with = ui->Pic_Button->width();
        int height = ui->Pic_Button->height();
        QPixmap fitpixmap = pixmap.scaled(with, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        ui->Pic_Button->setIconSize(ui->Pic_Button->rect().size());
        ui->Pic_Button->setIcon(pixmap);
        this->login_window.setting_window.filename = cur_pic_dir.section('/',-1,-1);
        qDebug() << this->login_window.setting_window.filename;
        this->login_window.setting_window.filedir = cur_pic_dir.replace(this->login_window.setting_window.filename,"");
        qDebug() << this->login_window.setting_window.filedir;
        ui->Start_Button->setEnabled(true);
    }
}

void MainWindow::on_Start_Button_clicked()
{
    ui->Pic_Button->setEnabled(false);
    ui->Start_Button->setEnabled(false);
    ui->Start_Button->setText("上传图片");

    //delaymsec(200);

    this->login_window.connection = new Network;
    this->login_window.connection->init(this->login_window.IP,this->login_window.port);
    int result1 = this->login_window.connection->send_file(this->login_window.setting_window.filedir,
                                                          this->login_window.setting_window.Upload_addr,
                                                          this->login_window.setting_window.filename);
    this->login_window.connection->closes();
    delete(this->login_window.connection);

    if(result1 == 0)
    {
        ui->Start_Button->setText("正在识别");
        delaymsec(1000);

        this->login_window.connection = new Network;
        this->login_window.connection->init(this->login_window.IP,this->login_window.port);
        int result2 = this->login_window.connection->request_decet((this->login_window.setting_window.Upload_addr + "/" + this->login_window.setting_window.filename),
                                                                   this->login_window.setting_window.Command);
        this->login_window.connection->closes();
        delete(this->login_window.connection);

    }
    ui->Pic_Button->setEnabled(true);
}

void MainWindow::on_pushButton_clicked()
{
    ui->Start_Button->setText("正在下载");
    QString result3;

    this->login_window.connection = new Network;
    this->login_window.connection->init(this->login_window.IP,this->login_window.port);
    result3 = this->login_window.connection->download_file("123",
                                                          this->login_window.setting_window.filename);
    this->login_window.connection->closes();
    delete(this->login_window.connection);
/*
    Thread th;
    th.run();
    if(!th.isRunning())
    {
        delaymsec(1000);
    }
*/



    if(result3 != "1")
    {
        this->login_window.setting_window.filesaved = result3;
        QImage Image;
        Image.load(this->login_window.setting_window.filesaved);
        QPixmap pixmap = QPixmap::fromImage(Image);
        int with = ui->Pic_Button->width();
        int height = ui->Pic_Button->height();
        QPixmap fitpixmap = pixmap.scaled(with, height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        ui->Pic_Button->setIconSize(ui->Pic_Button->rect().size());
        ui->Pic_Button->setIcon(pixmap);
    }
}
