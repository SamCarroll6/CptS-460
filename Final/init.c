#include "ucode.c"

int console, console2, console3;

int parent()
{
    int pid, status;
    while(1)
    {
        prints("Sams INIT : wait for ZOMBIE child\n");
        pid = wait(&status);
        if(pid == console)
        {
            prints("Sams INIT : forks a new console login\n");
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
            prints("Sams INIT : forks a new console login\n");
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
            prints("Sams INIT : forks a new console login\n");
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
    prints("Sams INIT : fork a login proc on console\n\r");

    int in, out, in2, out2, in3, out3;
    in = open("/dev/tty0", O_RDONLY);
    out = open("/dev/tty0", O_WRONLY);

    console = fork();
    if(console)
    {
        in2 = open("/dev/ttyS0", O_RDONLY);
        out2 = open("/dev/ttyS0", O_WRONLY);
        console2 = fork();
        if(console2)
        {
            in3 = open("/dev/ttyS1", O_RDONLY);
            out3 = open("/dev/ttyS1", O_WRONLY);
            console3 = fork();
            if(console3)
            {
                parent();
            }
            else
            {
                write(out3, "Login on port S1\n\r", 19);
                exec("login /dev/ttyS1");
            }
        }  
        else
        {
            write(out2, "Login on port S0\n\r", 19);
            exec("login /dev/ttyS0");
        }
    }
    else
    {
        write(out, "Login on port S0\n\r", 19);
        exec("login /dev/tty0");
    }
}
