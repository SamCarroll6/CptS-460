#include "ucode.c"

int main(int argc, char *argv[ ])
{
    char buf1[1024], grab, ttyget[32];
    int fd, n, i = 0;
    prints("****************************\n");
    prints("        Sams Cool Cat       \n");
    prints("****************************\n");

    if(argc > 1)
    {
        fd = open(argv[1], O_RDONLY);
        if(fd >= 0)
        {
            while(n = read(fd, buf1, 1024))
            {
                for(i = 0; i < n; i++)
                {
                    write(1, &buf1[i], 1);
                    if(buf1[i] == '\n')
                    {
                        write(1, "\r", 1);
                    }
                }
                // write(1, buf1, n);
                // write(1, "\r", 1);
                //prints(buf1);
            }
            close(fd);
        }
    }
    else
    {
        prints("cat from stdin\n");
        while(1)
        {
            n = read(0, &grab, 1);
            if(n < 1)
            {
                break;
            }
            // if(grab != '\r')
            //     write(1, &grab, 1);
            if(grab == '\r')
            {
                // write(1, "\n", 1);
                // write(1, &grab, 1);
                write(1, buf1, i);
                memset(buf1, 0, 1024);
                // if(buf1[i] != '\n')
                //     write(1, "\n", 1);
                i = -1;
               // write(1, "\n", 1);
                write(1, "\r", 1);
            }
            else
            {
                buf1[i] = grab;
            }
            i++;
        }
    }
    
}
