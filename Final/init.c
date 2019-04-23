#include "ucode.c"

int console;

int parent()
{
    int pid, status;
    while(1)
    {
        printf("Sams INIT : wait for ZOMBIE child\n");
        pid = wait(&status);
        if(pid == console)
        {
            printf("Sams INIT : forks a new console login\n");
            console = fork();
            if(console)
            {
                continue;
            }
            else
            {
                exec("login /dev/tty0");
            }
            printf("Sams INIT : I just buried an orphan child proc %d\n", pid);
        }
    }
}

int main(int argc, char *argv[ ])
{
    int in, out;
    in = open("/dev/tty0", O_RDONLY);
    out = open("/dev/tty0", O_WRONLY);
    printf("Sams INIT : fork a login proc on console\n");
    console = fork();
    if(console)
    {
        parent();
    }
    else
    {
        exec("login /dev/tty0");
    }
    
}
