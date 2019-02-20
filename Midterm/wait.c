int tswitch();

int wait(PROC *cur, int *status)
{
  int sr = int_off();
  PROC *p = cur->child;
  int zpid;
  if(p == 0)
  {
    return -1;
  }
  while(p)
  {
    if(p->status == ZOMBIE)
    {
      zpid = p->pid;
      *status = p->exitCode;
      p->ppid = 0;
      p->status = FREE;
      removeChild(p->pid, cur->pid);
      enqueue(&freeList, p);
      int_on(sr);
      return zpid;
    }
    p = p->sibling;
  }
  int_on(sr);
  ksleep(cur);
}


int ksleep(int event)
{
  int sr = int_off();
  printf("proc %d going to sleep on event=%d\n", running->pid, event);

  running->event = event;
  running->status = SLEEP;
  enqueue(&sleepList, running);
  printList("sleepList", sleepList);
  tswitch();
  int_on(sr);
}

int kwakeup(int event)
{
  PROC *temp, *p;
  temp = 0;
  int sr = int_off();
  
  printList("sleepList", sleepList);

  while (p = dequeue(&sleepList)){
     if (p->event == event){
	printf("wakeup %d\n", p->pid);
	p->status = READY;
	enqueue(&readyQueue, p);
     }
     else{
	enqueue(&temp, p);
     }
  }
  sleepList = temp;
  printList("sleepList", sleepList);
  int_on(sr);
}

int kexit(int exitValue)
{
  int check = 0;
  printf("proc %d in kexit(), value=%d\n", running->pid, exitValue);
  PROC *p = running->child;
  addChild(p, 1);
  running->child = 0;
  while(p)
  {
    p->ppid = 1;
    p->parent = &proc[1];
    p = p->sibling;
    check = 1;
  }
  running->exitCode = exitValue;
  running->status = ZOMBIE;
  if(running->parent->status == SLEEP)
  {
    int stat;
    kwakeup(running->parent);
    wait(running->parent, &stat);
  }
  if(check == 1)
  {
    kwakeup(&proc[1]);
  }
  tswitch();

  /*******************************************************/
  printf("proc %d in kexit(), value=%d\n", running->pid, exitValue);
  running->exitCode = exitValue;
  running->status = ZOMBIE;
  tswitch();
}


  
