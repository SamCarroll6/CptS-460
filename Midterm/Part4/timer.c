#define TLOAD 0x0
#define TVALUE 0x1
#define TCNTL 0x2
#define TINTCLR 0x3
#define TRIS 0x4
#define TMIS 0x5
#define TBGLOAD 0x6

typedef volatile struct timer{
    u32 *base; // timers base address
    int tick, hh, mm, ss; // per timer data area
    char clock[16];
}TIMER;

volatile TIMER timer[4]; // 4 timers, 2 per unit, at 0x00 and 0x20

void timer_init()
{
    int i;
    TIMER *tp;
    kprintf("timer_init()\n");
    for(i = 0; i < 4; i++)
    {
        tp = &timer[i];
        if(i==1)
            tp->base = (u32 *)0x101E2000;
        if(i==2)
            tp->base = (u32 *)0x101E2020;
        if(i==3)
            tp->base = (u32 *)0x101E3000;
        if(i==4)
            tp->base = (u32 *)0x101E3020;
        *(tp->base+TLOAD) = 0x0; //reset
        *(tp->base+TVALUE) = 0xFFFFFFFF;
        *(tp->base+TCNTL) = 0x0;
        *(tp->base+TINTCLR) = 0x0;
        *(tp->base+TRIS) = 0x100;
        *(tp->base+TMIS) = 0x66;
        *(tp->base+TBGLOAD) = 0x1C00;
        tp->tick = tp->hh = tp->mm = tp->ss = 0;
        strcpy2((char *)tp->clock, "00:00:00");
    }
}

void timer_start(int n)
{
    TIMER *tp = &timer[n];
    kprintf("timer_start %d base=%x\n", n, tp->base);
    *(tp->base+TCNTL) |= 0x80; 
    
}

int timer_clearinterrupt(int n)
{
    TIMER *tp = &timer[n];
    *(tp->base+TINTCLR) = 0xFFFFFFFF;
}

void timer_stop(int n)
{
    TIMER *tp = &timer[n];
    *(tp->base+TCNTL) &= 0x7F;
}

void timer_handler(int n)
{
    int i;
    TIMER *t = &timer[n];
    t->tick++; // assume 120 ticks per second
    if(t->tick == 120)
    {
        t->tick = 0;
        t->ss++;
        if(t->ss == 60)
        {
            t->ss = 0;
            t->mm++;
            if(t->mm == 60)
            {
                t->mm = 0;
                t->hh++;
            }
        }
        t->clock[7]='0' + (t->ss%10); t->clock[6] = '0' + (t->ss/10);
        t->clock[4] = '0' + (t->mm%10); t->clock[3] = '0' + (t->mm/10);
        t->clock[1] = '0' + (t->hh%10); t->clock[3] = '0' + (t->hh/10);
    }
    color = n;
    for(i = 0; i < 8; i++)
    {
        kprintf("HELLO %s", t->clock[i]);
        kpchar(t->clock[i], n, 70+i);
    }
    kprintf("\n");
    timer_clearinterrupt(n);
}