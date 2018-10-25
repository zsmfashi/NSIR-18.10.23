	#include <unistd.h>
	#include <stdio.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <sys/types.h>
	#include <stdlib.h>
	#include <string.h>
	using namespace std;

	//#define _GLIBCXX_USE_CXX11_ABI 0

	#define SERVER_PORT 5000
	#define LENGTH_OF_LISTEN_QUEUE 20
	#define BUFFER_SIZE 2048
	class network
	{
	public:
		struct sockaddr_in server_addr;
		int server_socket;
			int opt;


		int init_listen();
		int listen_run();
		int dect(char* _picture,char* command);

	};

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
			printf("Start listening on port:%d\n",SERVER_PORT);
		}
	}

	int network::listen_run()
	{
		 while(1)
		{
			struct sockaddr_in client_addr;
			int client_socket;
			socklen_t length;
			char Buffer[BUFFER_SIZE];

			// 连接客户端Socket
			length = sizeof(client_addr);
			client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &length);
			if (client_socket < 0)
			{
				printf("Server Accept Failed!\n");
				break;
			}

			// 从客户端接收数据
			while(1)
			{
				bzero(Buffer, BUFFER_SIZE);
				length = recv(client_socket, Buffer, BUFFER_SIZE, 0);
				if (length < 0)
				{
					printf("Server Recieve Data Failed!\n");
					//这里再加一个给客户端发信错误，重新发信
					break;
				}
				else if(length > 0)
				{
					printf("got a msg:%s\n", Buffer);//debug
					char* request;
					request = strtok(Buffer,"|");
					printf("%s\n", request);//debug
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
							result = "0";
						}
						else
						{
							result = "1";
						}
						int ret;
						ret = send(client_socket,(char*)result,1,0);
						break;
					}
				}
			}
			close(client_socket);
		}
		close(server_socket);
	}

	int main() // (int argc, char* argv[])
	{
		network ne;
		ne.init_listen();
		ne.listen_run();
	  
		
		return 0;
	}
