#include "ucode.c"

char *newargs[10];
int parseCount;

// Exec works with spaces at front, so only need to parse pipes for pipe value, spaces wont matter.

void tokenize(char *line, char delim)
{
  char *cp;
  cp = line;
  parseCount = 0;
  
  while (*cp != 0){
       while (*cp == delim) *cp++ = 0;        
       if (*cp != 0)
           newargs[parseCount++] = cp;         
       while (*cp != delim && *cp != 0) cp++;                  
       if (*cp != 0)   
           *cp = 0;                   
       else 
            break; 
       cp++;
  }
  newargs[parseCount] = 0;
}

void rmspaces(int num)
{
    char *cp;
    cp = newargs[num];
    while(*cp == ' ')
    {
        *cp++;
    }
    newargs[num] = cp;
}

int execute(char *command)
{
    // // int count;
    // char new[64];
    // // count = finddelim(command, ' ');
    // // printi(count);
    // // strncpy(new, command, count);
    // // command = &command[count + 1];
    // // prints(new);
    // // mputc('\n');
    // // prints(command);
    // // mputc('\n');
    // prints(tokenize(command, ' '));
    // printf("new: %s, command : %s\n", new, command);
    return exec(command);
}

//int dopipe(char *command, int)

int main(int argc, char *argv[ ])
{
    char command[128], *token, cdhold[128], phold[128];
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
        if(strcmp(command, "") == 0)
        {
            continue;
        }
        strcpy(cdhold, command);
        tokenize(cdhold, ' ');
        if(strcmp(newargs[0], "cd") == 0)
        {
            if(newargs[1])
                chdir(newargs[1]);
            else
            {
                chdir("/");
            }
            continue;
        }
        if(strcmp(newargs[0], "pwd") == 0)
        {
            pwd();
            continue;
        }

        pid = fork();

        if(pid)
        {
            pid = wait(&status);
            printf("\nsh %d child %d  exit status = %x\n", shell, pid, status);
        }
        else
        {
            tokenize(command, '|');
            rmspaces(1);
            rmspaces(2);
            printf("HERE : %s\n%s\n%s\n", newargs[0], newargs[1], newargs[2]);
            id = execute(newargs[2]);
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
