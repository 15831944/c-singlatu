#pragma once
#include "Reply.h"
#include <stdio.h>  //处理文件
#include <sys/types.h>  
#include <sys/time.h>   
#include <string.h>  
#include <unistd.h>  
#include <stdlib.h>  
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/sysinfo.h>
#include <ctype.h>

#define xrealloc_vector(vector, shift, idx) \
    xrealloc_vector_helper((vector), (sizeof((vector)[0]) << 8) + (shift), (idx))

enum {
    MI_MEMTOTAL,
    MI_MEMFREE,
    MI_MEMSHARED,
    MI_SHMEM,
    MI_BUFFERS,
    MI_CACHED,
    MI_SWAPTOTAL,
    MI_SWAPFREE,
    MI_DIRTY,
    MI_WRITEBACK,
    MI_ANONPAGES,
    MI_MAPPED,
    MI_SLAB,
    MI_MAX
};

typedef struct jiffy_counts_t {
    /* Linux 2.4.x has only first four */
    unsigned long long usr, nic, sys, idle;
    unsigned long long iowait, irq, softirq, steal;
    unsigned long long total;
    unsigned long long busy;
} jiffy_counts_t;


class CAging :public CReply
{
public:
    CAging(int connect);

    bool doSend();

    
private:

    pthread_t start_aging;
    static int aging_on;
    int num_cpus;
    jiffy_counts_t *cpu_prev_jif,*cpu_jif;


    static void* aging_thread_run(void* arg);

    void startup_aging_thread();

    int index_in_strings(const char *strings, const char *key);

    void parse_meminfo(unsigned long meminfo[MI_MAX]);

    jiffy_counts_t* xrealloc_vector_helper(jiffy_counts_t *vector, unsigned sizeof_and_shift, int idx);

    int read_cpu_jiffy(FILE *fp, jiffy_counts_t *p_jif);

    void get_jiffy_counts();

    void notifyAgingStatus();
  
}; 
