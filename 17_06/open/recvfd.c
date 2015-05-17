#include"fd_unsock.h"
#include<unistd.h>
#include<sys/socket.h>

#define MAXLINE 4096
#define CONTROLLEN CMSG_SPACE(sizeof(int))

static struct cmsghdr *cmptr = NULL;

int recv_fd(int fd, ssize_t (*userfunc)(int, const void*, size_t))
{
    int newfd, nr, status;
    char *ptr;
    char buf[MAXLINE];
    struct iovec iov[1];
    struct msghdr msg;

    char bb[200];

    status = -1;
    for(;;){
        iov[0].iov_base = buf;
        iov[0].iov_len = 2;
        msg.msg_iov = iov;
        msg.msg_iovlen = 1;
        msg.msg_name = NULL;
        msg.msg_namelen = 0;
//        if(cmptr == NULL && (cmptr = malloc(CONTROLLEN)) == NULL)
//            return (-1);
        cmptr = &bb;
        msg.msg_control = cmptr;
        msg.msg_controllen = CONTROLLEN;
        if((nr = recvmsg(fd, &msg, 0))<0){
            printf("recvmsg error\n");
            return (-1);
        }else if(nr == 0){
            printf("connection closed by server\n");
            return -1;
        }

        printf("received nr:%d %d %d %d %d %d %d %d\n", nr, *((int*)cmptr), *(((int*)cmptr)+1), *(((int*)cmptr)+2), *(((int*)cmptr)+3), *(((int*)cmptr)+4), *(((int*)cmptr)+5), (((int*)cmptr)+5));

        for(ptr = buf; ptr < &buf[nr];){
            if(*ptr++ == 0){
                if(ptr != &buf[nr-1])
                {
                    printf("message format error\n");
                    exit(-1);
                }
                status = *ptr & 0xFF;
                if(status == 0){
                    if(msg.msg_controllen != CONTROLLEN)
                    {
                        printf("status = 0 buf no fd %d %d %d %d %d\n", msg.msg_controllen, cmptr->cmsg_len, CONTROLLEN, *((int*)CMSG_DATA(cmptr))+1);
                        
                        exit(-1);

                    }
                    newfd = *(int*)CMSG_DATA(cmptr);
                }else{
                    newfd = -status;
                }
                nr -= 2;
            }
        }

        if(nr > 0 &&(*userfunc)(STDERR_FILENO, buf, nr) != nr)
            return -1;
        if(status >= 0)
            return newfd;
    }
}
