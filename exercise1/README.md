### 代码编译
> 使用g++在Linux环境下进行编译即可



#### [get_local_hostname.cpp](get_local_hostname.cpp)
> 获取本地的主机名称

```bash
# 进行编译
g++ get_local_hostname.cpp -o get_local_hostname
# 执行代码
./get_local_hostname
```



#### [get_hostname.cpp](get_hostname.cpp)

> 获取IP地址对应的主机号
```bash
# 进行编译
g++ get_hostname.cpp -o get_hostname
# 执行代码
./get_hostname IPv4Addr
```



#### [get_host_addr.cpp](get_host_addr.cpp)

> 获取主机对应的地址信息

```bash
# 进行编译
g++ get_host_addr.cpp -o get_host_addr
# 执行代码
./get_host_addr hostname service
```



<hr/>

对于wsl的安装，可以参见文档: https://docs.microsoft.com/en-us/windows/wsl/install-win10

对于Vscode中配置wsl，可以参见文档：https://code.visualstudio.com/docs/remote/wsl-tutorial

