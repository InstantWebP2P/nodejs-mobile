#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"

static uint64_t rdtsc()
{
   uint64_t x;

   #ifdef IA32
      uint32_t lval, hval;
      asm volatile ("rdtsc" : "=a" (lval), "=d" (hval));
      x = hval;
      x = (x << 32) | lval;
   #elif defined(IA64)
      asm ("mov %0=ar.itc" : "=r"(x) :: "memory");
   #elif defined(AMD64)
      uint32_t lval, hval;
      asm ("rdtsc" : "=a" (lval), "=d" (hval));
      x = hval;
      x = (x << 32) | lval;
   #else
      x = 0;
   #endif
  
   return x;
}

int main(int argc, char *argv[]) {
    uint64_t t1,t2;

    t1 = rdtsc();
    usleep(1);
    t2 = rdtsc();
    
    printf("t1:0x%llx,t2:0x%llx,t2-t1:%lld\n",t1,t2,t2-t1);

    return 0;
}

