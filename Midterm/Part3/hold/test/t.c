/*********** t.c file of A Multitasking System *********/
// #include <stdio.h>
#include "type.h"

/******** function prototypes *****************/
int body();
int tswitch();
int do_exit();

PROC proc[NPROC];      // NPROC PROCs
PROC *freeList;        // freeList of PROCs 
PROC *readyQueue;      // priority queue of READY procs
PROC *running;         // current running proc pointer
PROC *sleepList;

#include "string.c"
#include "kbd.c"
#include "vid.c"
#include "exceptions.c"

#include "queue.c"     // include queue.c file
#include "wait.c"      // include wait.c file
#include "pipe.c"
#include "pv.c"


int procsize = sizeof(PROC);

int scheduler();

int kprintf(char *fmt, ...);


/*******************************************************
  kfork() creates a child process; returns child pid.
  When scheduled to run, child PROC resumes to body();
********************************************************/
void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;
    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}

void IRQ_handler()
{
    int vicstatus, sicstatus;
    int ustatus, kstatus;

    // read VIC SIV status registers to find out which interrupt
    vicstatus = VIC_STATUS;
    sicstatus = SIC_STATUS;  
    if (vicstatus & 0x80000000){ // SIC interrupts=bit_31=>KBD at bit 3 
       if (sicstatus & 0x08){
          kbd_handler();
       }
    }
}

// initialize the MT system; create P0 as initial running process
int init() 
{
  int i;
  PROC *p;
  for (i=0; i<NPROC; i++){ // initialize PROCs
    p = &proc[i];
    p->pid = i;            // PID = 0 to NPROC-1  
    p->status = FREE;
    p->priority = 0;      
    p->next = p+1;
  }
  proc[NPROC-1].next = 0;  
  freeList = &proc[0];     // all PROCs in freeList     
  readyQueue = 0;          // readyQueue = empty
  sleepList = 0;
  
  // create P0 as the initial running process
  p = running = dequeue(&freeList); // use proc[0] 
  p->status = READY;
  p->ppid = 0;             // P0 is its own parent

  p->parent = p;
  p->child = 0;
  p->sibling = 0;
  
  printList("freeList", freeList);
  kprintf("init complete: P0 running\n"); 
}

int INIT()
{
  int pid, status;
  PIPE *p = &pipe;
  kprintf("P1 running: create pipe and writer reader processes\n");
  kpipe();
  kfork(pipe_writer);
  kfork(pipe_reader);
  kprintf("P1 waits for ZOMBIE child\n");
  while(1){
    kprintf("%d\n", pid);
    pid = wait(running, &status);
    kprintf("%d\n", pid);
    if (pid < 0){

      kprintf("no more child, P1 loops\n");
      while(1);
    }
    kprintf("P1 buried a ZOMBIE child %d\n", pid);
  }
}
  
int kfork(int func)
{
  int  i;
  PIPE *pp = &pipe;
  PROC *p = dequeue(&freeList);
  if (!p){
     kprintf("no more proc\n");
     return(-1);
  }
  /* initialize the new proc and its stack */
  p->status = READY;
  p->priority = 1;       // ALL PROCs priority=1,except P0
  p->ppid = running->pid;
  p->parent = running;
  p->child = 0;
  p->sibling = 0;

  //enter_child(p);
  
  /************ new task initial stack contents ************
   kstack contains: |pid|exit|retPC|eax|ebx|ecx|edx|ebp|esi|edi|eflag|
                     -1   -2   -3   -4  -5  -6  -7  -8  -9  -10  -11
  **********************************************************/
  for (i=1; i<12; i++)                 // zero out kstack cells
      p->kstack[SSIZE - i] = 0;

  p->kstack[SSIZE-1] = (int)func;      // retPC -> func()
  p->ksp = &(p->kstack[SSIZE - 9]);   // PROC.ksp -> saved eflag 
  enqueue(&readyQueue, p);             // enter p into readyQueue
  addChild(p, running->pid);
  return p->pid;
}

/*************** main() function ***************/
int main()
{
  int pid, status;
  int i; 
   char line[128]; 
   u8 kbdstatus, key, scode;
   KBD *kp = &kbd;
   color = WHITE;
   row = col = 0; 
   kprintf("Welcome to the MT Multitasking System\n");
   fbuf_init();
   kbd_init();
   /* enable SIC interrupts */
   VIC_INTENABLE |= (1<<31); // SIC to VIC's IRQ31
   /* enable KBD IRQ */
   SIC_INTENABLE = (1<<3); // KBD int=bit3 on SIC
   SIC_ENSET = (1<<3);  // KBD int=3 on SIC
   *(kp->base+KCNTL) = 0x12;

   init();    // initialize system; create and run P0
   INIT();
   //kfork(INIT);

   printList("readyQueue", readyQueue);
   while(1){
     kprintf("P0 switch process\n");
     while(!readyQueue);
     tswitch();
   }
}

/*********** scheduler *************/
int scheduler()
{ 
  //  kprintf("proc %d in scheduler()\n", running->pid);
  if (running->status == READY)
     enqueue(&readyQueue, running);
  //printList("readyQueue", readyQueue);
  running = dequeue(&readyQueue);
  //kprintf("next running = %d\n", running->pid);  
}
