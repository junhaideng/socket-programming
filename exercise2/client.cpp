#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

using std::cin;
using std::cout;
using std::endl;

#define MAXSIZE 1024

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    cout << "usage " << argv[0] << " host service" << endl;
    return -1;
  }

  addrinfo hint, *listp;
  memset(&hint, 0, sizeof(hint));

  hint.ai_family = AF_INET;
  hint.ai_socktype = SOCK_STREAM;
  hint.ai_protocol = IPPROTO_TCP;

  int code;
  if ((code = getaddrinfo(argv[1], argv[2], &hint, &listp)) < 0)
  {
    cout << "getaddrinfo failed with error: " << gai_strerror(code) << endl;
    return -1;
  }

  // socket 描述符
  int sockfd = -1;
  int count = 0;
  for (addrinfo *p = listp; p; p = p->ai_next)
  {
    count++;
    cout << "try to connect address [" << count << "]" << endl;

    sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

    if (sockfd < 0)
    {
      continue;
    }

    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == 0)
    {
      cout << "connect socket successfully" << endl;
      break;
    }
    else
    {
      cout << "connect address [" << count << "] failed" << endl;
      close(sockfd);

      sockfd = -1;
    }
  }

  // 清除掉所有的数据
  freeaddrinfo(listp);

  if (sockfd < 0)
  {
    cout << "create socket and connect failed" << endl;
    return -1;
  }

  // 发送的数据
  char sendbuf[MAXSIZE];
  char recvbuf[MAXSIZE];

  while (true)
  {
    memset(sendbuf, 0, sizeof(sendbuf));
    // 获取到用户的输入
    cin >> sendbuf;
    if (strcmp("quit", sendbuf) == 0)
    {
      break;
    }
    // 发送数据
    send(sockfd, sendbuf, int(strlen(sendbuf) + 1), 0);

    // 接受数据
    int n = recv(sockfd, recvbuf, MAXSIZE, 0);
    if (n <= 0)
    {
      close(sockfd);
    }
    cout << "Received: " << recvbuf << endl;
  }

  return 0;
}