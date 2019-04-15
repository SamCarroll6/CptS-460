#include "header.h"

int main(int argc, char *argv[])
{
    char *diskname = "mydisk";
    char *input, *hold, **path;
    int (*Farr[20])(void) = {lsdo, chdir, pwd, quit, mdir, creat_file, rm_dir, mysymlink, link, touch, myunlink, mychmod, menu, preopen, pfd, pre_close, my_read, my_cat, write_file, my_cp};
    if(argc > 1)
        diskname = argv[1];
    fd = open(diskname, O_RDWR);
    if(fd < 0)
    {
        printf("Checking EXT2 FS... Open %s failed\n", diskname);
        exit(1);
    }
    init();
    mountroot(diskname);
    int ret;
    MINODE* pathfollow;
    while(1)
    {
        input = readinput();
        hold = Parse(input);
        // store an unmodified copy of paths[0]
        if(hold)
            strcpy(pathname, paths[0]);
        ret = tokenize(hold);
        int i = 0;
        if(input != 0)
        {
            int check = getarrayval(input);
            if(check >= 0)
                Farr[check]();
            else
            {
                printf("Error: Invalid command %s\n", input);
            }
        }
        i = 0;
        while(paths[i])
        {
            free(paths[i]);
            paths[i] = NULL;
            i++;
        }
        input = NULL;
        hold = NULL;
    }
}
