#include "ucode.c"

int console, console2, console3;

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
        else if(pid == console2)
        {
            printf("Sams INIT : forks a new console login\n");
            console2 = fork();
            if(console2)
            {
                continue;
            }
            else
            {
                exec("login /dev/ttyS0");
            }
            printf("Sams INIT : I just buried an orphan child proc %d\n", pid);
        }
        if(pid == console3)
        {
            printf("Sams INIT : forks a new console login\n");
            console3 = fork();
            if(console3)
            {
                continue;
            }
            else
            {
                exec("login /dev/ttyS1");
            }
            printf("Sams INIT : I just buried an orphan child proc %d\n", pid);
        }
    }
}

int main(int argc, char *argv[ ])
{
    int in, out, in2, out2, in3, out3;
    in = open("/dev/tty0", O_RDONLY);
    out = open("/dev/tty0", O_WRONLY);
    in2 = open("/dev/ttyS0", O_RDONLY);
    out2 = open("/dev/ttyS0", O_WRONLY);
    in3 = open("/dev/ttyS1", O_RDONLY);
    out3 = open("/dev/ttyS1", O_WRONLY);
    printf("Sams INIT : fork a login proc on console\n");
    console = fork();
    if(console)
    {
        console2 = fork();
        if(console2)
        {
            console3 = fork();
            if(console3)
            {
                parent();
            }
            else
            {
                exec("login /dev/ttyS1");
            }
        }  
        else
        {
            exec("login /dev/ttyS0");
        }
    }
    else
    {
        exec("login /dev/tty0");
    }
    
}
