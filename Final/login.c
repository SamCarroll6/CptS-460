#include "ucode.c"

//int in, out, err;
//char name[128], password[128];

int main(int argc, char *argv[ ])
{
    // prints("****************************\n");
    // prints("          Sams Login        \n");
    // prints("****************************\n");    
    int in, out, err, fd, count, i = 0, r = 0, un = 0, p = 0;
    char username[64], password[64], buf1[1024], hold[1024];
    char uname[64], pass[64];
    char token[64];

    close(0);
    close(1);

    in = open(argv[1], 0);
    out = open(argv[1], 1);
    err = open(argv[1], 2);
    
    printi(in);
    printi(out);
    printi(err);

    fixtty(argv[1]);

    fd = open("/etc/passwd", O_RDONLY);

    printf("SAMS LOGIN : Open %s as stdin = 0, stdout = 1, err = 2\n", argv[1]);

    count = read(fd, buf1, 1024);
    strcpy(hold, buf1);

    while(1)
    {
        i = 0;
        prints("login: ");
        gets(username);
        prints("password: ");
        gets(password);

        while(i < count)
        {
            r = 0;
            un = 0;
            p = 0;
            while(hold[i] != '\n' && hold[i])
            {
                token[r] = hold[i];
                i++;
                r++;
            }
            i++;
            while(token[un] != ':')
            {
                uname[un] = token[un];
                un++;
            }
            p = un + 1;
            un = 0;
            while(token[p] != ':')
            {
                pass[un] = token[p];
                p++;
                un++;
            }
            p++;
            if((strcmp(username, uname) == 0) && (strcmp(password, pass) == 0))
            {
                char uid[8], gid[8], dir[32], cmd[32];
                int U, G;
                un = 0;
                // UID
                while(token[p] != ':')
                {
                    uid[un] = token[p];
                    p++;
                    un++;
                }
                un = 0;
                p++;
                // GID
                while(token[p] != ':')
                {
                    gid[un] = token[p];
                    p++;
                    un++;
                }
                p++;
                un = 0;
                U = atoi(uid);
                G = atoi(gid);
                // home dir
                while(token[p] != ' ')
                {
                    p++;
                }
                while(token[p] != ':')
                {
                    p++;
                }
                p++;
                while(token[p] != ':')
                {
                    dir[un] = token[p];
                    un++;
                    p++;
                }
                p++;
                un = 0;
                // command
                while(token[p])
                {
                    cmd[un] = token[p];
                    un++;
                    p++;
                }
                chuid(U,G);
                chdir(dir);

                close(fd);

                exec("sh");
                return 1;
                //prints(cmd);
                //prints(dir);
            }
            memset(uname, 0, 64);
            memset(pass, 0, 64);
        }
        memset(password, 0, 64);
        memset(username, 0, 64);
    }
}
