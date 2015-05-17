#include	"opend.h"
#include <errno.h>
#include	<fcntl.h>

#define MAXLINE 4096
void
handle_request(char *buf, int nread, int clifd, uid_t uid)
{
	int		newfd;

    printf("new request\n");
	if (buf[nread-1] != 0) {
		snprintf(errmsg, MAXLINE-1,
		  "request from uid %d not null terminated: %*.*s\n",
		  uid, nread, nread, buf);
		send_err(clifd, -1, errmsg);
		return;
	}
//	log_msg("request: %s, from uid %d", buf, uid);

	/* parse the arguments, set options */
	if (buf_args(buf, cli_args) < 0) {
		send_err(clifd, -1, errmsg);
//		log_msg(errmsg);
		return;
	}

    printf("pathname : %s\n", pathname);
	if ((newfd = open(pathname, oflag)) < 0) {
		snprintf(errmsg, MAXLINE-1, "can't open %s: %s\n",
		  pathname, strerror(errno));
		send_err(clifd, -1, errmsg);
        printf("open failed\n");
//		log_msg(errmsg);
		return;
	}

	/* send the descriptor */
	if (send_fd(clifd, newfd) < 0)
    {
        printf("send_fd error\n")       ;
		//log_sys("send_fd error");
    }
    printf("send fd %d over fd %d for %s\n", newfd, clifd, pathname);
//	log_msg("sent fd %d over fd %d for %s", newfd, clifd, pathname);
	close(newfd);		/* we're done with descriptor */
}
