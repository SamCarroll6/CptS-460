int tswitch();

// function to return an int for pip_reader since scanf doesn't work
int getint()
{
  char num[50];
  int i;
  int ret = 0;
  kgets(num);
  for(i = 0; i < 50; i++)
  {
    if(num[i] == '\0')
      break;
    ret = 10 * ret + (num[i] + '0');
  }
  return ret;
}



int wait(PROC *cur, int *status)
{
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
      return zpid;
    }
    p = p->sibling;
  }
  ksleep(cur);
}


int ksleep(int event)
{
  kprintf("proc %d going to sleep on event=%d\n", running->pid, event);

  running->event = event;
  running->status = SLEEP;
  enqueue(&sleepList, running);
  printList("sleepList", sleepList);
  tswitch();
}

int kwakeup(int event)
{
  PROC *temp, *p;
  temp = 0;
  printList("sleepList", sleepList);

  while (p = dequeue(&sleepList)){
     if (p->event == event){
	kprintf("wakeup %d\n", p->pid);
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
  kprintf("proc %d in kexit(), value=%d\n", running->pid, exitValue);
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
}


  
