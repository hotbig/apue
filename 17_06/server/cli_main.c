#include "open.h"
#include <fcntl.h>

#define MAXLINE 4096
#define BUFFSIZE 8192

int main(int argc, char *argv[])
{
    int n, fd;
    char buf[BUFFSIZE];
    char line[MAXLINE];

    while(fgets(line, MAXLINE, stdin) != NULL){
        if(line[strlen(line)-1] == '\n')
            line[strlen(line)-1] = 0;

        if((fd = csopen(line, O_RDONLY)) < 0)
            continue;

        while((n = read(fd, buf, BUFFSIZE)) > 0)
            if(write(STDOUT_FILENO, buf, n) != n)
            {
                printf("write error\n");
                exit(-1);
            }

        if(n < 0)
        {
            printf("read error\n");
            exit(-1);
        }

    }

    exit(0);
}
