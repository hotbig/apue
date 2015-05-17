#include<sys/socket.h>
#include<stdio.h>

#include"fd_unsock.h"
#include<errno.h>
#include<string.h>
#define CONTROLLEN CMSG_SPACE(sizeof(int))
#define MAXLINE 4096
static struct cmsghdr *cmptr = NULL; 

int send_fd(int fd, int fd_to_send)
{
    struct iovec iov[1];
    struct msghdr msg;
    char buf[2];
    int nr = 0;


    iov[0].iov_base = buf;
    iov[0].iov_len = 2;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    if(fd_to_send < 0){
        msg.msg_control = NULL;
        msg.msg_controllen = 0;
        buf[1] = -fd_to_send;
        if(buf[1] == 0)
            buf[1] = 1;
    }else{
        if(cmptr == NULL && (cmptr = malloc(CONTROLLEN)) == NULL)
            return -1;

        cmptr->cmsg_level = SOL_SOCKET;
        cmptr->cmsg_type = SCM_RIGHTS;
        cmptr->cmsg_len = CONTROLLEN;
        msg.msg_control = cmptr;
        msg.msg_controllen = CONTROLLEN;
        *(int*)CMSG_DATA(cmptr) = fd_to_send;

        buf[1] = 0;
    }
    buf[0] = 0;
    printf("received nr: %d %d %d %d %d %d %d\n",  *((int*)cmptr), *(((int*)cmptr)+1), *(((int*)cmptr)+2), *(((int*)cmptr)+3), *(((int*)cmptr)+4), *(((int*)cmptr)+5), (((int*)cmptr)+5));
    printf("msge leth %d, cmsg_level: %d, cmsg_type: %d, c_msg_len: %d\n", 
            msg.msg_controllen, 
            SOL_SOCKET, 
            SCM_RIGHTS,
            CONTROLLEN);
    if((nr = sendmsg(fd, &msg, 0))!=2)
    {
        printf("send nr: %d %s\n", nr, strerror(errno));
        return -1;
    }

    printf("msge leth %d\n", msg.msg_controllen);
    return 0;
}


