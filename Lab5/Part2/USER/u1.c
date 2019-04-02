#include "ucode.c"

int argc;
char *argv[32];



int main0(char *cmdline)
{
  printf("%s\n", cmdline);
  main(0, argv);
}

int main(int argc, char *argv[])
{
  printf("HI\n");
  ubody("one");
}
 
