int tswitch();

int wait(int *status)
{
  PROC *p = running->child;
  int zpid;
  if(p == NULL)
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
      removeChild(p->pid, running->pid);
      enqueue(&freeList, p);
      return zpid;
    }
    p = p->sibling;
  }
  sleep(running);
  return wait(status);
}

int sleep(int event)
{
  printf("proc %d going to sleep on event=%d\n", running->pid, event);

  running->event = event;
  running->status = SLEEP;
  enqueue(&sleepList, running);
  printList("sleepList", sleepList);
  tswitch();
}

int wakeup(int event)
{
  PROC *temp, *p;
  temp = 0;
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
}

int kexit(int exitValue)
{
  int check = 0;
  printf("proc %d in kexit(), value=%d\n", running->pid, exitValue);
  PROC *p = running->child;
  addChild(p, 1);
  running->child = NULL;
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
    wakeup(running->parent);
  }
  if(check == 1)
  {
    wakeup(&proc[1]);
  }
  tswitch();
}


  
