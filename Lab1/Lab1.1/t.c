int prints(char *s)
{
    call putc(c) to print string s;
}

int gets(char s[ ])
{
    call getc() to input a string into s[ ]
}

main()
{
   char name[64];
   while(1){
     prints("What's your name? ");
     gets(name);
     if (name[0]==0)
        break;
     prints("Welcome "); prints(name); prints("\n\r");
   }
   prints("return to assembly and hang\n\r");
}