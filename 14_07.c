#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>

#define FILE_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
static volatile sig_atomic_t sigflag;
static sigset_t newmask, oldmask, zeromask;

static void sig_usr(int signo)
{
    sigflag = 1;
    printf("received sig %d\n", signo);
}

void TELL_WAIT(void)
{
    if(signal(SIGUSR1, sig_usr) == SIG_ERR)
    {
        // ERR handling
    }
    if(signal(SIGUSR2, sig_usr) == SIG_ERR)
    {
        // ERR handling
    }
    sigemptyset(&zeromask);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR1);
    sigaddset(&newmask, SIGUSR2);

    if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
    {
        // ENR handling
    }
}

void TELL_PARENT(pid)
{
    kill(pid, SIGUSR2);
}

void WAIT_PARENT(void)
{
    while(sigflag == 0)
        sigsuspend(&zeromask);
    sigflag = 0;

    if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
    {
        //err HANDLING
    }
}

void TELL_CHILD(pid)
{
    kill(pid, SIGUSR1);
}

void WAIT_CHILD(void)
{
    while(sigflag == 0){
        sigsuspend(&zeromask);
    }
    sigflag = 0;

    if(sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
    {
        //err HANDLING
    }
}

int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
    struct flock lock;

    lock.l_type = type;
    lock.l_start = offset;
    lock.l_whence = whence;
    lock.l_len = len;

    return (fcntl(fd, cmd, &lock));
}

#define writew_lock(fd, offset, whence, len) \
    lock_reg((fd), F_SETLKW, F_WRLCK, (offset), (whence), (len))

static void lockabyte(const char *name, int fd, off_t offset)
{
    if(writew_lock(fd, offset, SEEK_SET, 1) < 0)
    {
        //ERR handling
    }
    printf("%s:got the lock, byte %lld\n", name, (long long)offset);
}

int main(void)
{
    int fd;
    pid_t pid;

    if((fd = creat("templock", FILE_MODE))<0)
    {
        //ERR handling
    }

    if(write(fd, "abxxxxx", 7) != 2)
    {
        //ERR handling
    }

    TELL_WAIT();
    if((pid = fork()) < 0)
    {
        //ERR handling
    }else if(pid == 0){
        printf("i'm child pid: %d\n", getpid());

        lockabyte("child", fd, 0);
        TELL_PARENT(getpid());

        /// !!!!!! note parent's sigsuspend might miss the signal 
        /// !!!!!! as parent process dosen't run into WAIT_CHILD function
        WAIT_PARENT();         
        lockabyte("child", fd, 1);
    }else{
        printf("i'm parent pid: %d\n", getpid());

        lockabyte("parent", fd, 1);
        TELL_CHILD(pid);
        WAIT_CHILD();
        lockabyte("parent", fd, 0);
//        lockabyte("parent", fd, 0);
        printf("parent lock down\n");
    }
    printf("i'm finished pid: %d\n", getpid());
    sleep(3);
    exit(0);
}
