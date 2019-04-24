#include "ucode.c"

int main(int argc, char *argv[ ])
{
    char buf[1024], grab;
    int fd, n, i = 0;
    prints("****************************\n");
    prints("        Sams Cool Cat       \n");
    prints("****************************\n");
    if(argc > 1)
    {
        fd = open(argv[1], O_RDONLY);
        while(n = read(fd, buf, 1024))
        {
            prints(buf);
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
            if(grab != '\r')
                mputc(grab);
            buf[i] = grab;
            i++;
            if(grab == '\r')
            {
                mputc('\n');

                buf[i] = '\0';
                buf[i-1] = '\0';
                // write(1, '\n', 1);
                // write(1, '\r', 1);
                write(1, buf, i);
                // write(1, '\n', 1);
                // write(1, '\r', 1);
                //mputc('\r');
                //buf[i] = '\0';
                // prints(buf);
                // mputc(buf[0]);
                //write(1, buf, i + 1);
                memset(buf, 0, 1024);
                i = 0;
                mputc('\n');
            }
        }
    }
    
}
