#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

#define DEFAULT_PORT "8000"
#define PORT 8000
#define QUEUE_SIZE 1024
#define DEFAULT_BUFLEN 1024

int main()
{
  // 我们需要监听80端口
  addrinfo hints, *listp;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  int code;

  code = getaddrinfo(NULL, DEFAULT_PORT, &hints, &listp);
  if (code < 0)
  {
    cout << "getaddrinfo failed with error: " << gai_strerror(code) << endl;
    return -1;
  }
  // 服务端套接字
  int sockfd;

  sockaddr_in addr;
  addr.sin_port = htons(PORT);
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  int counter = 0;
  // 遍历进行监听
  for (addrinfo *p = listp; p; p = p->ai_next)
  {
    counter++;
    cout << "try address [" << counter << "]" << endl;
    // 创建socket
    sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sockfd < 0)
    {
      cout << "create socket error" << endl;
      continue;
    }

    code = bind(sockfd, (sockaddr *)&addr, sizeof(addr));
    if (code < 0)
    {
      cout << "bind socket error" << endl;
      continue;
    }
    // 监听
    code = listen(sockfd, QUEUE_SIZE);
    if (code == -1)
    {
      cout << "listen socket error" << endl;
      continue;
    }
    break;
  }
  if (code < 0)
  {
    cout << "can not listen socket" << endl;
    return -1;
  }
  cout << "listen socket successfully" << endl;

  char recvbuf[DEFAULT_BUFLEN];
  char sendbuf[DEFAULT_BUFLEN];

  while (1)
  {
    int clientfd = accept(sockfd, NULL, 0);
    if (clientfd < 0)
    {
      cout << "accpet error" << endl;
      continue;
    }
    memset(&recvbuf, 0, sizeof(recvbuf));
    code = recv(clientfd, recvbuf, sizeof(recvbuf), 0);
    if (code < 0)
    {
      cout << "recv error" << endl;
      continue;
    }
    // 如果没有指定相应行出现不了正确的效果
    strcpy(sendbuf, "HTTP/1.1 200 OK\r\n\r\n<html><body><hr>This is a response <b>message</b> in HTML format. <font color=red>Wow!</font><hr></body></html>");
    send(clientfd, sendbuf, strlen(sendbuf), 0);
    close(clientfd);
  }
  return 0;
}