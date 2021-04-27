#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>

using namespace std;

//----- HTTP response messages ----------------------------------------------
#define OK_IMAGE "HTTP/1.0 200 OK\r\nContent-Type:image/gif\r\n\r\n"
#define OK_TEXT "HTTP/1.0 200 OK\r\nContent-Type:text/html\r\n\r\n"
#define NOTOK_404 "HTTP/1.0 404 Not Found\r\nContent-Type:text/html\r\n\r\n"
#define FORBIDDEN_403 "HTTP/1.0 403 Forbidden\r\nContent-Type:text/html\r\n\r\n"
#define MESS_404 "<html><body><h1>FILE NOT FOUND</h1></body></html>"
#define MESS_403 "<html><body><h1>Forbidden</h1></body></html>"

#define DEFAULT_PORT "8000"
#define PORT 8000
#define QUEUE_SIZE 1024
#define DEFAULT_BUFLEN 1024

void *handle_get(void *arg)
{
  char inbuf[DEFAULT_BUFLEN];
  char outbuf[DEFAULT_BUFLEN];

  int fh; // file handle
  char command[DEFAULT_BUFLEN];
  char filename[DEFAULT_BUFLEN];

  int retcode; // return code
  int j;

  // 客户端的socket 描述符
  int clientfd = *(int *)arg;

  cout << "thread " << clientfd << endl;

  retcode = recv(clientfd, inbuf, sizeof(inbuf), 0);
  if (retcode <= 0)
  {
    cout << "ERROR - Receive failed --- probably due to dropped connection" << endl;
    close(clientfd);
    return NULL;
  }

  // 解析请求的命令
  sscanf(inbuf, "%s /%s \n", command, filename);

  cout << "command: " << command << " filename:" << filename << endl;

  if (strcmp(command, "GET") != 0)
  {
    cout << "ERROR - Not a GET --- received command = " << command << endl;
    close(clientfd);
    return NULL;
  }

  // 打开文件
  int fd = open(filename, O_RDONLY);
  if (fd < 0)
  {
    cout << "can not find file: " << filename << endl;
    memset(outbuf, 0, sizeof(outbuf));
    strcat(outbuf, NOTOK_404);
    strcat(outbuf, MESS_404);
    send(clientfd, outbuf, strlen(outbuf), 0);
    close(clientfd);
    return NULL;
  }

  // Check that filename does not start with a "..", "/", "\", or have a ":" in
  // the second position indicating a disk identifier (e.g., "c:").
  //  - This is a security check to prevent grabbing any file on the server
  if (((filename[0] == '.') && (filename[1] == '.')) ||
      (filename[0] == '/') || (filename[0] == '\\') ||
      (filename[1] == ':'))
  {
    cout << "SECURITY VIOLATION --- trying to read " << filename << endl;
    memset(&outbuf, 0, sizeof(outbuf));
    strcpy(outbuf, FORBIDDEN_403);
    send(clientfd, outbuf, sizeof(outbuf), 0);
    send(clientfd, MESS_403, sizeof(MESS_403), 0);
    close(fd);
    close(clientfd);
    return NULL;
  }

  cout << "Thread " << clientfd << " , sending file " << filename << endl;

  memset(outbuf, 0, sizeof(outbuf));

  if (strstr(filename, ".gif") != NULL)
  {
    strcat(outbuf, OK_IMAGE);
  }
  else
  {
    strcat(outbuf, OK_TEXT);
  }

  send(clientfd, outbuf, strlen(outbuf), 0);

  ssize_t n = 0;
  do
  {
    n = read(fd, outbuf, sizeof(outbuf));
    send(clientfd, outbuf, n, 0);
  } while (n > 0);

  close(fd);
  close(clientfd);
  cout << "socket closed" << endl;
  cout << "thread " << clientfd << " end" << endl;
}

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
    // 新建一个线程进行处理
    pthread_t ptid;
    pthread_create(&ptid, NULL, handle_get, (void *)&clientfd);
  }
  return 0;
}