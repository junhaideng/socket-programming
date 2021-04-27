#include<iostream>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<arpa/inet.h>

using namespace std;

#define BUFF_SIZE 1024

void *recv_msg(void *arg){
  int fd = *(int*)arg;
  char buff[BUFF_SIZE];
  while(1){
    memset(&buff, 0, sizeof(buff));
    ssize_t n = recvfrom(fd, buff, sizeof(buff), 0, NULL, 0);
    cout <<"client receive: "<< buff<<endl;
  }
}

int main(int argc, char *argv[]){
  if(argc !=3){
    cout << "usage: "<<argv[0]<< " ip port"<<endl;
    return -1;
  }

  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockfd < 0){
    cout << "create socket error"<<endl;
    return -1;
  }


  sockaddr_in addr;
  if(inet_pton(AF_INET, argv[1], &addr.sin_addr)<0){
    cout << "invalid ip address: "<< argv[1]<<endl;
    return -1;
  }
  addr.sin_family = AF_INET;
  addr.sin_port = htons(stoi(argv[2]));

  char buff[BUFF_SIZE];

  // 开启一个新的线程监听另一方发送消息过来
  pthread_t ptid;
  pthread_create(&ptid, NULL, recv_msg, (void*)&sockfd);
  cout << "connect to "<< argv[1] << ":"<<argv[2] << " successfully"<<endl;
  while(1){
    memset(buff, 0, sizeof(buff));
    cin.getline(buff, sizeof(buff));
    cin.clear();
    if(strlen(buff)==0){
      continue;
    }
    // cin >> buff;
    sendto(sockfd, buff, strlen(buff), 0, (sockaddr*)&addr, sizeof(addr));
  }
}