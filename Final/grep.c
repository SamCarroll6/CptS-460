#include "ucode.c"

int main(int argc, char *argv[ ])
{
    prints("****************************\n");
    prints("          Sams Grep         \n");
    prints("****************************\n");
    int fd, count, i = 0, c = 0, n = 0, check = 0, c2 = 0;
    char buf1[1024], pattern[64], grab;
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
                    break;

                line[i] = grab;
                
                if(grab == 10)
                {
                    line[++i] = 13;
                    if(check)
                    {
                        write(1, line, i);
                        //write(1, "\n", 1);
                        //write(1, "\r", 1);
                    }
                    memset(line, 0, 1024);
                    check = 0;
                }
                //write(1, buf1, 1);
                if(line[i] == pattern[n])
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
                i++;
        }
        // while(1)
        // {
        //     count = read(0, &grab, 1);
        //     if(count < 1)
        //     {
        //         break;
        //     }
        //     if(grab != '\r')
        //         write(1, grab, 1);
        //     if(grab == pattern[n])
        //     {
        //         n++;
        //         if(!pattern[n])
        //         {
        //             check = 1;
        //         }
        //     }
        //     else
        //     {
        //         n = 0;
        //     }            
        //     buf1[i] = grab;
        //     i++;
        //     if(grab == '\r')
        //     {
        //         write(1, "\n", 1);

        //         buf1[i] = '\0';
        //         buf1[i-1] = '\0';
        //         // write(1, '\n', 1);
        //         // write(1, '\r', 1);
        //         if(check)
        //         {
        //             write(1, buf1, i);
        //             write(1, "\n", 1);
        //         }
        //         // write(1, '\n', 1);
        //         // write(1, '\r', 1);
        //         //mputc('\r');
        //         //buf1[i] = '\0';
        //         // prints(buf1);
        //         // mputc(buf1[0]);
        //         //write(1, buf1, i + 1);
        //         memset(buf1, 0, 1024);
        //         i = 0;
        //         check = 0;
        //         n = 0;
        //     }
        // }
    }
    else
    {
        strcpy(pattern, argv[1]);
        fd = open(argv[2], O_RDONLY);
        if(fd >= 0)
        {
            while(1)
            {
                count = read(fd, &grab, 1);
                if(count < 1)
                {
                    break;
                }

                line[i] = grab;
                    
                if(grab == 10)
                {
                    line[++i] = 13;
                    if(check)
                    {
                        write(1, "\r", 1);
                        //write(1, "\r", 1);
                        write(1, line, i);
                        //write(1, "\n", 1);
                        write(1, "\r", 1);
                    }
                    memset(line, 0, 1024);
                    check = 0;

                }
                    //write(1, buf1, 1);
                if(line[i] == pattern[n])
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
                i++;
            }
        }
        else
        {
            printf("grep: unable to open file %s\n", argv[2]);
            return -1;
        }
    }
}
