#include "clientUpload.h"
#include <string>
#include <string.h>

int do_upload(int fd, const char* filename, const uint64_t filesize) {
    //TcpSocket socket_fd = TcpSocket();
    //socket_fd.Socket(); //初始化socket
    //socket_fd.Connect(SERVER_IP_ADDR,SERVER_PORT);
    thr_start(filename,fd,filesize);
    return 1;
}


void *thr_start(const char* file_name, int fd,const uint64_t real_block_size) {

  // socket init
  TcpSocket socket_fd = TcpSocket();
  socket_fd.Socket();

  if (socket_fd.Connect(SERVER_IP_ADDR, SERVER_PORT)) {
      printf("=== connect succeed ! ===\n");
  } else {
      printf("connect failed !\n");
  }

  //send head package

  std::cout << " === SEND HEAD START===" << std::endl;
  std::string head_package = std::string("test") + " ";
  head_package += std::to_string(real_block_size) + " ";
  std::cout <<"head package content :" << head_package << std::endl;
  std::cout <<"head package size :" << head_package.size() << std::endl;
  if (socket_fd.Send(head_package.c_str(),head_package.size()))
      std::cout<< "head packege send succeed !! " << std::endl ;
  else{
      std::cout << "head package send failed !!" << std::endl;
      return nullptr;
  }

  // wait for ack from server

  char* head_ack_buffer = new char[10];
  bzero(head_ack_buffer,10);
  ssize_t read_size_head_ack = recv(socket_fd._fd,head_ack_buffer,10,0);//socket_fd.RecvFromServer(readBuffer, 10);
  printf("ack is :%s\n",head_ack_buffer);
  if (read_size_head_ack == 0) {
      return nullptr;
  }


  std::cout << " === SEND HEAD COMPLETE AND RECIEVED ACK===\n == SEND BODY == " << std::endl;
  // send file block
  File_Opt file_content(fd,real_block_size);
  const char* filecontent =file_content.read_all();
  if (!(socket_fd.Send(filecontent,real_block_size))) {
      perror("send file error");
  }


  // wait server close
  char* readBuffer = new char[10];
  bzero(readBuffer,10);
  ssize_t read_size = recv(socket_fd._fd,readBuffer,10,0);//socket_fd.RecvFromServer(readBuffer, 10);

  if (read_size == 0) {
    std::cout << "Transfer complete!\n";
  } else {
    std::cout << "UNKNOWN MESSAGE FROM SERVER! Length: " << read_size << "\n";
  }
  printf("rec from server is : %s \n", readBuffer);
  socket_fd.Close();
  return nullptr;
}
