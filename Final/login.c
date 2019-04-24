#include "ucode.c"

//int in, out, err;
//char name[128], password[128];

int main(int argc, char *argv[ ])
{
    // prints("****************************\n");
    // prints("          Sams Login        \n");
    // prints("****************************\n");    
    int in, out, err, fd;
    char username[64], password[64];

    close(0);
    close(1);

    in = open(argv[1], 0);
    out = open(argv[1], 1);
    err = open(argv[1], 2);
    
    fixtty(argv[1]);

    fd = open("/etc/passwd", O_RDONLY);

    while(1)
    {
        prints("login:\r");
        gets(username);
        prints("password:\r");
        gets(password);
    }
}
