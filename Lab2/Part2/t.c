#include "defines.h"
#include "vid.c"
#include "uart.c"

int color;
char *tab = "0123456789ABCDEF";
//int reduce;

UART uart[4];

extern char _binary_wsu_bmp_start;

int color;
UART *up;

int main()
{
   char c, *p;
   int mode, i = 1;
   uart_init();
   up = &uart[0];

   mode = 0;
   fbuf_init(mode);

   p = &_binary_wsu_bmp_start;
   //reduce = i;
   show_bmp(p, 0, 0, i); 

   while(1){
     if(i > 4)
     {
         i = 1;
     }
     //kprintf("enter a key from this UART : ");
     ugetc(up);
     p = &_binary_wsu_bmp_start;
     show_bmp(p, 0, 0, i);
     i++;
   }
//    while(1);   // loop here  
}