#include "ucode.c"

char *newargs[10];

int finddelim(char *command, char delim)
{
    int len, i = 0;
    len = strlen(command);
    for(i = 0; i < len; i++)
    {
        if(command[i] == delim)
            return i;
    }
    return -1;
}

int execute(char *command)
{
    int count;
    char new[64];
    count = finddelim(command, ' ');
    printi(count);
    strncpy(new, command, count);
    command = &command[count + 1];
    prints(new);
    mputc('\n');
    prints(command);
    mputc('\n');
    return exec(command);
}

//int dopipe(char *command, int)

int main(int argc, char *argv[ ])
{
    char command[128], *token;
    int shell = getpid(), pid, status, id;
    // Main shell loop, never ends.
    while(1)
    {
        memset(command, 0, 128);
        while(!command[0])
        {
            printf("Sam sh#%d", shell);
            gets(command);
        }
        if(strcmp(command, "logout") == 0)
        {
            exit(1);
        }


        pid = fork();
        if(pid)
        {
            pid = wait(&status);
            printf("\nsh %d child %d  exit status = %x\n", shell, pid, status);
        }
        else
        {
            id = execute(command);
            if(id == -1)
            {
                exit(1);
            }

        }
        
    }
}


// int tokenize(char *command, char delim)
// {
//     int i = 0, n = 0, p = 0;
//     char buf[64];
//     for(i = 0; i < 10; i++)
//     {
//         newargs[i] = 0;
//     }
//     i = 0;
//     while(command[i])
//     {
//         if(command[i] != delim)
//         {
//             buf[p] = command[i];
//             printi(n);
//             printi(p);
//             p++;
//         }
//         else
//         {
//             strcpy(newargs[n], buf);
//             prints(newargs[n]);
//             mputc('\n');
//             n++;
//             p = 0;
//             memset(buf, 0, 64);
//         }
//         i++;
//     }
//     buf[p] = '\0';
//     i = 0;
//     while(newargs[i] != 0)
//     {
//         prints(newargs[i]);
//         mputc('\n');
//         i++;
//     }
// }
