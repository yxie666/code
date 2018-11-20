#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>

#define BUFLEN  1024
#define PORT       1234

int main(int argc,  char *argv[])
{
	int sockfd;
	struct sockaddr_in s_addr;
	socklen_t len;
	unsigned int port;
	char buf[BUFLEN];
	fd_set rfds;
	struct timeval tv;
	int retval, maxfd;
	
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket failed\n");
		exit(-1);
	}
	
	memset(&s_addr, 0, sizeof(s_addr));
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(PORT);
	if(inet_aton(argv[1], (struct in_addr *)&s_addr.sin_addr.s_addr)  == 0)
	{
		perror(argv[1]);
		exit(-1);
	}
	
	if(connect(sockfd, (struct sockaddr*)&s_addr,sizeof(struct sockaddr)) == - 1)
	{
		perror("connect failed\n\n");
		exit(-1);
	}
	
	while(1)
	{
		FD_ZERO(&rfds);
		FD_SET(0, &rfds);
		maxfd = 0;
		FD_SET(sockfd, &rfds);
		if(maxfd < sockfd)
			maxfd = sockfd;
		tv.tv_sec = 2;
		tv.tv_usec = 0;
		retval = select(maxfd+1, &rfds, NULL, NULL, &tv);
		if(retval == -1)
		{
			printf("select is wrong!\n");
			break;
		}
		else if(retval == 0)
		{
			printf("waiting....\n");
			continue;
		}
		else
		{
			//服务器发来消息
			if(FD_ISSET(sockfd, &rfds))
			{
				bzero(buf, BUFLEN);
				len = recv(sockfd, buf, BUFLEN, 0);
				if(len > 0)
					printf("the recv data is: %s\n",buf);
				else
				{
					if(len < 0)
						printf("the recv message failed\n");
					else
						printf("the server is quited\n");
					break;
				}
			}
			
			//用户输入信息，开始处理信息并发送
			if(FD_ISSET(0, &rfds))
			{
				bzero(buf, BUFLEN);
				fgets(buf, BUFLEN, stdin);
				
				if(!strncasecmp(buf, "quit", 4))
				{
					printf("client is quitting chat\n");
					break;
				}
				
				len = send(sockfd, buf, strlen(buf), 0);
				if(len > 0)
					printf("the message sending \n");
				else
				{
					printf("the message send failed\n");
					break;
				}
			}
		}
	}
	
	close(sockfd);
	return 0;
}