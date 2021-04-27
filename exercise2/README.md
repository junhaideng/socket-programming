### 代码编译
> 使用g++在Linux环境下进行编译即可



#### [client.cpp](client.cpp)

> 客户端

```bash
g++ client.cpp -o client
```



#### [server-single-thread.cpp](server-single-thread.cpp)

> 单线程式服务端

```bash
g++ server-single-thread.cpp -o server-single-thread
```



### [server-multiple-thread.cpp](server-multiple-thread.cpp)

> 多线程式服务端

```bash
# 需要链接pthread
g++ -lpthread server-multiple-thread.cpp -o server-multiple-thread 
```




<hr/>

对于wsl的安装，可以参见文档: https://docs.microsoft.com/en-us/windows/wsl/install-win10

对于Vscode中配置wsl，可以参见文档：https://code.visualstudio.com/docs/remote/wsl-tutorial

