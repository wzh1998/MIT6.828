#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
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

uint64
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

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

#ifdef LAB_PGTBL
// system call that reports which pages have been accessed
uint64
sys_pgaccess(void)
{
  // define user page's virtual addr (usr_va), and physical addr of access bits stored in user space (usr_pa)
  uint64 usr_va, usr_pa;
  int page_num;

  // parse three input arguments
  // 1st: the starting virtual addr of the 1st user page
  // 2nd: the number of pages to check
  // 3rd: the user addr to the buffer, which will store the the bitmask result
  if(argaddr(0, &usr_va) < 0) return -1;
  if(argint(1, &page_num) < 0) return -1;
  if(argaddr(2, &usr_pa)) return -1;

  // check if the number of pages is valid 
  // since we are using uint64 to store the bits, the max page allowed to check is 64.
  if(page_num > 64) return -1;

  // store the mask in a temp buffer in the kernel, then copy it out to user space
  pte_t* pte;
  struct proc *p = myproc();
  unsigned int ker_abits = 0;

  // note: the page corresponds to usr_va will not be checked, checking starts at page with index 1.
  for(int i = 1; i <= page_num; i++) {
    pte = walk(p->pagetable, usr_va + i*PGSIZE, 0);
    // set the corresponding bit in the temp buffer, first page corresponds to LSB
    if((*pte & PTE_V) && (*pte & PTE_U)) {
      if(*pte & PTE_A){
        ker_abits |= 1 << i;
        continue;
      }
      // clear access bit after accessed the PTE
      *pte &= ~PTE_A;
    }
  }

  // copy the access bits from kernel space to user space
  if(copyout(p->pagetable, usr_pa, (char *)&ker_abits, sizeof(ker_abits)) < 0)
    return -1;

  return 0;
}

#endif