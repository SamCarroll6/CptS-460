/***************** queue.c file *****************/
int enqueue(PROC **queue, PROC *p) 
{
  PROC *q = *queue;
  if (q == 0 || p->priority > q->priority){
     *queue = p;
     p->next = q;
  }
  else{
     while (q->next && p->priority <= q->next->priority)
            q = q->next;
     p->next = q->next;
     q->next = p;
  }
}
PROC *dequeue(PROC **queue) 
{
     PROC *p = *queue;
     if (p)
        *queue = (*queue)->next;
     return p;
}
int printList(char *name, PROC *p) 
{
  printf("%s = ", name);
  while(p){
     printf("[%d %d]->", p->pid, p->priority);
     p = p->next;
  }
  printf("NULL\n");
}

int printSleep(char *name, PROC *p) 
{
  printf("%s = ", name);
  while(p){
     printf("[%d event=%d]->", p->pid, p->event);
     p = p->next;
  }
  printf("NULL\n");
}

int addChild(PROC *kid)
{
  PROC *hold = running->child;
  if(hold)
  {
    while (hold->sibling)
    {
      hold = hold->sibling;
    }
    hold->sibling = kid;
    kid->parent = running;
    return 1;
  }
  running->child = kid;
  kid->parent = running;
  return 1;
}

int printBody()
{
  char *status[ ] = {"FREE", "READY", "SLEEP", "ZOMBIE"};
  printf("  ChildList = ");
  if(running)
  {
    PROC *p = running->child;
    // printf("[%d  %s]->\n", p->pid, status[p->status]);
    // p = p->sibling;
    while (p)
    {
      printf("[%d  %s]->", p->pid, status[p->status]);
      p = p->sibling;
    }
    printf("NULL\n");
    return 1;
  }
  putchar('\n');
  return -1;
}