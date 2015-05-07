#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/mman.h>

#define FILE_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

#define COPYINCR (1024*1024*1024)

int main(int argc, char *argv[])
{
    int fdin, fdout;
    void *src, *dst;
    size_t copysz;
    struct stat sbuf;
    off_t fsz = 0;

    if(argc != 3)
    {
        printf("invalid arguments\n");
        exit(1);
    }

    if((fdin = open(argv[1], O_RDONLY))<0)
    {
        //ERR handling
        printf("open file failed\n");
        exit(1);
    }

    if((fdout = open(argv[2], O_RDWR|O_CREAT|O_TRUNC, FILE_MODE))<0)
    {
        //ERR hanlding
        printf("open file failed\n");
        exit(1);
    }

    if(fstat(fdin, &sbuf) < 0)
    {
        //ERR handling
        printf("fstat failed\n");
        exit(1);
    }

    if(ftruncate(fdout, sbuf.st_size)<0)
    {
        //ERR handling
        printf("ftruncate failed\n");
        exit(1);
    }

    while(fsz < sbuf.st_size){
        if((sbuf.st_size - fsz) > COPYINCR)
            copysz = COPYINCR;
        else
            copysz = sbuf.st_size - fsz;

        if((src = mmap(0, copysz, PROT_READ, MAP_SHARED, fdin, fsz))==MAP_FAILED)
        {
            //ERR handling
            printf("mmap fdin failed\n");
            exit(1);
        }

        if((dst = mmap(0, copysz, PROT_READ|PROT_WRITE, MAP_SHARED, 
                        fdout, fsz)) == MAP_FAILED)
        {
            //ERR hanling
            printf("mmap fdout failed!\n");
            exit(1);
        }
        
        memcpy(dst, src, copysz);
        munmap(src, copysz);
        munmap(dst, copysz);
        fsz += copysz;
    }

    exit(0);
}
