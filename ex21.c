#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if(argc != 3){
        exit(-1);
    }
    size_t check = 1, check2 = 1;
    int file1 = open(argv[1], 'r');
    if (file1 < 0){
        exit(-1);
    }
    int file2 = open(argv[2], 'r');
    if (file2 < 0){
        close(file1);
        exit(-1);
    }
    char c1, c2;
    int similar = 0;
    while (1) {
        check = read(file1, &c1, 1);
        if (check < 0) {
            close(file1);
            close(file2);
            exit(-1);
        }
        check2 = read(file2, &c2, 1);
        if (check2 < 0) {
            close(file1);
            close(file2);
            exit(-1);
        }
        if (check == 0 && check2 == 0){
            break;
        }
        if ((check == 0 && c2 != ' ' && c2 != '\n' ) || (check2 == 0 && c1 != ' ' && c1 != '\n') || (c1 != c2 && abs(c1 - c2) != 32
                                                                                                     && c1 != ' ' && c1 != '\n' && c2 != ' ' && c2 != '\n')) {
            close(file1);
            close(file2);
            return 2;
        }
        if ((c1 == ' ' && c2 != ' ') || (c1 != ' ' && c2 == ' ') || (c1 == '\n' && c2 != '\n') || (c1 != '\n' && c2 == '\n')
            || (abs(c1 - c2) == 32)) {
            similar = 1;
            if (c1 == '\n' || c1 == ' ') {
                while (c1 == '\n' || c1 == ' '){
                    check = read(file1, &c1, 1);
                    if (check < 0) {
                        close(file1);
                        close(file2);
                        exit(-1);
                    } else if(check == 0) {
                        break;
                    }
                }
            }
            if (c2 == '\n' || c2 == ' ') {
                while (c2 == '\n' || c2 == ' '){
                    check = read(file2, &c2, 1);
                    if (check < 0) {
                        close(file1);
                        close(file2);
                        exit(-1);
                    } else if(check == 0) {
                        break;
                    }
                }
            }
            if (check != 0 && check2 != 0 && c1 != c2 && abs(c1 - c2) != 32){
                return 2;
            }
        }
    }
    close(file1);
    close(file2);
    if(similar == 1){
        return 3;
    }
    return 1;
}
