#ifndef NETWORK_H
#define NETWORK_H

#include<QTcpSocket>
#include<winsock2.h>
#include<QString>


class Network
{
public:
    Network();

    int init(QString IP,QString port);//初始化socket，成功返回0，错误返回-1

    int request_decet(QString _dirfile,QString command);//执行检测图片命令，成功返回0 _dirfile为：目录/文件名.扩展名 的格式 command暂时无用，修改需到服务端代码修改;

    int send_check(int ret);

    int send_file(QString _dir,QString upload_dir,QString filename);//发送图片，成功返回0，无法打开本地文件返回-2._dir为本地目录（最后字符不带'/'） upload_dir为上传至目录（最后字符不带'/'）,filename为图片名

    QString download_file(QString download_dir,QString filename);//下载图片，默认服务器代码运行根目录 download_dir留空 成功返回该图片保存目录 失败返回QString类 "1"

    int testing();//测试服务端，成功返回0，错误返回-1

    int closes();//关闭socket

    //具体发信格式写在服务端net.cpp里

public:
    SOCKET ClientSocket;
    struct sockaddr_in ClientAddr; // 一个sockaddr_in型的结构体对象
    //char SendBuffer[MAX_PATH];   // Windows的MAX_PATH默认是260
};

#endif // NETWORK_H
