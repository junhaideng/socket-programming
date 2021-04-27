#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

using namespace std;

#define PORT 8888
#define BUFF_SIZE 1024

void *recv_msg(void *arg)
{
  int fd = *(int *)arg;
  char buff[BUFF_SIZE];
  while (1)
  {
    memset(&buff, 0, sizeof(buff));
    ssize_t n = recvfrom(fd, buff, sizeof(buff), 0, NULL, 0);
    cout << "server receive: " << buff << endl;
  }
}

int main()
{
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
  {
    cout << "create socket failed" << endl;
    return -1;
  }

  sockaddr_in addr;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(PORT);
  addr.sin_family = AF_INET;

  int code = 0;
  code = bind(sockfd, (sockaddr *)&addr, sizeof(addr));
  if (code < 0)
  {
    cout << "bind socket failed" << endl;
    return -1;
  }

  char buff[BUFF_SIZE];
  sockaddr_in clientaddr;

  // 首先获取到另一个用户的地址
  memset(&buff, 0, sizeof(buff));
  memset(&clientaddr, 0, sizeof(clientaddr));
  socklen_t length = sizeof(clientaddr);
  cout << "waiting..." << endl;
  code = recvfrom(sockfd, buff, sizeof(buff), 0, (sockaddr *)&clientaddr, &length);
  
  if (code == -1)
  {
    cout << "recvfrom failed: " << code << endl;
    return -1;
  }
  char tmp[128];
  inet_ntop(AF_INET, &clientaddr.sin_addr, tmp, sizeof(tmp));
  cout << "client ip address: " << inet_ntop(AF_INET, &clientaddr.sin_addr, tmp, sizeof(tmp)) << endl;
  cout << "server receive: " << buff << endl;

  // 开启子线程用来接收用户的数据
  pthread_t ptid;
  pthread_create(&ptid, NULL, recv_msg, (void *)&sockfd);

  while (1)
  {
    memset(&buff, 0, sizeof(buff));
    cin.getline(buff, sizeof(buff));
    cin.clear();
    if (strlen(buff) == 0)
    {
      continue;
    }
    code = sendto(sockfd, buff, strlen(buff), 0, (sockaddr *)&clientaddr, length);
    if (code == -1)
    {
      cout << "send message failed: " << code << endl;
      continue;
    }
  }
}