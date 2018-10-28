#include <stdio.h>
#include <Windows.h>
#include <network.h>
#include <string.h>
#include <io.h>
#include<QFile>
#include<QDataStream>
#include<QDateTime>
#include<QCoreApplication>
#include<malloc.h>
#include<memory.h>

#pragma comment(lib, "ws2_32.lib")
//#define Port 5000
//#define IP_ADDRESS "210.30.97.233"

Network::Network()
{

}

int Network::init(QString IP,QString port)
{
    WSADATA s; // 用来储存调用AfxSocketInit全局函数返回的Windows Sockets初始化信息
    // 初始化Windows Socket
    if (WSAStartup(MAKEWORD(2, 2), &s) != 0) // 通过连接两个给定的无符号参数,首个参数为低字节
    {
        qDebug() << "Init Windows Socket Failed! Error: \n";
    }
    // 创建一个套接口
    // 当会话结束后，调用closesocket()
    //ClientSocket = (SOCKET)malloc(102400);
    ClientSocket = socket(AF_INET, // 只支持ARPA Internet地址格式
    SOCK_STREAM, // 新套接口的类型描述
    IPPROTO_TCP); // 套接口所用的协议
    if (ClientSocket == INVALID_SOCKET)
    {
        qDebug() << "Create Socket Failed! Error: %d\n";
        return -1;
    }
    char* ipc = new char[6];
    QByteArray temp = IP.toLatin1();
    ipc = temp.data();

    ClientAddr.sin_family = AF_INET;
    ClientAddr.sin_addr.s_addr = inet_addr(ipc); // 定义IP地址
    ClientAddr.sin_port = htons(port.toInt()); // 将主机的无符号短整形数转换成网络字节顺序
    memset(ClientAddr.sin_zero, 0X00, 8); // 函数通常为新申请的内存做初始化工作
    int ret = 0;
    // 连接Socket
    ret = connect(ClientSocket,
        (struct sockaddr*)&ClientAddr,
        sizeof(ClientAddr));
    if (ret == SOCKET_ERROR)
    {
        qDebug() << "Socket Connect Failed! Error:%d\n";
        qDebug() << GetLastError();
        return -1;
    }
    else
    {
        qDebug() << "Socket Connect Succeed!";
        return 0;
    }
}

int Network::send_check(int ret)
{
    if (ret == SOCKET_ERROR)
    {
        qDebug() << "send failed";
        qDebug() << GetLastError();
        return GetLastError();
    }
    else
    {
        qDebug() << "send success";
        return 0;
    }
}

QString Network::download_file(QString proc_dirt,QString filenamet)
{
    qint64 timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
    QString file_name = filenamet.section('.',0,0);
    QString ext_name = filenamet.section('.',1,1);
    file_name = file_name +"_" + QString::number(timestamp);
    QString filename = file_name + "." + ext_name;
    QString current_path = qApp->applicationDirPath();
    QString _dirt = current_path + "/temp/"+ filename;
    while(1)
    {
        int place = _dirt.indexOf('/');
        if(place == -1)
        {
            break;
        }
        _dirt.replace(place,1,"\\");
    }
    qDebug() << _dirt;
    char*  _dir;
    QByteArray temp = _dirt.toLatin1();
    _dir=temp.data();
    FILE *new_pic;
    new_pic = fopen(_dir,"wb+");
    if(new_pic == NULL)
    {
        return "-1";
    }

    QString qrequest1;
    qrequest1 = "DOWNLOAD|" + proc_dirt + "|predictions." + ext_name + "|";
    char*  request1;
    QByteArray temp2 = qrequest1.toLatin1();
    request1=temp2.data();
    qDebug() << request1;
    int ret1 = 0;

    ret1 = send(ClientSocket,request1,strlen(request1),0);
    int echo1 = 0;
    char *recv1 = new char[2000];
    //recv1 = (char*)malloc(sizeof(char) * 20);
    memset(recv1,0,16);
    echo1 = recv(ClientSocket,recv1,2000,0);
    qDebug()<<"last error" << GetLastError();
    qDebug() << "echo1" << echo1;
    //recv1[echo1+1] = '\0';
    qDebug() <<  recv1;
    char* return_check = strtok(recv1,"|");
    char* return_size = strtok(NULL,"|");
    qDebug() << return_check;
    qDebug() << return_size;
    int filesize = atoi(return_size);
    if((strcmp(return_check,"0") == 0))
    {
        int ret2 = 0;
        ret2 = send(ClientSocket,"SEND|",5,0);
        int nCount = 0;
        char buffer[15000];
        while(1)
        {
            nCount = recv(ClientSocket, buffer, 15000, 0);
            if(nCount > 0)
            {
                fwrite(buffer, nCount, 1, new_pic);
                filesize -= nCount;
            }
            else
            {
                break;
            }
            if(filesize <= 0)
            {
                break;
            }
        }
    }
    else
    {
        return "-1";
    }
    fclose(new_pic);
    return _dirt;
}

int Network::send_file(QString _dirt,QString upload_dirt,QString filenamet)
{
    _dirt = _dirt + "/" + filenamet;
    char* _dir;
    QByteArray temp = _dirt.toLatin1();
    _dir = temp.data();

    FILE *file;
    file = fopen(_dir,"rb");
    if(file == NULL)
    {
        qDebug() << "无法打开文件";
        return -2;
    }

    int filesize = filelength(fileno(file));
    QString filesize_c = QString::number(filesize);
    qDebug() << filesize_c;

    QString qrequest;
    char* request;
    qrequest = "UPLOAD|" + upload_dirt + "/|" + filenamet + "|" + filesize_c + "|";
    QByteArray temp2 = qrequest.toLatin1();
    request = temp2.data();
    qDebug() << request;

    int ret = 0;
    int timeout = 5000;//..毫秒后超时
    int echo = 0;
    char RecvBuffer[8];
    ret = send(ClientSocket,request,100,0);
    qDebug() << GetLastError();

    setsockopt(ClientSocket,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(int));
    echo = recv(ClientSocket,RecvBuffer,8,MSG_WAITALL);
    qDebug() << RecvBuffer;

    char* return_check = strtok(RecvBuffer,"|");
    char* return_flag = strtok(NULL,"|");
     if((strcmp(return_check,"SEND") == 0)&&(strcmp(return_flag,"0") == 0))
    {
        char buffer[1400];
        int per_size = 0;
         while(1)
         {
            per_size = fread(buffer,sizeof(char),1400,file);
            send(ClientSocket, (char*)buffer, per_size, 0);
            if(per_size < 1400)
            {
                break;
            }
            Sleep(1);
         }
    }
    echo = recv(ClientSocket,RecvBuffer,7,MSG_WAITALL);//收上传完成回执
    char* return_finish = strtok(RecvBuffer,"|");
    if(strcmp(return_finish,"FINISH") == 0)
    {
        return 0;
    }
    return -1;
    //以后加入如果错误重新传文件
}

int Network::request_decet(QString _dirfile,QString command)
{
    QString buffer("DETEC|" + _dirfile + "|" + command + "|");
    QByteArray temp = buffer.toLatin1();
    char *SendBuffer = temp.data();
    int ret = 0;
    ret = send(ClientSocket,SendBuffer,(int)strlen(SendBuffer),0);
    if (send_check(ret) != 0)
    {
        return send_check(ret);
    }
    int echo = 0;
    char RecvBuffer[1];
    //RecvBuffer = (char*)malloc(sizeof(char) * BufferSize);//为接收缓冲区分配内存
    int timeout = 60000;//..毫秒后超时
    setsockopt(ClientSocket,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(int));
    echo = recv(ClientSocket,RecvBuffer,2,0);//recv默认阻塞模式
   // RecvBuffer[echo]='\0';
    char* return_flag = strtok(RecvBuffer,"|");
    if(echo >= 0)
    {
        qDebug() << "got from server:" << return_flag;
        if(strcmp(return_flag,"0") == 0)
        {
            qDebug() << "success";
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        qDebug() << "请求发出没有收到回信或出错";
        return 1;
    }
}

int Network::testing()
{
    char* test = "TEST|";
    int ret1 = 0;
    ret1 = send(ClientSocket,test,5,0);
    int echo = 0;
    char recvtest[5];
    int timeout = 5000;//..毫秒后超时
    setsockopt(ClientSocket,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(int));
    echo = recv(ClientSocket,recvtest,5,0);
    qDebug() << recvtest;
    char* return_flag = strtok(recvtest,"|");
    if(strcmp(return_flag,"TEST") == 0)
    {
        return 0;
    }
    return -1;
}

int Network::closes()
{
    closesocket(ClientSocket);
}
