typedef struct semaphore{
  int value;
  PROC *queue;
}SEMAPHORE;

int show_buffer();

int P(struct semaphore *s)
{
  s->value--;
  if (s->value < 0){
    kprintf("proc %d BLOCK\n", running->pid);
    show_buffer();
    running->status = BLOCK;
    enqueue(&s->queue, running);
    tswitch();
  }
}

int V(struct semaphore *s)
{
  PROC *p;
  s->value++;
  if (s->value <= 0){
    p = dequeue(&s->queue);
    p->status = READY;
    enqueue(&readyQueue, p);
    kprintf("V-up %d\n", p->pid);
    show_buffer();
  }
}

#define BSIZE  8

typedef struct buffer{
  char buf[BSIZE];
  int head, tail;
  struct semaphore data, room;
  struct semaphore mutex;
}BUFFER;

BUFFER buffer;

int show_buffer()
{
  BUFFER *p = &buffer;
  int i;
  kprintf("------------ BF -----------------\n");
  kprintf("room=%d data=%d buf=", p->room.value, p->data.value);
  for (i=0; i<p->data.value; i++)
    kputc(p->buf[p->tail+i]);
  kprintf("\n");
  kprintf("----------------------------------\n");
}

int buffer_init()
{
  int i;
  BUFFER *p = &buffer;
  p->head = p->tail = 0;
  p->data.value = 0;      p->data.queue = 0;
  p->room.value = BSIZE;  p->room.queue = 0;
  p->mutex.value = 1;     p->mutex.queue = 0;
}

int produce(char c)
{
  BUFFER *p = &buffer;
  P(&p->room);
  P(&p->mutex);
  p->buf[p->head++] = c;
  p->head %= BSIZE;
  V(&p->mutex);
  V(&p->data);
}

int consume()
{
  int c;
  BUFFER *p = &buffer;
  P(&p->data);
  P(&p->mutex);
  c = p->buf[p->tail++];
  p->tail %= BSIZE;
  V(&p->mutex);
  V(&p->room);
  return c;
}
 
int consumer()
{
  char line[128];
  int nbytes, n, i;

  kprintf("proc %d as consumer\n", running->pid);
 
  while(1){
    kprintf("input nbytes to read : " );
    kgets(line);
    line[strlen(line)-1] = 0;
    nbytes = getint();
    show_buffer();
    for (i=0; i<nbytes; i++){
       line[i] = consume();
       kprintf("%c", line[i]);
    }
    kprintf("\n");
    show_buffer();
    kprintf("consumer %d got n=%d bytes : line=%s\n", running->pid, n, line);
  }
}

int producer()
{
  char line[128];
  int nbytes, n, i;

  kprintf("proc %d as producer\n", running->pid);

  while(1){
    kprintf("input a string to produce : " );
    
    kgets(line);
    line[strlen(line)-1] = 0;

    nbytes = strlen(line);
    kprintf("nbytes=%d line=%s\n", nbytes, line);
    show_buffer();
    for (i=0; i<nbytes; i++){
      produce(line[i]);
    }
    show_buffer();
    kprintf("producer %d put n=%d bytes\n", running->pid, nbytes);
  }
}


