/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
/**************************
 need to catch Control-C, Contorl-D keys
 so need to recognize LCTRL key pressed and then C or D key
******************************/

#include "keymap"
#include "keymap2"

#define LSHIFT 0x12
#define RSHIFT 0x59
#define ENTER  0x5A
#define LCTRL  0x14
#define RCTRL  0xE014

#define ESC    0x76
#define F1     0x05
#define F2     0x06
#define F3     0x04
#define F4     0x0C

#define KCNTL 0x00
#define KSTAT 0x04
#define KDATA 0x08
#define KCLK  0x0C
#define KISTA 0x10

typedef volatile struct kbd{
  char *base;
  char buf[128];
  int head, tail;
  int data, room;
}KBD;

int held[128] = {0};

volatile KBD kbd;
int shifted = 0;
int release = 0;
int control = 0;

volatile int keyset;
int kputc(char);
extern int color;

int kbd_init()
{
  char scode;
  keyset = 1; // default to scan code set-1
  
  KBD *kp = &kbd;
  kp->base = (char *)0x10006000;
  *(kp->base + KCNTL) = 0x10; // bit4=Enable bit0=INT on
  *(kp->base + KCLK)  = 8;
  kp->head = kp->tail = 0;

  kp->data = 0;
  kp->room = 128;
   
  shifted = 0;
  release = 0;
  control = 0;

  printf("Detect KBD scan code: press the ENTER key : ");
  while( (*(kp->base + KSTAT) & 0x10) == 0);
  scode = *(kp->base + KDATA);
  printf("scode=%x ", scode);
  if (scode==0x5A)
    keyset=2;
  printf("keyset=%d\n", keyset);
}


// kbd_handler1() for scan code set 1
void kbd_handler1()
{
  u8 scode, c;
  KBD *kp = &kbd;

  scode = *(kp->base + KDATA);

  //printf("scan code = %x ", scode);
  
  if (scode & 0x80)
    return;
  c = unsh[scode];

  if (c == '\r')
     kputc('\n');
  kputc(c);

  kp->buf[kp->head++] = c;
  kp->head %= 128;

  kp->data++;
  kp->room--;
  wakeup(&kp->data);
}

// kbd_handelr2() for scan code set 2
void kbd_handler2()
{
  u8 scode, c;
  KBD *kp = &kbd;

  scode = *(kp->base + KDATA);

  //printf("scan code = %x ", scode);
  
  if (scode == 0xF0){       // key release 
     //release = 1;           // set flag
     return;
  }
  
  if (kp->data == 128) // if input buffer FULL
  {
    return;           // ignore current key
  }

  if (held[scode] == 1 && scode){    // next scan code following key release
     held[scode] = 0;
     //release = 0;           // clear flag 
     return;
  }

  held[scode] = 1;

  if(scode == 0x12 || scode == 0x59 || scode == 0x14)
    return;

  if(held[0x14] == 1 && ltab[scode] == 'c') // if control held and 'c' pressed
  {
    printf("Control-c Key\n");
    return;
  }
  else if(held[0x14] == 1 && ltab[scode] == 'd') // if control held and 'd' pressed
  {
     c = 0x04;
  }
  else if (held[0x12] == 1 || held[0x59] == 1)  // If shift key held
  {         
     c = utab[scode];
  }
  else  // No significant keys held               
  {
     c = ltab[scode];
  }

  if (c == '\r')
     kputc('\n');
  kputc(c);

  kp->buf[kp->head++] = c;
  kp->head %= 128;

  kp->data++;
  kp->room--;
  wakeup(&kp->data);
  // YOUR kbd handler for SET 2 scan code 
}

void kbd_handler()
{
  if (keyset == 1)
    kbd_handler1();
  else
    kbd_handler2();
}

int kgetc()
{
  char c;
  KBD *kp = &kbd;
  
  while(1)   // BUSY wait for data
  {
  lock();   // disable IRQ interrupts
   if(kp->data == 0)  // Check data with IRQ disabled
   {
     unlock();  //enable IRQ interrupts
     sleep(&kp->data);  // sleep for data
   }
   else{
    break;
   }
  }
   c = kp->buf[kp->tail++]; // get a c and update tail index
   kp->tail %= 128;
   kp->data--; // Update with interrups off
   kp->room++;
  unlock(); // enable IRQ interrupts
  
  return c;
}

int kgets(char s[ ])
{
  char c;
  KBD *kp = &kbd;
  
  while( (c = kgetc()) != '\r'){
    if (c=='\b'){
      s--;
      continue;
    }
    *s++ = c;
  }
  *s = 0;
  return strlen(s);
}
