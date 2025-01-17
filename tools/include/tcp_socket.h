#pragma once

#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <stdint.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include "../../etc/config.h"



inline bool CHECK_RET(int32_t res, const char *errMsg) {
  if (res >= 0)
    return true;
  perror(errMsg);
  return false;
}

/*
 * @description: 对socket函数进行封装 : ipv4 tcp协议
 *               一个fd对应一个类实例，服务端和客户端通用
 * @说明: const 类型参数为传入参数
 *        *     指针类型参数为传出参数
 *        &     非const引用类型为传入传出参数
 */

class TcpSocket {
public:
  TcpSocket(int32_t fd = -1) : _fd(fd) {}

  ~TcpSocket() { Close(); }

  bool Socket() {
    _fd = socket(AF_INET, SOCK_STREAM, 0);

    int32_t snd_size = SOCKET_SENT_BUFFER; // 4M
    int32_t rcv_size = SOCKET_REC_BUFFER;
    if (setsockopt(_fd, SOL_SOCKET, SO_SNDBUF, &snd_size, sizeof(socklen_t))) {
      std::cerr << "设置发送缓冲区大小错误\n";
    }

    if (setsockopt(_fd, SOL_SOCKET, SO_RCVBUF, &rcv_size, sizeof(socklen_t))) {
      std::cerr << "设置接收缓冲区大小错误\n";
    }

    return CHECK_RET(_fd, "socket error!");
  }

  bool Close() {
    if (_fd < 0)
      return false;
    int32_t res = close(_fd);
    _fd = -1;
    return CHECK_RET(res, "close error!!");
  }

  bool Bind(const std::string &ip, const uint16_t port) {
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = 0; //std::move(inet_addr(ip.c_str()));
    addr.sin_port = std::move(htons(port));
    int res = bind(_fd, (sockaddr *)&addr, sizeof(addr));
    return CHECK_RET(res, "bind error!");
  }

  bool Listen(const int32_t backlog = 20) {
    int32_t res = listen(_fd, backlog);
    return CHECK_RET(res, "listen error!");
  }

  bool Accept(TcpSocket &peer, std::string *ip = nullptr,
              uint16_t *port = nullptr) {
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int32_t new_sock = accept(_fd, (sockaddr *)&addr, &len);
    if (new_sock < 0) {
      perror("accept error!");
      return false;
    }
    peer.SetFd(new_sock);
    conn_fd = new_sock;
    if (ip != nullptr)
      *ip = inet_ntoa(addr.sin_addr);
    if (port != nullptr)
      *port = ntohs(addr.sin_port);
    return true;
  }

  bool Send(const void *buf, const ssize_t size) {
    printf("马上开始发！！   buf is %s  and size is %d\n",buf,size);
    size_t bytes_need_to_send = size;
    while (bytes_need_to_send > 0) {
      ssize_t res = send(_fd, buf, bytes_need_to_send, 0);
      printf("res == %d\n",res);
      if (!CHECK_RET(res, "sendfile error!!"))
        return false;
      bytes_need_to_send -= res;
    }
    return true;
  }

  bool SendBackFromServer(const void* buf, const ssize_t size) {
      size_t bytes_need_to_send = size;
      while (bytes_need_to_send > 0) {
          ssize_t res = send(conn_fd, buf, bytes_need_to_send, 0);
          if (!CHECK_RET(res, "sendfile error!!"))
              return false;
          bytes_need_to_send -= res;
      }
      return true;
  }
/*
  bool SendFile(int disk_id, const int32_t file_fd, off_t *offset,
                const size_t size) {
    size_t bytes_need_to_send = size;
    while (bytes_need_to_send > 0) {
      ssize_t res = sendfile64(_fd, file_fd, offset, bytes_need_to_send);
      printf("=== THREAD %d SENDING %ld / %zu===\n", disk_id, *offset,
             bytes_need_to_send);
      if (!CHECK_RET(res, "sendfile error!!"))
        return false;
      bytes_need_to_send -= res;
    }
    return true;
  }
*/
  bool Connect(const std::string &ip, const uint16_t port) {
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip.c_str());
    addr.sin_port = htons(port);
    int32_t res = connect(_fd, (sockaddr *)&addr, sizeof(addr));
    return CHECK_RET(res, "connect error!");
  }
/*
  bool SendFile(int disk_id, const int32_t file_fd, off_t *offset,
                const size_t size) {
    size_t bytes_need_to_send = size;
    while (bytes_need_to_send > 0) {
      ssize_t res = sendfile64(_fd, file_fd, offset, bytes_need_to_send);
      printf("=== THREAD %d SENDING %ld / %zu===\n", disk_id, *offset,
             bytes_need_to_send);
      if (!CHECK_RET(res, "sendfile error!!"))
        return false;
      bytes_need_to_send -= res;
    }
    return true;
  }
*/
  ssize_t Recv(void *buf, const size_t size) {
    ssize_t len = recv(conn_fd, buf, size, 0);
    if (len < 0)
      //std::cerr << "recv error!\n";
    if (len == 0)
      std::cerr << "recv return 0!\n";
    return len;
  }

  void SetFd(const int fd) { _fd = fd; }

  int32_t GetFd() const { return _fd; }

  int32_t _fd;
private:
  int32_t conn_fd;
};
