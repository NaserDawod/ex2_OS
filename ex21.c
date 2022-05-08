#include <stdio.h>
//#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

char read_char(int fd, int close_fd2) {
    char c = ' ';
    int check = 1;
    while (c == '\n' || c == ' '){
        check = read(fd, &c, 1);
        if (check < 0) {
            close(fd);
            close(close_fd2);
            exit(0);
        } else if(check == 0){
            return 0;
        }
    }
    return c;
}

int main(int argc, char** argv) {
    if(argc != 3){
        exit(2);
    }
    size_t check = 1, check2 = 1;
    int file1 = open(argv[1], 'r');
    if (file1 < 0){
        exit(2);
    }
    int file2 = open(argv[2], 'r');
    if (file2 < 0){
        close(file1);
        exit(2);
    }
    char c1, c2;
    int similar;
    do {
        c1 = read_char(file1, file2);
        c2 = read_char(file2, file1);
        if((c1 == 0 && c2 != 0) || (c1 != 0 && c2 == 0) || (c1 != c2 && abs(c1 - c2) != 32)) {
            close(file1);
            close(file2);
            return 2;
        } else if (abs(c1 - c2) == 32) {
            similar = 1;
        }
    }while (c1 != 0 && c2 != 0);
    close(file1);
    close(file2);
    if(similar == 1){
        return 3;
    }
    return 1;
}
