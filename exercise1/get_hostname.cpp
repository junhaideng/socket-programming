/*
  使用getnameinfo对给定的ip地址进行解析获取到对应的主机名
*/
#include <iostream>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>

using std::cout;
using std::endl;

int main(int argc, char *argv[])
{
  // 检验参数合理性
  if (argc != 2)
  {
    cout << "usage:" << argv[0] << " IPv4 address" << endl;
    cout << "  to return local hostname" << endl;
    cout << "       " << argv[0] << " 127.0.0.1" << endl;
    return 1;
  }

  // 套接字地址
  sockaddr_in sa;
  u_short port = 80;
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);

  // 将输入的ip地址转换成对应的网络地址
  if (inet_pton(AF_INET, argv[1], &sa.sin_addr) == -1)
  {
    cout << "convert ip address to network format failed" << endl;
    return -1;
  }

  char host[NI_MAXHOST];
  char service[NI_MAXSERV];
  memset(&host, 0, NI_MAXHOST);
  // 获取sockaddr对应的相关信息
  int code = getnameinfo((sockaddr *)&sa, sizeof(sa), host, sizeof(host), service, sizeof(service), NI_NAMEREQD);

  if (code != 0)
  {
    cout << "get name info failed: " << gai_strerror(code) << endl;
    return -1;
  }
  cout << "getnameinfo returned hostname = " << host << endl;
  cout << "getnameinfo returned service = " << service << endl;
}

// 在进行测试的时候，函数的调用都没有问题，但是发现getnameinfo返回的主机号一直是对应的ip地址
// 刚开始的时候认为是错误的，后来才发现应该是remote地址没有对应的主机号，所以返回了ip地址
// 当然我们可以在getnameinfo指定 NI_NAMEREQD，如果没找到对应的主机号，则返回错误码