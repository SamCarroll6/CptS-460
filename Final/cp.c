#include "ucode.c"

int main(int argc, char *argv[ ])
{
    prints("****************************\n");
    prints("           Sams CP          \n");
    prints("****************************\n");

    int src, dest, count;
    char buf[1024];

    if(argc < 3)
    {
        prints("cp: missing destination file operand.\n");
        return -1;
    }
    src = open(argv[1], O_RDONLY);
    dest = open(argv[2], O_WRONLY | O_CREAT);

    while(count = read(src, buf, 1024))
    {
        write(dest, buf, count);
    }
    close(src);
    close(dest);
    return 1;
}
