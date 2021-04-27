/*
  使用getaddrinfo获取主机对应的相关数据
*/
#include <iostream>
#include <netdb.h>
#include <string.h>
#include <iomanip>
#include <netdb.h>
#include <arpa/inet.h>

using std::cout;
using std::dec;
using std::endl;
using std::hex;

#define SIZE 128

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    cout << "usage: " << argv[0] << " <hostname> <servicename>" << endl;
    cout << "       provides protocol-independent translation" << endl;
    cout << "       from an ANSI host name to an IP address" << endl;
    cout << argv[0] << " example usage" << endl;
    cout << "    " << argv[0] << " www.contoso.com 0" << endl;
    return -1;
  }

  addrinfo hints, *res;
  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  int code = getaddrinfo(argv[1], argv[2], &hints, &res);
  if (code != 0)
  {
    cout << "getaddrinfo failed with error: " << gai_strerror(code) << endl;
    return -1;
  }
  // 第几个addrinfo
  int i = 1;
  // 保存转换成的ip地址
  sockaddr_in *ipv4;
  sockaddr_in6 *ipv6;

  char ipbuff[SIZE];

  for (addrinfo *p = res; p; p = p->ai_next)
  {
    cout << "getaddrinfo response " << i << endl;
    cout << "\tFlags: 0x" << hex << p->ai_flags << dec << endl;
    cout << "\tFamily: ";
    switch (p->ai_family)
    {
    case AF_UNSPEC:
      cout << "Unspecified" << endl;
      break;
    case AF_INET:
      cout << "AF_INET (IPv4)" << endl;
      ipv4 = (sockaddr_in *)p->ai_addr;
      cout << "\tIPv4 address " << inet_ntop(AF_INET, &ipv4->sin_addr, ipbuff, sizeof(ipbuff)) << endl;
      cout << "\tport = " << ntohs(ipv4->sin_port) << endl;
      break;
    case AF_INET6:
      cout << "AF_INET6 (IPv6)" << endl;
      ipv6 = (sockaddr_in6 *)p->ai_addr;
      cout << "\tIPv6 address " << inet_ntop(AF_INET6, &ipv6->sin6_addr, ipbuff, sizeof(ipbuff));
      cout << "\tport = " << ntohs(ipv6->sin6_port) << endl;
    case AF_NETBEUI:
      cout << "AF_NETBEUI (NetBEUI)" << endl;
      break;
    default:
      cout << "Other " << p->ai_family << endl;
      break;
    }

    // 判断socket类型
    cout << "\tSocket type: ";
    switch (p->ai_socktype)
    {
    case 0:
      cout << "Unspecified" << endl;
      break;
    case SOCK_STREAM:
      cout << "SOCK_STREAM (stream)" << endl;
      break;
    case SOCK_DGRAM:
      cout << "SOCK_DGRAM (datagram)" << endl;
      break;
    case SOCK_RAW:
      cout << "SOCK_RAW (raw)" << endl;
      break;
    case SOCK_RDM:
      cout << "SOCK_RDM (reliable message datagram)" << endl;
    case SOCK_SEQPACKET:
      cout << "SOCK_SEQPACKET (pseudo-stream packet)" << endl;
      break;
    default:
      cout << "Other " << p->ai_socktype << endl;
      break;
    }

    // 判断使用的协议
    cout << "\tProtocol: ";
    switch (p->ai_protocol)
    {
    case 0:
      cout << "Unspecified" << endl;
      break;
    case IPPROTO_TCP:
      cout << "IPPROTO_TCP (TCP)" << endl;
      break;
    case IPPROTO_UDP:
      cout << "IPPROTO_UDP (UDP)" << endl;
      break;
    default:
      cout << "Other " << p->ai_protocol << endl;
      break;
    }

    cout << "\tLength of this sockaddr: " << p->ai_addrlen << endl;
    // 使用cout 输出null的时候容易崩溃
    // 但是如果使用printf就不会
    // cout << "\tCanonical name: " << p->ai_canonname << endl;
    printf("\tCanonical name: %s\n", p->ai_canonname);
    i++;
  }
  return 0;
}