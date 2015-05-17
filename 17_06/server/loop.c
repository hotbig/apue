#include "opend.h"
#include <sys/select.h>

#define MAXLINE 4096

void loop(void)
{
    int i, n, maxfd, maxi, listenfd, clifd, nread;
    char buf[MAXLINE];

    uid_t uid;
    fd_set rset, allset;

    FD_ZERO(&allset);

    if((listenfd = serv_listen(CS_OPEN))<0)
    {
        printf("serv_listen error\n");
        exit(2);
    }

    FD_SET(listenfd, &allset);
    maxfd = listenfd;
    maxi = -1;

    for(;;){
        rset = allset;
        if((n = select(maxfd + 1, &rset, NULL, NULL, NULL))<0)
        {
            printf("select error\n");
            exit(2);
        }
        
        if(FD_ISSET(listenfd, &rset)){
            if((clifd = serv_accept(listenfd, &uid))<0)
            {
                printf("serv accept error: %d\n", clifd);
                exit(2);
            }
            i = client_add(clifd, uid);
            FD_SET(clifd, &allset);
            if(clifd > maxfd)
                maxfd = clifd;
            if(i > maxi)
                maxi = i;
            printf("new connection: uid %d, fd %d", uid, clifd);
            continue;
        }

        for(i = 0; i<=maxi; i++){
            if((clifd = client[i].fd) < 0)
                continue;
            if(FD_ISSET(clifd, &rset)){
                if((nread = read(clifd, buf, MAXLINE)) < 0){
                    printf("read error on fd %d", clifd);
                    exit(2);
                }
                else if(nread == 0)
                {
                    printf("closed: uid %d, fd %d", client[i].uid, clifd);
                    client_del(clifd);
                    FD_CLR(clifd, &allset);
                    close(clifd);
                }
                else
                {
                    handle_request(buf, nread, clifd, client[i].uid);
                }
            }
        }
    }
}
