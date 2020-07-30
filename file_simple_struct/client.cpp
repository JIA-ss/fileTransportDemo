#include "clientUpload.h"


int main (int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    }
    const char* use__= "/Users/sjs/Documents/git_Pro/file_simple_struct/test.file";
    printf(" === === 客户端已开启 === === \n");
    int fd = open(use__,O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
    struct stat file_state;
    if (fstat(fd,&file_state) < 0) {
        perror("fstat error:");
        close(fd);
        return 0;
    }
    uint64_t file_size = file_state.st_size;
    printf("[info]:\n\t->filename: %s\n\t->fd: %d\n\t->filesize: %d  \n",use__,fd,file_size);
    do_upload(fd,use__,file_size);

    return 0;
}