#include "../include/clientUpload.h"


int main (int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]); 
        return 0;
    }
    printf("start running !\n");
    int fd = open(argv[1],O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
    printf("open succeed ! fd is : %d \n",fd);
    struct stat file_state;
    if (fstat(fd,&file_state) < 0) {
        perror("fstat error:");
        close(fd);
        return 0;
    }
    uint64_t file_size = file_state.st_size;
    printf("%s: file size is :",argv[1]);
    do_upload(fd,argv[1],file_size);

    return 0;
}