## 1	创建socket

```c
#incldue <sys/types.h>
#include <sys/socket.h>
//成功返回一个socket文件描述符，失败-1设置errno
int socket(int domain,int type,int protocol);
//domain 底层协议族	PF_INET or PF_INET6
//type 	服务类型	SOCK_STREAM	SOCK_UGRAM
//protocol	几乎所有情况下，都应为0
```

## 2	命名socket

```c
#incldue <sys/types.h>
#include <sys/socket.h>
//成功0	失败-1设置errno
int bind(int sockfd,const struct sockaddr* my_addr,socklen_t addrlen);
//绑定服务器sockaddr
```

## 3	监听socket

```c
#include <sys/socket.h>
int listen(int sockfd,int backlog);
//sockfd指定被监听的socket
//backlog提示内核监听队列的最大长度
```

## 4	接收连接

```c
#incldue <sys/types.h>
#include <sys/socket.h>

int accept(int sockfd,struct sockaddr* addr,socklen_t* addrlen);
```

## 5	发起连接

```c
#incldue <sys/types.h>
#include <sys/socket.h>
//客户端主动连接服务器
int connect(int sockfd,const struct sockaddr* serv_addr,socklen_t addrlen);
```

## 6	关闭连接

```c
#include <unistd.h>
//将fd引用计数-1，fd引用计数为0才关闭连接
int close(int fd);

//如果无论如何要立刻终止连接
#include <sys/socket.h>
int shutdown(int sockfd,int howto);
```

![image-20220622182302902](../image/image-20220622182302902.png)

## 7	例子

### 7.1	服务器

```cpp
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#define BUFSIZE 1024
int main(int argc,char *argv[]){
    if(argc <=2){
        printf("usage: %s ip port\n",basename(argv[0]));
        return 1;
    }
    const char* ip=argv[1];
    int port=atoi(argv[2]);

    struct sockaddr_in addr;
    bzero(&addr,sizeof(addr));
    addr.sin_family=AF_INET;
    inet_pton(AF_INET,ip,&addr.sin_addr);
    addr.sin_port=htons(port);

    int sock=socket(PF_INET,SOCK_STREAM,0);
    assert(sock>=0);

    int ret = bind(sock,(struct sockaddr*)&addr,sizeof(addr));
    assert(ret!=-1);
    
    ret = listen(sock,5);
    assert(ret!=-1);
    struct sockaddr_in client;
    socklen_t clientlen=sizeof(client);
    int connfd=accept(sock,(struct sockaddr*)&client,&clientlen);
    if(connfd<0){
        printf("errno is %d\n",errno);
    }else{
        char buffer[BUFSIZE];
        memset(buffer,'\0',BUFSIZE);
        ret = recv(connfd,buffer,BUFSIZE-1,0);
        printf("got %d bytes of normal data '%s'\n",ret,buffer);
    
        memset(buffer,'\0',BUFSIZE);
        ret = recv(connfd,buffer,BUFSIZE-1,MSG_OOB);
        printf("got %d bytes of oob data '%s'\n",ret,buffer);
        
        memset(buffer,'\0',BUFSIZE);
        ret = recv(connfd,buffer,BUFSIZE-1,0); 
        printf("got %d bytes of normal data '%s'\n",ret,buffer);
        close(connfd);

    }
    close(sock);
    return 0;

}
```

### 7.2	客户端

```cpp
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

int main(int argc,char *argv[]){
    if(argc <=2){
        printf("usage: %s ip port\n",basename(argv[0]));
        return 1;
    }
    const char* ip=argv[1];//服务器ip
    int port=atoi(argv[2]);//服务器端口号

    struct sockaddr_in serveraddr;
    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    inet_pton(AF_INET,ip,&serveraddr.sin_addr);
    serveraddr.sin_port=htons(port);

    int sock=socket(PF_INET,SOCK_STREAM,0);
    assert(sock>=0);
    if(connect(sock,(struct sockaddr*)&serveraddr,sizeof(serveraddr))<0){
        printf("connection failed\n");

    }
    else{
        const char* oob_data="abcwhat a oob data!";
        const char* normal_data="123";
        send(sock,normal_data,strlen(normal_data),0);
        sleep(3);
        send(sock,oob_data,strlen(oob_data),MSG_OOB);
        sleep(3);
        send(sock,normal_data,strlen(normal_data),0);
    }
    close(sock);
    return 0;
}
```

