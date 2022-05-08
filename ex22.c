#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <wait.h>
#include <errno.h>
extern int errno;

int compile(char* file) {
    int stat;
    int pid = fork();
    if(pid == 0){
        execlp("gcc", "gcc", file,NULL);
        perror("execlp failed");
        exit(0);
    }
    wait(&stat);
    if(stat != 0){ // compile failed
        return 10;
    }
    return 0;
}

int run(int input, char* output) {
    int stat, pid;
    if(dup2(input, 0) == -1){
        return -1;
    }
    int out = open("output.txt", O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
//    if(out == -1) {
//        return -1;
//    }
//    if(dup2(out, 1) == -1){
//        return -1;
//    }
    pid = fork();
    if(pid == 0){
        if(out == -1) {
            return -1;
        }
        if(dup2(out, 1) == -1){
            return -1;
        }
        execlp("./a.out", "./a.out",NULL);
        perror("execlp failed");
        exit(0);
    }
    wait(&stat);
    close(out);
    pid = fork();
    if(pid == 0){
        execlp("./home/naser/OperatingSys/ex2/comp.out","./home/naser/OperatingSys/ex2/comp.out", "output.txt", output,NULL);
        exit(0);
    }
    int t = wait(&stat);
    if(dup2(1, out) == -1){
        return -1;
    }
    printf("\n%d\n", t);
    return 0;
}

int Grade(int input, char* output){
    DIR *dr = opendir(".");
    if(dr == NULL){
        return -1;
    }
    struct dirent *de;
    int pid, stat, grade;
    char* ext = NULL;
    while ((de = readdir(dr))){
        if((ext = strrchr(de->d_name,'.')) != NULL ) {
            if(strcmp(ext,".c") == 0) {
//                printf("%s\n",de->d_name);
                grade = compile(de->d_name);
                if(grade == 10){
                    return 10;
                }
                run(input,output);
                return grade;
            }
        }
    }
    return 0; // no c file found
}

int main(int argc, char** argv) {
    int fd = open(argv[1], 'r');
    int errorFd = open("errors.txt", O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    int results = open("results.txt", O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if(dup2(errorFd, 2) == -1){
        return -1;
    }
    if(fd == -1) {
        return -1;
    }
    if(dup2(fd, 0) == -1){
        return -1;
    }
    char dirPath[150], input[150], output[150];
    scanf(" %[^\n]s", dirPath);
    scanf(" %[^\n]s", input);
    scanf(" %[^\n]s", output);
    DIR *dir = opendir(dirPath);
    if(dir == NULL){
        write(1, "Not a valid directory\n", 23);
        return -1;
    }
    if(access(input, F_OK) == -1){
        write(1, "Input file not exist\n", 22);
        return -1;
    }
    if(access(output, F_OK) == -1){
        write(1, "Output file not exist\n", 22);
        return -1;
    }
    int inputFile = open(input, 'r');
    if(inputFile == -1) {
        return -1;
    }
    struct dirent* folder;
    char pwd[150];
    getcwd(pwd,150);
    chdir(dirPath);
    while ((folder = readdir(dir))){
        if(folder->d_type == DT_DIR && strcmp(folder->d_name,".") !=0 && strcmp(folder->d_name,"..") !=0){
            chdir(folder->d_name);
            int grade = Grade(inputFile, output);
        }
        chdir(dirPath);
    }
    closedir(dir);
    close(fd);
    close(errorFd);
}

