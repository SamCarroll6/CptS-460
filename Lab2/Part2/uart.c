#define DR   0x00
#define FR   0x18

#define RXFE 0x10
#define TXFF 0x20


typedef struct uart{
  char *base;
  int n;
}UART;

UART uart[4];

int uart_init()
{
  int i; UART *up;

  for (i=0; i<4; i++){
    up = &uart[i];
    up->base = (char *)(0x101F1000 + i*0x1000);
    up->n = i;
  }
  uart[3].base = (char *)(0x10009000); // uart3 at 0x10009000
}

int ugetc(UART *up)
{
  while (*(up->base + FR) & RXFE);
  return *(up->base + DR);
}

int uputc(UART *up, char c)
{
  while(*(up->base + FR) & TXFF);
  *(up->base + DR) = c;
}

int ugets(UART *up, char *s)
{
  while ((*s = (char)ugetc(up)) != '\r'){
    uputc(up, *s);
    s++;
  }
 *s = 0;
}

int uprints(UART *up, char *s)
{
  while(*s)
    uputc(up, *s++);
}

int rpu(UART *up, u32 x)
{
    char c;
    if(x)
    {
        c = tab[x % 10];
        rpu(up, x / 10);
        uputc(up, c);
    }
}

int xrpu(UART *up, u32 x)
{
    char c;
    if(x)
    {
        c = tab[x % 16];
        rpu(up, x / 16);
        uputc(up, c);
    }
}

int uprinti(UART *up, int i)
{
    if(i < 0)
    {
        uputc(up, '-');
        i = i * (-1);
    }
    (i == 0) ? uputc(up, '0') : rpu(up, i);
}

int uprintuh(UART *up, u32 i)
{
    uputc(up, '0');
    uputc(up, 'x');
    (i == 0) ? uputc(up, '0') : xrpu(up, i);
}

int fuprintf(UART *up, char *fmt, ...)
{
    char *cp = fmt;
    int *ip = (int *)(&fmt + 1);
    while(*cp)
    {
        if(*cp != '%')
        {
            uputc(up, *cp);
        }
        else
        {
            cp++;
            switch(*cp)
            {
                case 'c':
                    uputc(up, (char)*ip);
                    break;
                case 's':
                    uprints(up, (char*)*ip);
                    break;
                case 'd':
                    uprinti(up, (int)*ip);
                    break;
                case 'u':
                    uprintuh(up, (u32)*ip);
                    break;
            }
            ip++;
        }
        cp++;
    }
}

int uprintf(char *fmt,...)
{
  int *ip;
  char *cp;
  cp = fmt;
  ip = (int *)&fmt + 1;

  UART *up = &uart[0];

  while(*cp){
    if (*cp != '%'){
      uputc(up, *cp);
      cp++;
      continue;
    }
    cp++;
    switch(*cp){
    case 'c': uputc(up, (char)*ip);      break;
    case 's': uprints(up, (char *)*ip);  break;
    case 'd': uprinti(up, *ip);          break;
    case 'x': uprintuh(up, *ip);          break;
    }
    cp++; ip++;
  }
}