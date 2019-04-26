#include "ucode.c"

char *newargs[10];
char *iargs[10];
int icount;
int parseCount;


/************************* Indirects **************************/

int finddelim(char *line, char delim)
{
    char *cp;
    cp = line;
    int ret = 0;
    while(*cp && *cp != delim)
    {
        *cp++;
        ret++;
    }
    return ret;
}

void tokenize2(char *line, char delim)
{
  char *cp;
  cp = line;
  icount = 0;
  
  while (*cp != 0){
       while (*cp == delim) *cp++ = 0;        
       if (*cp != 0)
           iargs[icount++] = cp;         
       while (*cp != delim && *cp != 0) cp++;                  
       if (*cp != 0)   
           *cp = 0;                   
       else 
            break; 
       cp++;
  }
  iargs[icount] = 0;
}

void rmspaces2(int num)
{
    char *cp;
    cp = iargs[num];
    while(*cp == ' ')
    {
        *cp++;
    }
    iargs[num] = cp;
}

int do_indirects(int n)
{
    int len = strlen(newargs[n]);
    int i =0, check, fd;
    char hold[64];
    strcpy(hold, newargs[n]);
    check = finddelim(hold, '<');
    if(check == len)
    {
        check = finddelim(hold, '>');
        if(check == len)
        {
            return exec(newargs[n]);
        }
        else if(newargs[n][check+1] == '>')
        {
            memset(hold, 0, 64);
            strcpy(hold, newargs[n]);
            tokenize2(hold, '>');
            for(i = 0; i < icount; i++)
            {
                rmspaces2(i);
                prints(iargs[i]);
            }
            fd = open(iargs[1], O_WRONLY | O_CREAT | O_APPEND);
            if(fd < 0)
            {
                prints("IO Redirection Error, file could not be opened\n");
                exit(1);
            }
            dup2(fd, 1);
            close(fd);
            return exec(iargs[0]);
        }
        else
        {
            memset(hold, 0, 64);
            strcpy(hold, newargs[n]);
            tokenize2(hold, '>');
            for(i = 0; i < icount; i++)
            {
                rmspaces2(i);
                prints(iargs[i]);
            }
            fd = open(iargs[1], O_WRONLY | O_CREAT);
            if(fd < 0)
            {
                prints("IO Redirection Error, file could not be opened\n");
                exit(1);
            }
            dup2(fd, 1);
            close(fd);
            return exec(iargs[0]);
        }  
    }
    else
    {
        memset(hold, 0, 64);
        strcpy(hold, newargs[n]);
        tokenize2(hold, '<');
        for(i = 0; i < icount; i++)
        {
            rmspaces2(i);
            prints(iargs[i]);
        }
        fd = open(iargs[1], O_RDONLY);
        if(fd < 0)
        {
            prints("IO Redirection Error, file could not be opened\n");
            exit(1);
        }
        dup2(fd, 0);
        close(fd);
        return exec(iargs[0]);
    }
    
    // tokenize2(newargs[n], );
    // for(i = 0; i < iCount; i++)
    // {
    //     rmspaces2();
    // }

}

/************************* Pipes **************************/

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

int do_pipe(int o, int t)
{
    int i = 1;
    int pid, pd[2];
    int status;
    for(i = 1; i < parseCount; i++)
    {
        rmspaces(i);
    }
    pipe(pd);

    pid = fork();
    if(pid)
    {
        close(pd[1]); // close writer
        dup2(pd[0], 0);
        if(t < parseCount - 1)
        {
            do_pipe(t, t+1);
        }
        //exec(newargs[t]);
        do_indirects(t);
    }
    else
    {
        close(pd[0]); // close reader
        dup2(pd[1], 1);
        //exec(newargs[o]);
        do_indirects(o);
    }
}

int execute(char *command)
{
    char hold[128];
    strcpy(hold, command);
    tokenize(hold, '|');
    if(parseCount > 1)
    {
        return do_pipe(0, 1);
    }
    return do_indirects(0);
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
            strcpy(phold, command);

            id = execute(phold);
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
