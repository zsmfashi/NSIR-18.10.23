	#include <unistd.h>
	#include <stdio.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <sys/types.h>
	#include <stdlib.h>
	#include <string.h>
	#include <errno.h>
	#include <fstream>
	#include <unistd.h>
	using namespace std;

	//#define _GLIBCXX_USE_CXX11_ABI 0

	#define SERVER_PORT 5000
	#define LENGTH_OF_LISTEN_QUEUE 20
	#define BUFFER_SIZE 20480
	class network
	{
	public:
		struct sockaddr_in server_addr;
		int server_socket;
		int client_socket;
		int opt;

		int init_listen();
		int listen_run();
		int dect(char* _picture,char* command);
		int recv_file(char* _dir,char* filename,char* filesize_c);
		int send_file(char* _dir,char* filename);
		void CheckBufSize();
	};

	void network::CheckBufSize()
	{
		int defRcvBufSize = -1;
		socklen_t optlen = sizeof(defRcvBufSize);
		if (getsockopt(server_socket, SOL_SOCKET, SO_RCVBUF, &defRcvBufSize, &optlen) < 0)
		{
			printf("getsockopt error=%d(%s)!!!\n", errno, strerror(errno));
		}
		printf("OS default tcp socket recv buff size is: %d\n", defRcvBufSize);
	}

	int network::send_file(char* _dir,char* filename)//回执格式 0/-1（打开/未找到）|文件大小(字节)|
	{
		FILE *pic;
		pic = fopen(filename,"rb");
		char* flag = new char[50];
		flag[0] = '\0';
		//flag = (char*)malloc(sizeof(char) * 50);
		if(pic == NULL)
		{
			flag = "-1";
		}
		else
		{
			strcat(flag,"0|");
			flag[2] = '\0';
			int filesize;
			fseek(pic, 0L, SEEK_END);  
			filesize = ftell(pic);  
			char *filesize_c;
			filesize_c = (char*)malloc(sizeof(char) * 30);
			sprintf(filesize_c,"%d",filesize);
			strcat(flag,filesize_c);
			strcat(flag,"|");
			//printf("%s\n",flag);//debug
		}
		fseek(pic, 0L, SEEK_SET);  
		int ret1 = 0;
		ret1 = send(client_socket,flag,15,0);
		
		//usleep(100000);
		char recv1[5];
		int echo1 = 0;
		struct timeval timeout={5,0};//5s
		extern int errno;
		int set_ret=setsockopt(client_socket,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout));
		
		echo1 = recv(client_socket,recv1,5,0);
		printf("echo1=%d\n",echo1);
		printf("errno=%d\n",errno);
		char* return_check = strtok(recv1,"|");
		printf("2\n");
		if(1)//strcmp(recv1,"SEND") == 0)
		{
			char buffer[1400];
			int per_size = 0;
			while(1)
			{
				per_size = fread(buffer,sizeof(char),1400,pic);
				send(client_socket, (char*)buffer, per_size, 0);
				if(per_size < 1400)
				{
					break;
				}
				usleep(1000);//1000微秒
			}
		}
	}

	int network::recv_file(char* _dir,char* filename,char* filesize_c)
	{
		printf("Receving File.....\n");
		int filesize = 0;
		char* return_flag;
		filesize = atoi(filesize_c);
		strcat(_dir,filename);
		FILE *new_pic;
		char* flag;
		new_pic = fopen(_dir,"wb+");
		if(new_pic == NULL) 
		{
			flag = "SEND|-1|";
		}
		else
		{
			flag = "SEND|0|";
		}
		int ret = 0;
		ret = send(client_socket,flag,8,0);
		//加一个检查发送

		char *buffer;
		buffer = (char*)malloc(sizeof(char) * filesize);
		int echo = 0;
		struct timeval timeout={5,0};//5s
		int set_ret=setsockopt(client_socket,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout));
		int nCount = 0;
		while(1)
		{
			nCount = recv(client_socket, buffer, BUFFER_SIZE, 0);
			if(nCount > 0)
			{
				fwrite(buffer, nCount, 1, new_pic);
				filesize -= nCount;
				//printf("%d\n",nCount);//debug
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
		fclose(new_pic);
		//以后加if检查文件正确，发送回执
		ret = send(client_socket,"FINISH|",7,0);
		printf("File Received!\n");
	}

	int network::dect(char* _picture,char* command)
	{

		pid_t status;
		char _file[100] = "./darknet detect cfg/yolov3.cfg yolov3.weights ";
		strcat(_file,_picture);
		printf(_file);
		status = system(_file);
		if (-1 == status)
		{
			printf("system error!");
			return -1;
		}
		else
		{
			printf("exit status value = [0x%x]\n", status);
			return 0;
		}
	}

	int network::init_listen()
	{
		opt = 1;
		bzero(&server_addr, sizeof(server_addr)); // 置字节字符串前n个字节为0，包括'\0'
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = htons(INADDR_ANY); // 转小端,INADDR_ANY就是指定地址为0.0.0.0的地址
		server_addr.sin_port = htons(SERVER_PORT);  

		// 创建一个Socket
		server_socket = socket(PF_INET, SOCK_STREAM, 0);

		if (server_socket < 0)
		{
			printf("Create Socket Failed!\n");
			exit(1);
		}

		// bind a socket
		setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
		if(bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))
		{
			printf("Server Bind Port: %d Failed!\n", SERVER_PORT);
			exit(1);
		}

		// 监听Socket
		if (listen(server_socket, LENGTH_OF_LISTEN_QUEUE))
		{
			printf("Server Listen Failed!\n");
			exit(1);
		}   
		else
		{
			printf("\033[32mStart listening on port:\033[0m%d\n",SERVER_PORT);
		}
	}

	int network::listen_run()
	{
		 while(1)
		{
			struct sockaddr_in client_addr;
			socklen_t length;
			char Buffer[BUFFER_SIZE];

			// 连接客户端Socket
			length = sizeof(client_addr);
			client_socket = accept(server_socket,(struct sockaddr*)&client_addr,&length);
			if (client_socket < 0)
			{
				printf("Server Accept Failed!\n");
				break;
			}
			//printf("Start listening on port:%d\n",SERVER_PORT);
			// 从客户端接收数据
			while(1)
			{
				bzero(Buffer,BUFFER_SIZE);
				length = recv(client_socket,Buffer,BUFFER_SIZE, 0);
				if (length < 0)
				{
					printf("Server Recieve Data Failed!\n");
					printf("\n");
				}
				else if(length == 0)
				{
					printf("\033[31mClient Closed!\033[0m\n");
					return 0;
				}
				else if(Buffer && *Buffer != '\0')
				{
					printf("====================\n");
					printf("\033[32mGot a msg from Client:\033[0m%s\n",Buffer);//debug
					char* request;
					request = strtok(Buffer,"|");
					printf("REQUEST = \033[33m%s\033[0m\n",request);//debug
					if(strcmp(request,"DETEC") == 0)
					{
						printf("enter area of detec\n");//debug
						char* _dir;
						char* command;
						_dir = strtok(NULL,"|");
						command = strtok(NULL,"|");
						printf("%s\n",_dir); //debug
						printf("%s\n",command);//debug

						char *result;
						if(dect(_dir,command) == 0)
						{
							result = "0|";
						}
						else
						{
							result = "1|";
						}
						int ret;
						ret = send(client_socket,result,2,0);
						printf("%d\n",ret);
					}
					else if(strcmp(request,"UPLOAD") == 0)//请求格式： UPLOAD|目录|文件名.扩展名|文件大小(字节)|
					{
						char* _dir;
						char* filename;
						char* filesize_c;
						_dir = strtok(NULL,"|");
						filename = strtok(NULL,"|");
						filesize_c = strtok(NULL,"|");
						printf("Uploading to:%s",_dir);
						printf("%s\n",filename);
						printf("With a size of %s\n",filesize_c);
						char* result;
						if(recv_file(_dir,filename,filesize_c) == 0)
						{
							result = "0";
						}
						else
						{
							result = "1";
						}
						int ret;
						//ret = send(client_socket,(char*)result,1,0);
					}
					else if(strcmp(request,"DOWNLOAD") == 0)//请求格式： DOWNLOAD|目录(暂时没用，但要有)|文件名| (默认输出predictions.* 扩展名会在客户端自己添加)
					{
						char* _dir;
						char* filename;
						_dir = strtok(NULL,"|");
						filename = strtok(NULL,"|");
						//printf("%s\n",_dir);
						printf("Sending ");
						printf("%s\n",filename);
						send_file(_dir,filename);
					}
					else if(strcmp(request,"TEST") == 0)
					{
						int ret = 0;
						ret = send(client_socket,"TEST|",5,0);
						if (ret == 5)
						{
							printf("ECHO = TEST ...sent!\n");
						}
	
					}
					printf("\033[32mRequest Compeleted!\033[0m\n");
					printf("====================\n");
				}
				break;
			}
			close(client_socket);
			break;
		}
		close(server_socket);
	}

	int main() // (int argc, char* argv[])
	{
		network *ne;
		while(1)
		{
			ne = new network;
			ne->init_listen();
			ne->CheckBufSize();
			ne->listen_run();
			delete(ne);
		}
		return 0;
	}
