#include "ucode.c"

char l2u(char val)
{
    if(val >= 'a' && val <= 'z')
    {
        val = val - 32;
    }
    return val;
}

int main(int argc, char *argv[ ])
{
    prints("****************************\n");
    prints("          Sams l2u          \n");
    prints("****************************\n");

    int src, dest, count, i = 0, n;
    char buf[1024], grab;

    if(argc == 2)
    {
        prints("l2u: Invalid number of parameters, must have src and dest or no parameters.\n");
        return -1;
    }
    if(argc > 2)
    {
        src = open(argv[1], O_RDONLY);
        dest = open(argv[2], O_WRONLY | O_CREAT);
        while(count = read(src, buf, 1024))
        {
            for(i = 0; i < count; i++)
            {
                buf[i] = l2u(buf[i]);
            }
            write(dest, buf, count);
        }
    }
    else
    {
        prints("l2u from stdin\n");
        while(1)
        {
            n = read(0, &grab, 1);
            if(n < 1)
            {
                break;
            }
            if(grab != '\r')
                mputc(l2u(grab));
            if(grab == '\r')
            {
                mputc('\n');
            }
        }
    }
    
}
