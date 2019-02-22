#define NPIPE 10
#define PSIZE  8

typedef struct pipe{
  char buf[8];
  int head, tail;
  int data, room;
}PIPE;

int broke = 0;

PIPE pipe;

int show_pipe()
{
  PIPE *p = &pipe;
  int i;
  kprintf("----------------------------------------\n");
  kprintf("room=%d data=%d buf=", p->room, p->data);
  for (i=0; i<p->data; i++)
    kputc(p->buf[p->tail+i]);
  kprintf("\n");
  kprintf("----------------------------------------\n");
}
int kpipe()
{
  int i;
  PIPE *p = &pipe;
  p->head = p->tail = 0;
  p->data = 0; p->room = PSIZE;
}

int read_pipe(PIPE *p, char *buf, int n)
{
  int ret;
  char c;
  
  if (n<=0)
    return 0;
  show_pipe();

  while(n){
    kprintf("reader %d reading pipe\n", running->pid);
    ret = 0;
    while(p->data){
        *buf = p->buf[p->tail++];
        p->tail %= PSIZE;
        buf++;  ret++; 
        p->data--; p->room++; n--;
        if (n <= 0)
            break;
    }
    show_pipe();
    if (ret){   /* has read something */
       kwakeup(&p->room);
       return ret;
    }
    // pipe has no data
    kprintf("reader %d sleep for data\n", running->pid);
    kwakeup(&p->room);
    ksleep(&p->data);
    continue;
  }
}

int write_pipe(PIPE *p, char *buf, int n)
{
  char c;
  int ret = 0; 
  show_pipe();
  while (n){
    if(broke)
    {
        kprintf("Broken pipe, exiting write process\n");
        kexit(running);
    }
    kprintf("writer %d writing pipe\n", running->pid);
    while (p->room){
       p->buf[p->head++] = *buf; 
       p->head  %= PSIZE;
       buf++;  ret++; 
       p->data++; p->room--; n--;
       if (n<=0){
         show_pipe();
	 kwakeup(&p->data);
	 return ret;
       }
    }
    show_pipe();
    kprintf("writer %d sleep for room\n", running->pid);
    kwakeup(&p->data);
    ksleep(&p->room);
  }
}
 
int pipe_reader()
{
  char line[128];
  int nbytes, n;
  PIPE *p = &pipe;
  kprintf("proc %d as pipe reader\n", running->pid);
 
  while(1){
    kprintf("input nbytes to read : " );
    nbytes = geti();
    if(nbytes == 0)
    {
      broke = 1;
      kexit(running);
    }
    n = read_pipe(p, line, nbytes);
    line[n] = 0;
    kprintf("Read n=%d bytes from pipe: line=[ %s ]\n", n, line);
  }
}


int pipe_writer()
{
  char line[128];
  int nbytes, n;
  PIPE *p = &pipe;
  kprintf("proc %d as pipe writer\n", running->pid);
  kgets(line);
  while(1){
    kprintf("Enter a line to write or a NULL line to exit: " );

    kgets(line);
    line[strlen(line)] = 0;

    if (strcmp(line, "")==0)
    {
      kexit(running);
      continue;
    }

    nbytes = strlen(line);
    kprintf("nbytes=%d buf=%s\n", nbytes, line);
    n = write_pipe(p, line, nbytes);
    kprintf("wrote n=%d bytes\n", n);
  }
}


