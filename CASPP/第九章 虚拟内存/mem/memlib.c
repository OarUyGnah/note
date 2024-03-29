/*
 * memlib.c - a module that simulates the memory system.  Needed
 * because it allows us to interleave calls from the student's malloc
 * package with the system's malloc package in libc.
 *
 */
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "csapp.h"
#include "memlib.h"

#define MAX_HEAP 8 * 1024 * 1024

/* $begin memlib */
/* Private global variables */
static char* mem_heap; /* Points to first byte of heap */
static char* mem_brk; /* Points to last byte of heap plus 1 */
static char* mem_max_addr; /* Max legal heap addr plus 1*/

/*
 * mem_init - Initialize the memory system model
 */
void mem_init(void)
{
    mem_heap = (char*)malloc(MAX_HEAP);
    mem_brk = (char*)mem_heap;
    mem_max_addr = (char*)(mem_heap + MAX_HEAP);
    fprintf(stderr,"mem_heap = 0x%x\tmem_max_addr = 0x%x\n",mem_heap,mem_max_addr);
}

/*
 * mem_sbrk - Simple model of the sbrk function. Extends the heap
 *    by incr bytes and returns the start address of the new area. In
 *    this model, the heap cannot be shrunk.
 */
 // 只能申请小于MAX_HEAP大小的内存，即只能申请mem_init申请的内存，超出申请的则报错
void* mem_sbrk(int incr)
{
    char* old_brk = mem_brk;
    fprintf(stderr,"incr is %d\n",incr);
    if ((incr < 0) || ((mem_brk + incr) > mem_max_addr)) {
        errno = ENOMEM;
        fprintf(stderr, "ERROR: mem_sbrk failed. Ran out of memory...\n");
        return (void*)-1;
    }
    mem_brk += incr;
    return (void*)old_brk;
}
/* $end memlib */

/*
 * mem_deinit - free the storage used by the memory system model
 */
void mem_deinit(void)
{
}

/*
 * mem_reset_brk - reset the simulated brk pointer to make an empty heap
 */
void mem_reset_brk()
{
    mem_brk = (char*)mem_heap;
}

/*
 * mem_heap_lo - return address of the first heap byte
 */
void* mem_heap_lo()
{
    return (void*)mem_heap;
}

/*
 * mem_heap_hi - return address of last heap byte
 */
void* mem_heap_hi()
{
    return (void*)(mem_brk - 1);
}

/*
 * mem_heapsize() - returns the heap size in bytes
 */
size_t mem_heapsize()
{
    return (size_t)((void*)mem_brk - (void*)mem_heap);
}

/*
 * mem_pagesize() - returns the page size of the system
 */
size_t mem_pagesize()
{
    return (size_t)getpagesize();
}