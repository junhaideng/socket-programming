#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

using std::cin;
using std::cout;
using std::endl;

#define BUFF_SIZE 1024
#define QUEUE_SIZE 1024
#define PORT "27015"

int main(int argc, char *argv[])
{

  addrinfo hints, *listp;
  // 清零
  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_INET;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  int code;
  if ((code = getaddrinfo(NULL, PORT, &hints, &listp)) < 0)
  {
    cout << "getaddrinfo failed with error: " << gai_strerror(code) << endl;
    return -1;
  }

  // socket描述符
  int sockfd;

  for (addrinfo *p = listp; p; p = p->ai_next)
  {
    // 首先创建socket
    sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sockfd < 0)
    {
      continue;
    }

    // 然后绑定socket
    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
    {
      cout << "bind socket failed" << endl;
      continue;
    }

    // 监听当前的端口
    if (listen(sockfd, QUEUE_SIZE) == 0)
    {
      cout << "connect socket successfully" << endl;
      break;
    }
    else
    {
      cout << "listen failed, close socket" << endl;
      close(sockfd);
      sockfd = -1;
    }
  }
  freeaddrinfo(listp);

  sockaddr_in client_addr;
  socklen_t length = sizeof(client_addr);

  cout << "accpet client...." << endl;
  // client_addr 是用户端的地址
  int conn = accept(sockfd, (sockaddr *)&client_addr, &length);
  if (conn < 0)
  {
    return -1;
  }

  char buff[BUFF_SIZE];
  // 接受客户端的情况
  while (true)
  {
    memset(buff, 0, sizeof(buff));
    int n = recv(conn, buff, sizeof(buff), 0);
    if (n <= 0)
    {
      break;
    }
    cout << "Recieved: " << buff << " (" << n << " bytes)" << endl;
    send(conn, buff, n, 0);
  }

  return 0;
}