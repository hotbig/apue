#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<syslog.h>
#include<sys/socket.h>
#include<errno.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/resource.h>
#include<signal.h>
#include<string.h>
#include<netdb.h>
#define BUFLEN 128
#define QLEN 10

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif

#include <fcntl.h>

int
set_cloexec(int fd)
{
	int		val;

	if ((val = fcntl(fd, F_GETFD, 0)) < 0)
		return(-1);

	val |= FD_CLOEXEC;		/* enable close-on-exec */

	return(fcntl(fd, F_SETFD, val));
}

int initserver(int type, const struct sockaddr *addr, socklen_t alen, int qlen)
{
    int fd, err;
    int reuse =1;

    if((fd = socket(addr->sa_family, type, 0)) <0 )
        return (-1);
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int))<0)
        goto errout;
    if(bind(fd, addr, alen)<0)
        goto errout;
    if(type == SOCK_STREAM || type == SOCK_SEQPACKET)
        if(listen(fd, qlen)<0)
            goto errout;
    return (fd);
errout:
    err = errno;
    close(fd);
    errno = err;
    return (-1);
}

void
daemonize(const char *cmd)
{
	int i, fd0, fd1, fd2;
	pid_t pid;
	struct rlimit rl;
	struct sigaction sa;

	/*
	 * Clear file creation mask.
	 */
	umask(0);

	/*
	 * Get maximum number of file descriptors.
	 */
	if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
    {
		printf("%s: can't get file limit\n", cmd);
        exit(1);
    }

	/*
	 * Become a session leader to lose controlling TTY.
	 */
	if ((pid = fork()) < 0)
    {
		printf("%s: can't fork\n", cmd);
        exit(1);
    }
	else if (pid != 0) /* parent */
		exit(0);
	setsid();

   
	/*
	 * Ensure future opens won't allocate controlling TTYs.
	 */
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0)
    {       
		printf("%s: can't ignore SIGHUP\n", cmd);
        exit(1);
    }

	if ((pid = fork()) < 0)
    {
        printf("%s: can't fork\n", cmd);
        exit(1);
    }
	else if (pid != 0) /* parent */
		exit(0);

	/*
	 * Change the current working directory to the root so
	 * we won't prevent file systems from being unmounted.
	 */
	if (chdir("/") < 0)
    {
		printf("%s: can't change directory to /\n", cmd);
        exit(1);
    }

	/*
	 * Close all open file descriptors.
	 */
	if (rl.rlim_max == RLIM_INFINITY)
		rl.rlim_max = 1024;
	for (i = 0; i < rl.rlim_max; i++)
		close(i);

	/*
	 * Attach file descriptors 0, 1, and 2 to /dev/null.
	 */
	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(0);
	fd2 = dup(0);

	/*
	 * Initialize the log file.
	 */
	openlog(cmd, LOG_CONS, LOG_DAEMON);
	if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
		syslog(LOG_ERR, "unexpected file descriptors %d %d %d",
		  fd0, fd1, fd2);
		exit(1);
	}
}

void serve(int sockfd)
{
    int clfd;
    FILE *fp;

    char buf[BUFLEN];

    set_cloexec(sockfd);

    for(;;){
        if((clfd = accept(sockfd, NULL, NULL)) <0){
            syslog(LOG_ERR, "ruptimed: accept error: %s", strerror(errno));
            exit(1);
        }
        set_cloexec(clfd);
        if((fp = popen("/usr/bin/uptime", "r")) == NULL){
            //ERR handling
        }
        else
        {
            while(fgets(buf, BUFLEN, fp) != NULL)
                send(clfd, buf, strlen(buf), 0);
            pclose(fp);
        }
        close(clfd);
    }
}

int main(int argc, char *argv[])
{
    struct addrinfo *ailist, *aip;
    struct addrinfo hint;
    int sockfd, err, n;
    char *host;

    if(argc != 1)
    {
        printf("usage: ruptimed\n");
        exit(1);
    }
    if((n = sysconf(_SC_HOST_NAME_MAX)) < 0)
        n = HOST_NAME_MAX;
    if((host = malloc(n)) == NULL)
    {
        printf("mallock error\n");
        exit(1);
    }
    if(gethostname(host, n)<0)
    {
        printf("gethostname error\n");
        exit(1);
    }


    daemonize("16_17");
    
    memset(&hint, 0, sizeof(hint));
//    hint.ai_flags = AI_CANONNAME;
    hint.ai_flags = AI_PASSIVE;

    hint.ai_socktype = SOCK_STREAM;
    hint.ai_addr = NULL;
    hint.ai_canonname = NULL;
    hint.ai_next = NULL;

    
    //if((err = getaddrinfo(host, "", &hint, &ailist))!=0){
    if((err = getaddrinfo(NULL, "8888", &hint, &ailist))!=0){
       syslog(LOG_INFO, "error to get addrino %s %s", gai_strerror(err), host); 
    }

    for(aip = ailist; aip != NULL; aip = aip->ai_next)
    {
        if((sockfd = initserver(SOCK_STREAM, aip->ai_addr, aip->ai_addrlen, QLEN)) >= 0)
        {
            serve(sockfd);

            exit(0);
        }
    }

    exit(1);
}


