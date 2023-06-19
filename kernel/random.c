#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "file.h"

//char random_seed = 0x2A;

struct {
  struct spinlock lock;
  
  char random_seed ;
} rand;


char get_random_seed()
{
    return rand.random_seed;
}

void set_random_seed(char seed)
{
    rand.random_seed = seed;
}


uint8 lfsr_char(uint8 lfsr)
{
    uint8 bit;
    bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 4)) & 0x01;
    lfsr = (lfsr >> 1) | (bit << 7);
    return lfsr;
}

int read(int fd,uint64 dst, int n){
    char *buffer = (char *)dst;
    char lfsr = get_random_seed(); 

    uint64 src;
    int bytes_written = 0;

    if(dst<KERNBASE || dst >= PHYSTOP)              // check wheter its not a valid address
        return bytes_written;
    acquire(&rand.lock);
    for (int i = 0; i < n; i++){

        buffer[i] = lfsr_char(lfsr);
        lfsr = buffer[i];
        bytes_written++;
    }
    release(&rand.lock);
    return bytes_written;
}

int write(int fd, uint64 src, int n)
{
    if (n != 1)
        return -1; 
    char byte = (char)src;
    acquire(&rand.lock);
    set_random_seed(byte);
    release(&rand.lock); 

    return 1; 
}

void
randominit(void)
{
    initlock(&rand.lock, "random");
    uartinit();

    devsw[RANDOM].read = read;
    devsw[RANDOM].write = write;
    set_random_seed(0x2A);  
}


    
