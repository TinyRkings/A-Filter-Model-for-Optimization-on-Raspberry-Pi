#include <sys/time.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include <fcntl.h> // for open
#include<unistd.h> //close()
#include<netinet/in.h>//struct sockaddr_in
#include<arpa/inet.h>//inet_ntoa

#define SOURCE_PORT 60002
#define QUEUE_LINE  1 //number of raspberry pis
#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
#define CHANNEL 3
#define Window 7
typedef unsigned char uchar;

typedef struct CLIENTLIST{
  int c_fd;
  unsigned short port;
  char *ip;
}cl;

int start_connection();
void ser_accept(int listenfd);
int ser_initialrecv();
void server_recv(char* allimages, int size);
void power_recv(char* currentpower, int size);