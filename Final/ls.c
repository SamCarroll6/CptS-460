#include "ucode.c"

int ls_file(char *name, struct stat *stats)
{

}

int main(int argc, char *argv[ ])
{
    char diropen[64];
    int fd, num;
    char buf[1024];
    char *cp;
    DIR *dp;

    if(argc > 1)
    {
        strcpy(diropen, argv[1]);
    }
    else
    {
        getcwd(diropen);
    }
    fd = open(diropen, O_RDONLY);

    if(fd < 0)
    {
        printf("File could not be open\n");
        return 0;
    }


}
