#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>

using namespace std;

#define BUFF_LEN 1024

//计算网际效验和函数 pBuf指向ICMP数据开始的地方
uint16_t checksum(uint16_t *pBuf, int iSize)
{
  uint32_t cksum = 0;
  while (iSize > 1)
  {
    cksum += *pBuf++;
    iSize -= sizeof(uint16_t);
  }
  if (iSize)
  {
    cksum += *(uint16_t *)pBuf;
  }
  cksum = (cksum >> 16) + (cksum & 0xffff);
  cksum += (cksum >> 16);
  return (uint16_t)(~cksum);
}

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    cout << "usage: " << argv[0] << " host" << endl;
    return -1;
  }

  int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (sockfd < 0)
  {
    cout << "create socket failed" << endl;
    return -1;
  }
  cout << "create socket successfully" << endl;

  addrinfo hints, *listp;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;

  int code;

  code = getaddrinfo(argv[1], NULL, &hints, &listp);
  if (code < 0)
  {
    cout << "getaddrinfo failed with error: " << gai_strerror(code) << endl;
    return -1;
  }

  // 主机对应的地址
  sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;

  // 获取到主机号所对应的ip地址
  bool flag = true;
  for (addrinfo *p = listp; p && flag; p = p->ai_next)
  {
    switch (p->ai_family)
    {
    case AF_INET:
      addr = *(sockaddr_in *)p->ai_addr;
      flag = false;
      break;
    }
  }

  // Type(8) Code(8) CheckSum(16)
  // Identifier(8)  Sequence(8)
  // data
  // 该开始checksum为0
  uint8_t buff[] = {0x8, 0, 0, 0, 0, 1, 0, 0xff, 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i'};
  uint16_t sum = htons(checksum((uint16_t *)&buff, 40));

  // 设置checksum
  buff[2] = sum >> 8;
  buff[3] = uint8_t(sum & 0xff);
  char tmp[BUFF_LEN];
  memset(&tmp, 0, sizeof(tmp));
  cout << "Pinging " << inet_ntop(AF_INET, &addr.sin_addr, tmp, sizeof(tmp)) << " with 32 bytes of data:" << endl;

  unsigned char recvbuf[BUFF_LEN];
  // 数据发送时间
  timeval start;
  // 数据接收到的时间
  timeval end;
  while (1)
  {
    // 获取当前时间
    gettimeofday(&start, NULL);
    // 发送数据
    sendto(sockfd, buff, 40, 0, (sockaddr *)&addr, sizeof(addr));
    cout << "Reply from " << tmp << ": ";

    memset(&recvbuf, 0, sizeof(recvbuf));
    // 接收数据
    recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0, NULL, 0);
    // recvbuf中的第三和第四位确定包的长度
    cout << "bytes=" << recvbuf[2] * 16 * 16 + recvbuf[3] - 28;
    gettimeofday(&end, NULL);
    cout << " time=" << (end.tv_usec - start.tv_usec) / 1000 << "ms";
    // 第九个字节确定TTL值
    printf(" TTL=%u\n", recvbuf[8]);
    sleep(1);
  }
}

// mention: 需要使用root命令进行运行