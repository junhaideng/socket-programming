### 代码编译
> 使用g++在Linux环境下进行编译即可

### [client.cpp](client.cpp)

> 客户端

```bash
# 需要链接pthread
g++ client.cpp -o client -lpthread
```

### [server.cpp](server.cpp)

```bash
# 需要链接pthread
g++ server.cpp -o client -lpthread
```




<hr/>

对于wsl的安装，可以参见文档: https://docs.microsoft.com/en-us/windows/wsl/install-win10

对于Vscode中配置wsl，可以参见文档：https://code.visualstudio.com/docs/remote/wsl-tutorial

