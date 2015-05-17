#include "opend.h"
#include <syslog.h>

#define MAXLINE 4096

int debug, oflag, client_size, log_to_stderr;
char errmsg[MAXLINE];
char *pathname;
Client *client = NULL;

int main(int argc, char *argv[])
{
    int c;

    openlog("open.serv", LOG_PID, LOG_USER);

    opterr = 0;
    while((c = getopt(argc, argv, "d")) != EOF)
    {
        switch(c){
            case 'd':
                debug = log_to_stderr = 1;
                break;
            case '?':
                printf("unrecognized option: -%c\n", optopt);
        }
    }

//    if(debug == 0)
//        daemonize("opend");

    loop();
}
