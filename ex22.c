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
    if (pid == -1) {
        perror("Error in: fork");
        return -1;
    }
    if(pid == 0){
        execlp("gcc", "gcc", file,NULL);
        perror("Error in: execlp");
        exit(0);
    }
    if (wait(&stat) == -1) {
        perror("Error in: wait");
        return -1;
    }
    if(stat != 0){ // compile failed
        return 10;
    }
    return 0;
}

int compare(char* output) {
    int newstat, pid;
    pid = fork();
    if (pid == -1) {
        perror("Error in: fork");
        return -1;
    }
    if(pid == 0){
        execlp("/home/naser/OperatingSys/ex2/comp.out","/home/naser/OperatingSys/ex2/comp.out", "output.txt", output,NULL);
        perror("Error in: execlp");
        exit(0);
    }
    if (waitpid(pid,&newstat,0) == -1) {
        perror("Error in: wait");
        return -1;
    }
    return WEXITSTATUS(newstat);
}

int run(int input, char* output) {
    if(lseek(input,0, SEEK_SET) == -1) { // go back to the start of the file
        perror("Error in: lseek");
        return -1;
    }
    int stat, pid;
    if(dup2(input, 0) == -1){
        perror("Error in: dup2");
        return -1;
    }
    int out = open("output.txt", O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if(out == -1) {
        perror("Error in: open");
        return -1;
    }
    pid = fork();
    if(pid == 0){
        if(dup2(out, 1) == -1){
            return -1;
        }
        execlp("./a.out", "./a.out",NULL);
        perror("Error in: execlp");
        exit(0);
    }
    if (wait(&stat) == -1) {
        perror("Error in: wait");
        close(out);
        return -1;
    }
    close(out);
    int s = compare(output);
    remove("a.out");
    remove("output.txt");
    return s;
}

int Grade(int input, char* output){
    DIR *dr = opendir(".");
    if(dr == NULL){
        perror("Error in: opendir");
        return -1;
    }
    struct dirent *de;
    int grade;
    char* ext = NULL;
    while ((de = readdir(dr))){
        if((ext = strrchr(de->d_name,'.')) != NULL && strcmp(ext,".c") == 0) {
            if(compile(de->d_name) == 10){
                closedir(dr);
                return 10;
            }
            grade = run(input,output);
            closedir(dr);
            return grade;
        }
    }
    return 0; // no c file found
}

char* comment(int grade) {
    if (grade == 0) {
        return ",0,NO_C_FILE\n";
    } else if (grade == 10) {
        return ",10,COMPILATION_ERROR\n";
    } else if (grade == 2) {
        return ",50,WRONG\n";
    } else if (grade == 3) {
        return ",75,SIMILAR\n";
    } else {
        return ",100,EXCELLENT\n";
    }
}

char* strmrg(char* str1, char* str2) {
    int len1 = strlen(str1), len2 = strlen(str2);
    char* str = (char*) malloc(len1 + len2 +1);
    int i = 0;
    for (; i < len1; ++i) {
        str[i] = *str1 ++;
    }
    for (; i < len2 + len1; ++i) {
        str[i] = *str2 ++;
    }
    str[i] = '\0';
    return str;
}

int main(int argc, char** argv) {
    int fd = open(argv[1], 'r');
    if(fd == -1) {

        exit(-1);
    }
    int errorFd = open("errors.txt", O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if(errorFd == -1) {

        exit(-1);
    }
    int results = open("results.txt", O_CREAT | O_TRUNC | O_WRONLY , S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if(results == -1) {

        exit(-1);
    }
    if(dup2(errorFd, 2) == -1){
        exit(-1);
    }
    if(dup2(fd, 0) == -1){
        exit(-1);
    }

    char dirPath[150], input[150], output[150];
    scanf(" %[^\n]s", dirPath);
    scanf(" %[^\n]s", input);
    scanf(" %[^\n]s", output);
    close(fd);

    DIR *dir = opendir(dirPath);
    if(dir == NULL){
        write(1, "Not a valid directory\n", 23);
        exit(-1);
    }
    if(access(input, F_OK) == -1){
        write(1, "Input file not exist\n", 22);
        exit(-1);
    }
    if(access(output, F_OK) == -1){
        write(1, "Output file not exist\n", 22);
        exit(-1);
    }
    int inputFile = open(input, 'r');
    if(inputFile == -1) {
        exit(-1);
    }

    struct dirent* student;
    char pwd[150];
    char* student_grade;
    getcwd(pwd,150);
    if (chdir(dirPath) == -1) {
        exit(-1);
    }
    while ((student = readdir(dir))){
        if(student->d_type == DT_DIR && strcmp(student->d_name,".") !=0 && strcmp(student->d_name,"..") !=0){
            if (chdir(student->d_name) == -1){
                continue;
            }
            int grade = Grade(inputFile, output);
            if (grade != -1){
                student_grade = strmrg(student->d_name, comment(grade));
                if (write(results, student_grade, strlen(student_grade)) == -1) {
                    continue;
                }
                free(student_grade);
            }
        }
        if (chdir(dirPath) == -1) {
            exit(-1);
        }
    }

    close(results);
    close(errorFd);
    closedir(dir);
    close(fd);
    close(errorFd);
}

