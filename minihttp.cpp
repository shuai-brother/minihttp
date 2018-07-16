#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <fstream>
#include <iostream>
#include <string>
using namespace std;
int main()
{
	SOCKET soc,csoc;
	SOCKADDR_IN serverAddr,clientAddr;
	char buf[1024];
	int len;
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 0), &wsa); 
	if ((soc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) <= 0)
	{
		printf("套接字socket创建失败!\n");
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(9000);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	if (bind(soc, (SOCKADDR *)&serverAddr, sizeof(serverAddr)) != 0)
	{
		printf("套接字绑定失败!\n");
	}

start:
	printf("开始监听...\n");
	if (listen(soc, 1) != 0)
	{
		printf("监听失败!\n");
	}

	len = sizeof(SOCKADDR_IN);

	if ((csoc = accept(soc, (SOCKADDR *)&clientAddr, &len)) <= 0)
	{
		printf("接受连接失败!\n");
	}
	printf("**************连接成功*********\n");

	Sleep(100);

	bool image = false;
	bool image_contimue = false;
	bool text = false;
	while (1)
	{
		memset(buf, 0, 1024);
		int x = 0;
		x = recv(csoc, buf, 1024, 0);
		if (x <= 0)
		{
			printf("关闭连接!\n");
			closesocket(csoc);
			goto start;
		}
		cout << buf << endl;
		string XML(buf);

		if (XML.find("GET") != string::npos)
		{
			sprintf(buf, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n%s", strlen("Hello World!"), "Hello World!");
			send(csoc, buf, strlen(buf) + 1, 0);
			printf("已回复 200 OK\n");
			continue;
		}
		if (XML.find("100-continue") != string::npos)
		{
			sprintf(buf, "HTTP/1.1 100-continue\r\n");
			send(csoc, buf, strlen(buf) + 1, 0);
			printf("已回复100-continue!\n");
			image_contimue = false;
		}
		else
		{
			int off_set = 0;
			if (off_set = XML.find("Content-Type: image"))
			{
				if (off_set>0 && image_contimue == false)
				{
					image = true;
					image_contimue = true;
					off_set = XML.find_last_of("Content-Type: image");
					if (off_set <= 0)
						continue;
					string tmp = XML.substr(off_set + 5);

					cout << tmp << endl;
					ofstream fout;
					fout.open("recieved.jpeg", ios::binary | ios::app);
					fout.write(buf + off_set + 5, x - off_set - 5);
					fout.close();
				}
				else if (image == true)
				{
					if (image_contimue == true)
					{
						ofstream fout;
						fout.open("recieved.jpeg", ios::binary | ios::app);
						fout.write(buf, x);
						fout.close();
					}

				}

			}
		}
	}
	printf("关闭连接!\n");
	closesocket(csoc);
	goto start;
	WSACleanup();     
	return 0;
}
