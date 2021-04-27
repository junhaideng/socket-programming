### 代码编译
> 使用g++在Linux环境下进行编译即可



#### [webclient.cpp](webclient.cpp)

> 客户端

```bash
g++ webclient.cpp -o webclient
```



#### [webserver.cpp](webserver.cpp)

> web服务端，使用多线程支持多用户同时请求，但是只返回特定内容

```bash
g++ webserver.cpp -o webserver
```



#### [webserver2.cpp](webserver2.cpp)

> web服务端，使用多线程支持多用户同时请求，可以访问本地文件

```bash
g++ webserver2.cpp -o webserver2
```




<hr/>

对于wsl的安装，可以参见文档: https://docs.microsoft.com/en-us/windows/wsl/install-win10

对于Vscode中配置wsl，可以参见文档：https://code.visualstudio.com/docs/remote/wsl-tutorial

