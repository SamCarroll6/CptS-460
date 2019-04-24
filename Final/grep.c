#include "ucode.c"

int main(int argc, char *argv[ ])
{
    prints("****************************\n");
    prints("          Sams Grep         \n");
    prints("****************************\n");
    int fd, count, i = 0, c = 0, n = 0, check = 0, c2 = 0;
    char buf[1024], pattern[64], grab;
    char line[1024];
    if(argc < 2)
    {
        prints("grep: command error, EX: grep pattern [filename]\n");
        return -1;
    }
    else if(argc == 2)
    {
        strcpy(pattern, argv[1]);
        prints("grep from stdin\n");
        while(1)
        {
            count = read(0, &grab, 1);
            if(count < 1)
            {
                break;
            }
            if(grab != '\r')
                mputc(grab);
            if(grab == pattern[n])
            {
                n++;
                if(!pattern[n])
                {
                    check = 1;
                }
            }
            else
            {
                n = 0;
            }            
            buf[i] = grab;
            i++;
            if(grab == '\r')
            {
                mputc('\n');

                buf[i] = '\0';
                buf[i-1] = '\0';
                // write(1, '\n', 1);
                // write(1, '\r', 1);
                if(check)
                {
                    write(1, buf, i);
                    mputc('\n');

                }
                // write(1, '\n', 1);
                // write(1, '\r', 1);
                //mputc('\r');
                //buf[i] = '\0';
                // prints(buf);
                // mputc(buf[0]);
                //write(1, buf, i + 1);
                memset(buf, 0, 1024);
                i = 0;
                check = 0;
                n = 0;
            }
        }
    }
    else
    {
        strcpy(pattern, argv[1]);
        fd = open(argv[2], O_RDONLY);
        if(fd >= 0)
        {
            while(count = read(fd, buf, 1024))
            {
                for(i = 0; i < count; i++)
                {
                    line[c] = buf[i];
                    if(line[c] == '\n')
                    {
                        if(check)
                        {
                            prints(line);
                            mputc('\r');
                            check = 0;
                        }
                        memset(line, 0, c);
                        c = 0;
                        c2 = 1;
                    }
                    if(line[c] == pattern[n])
                    {
                        n++;
                        if(!pattern[n])
                        {
                            check = 1;
                        }
                    }
                    else
                    {
                        n = 0;
                    }
                    if(c2)
                    {
                        c = 0;
                        c2 = 0;
                    }
                    else
                    {
                        c++;
                    }
                }
            }
        }
        else
        {
            printf("grep: unable to open file %s\n", argv[2]);
            return -1;
        }
    }
}
