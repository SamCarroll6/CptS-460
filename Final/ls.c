#include "ucode.c"

char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";
struct stat mystat, *sp;

int ls_file(char *fname)
{
    int r, i;
    char sbuf[4096];
    // r = stat(fname, sp);
    if((sp->st_mode & 0040000) == 0040000)
    {
        mputc('d');
    }
    if((sp->st_mode & 0100000) == 0100000)
    {
        mputc('-');
    }
    if ((sp->st_mode & 0120000) == 0120000)
    {
        mputc('l');
    }
    for(i = 8; i >= 0; i--)
    {
        if(sp->st_mode & 1<<i)
        {
            mputc(t1[i]);
            //printf("%c", t1[i]);
        }
        else
        {
            mputc(t2[i]);
            //printf("%c", t2[i]);
        }
    }
    printf(" %d ", sp->st_nlink);
    printf("%d ", sp->st_uid);
    printf("%d ", sp->st_size);
    printf(" %s", fname);
    if((sp->st_mode & 0120000) == 0120000)
    {
        r = readlink(fname, sbuf);
        printf(" -> %s", sbuf);
    }
    prints("\n");
}

int ls_dir(char *fname)
{
    int size = sp->st_size;
    int fd, count, r, i;
    char buf[1024], hold[64];
    char *cp;
    DIR *dp;

    fd = open(fname, O_RDONLY);

    while(count = read(fd, buf, 1024))
    {
        cp = buf;
        dp = (DIR*)cp;
        while(cp < buf + 1024)
        {
            if(dp->inode != 0)
            {
                sp = &mystat;
                strncpy(hold, dp->name, dp->name_len);
                if((r = stat(hold, sp)) >= 0)
                {
                    ls_file(hold);
                }
            }
            cp = cp + dp->rec_len;
            dp = (DIR*)cp;
        }
    }

}

int main(int argc, char *argv[ ])
{
    prints("***************************\n");
    prints("           Sams ls         \n");
    prints("***************************\n");
    int r;
    char *s;
    char filename[1024], cwd[1024];
    if(argc == 1)
    {
        s = "./";
    }
    else
    {
        s = argv[1];
    }
    sp = &mystat;
    if((r = stat(s, sp)) < 0)
    {
        printf("ls error\n");
        exit(1);
    }
    strcpy(filename, s);
    if(s[0] != '/')
    {
        getcwd(cwd);
        strcpy(filename, cwd);
        strcat(filename, "/");
        strcat(filename, s);
    }
    if((sp->st_mode & 0040000) == 0040000)
    {
        ls_dir(filename);
    }
    else
    {
        ls_file(filename);
    }

}



// #include "ucode.c"

// char *t1 = "xwrxwrxwr-------";
// char *t2 = "----------------";
// struct stat mystat, *sp;

// int ls_file(char *fname)
// {
//     struct stat fstat, *sp = &fstat;
//     int r, i;
//     char sbuf[4096];
//     r = stat(fname, sp);
//     if((sp->st_mode & 0040000) == 0040000)
//     {
//         prints("d");
//     }
//     if((sp->st_mode & 0100000) == 0100000)
//     {
//         prints("-");
//     }
//     if((sp->st_mode & 0120000) == 0120000)
//     {
//         prints("l");
//     }
//     for(i = 0; i < 8; i++)
//     {
//         if(sp->st_mode & 1<<i)
//         {
//             printf("%c", t1[i]);
//         }
//         else
//         {
//             printf("%c", t2[i]);
//         }
//     }
//     printf("%4d ", sp->st_nlink);
//     printf("%4d ", sp->st_uid);
//     printf("%8d ", sp->st_size);
//     printf(" %s", fname);
//     if((sp->st_mode & 0120000) == 0120000)
//     {
//         r = readlink(fname, sbuf);
//         printf(" -> %s", sbuf);
//     }
//     printf("\n");
// }

// int ls_dir(char *dname)
// {
//     char fname[256];
//     DIR *dp;
//     DIR *ep;
//     dp = opendir(dname);
//     while(ep = readdir(dp))
//     {
//         strcpy(fname, ep->name);
//         if(strcmp(fname, ".") == 0 || strcmp(fname, ".."))
//         {
//             continue;
//         }
//         strcpy(fname, dname);
//         strcat(fname, "/");
//         strcat(fname, ep->name);
//         ls_file(fname);
//     }
// }

// int main(int argc, char *argv[ ])
// {
//     struct stat mystat, *sp;
//     int r;
//     char *s;
//     char filename[1024], cwd[1024];
//     if(argc == 1)
//     {
//         s = "./";
//     }
//     else
//     {
//         s = argv[1];
//     }
//     sp = &mystat;
//     if((r = stat(s, sp)) < 0)
//     {
//        // perror("ls");
//         exit(1);
//     }
//     strcpy(filename, s);
//     if(s[0] != '/')
//     {
//         getcwd(cwd);
//         strcpy(filename, cwd);
//         strcpy(filename, "/");
//         strcpy(filename, s);
//     }
//     if((sp->st_mode & 0040000) == 0040000)
//     {
//         ls_dir(filename);
//     }
//     else
//     {
//         ls_file(filename);
//     }

// }




