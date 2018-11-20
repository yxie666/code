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
#define PORT    1234
#define LISTNUM 20


int main(int argc, char *argv[])
{
  int sockfd, newfd;
  struct sockaddr_in s_addr, c_addr;
  char buf[BUFLEN];
  socklen_t len;
  fd_set rfds;
  struct timeval tv;
  int retval, maxfd;

  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
   {
      perror("socket failed");
      exit(-1);
   }
 
   memset(&s_addr,0,sizeof(s_addr));
   s_addr.sin_family = AF_INET;
   s_addr.sin_port = htons(PORT);
   s_addr.sin_addr.s_addr = htons(INADDR_ANY);
   
   if((bind(sockfd, (struct sockaddr*)&s_addr, sizeof(struct sockaddr))) == -1)
   {
      perror("bind failed");
      exit(-1);
   }

   listen(sockfd, LISTNUM);
   
   while(1)
   {
     printf("***********开始聊天*************\n");
     len = sizeof(struct sockaddr);
     if((newfd = accept(sockfd, (struct sockaddr*)&c_addr, &len)) == -1)
     {
        perror("accept failed");
        exit(-1);
     }
   
     printf("与你通信的人： %s : %d \n",\
            inet_ntoa(c_addr.sin_addr),ntohs(c_addr.sin_port));
     while(1)
     {
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        maxfd = 0;
        FD_SET(newfd, &rfds);
        //找出文件描述符集合中最大的文件描述符
		if(maxfd < newfd) 
          maxfd = newfd;
		//设置超时时间
		tv.tv_sec = 2;
		tv.tv_usec = 0;
		//等待聊天
		retval = select(maxfd+1, &rfds, NULL, NULL, &tv);
		if(retval == -1)
		{
			printf("select failed\n");
			break;
		}
		else if(retval == 0)
		{
		  printf("waiting...\n");
		  continue;
		}
		else
		{
		  //用户输入信息
		  if(FD_ISSET(0, &rfds))
		  {
			memset(buf, 0, sizeof(buf));
			fgets(buf, BUFLEN, stdin);
			if(!strncasecmp(buf,"quit",4))
			{
				printf("quit server chat\n");
				break;
			}
			len = send(newfd, buf, strlen(buf), 0);
			if(len > 0)
				printf("send success\n");
			else
			{
				printf("send failed\n");
				break;
			}
		  }

		//客户端发来了消息
		if(FD_ISSET(newfd, &rfds))
		{
		   memset(buf, 0, sizeof(buf));
		   len = recv(newfd, buf, BUFLEN, 0);
		   if(len > 0)
				printf("the recv message is: %s\n",buf);
		   else
		   {
			   if(len < 0)
					printf("the recv message failed\n");
				else
					printf("the client is quiting!\n");
				break;
		   }
			}
		}
	 }
		 
	   close(newfd);
	   printf("the server weather quit: y->yes; n->no\n");
	   bzero(buf, BUFLEN);
	   fgets(buf, BUFLEN, stdin);
	   if(!strncasecmp(buf, "y", 1))
	   {
			printf("server quit\n");
			break;
	   }
	   }
	   
	   close(sockfd);
	   return 0;
 }
