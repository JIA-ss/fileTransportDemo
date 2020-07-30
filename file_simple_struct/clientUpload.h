#ifndef __CLIENT_UPLOAD__
#define __CLIENT_UPLOAD__
#include <iostream>

#include<sys/types.h>   //open()
#include<sys/stat.h>    //open()
#include<fcntl.h>       //open()

#include<stdio.h>   //perror

#include "tcp_socket.h"
#include "file.h"
#include "config.h"


int do_upload(int fd, const char* filename, const uint64_t filesize);

void *thr_start(const char* file_name, int fd,const uint64_t real_block_size); 


#endif //__CLIENT_UPLOAD__
