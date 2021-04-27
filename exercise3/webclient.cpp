#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

using namespace std;

#define DEFAULT_PORT "80"
#define SIZE 1024

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    cout << "usage: " << argv[0] << " host" << endl;
    return 1;
  }

  addrinfo hints, *listp;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_ADDRCONFIG;

  int code;

  code = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &listp);
  if (code != 0)
  {
    cout << "getaddrinfo failed with error: " << gai_strerror(code) << endl;
    return -1;
  }

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
    cout << "create socket failed" << endl;
    return -1;
  }
  int counter = 0;
  // 遍历所有的addrinfo进行连接
  for (addrinfo *p = listp; p; p = p->ai_next)
  {
    counter++;
    cout << "try address [" << counter << "]" << endl;

    // 然后进行连接
    code = connect(sockfd, (sockaddr *)p->ai_addr, p->ai_addrlen);
    if (code == -1)
    {
      cout << "connect server failed, try again" << endl;
      continue;
    }
    cout << "connect server successfully" << endl;
    break;
  }
  freeaddrinfo(listp);
  if (code == -1)
  {
    cout << "can not connect the server" << endl;
    return -1;
  }
  //  碰到的问题: \n\n 可以生效，但是\r\n不能，还需要在后面添加一个\r\n
  // 发送HTTP请求
  char sendbuf[SIZE] = "GET / HTTP/1.1\r\n";

  strcat(sendbuf, "Host: ");
  strcat(sendbuf, argv[1]);
  strcat(sendbuf, "\r\n");

  strcat(sendbuf, "Accept: */*\r\n");
  strcat(sendbuf, "Connection: keep-alive\r\n");

  strcat(sendbuf, "\r\n");
  cout << "send msg: "<<sendbuf<<endl;
  code = send(sockfd, sendbuf, strlen(sendbuf), 0);
  if (code == -1)
  {
    cout << "send HTTP request failed" << endl;
    return -1;
  }
  cout << "send HTTP request successfully" << endl;

  char recvbuf[SIZE];
  do
  {
    memset(&recvbuf, 0, sizeof(recvbuf));
    code = recv(sockfd, recvbuf, sizeof(recvbuf), 0);
    if (code > 0)
    {
      cout << "recv " << code << " (bytes)" << endl;
      cout << recvbuf << endl;
    }
    else if (code == 0)
    {
      cout << "connection closed" << endl;
    }
    else
    {
      cout << "recv failed" << endl;
    }
  } while (code > 0);

  close(sockfd);
  return 0;
}