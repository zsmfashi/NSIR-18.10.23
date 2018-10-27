#ifndef NETWORK_H
#define NETWORK_H

#include<QTcpSocket>
#include<winsock2.h>
#include<QString>


class Network
{
public:
    Network();

    int init(QString IP,QString port);

    int request_decet(QString _dir,QString command);

    int send_check(int ret);

    int send_file(QString _dir,QString upload_dir,QString filename);

    QString download_file(QString download_dir,QString filename);

public:
    SOCKET ClientSocket;
    struct sockaddr_in ClientAddr; // 一个sockaddr_in型的结构体对象
    //int ret = 0;
    //char SendBuffer[MAX_PATH];   // Windows的MAX_PATH默认是260
};

#endif // NETWORK_H
