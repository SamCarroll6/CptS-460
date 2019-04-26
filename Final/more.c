#include "ucode.c"

// line = 24
int main(int argc, char *argv[ ])
{
    int fd, n, count = 0, n2, ttyo, ttyi, i = 0;
    char grab, tty[32], get, buf1[1024];
    prints("****************************\n");
    prints("          Sams More         \n");
    prints("****************************\n");

    gettty(tty);
    if(argc == 1)
    {
        while(1)
        {
            n = read(0, &grab, 1);
            if(n < 1)
            {
                break;
            }
            if(grab != '\r')
                write(1, &grab, 1);
            if(grab == '\r')
            {
                write(1, "\n", 1);
                write(1, &grab, 1);
                write(1, buf1, i);
                memset(buf1, 0, 1024);
                i = 0;
                write(1, "\n", 1);
                write(1, "\r", 1);
            }
            buf1[i] = grab;
            i++;
        }
        return 1;
    }
    else if(argc < 2)
    {
        prints("more: bad usage, EX more [ filename ]\n");
        return -1;
    }
    fd = open(argv[1], O_RDONLY);
    if(fd < 0)
    {
        printf("more: File %s could not be found\n", argv[1]);
        return -1;
    }
    ttyi = open(tty, O_RDONLY);
    // ttyo = open(tty, O_WRONLY);
    if(fd >= 0)
    {
        while(n = read(fd, &grab, 1))
        {
            if(count < 24)
            {
                mputc(grab);
                if(grab == '\n')
                {
                    count++;
                }
            }
            while(count >= 24)
            {
                n2 = read(ttyi, &get, 1);
                if(get == ' ')
                {
                    count = 0;
                    break;
                }
                else if(get == '\r')
                {
                    count--;
                    break;
                }
            }
        }
    }
}
