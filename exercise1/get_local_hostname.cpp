/*
  获取本地主机号
  需要包含 unistd.h 
*/
#include <iostream>
#include <unistd.h>

#define SIZE 128

using std::cout;
using std::endl;
int main(int argc, char *args[])
{
  char hostname[SIZE];
  // 调用gethostname获取本地用户的hostname
  int code = gethostname(hostname, sizeof(hostname));
  if (code != 0)
  {
    cout << "get hostname failed" << endl;
    return -1;
  }
  cout << hostname << endl;
}