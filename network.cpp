#include <stdio.h>
#include <Windows.h>
#include <network.h>
#include <string.h>
#include <io.h>
#include<QFile>
#include<QDataStream>

#pragma comment(lib, "ws2_32.lib")
#define Port 5000
#define IP_ADDRESS "210.30.97.233"

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
        getchar();
    }
    // 创建一个套接口
    // 当会话结束后，调用closesocket()
    ClientSocket = socket(AF_INET, // 只支持ARPA Internet地址格式
    SOCK_STREAM, // 新套接口的类型描述
    IPPROTO_TCP); // 套接口所用的协议
    if (ClientSocket == INVALID_SOCKET)
    {
        qDebug() << "Create Socket Failed! Error: %d\n";
        getchar();
        return -1;
    }
    ClientAddr.sin_family = AF_INET;
    ClientAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS); // 定义IP地址
    ClientAddr.sin_port = htons(Port); // 将主机的无符号短整形数转换成网络字节顺序
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
        getchar();
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

int Network::send_file(QString _dirt,QString upload_dirt,QString filenamet)
{
    _dirt = "horses.jpg";
    char* _dir;
    QByteArray temp = _dirt.toLatin1();
    _dir = temp.data();

    FILE *file;
    file = fopen("C:/Users/Administrator/Desktop/test/project/NSIR/NSIR/test.jpg","rb");
    if(file == NULL)
    {
        qDebug() << "无法打开文件";
        //return -1;
    }

    int filesize = filelength(fileno(file));
    QString filesize_c = QString::number(filesize);
    qDebug() << filesize_c;

    QString qrequest;
    char* request;
    qrequest = "UPLOAD|" + upload_dirt + "|" + filenamet + "|" + filesize_c + "|";
    QByteArray temp2 = qrequest.toLatin1();
    request = temp2.data();
    qDebug() << request;

    int ret = 0;
    int timeout = 5000;//..毫秒后超时
    int echo = 0;
    char RecvBuffer[8];
    ret = send(ClientSocket,request,strlen(request),0);
    setsockopt(ClientSocket,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(int));
    echo = recv(ClientSocket,RecvBuffer,8,MSG_WAITALL);
    qDebug() << RecvBuffer;

    char* return_check = strtok(RecvBuffer,"|");
    char* return_flag = strtok(NULL,"|");
     if((strcmp(return_check,"SEND") == 0)&&(strcmp(return_flag,"1") == 0))
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
    //以后加入如果错误重新传文件
}

int Network::request_decet(QString _dir,QString command)
{
    QString buffer("DETEC|" + _dir + "|" + command + "|");
    QByteArray temp = buffer.toLatin1();
    char *SendBuffer = temp.data();
    int ret = 0;
    ret = send(ClientSocket,SendBuffer,(int)strlen(SendBuffer),0);
    if (send_check(ret) != 0)
    {
        return send_check(ret);
    }
    int echo = 0;
    char RecvBuffer[10];
    //RecvBuffer = (char*)malloc(sizeof(char) * BufferSize);//为接收缓冲区分配内存
    int timeout = 60000;//..毫秒后超时
    setsockopt(ClientSocket,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(int));
    echo = recv(ClientSocket,RecvBuffer,1,0);//recv默认阻塞模式
    RecvBuffer[echo]='\0';
    if(echo >= 0)
    {
        qDebug() << "got from server:" << RecvBuffer;
        if(strcmp(RecvBuffer,"0") == 0)
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

void Network::test()
{
        while (1)
        {
            // 发送数据至服务器
            //SendBuffer[0] = 'd';
            int ret = 0;
            ret = send(ClientSocket,
                "SendBuffer",
                (int)strlen("SendBuffer"), // 返回发送缓冲区数据长度
                0);

            if (ret == SOCKET_ERROR)
            {
                printf("Send Information Failed! Error:%d\n", GetLastError());
                getchar();
                break;
            }
            else
            {
                qDebug() << "send success";
            }

            break;
        }

        // 关闭socket
        closesocket(ClientSocket);
    WSACleanup();
}
