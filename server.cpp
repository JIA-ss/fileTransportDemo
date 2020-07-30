#include "tools/include/tcp_socket.h"
#include "tools/include/file.h"
#include <string>
#include <string.h>

// 用这个函数的好处就是自动帮你关闭连接
int tcp_receive(int client_fd, void *buf, int64_t n) {
    auto ret = recv(client_fd, buf, n, 0); // 取一个 header 这么大的数据

    if (ret < 0) {
        // 连接被重置
        if (errno == ECONNRESET) {
            perror("ECONNRESET");
        } else if (errno == EAGAIN) {
            perror("EAGAIN");
        } else {
            perror("tcp_receive error");
        }
        close(client_fd);
        return -1;
    } else if (ret == 0) {
        // 客户端关闭连接
        close(client_fd);
        printf("client %d connection closed\n", client_fd);
        return -1;
    }
    return ret;
}

int main() {
    printf(" === === 服务器已开启 === ===   \n");
    printf("\t-> ip: %s\n\t-> port:%d\n",SERVER_IP_ADDR,SERVER_PORT);
    TcpSocket server_socket;
    server_socket.Socket();
    server_socket.Bind("127.0.0.1",SERVER_PORT);
    server_socket.Listen();
listenAgain:
    TcpSocket* client_socket = new TcpSocket;
    server_socket.Accept(*client_socket);
    printf("=== [recv start] === \n");
    char* head_package = new char[1024];
    bzero(head_package,1024);
    server_socket.Recv(head_package,1024);
    printf("head package content: %s\n",head_package);
    std::string head_p(head_package);
    int split_idx = head_p.find(' ');
    std::string filename = head_p.substr(0,split_idx);
    std::string num = head_p.substr(split_idx + 1,head_p.size());
    int filesz = std::stoll(num);
    printf("\t->filename: %s\n\t->filesize: %d\n",filename.c_str(),filesz);
    server_socket.SendBackFromServer(num.c_str(),num.size());
    server_socket.Listen();
    File_Opt* writeTo = new File_Opt(filename.c_str(),filesz);
    int ret = 0;
    off_t receieved = 0;
    while(1) {
        while (receieved < filesz) {
            ret = server_socket.Recv((char*)writeTo->mmap_addr + receieved,1024 * 10);
            if (ret < 0) {
                //printf("Error occurred!\n");
                break;
            }
            receieved += ret;
            printf("[recv]   -> (%d / %d)\n",ret,receieved);
            if (receieved == filesz) {
                printf("\t->recv complete ! \n");
                const char* ack = "over";
                if (server_socket.SendBackFromServer(ack,strlen(ack)));
                    printf("\t->ack(%s) succeed\n",ack);
                printf("=== === ===[recv end] === === === \n");
                receieved = 0;
                //goto listenAgain;
            }
            if (ret == 0)
                goto listenAgain;
            delete client_socket;
        }

    }
    return 0;
}