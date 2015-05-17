#include "open.h"
#include <sys/uio.h>

int csopen(char *name, int oflag)
{
    int len;
    char buf[12];
    struct iovec iov[3];
    static int csfd = -1;

    if(csfd < 0){
        if((csfd = cli_conn(CS_OPEN))<0){
            printf("cli_conn error\n");
            return (-1);
        }
    }

    sprintf(buf, " %d", oflag);
    iov[0].iov_base = CL_OPEN " ";
    iov[0].iov_len = strlen(CL_OPEN) + 1;
    
    iov[1].iov_base = name; 
    iov[1].iov_len = strlen(name);
    iov[2].iov_base = buf; 
    iov[2].iov_len = strlen(buf) + 1;
    len = iov[0].iov_len + iov[1].iov_len + iov[2].iov_len;

    if(writev(csfd, &iov[0], 3) != len)
    {
        printf("writev error\n");
        return (-1);
    }

    return (recv_fd(csfd, write));
}

