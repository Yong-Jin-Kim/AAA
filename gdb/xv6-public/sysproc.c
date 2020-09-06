#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached - Why?
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getppid(void)
{
    return getppid();
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_yield(void)
{
  yield();
  return 0;
}

int
sys_procdump(void)
{
  procdump();
  return 0;
}

int
sys_getlev(void)
{
  return myproc()->mlfqlev;
}

int
sys_set_cpu_share(void)
{
  int share;

  cli();
  if(argint(0, &share) < 0)
    return -1;
  if(share <= 0) return -1;
  //cprintf("SHARE %d\n", share);
  if(share + total_share > 80) {
    return -1;
  }
  myproc()->mlfqlev = -1;
  //myproc()->hitzone = 0;
  //myproc()->strike = 0;
  myproc()->is_stride = 1;
  myproc()->share = share;
  __sync_synchronize();
  set_stride();
  sti();
  yield();
  return 0;
}

int
sys_thread_join(void)
{
  thread_t thread;
  void **retval;
  if(argint(0, (int*)&thread) < 0)
    return -1;
  if(argptr(1, (char**)&retval, 4) < 0)
    return -1;
  //cprintf("------------------join %d\n", thread);
  return thread_join(thread, retval);
}

int
sys_thread_exit(void)
{
  void *retval;
  if(argint(0, (int*)&retval) < 0)
    return -1;
  thread_exit(retval);
  return 0;
}

int
sys_print_order(void)
{
  int a, b, c;
  if(argint(0, &a) < 0)
    return -1;
  if(argint(1, &b) < 0)
    return -1;
  if(argint(2, &c) < 0)
    return -1;
  cprintf("%d %d %d ", a, b, c);
  return 0;
}

int
sys_thread_create(void)
{
  thread_t *thread;
  void *(*start_routine)(void*);
  void *arg;

  if(argptr(0, (char**)&thread, sizeof(thread)) < 0)
  {
    //panic("SYSPROC A");
    return -1;
  }
  if(argptr(1, (char**)&start_routine, sizeof(start_routine)) < 0)
  {
    //panic("SYSPROC B");
    return -1;
  }
  if(argint(2, (int*)&arg) < 0)
  {
    //panic("SYSPROC C");
    return -1;
  }
  //return 0;
  return thread_create(thread, start_routine, arg);
}