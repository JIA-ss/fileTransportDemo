#include "../include/clientUpload.h"
#include "../../tools/include/file.h"
#include "../../etc/config.h"

int do_upload(int fd, const char* filename, const uint64_t filesize) {
    TcpSocket socket_fd = TcpSocket();
    socket_fd.Socket(); //初始化socket
    socket_fd.Connect(SERVER_IP_ADDR,SERVER_PORT);
    thr_start(filename,fd,filesize);
    return 1;
}


void *thr_start(const char* file_name, int fd,const uint64_t real_block_size) {

  // socket init
  TcpSocket socket_fd = TcpSocket();
  socket_fd.Socket();

  socket_fd.Connect(SERVER_IP_ADDR, SERVER_PORT);

  // send file block
  File_Opt file_content(fd,real_block_size);
  if (!(socket_fd.Send(file_content.read_all(),real_block_size))) {
      perror("send file error");
  }
  std::cout << " === SEND  ===" << std::endl;

  // wait server close
  char readBuffer[ACK_SIZE];
  ssize_t read_size = socket_fd.Recv(readBuffer, ACK_SIZE);
  if (read_size == 0) {
    std::cout << "Transfer complete!\n";
  } else {
    std::cout << "UNKNOWN MESSAGE FROM SERVER! Length: " << read_size << "\n";
  }
  socket_fd.Close();
  return nullptr;
}
