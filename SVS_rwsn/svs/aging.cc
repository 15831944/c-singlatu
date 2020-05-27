#include "aging.h"

int CAging::aging_on = 1;

CAging::CAging(int connect)
{
    m_iconnet_accept = connect;
    num_cpus=0;
}

bool CAging::doSend()
{  
    startup_aging_thread();
    return true;
}

/*
* CPU 100%Õ¼ÓÃÏß³Ì
* Ò»¸öËÀÑ­»·£¬Ö±µ½aging_on=0£¬Ïß³Ì½áÊø
*/
void* CAging::aging_thread_run(void* arg)
{ 
    while(aging_on);    
}

/*
* Æô¶¯CPU 100%Õ¼ÓÃÏß³Ì
*/
void CAging::startup_aging_thread()
{
    FILE *fd;
    char buff[256]; 
    int i;
    /*¶ÁÈ¡CPUºËÐÄ¸öÊý*/
    int nr_processor = get_nprocs();
    /*¸ù¾ÝCPUºË¸öÊý£¬´´½¨¶ÔÓ¦µÄÏß³ÌÊý*/
    pthread_t *aging_thread_id = (pthread_t *)malloc(nr_processor*sizeof(pthread_t));


    if(aging_thread_id){

        /*´´½¨nr_processor¸öÏß³ÌÀ´·Ö±ðÕ¼ÓÃ¸÷¸öCPUÔËÐÐ*/
        for(i=0; i<nr_processor; i++){
            pthread_create(aging_thread_id+i, NULL, aging_thread_run,NULL); 
        }
        get_jiffy_counts();
        /*每秒向客户发一次数据*/
		//如果让数据一真向上反，就把下面的注释打开
        //while(aging_on){
            sleep(1);
            notifyAgingStatus();
        //}

        /*Èç¹ûÍ£Ö¹²âÊÔ£¬µÈ´ýÏß³ÌÍ£Ö¹£¬½áÊøÏß³Ì²¢ÊÍ·ÅÄÚ´æ*/
        /*for(i=0; i<nr_processor; i++){
            pthread_join(aging_thread_id[i],NULL);
        }
        num_cpus = 0;
        free(cpu_prev_jif);free(cpu_jif);
        free(aging_thread_id);*/
    }
}

int CAging::index_in_strings(const char *strings, const char *key)
{
        int idx = 0;

        while (*strings) {
                if (strcmp(strings, key) == 0) {
                        return idx;
                }
                strings += strlen(strings) + 1; /* skip NUL */
                idx++;
        }
        return -1;
}

void CAging::parse_meminfo(unsigned long meminfo[MI_MAX])
{
    const char fields[] =
                "MemTotal\0"
                "MemFree\0"
                "MemShared\0"
                "Shmem\0"
                "Buffers\0"
                "Cached\0"
                "SwapTotal\0"
                "SwapFree\0"
                "Dirty\0"
                "Writeback\0"
                "AnonPages\0"
                "Mapped\0"
                "Slab\0";
    char buf[60]; /* actual lines we expect are ~30 chars or less */
    FILE *f;
    int i;

    memset(meminfo, 0, sizeof(meminfo[0]) * MI_MAX);
    f = fopen("/proc/meminfo","r");
    while (fgets(buf, sizeof(buf), f) != NULL) {
        char *c = strchr(buf, ':');
        if (!c) continue;
        *c = '\0';
        i = index_in_strings(fields, buf);
        if (i >= 0) meminfo[i] = strtoul(c+1, NULL, 10);
    }
    fclose(f);
}

jiffy_counts_t* CAging::xrealloc_vector_helper(jiffy_counts_t *vector, unsigned sizeof_and_shift, int idx)
{
        int mask = 1 << (unsigned int)sizeof_and_shift;

        if (!(idx & (mask - 1))) {
                sizeof_and_shift >>= 8; /* sizeof(vector[0]) */
				jiffy_counts_t* new_vector;
                if((new_vector = (jiffy_counts_t*)realloc(vector, sizeof_and_shift * (idx + mask + 1)))==NULL)
					return vector;
				vector = new_vector;
                memset((char*)vector + (sizeof_and_shift * idx), 0, sizeof_and_shift * (mask + 1));
        }

        return vector;
}

int CAging::read_cpu_jiffy(FILE *fp, jiffy_counts_t *p_jif)
{
    const char fmt[] = "cp%*s %llu %llu %llu %llu %llu %llu %llu %llu";
    int ret;
    char line_buf[PATH_MAX];

    if (!fgets(line_buf, PATH_MAX, fp) || line_buf[0] != 'c' /* not "cpu" */)
        return 0;
    ret = sscanf(line_buf, fmt,
            &p_jif->usr, &p_jif->nic, &p_jif->sys, &p_jif->idle,
            &p_jif->iowait, &p_jif->irq, &p_jif->softirq,
            &p_jif->steal);
    if (ret >= 4) {
        p_jif->total = p_jif->usr + p_jif->nic + p_jif->sys + p_jif->idle
            + p_jif->iowait + p_jif->irq + p_jif->softirq + p_jif->steal;
        /* procps 2.x does not count iowait as busy time */
        p_jif->busy = p_jif->total - p_jif->idle - p_jif->iowait;
    }

    return ret;
}

void CAging::get_jiffy_counts()
{
    FILE* fp = fopen("/proc/stat","r");
    jiffy_counts_t *tmp;
    int i;

    /* Otherwise the first per cpu display shows all 100% idles */
    if (!num_cpus) {
        /* First time here. How many CPUs?
         * There will be at least 1 /proc/stat line with cpu%d
         */
		cpu_jif = (jiffy_counts_t*)malloc(0);
        while (1) {
            cpu_jif = xrealloc_vector(cpu_jif, 1, num_cpus);
            if (read_cpu_jiffy(fp, &cpu_jif[num_cpus]) <= 4)
                break;
            num_cpus++;
        }
        
        cpu_prev_jif = (jiffy_counts_t*)malloc(sizeof(jiffy_counts_t) * num_cpus);
        memset(cpu_prev_jif,0,sizeof(jiffy_counts_t) * num_cpus);

        /* Otherwise the first per cpu display shows all 100% idles */
        usleep(50000);
    } else { /* Non first time invocation */
        jiffy_counts_t *tmp;
        int i;

        /* First switch the sample pointers: no need to copy */
        tmp = cpu_prev_jif;
        cpu_prev_jif = cpu_jif;
        cpu_jif = tmp;

        /* Get the new samples */
        for (i = 0; i < num_cpus; i++)
            read_cpu_jiffy(fp, &cpu_jif[i]);
    }
    fclose(fp);
}

void CAging::notifyAgingStatus()
{
    int i;
    char cpu_name[32];

        unsigned long meminfo[MI_MAX];
        parse_meminfo(meminfo);
         //´¦ÀíÍê³Éºó£¬¶ÔÓ¦µÄÄÚ´æÕ¼ÓÃÐÅÏ¢£¬¿ÉÒÔ½«ÕâÐ©ÐÅÏ¢·¢ËÍ¸øPC²à±ãÓÚPC¼à¿Ø
        //"used"          meminfo[MI_MEMTOTAL] - meminfo[MI_MEMFREE]
        //"free"           meminfo[MI_MEMFREE]
        //"shrd"          meminfo[MI_MEMSHARED] + meminfo[MI_SHMEM]
        //"buff"           meminfo[MI_BUFFERS]
        //"cached"       meminfo[MI_CACHED]

        jiffy_counts_t *p_jif, *p_prev_jif;
        unsigned total_diff;
# define  CALC_TOTAL_DIFF do { \
    total_diff = (unsigned)(p_jif->total - p_prev_jif->total); \
    if (total_diff == 0) total_diff = 1; \
} while (0)

#  define CALC_STAT(xxx) 100 * (unsigned)(p_jif->xxx - p_prev_jif->xxx) / total_diff

        get_jiffy_counts();

        for (i = 0; i < num_cpus; i++) {
            if(i == 0) sprintf(cpu_name,"cpu");
            else sprintf(cpu_name,"cpu%d",i);
            
            p_jif = &cpu_jif[i];
            p_prev_jif = &cpu_prev_jif[i];
            CALC_TOTAL_DIFF;
            //i = 0ÊÇ×ÜµÄCPUµÄÊ¹ÓÃÁ¿£¬ÆäËû£¬ÔòÊÇµ¥¸öµÄÊ¹ÓÃÁ¿£¬ÕâÐ©ÐÅÏ¢¿ÉÒÔ´«¸øPC£¬±ãÓÚ¼à¿Ø
            //Ã¿¸öCPU¸÷¸öÄ£¿éÕ¼ÓÃµÄ±ÈÀý£¬°üÀ¨ÓÃ»§¿Õ¼äusr,ÏµÍ³¿Õ¼äsysµÈ   
            //"usr" CALC_STAT(usr);
            //"sys" CALC_STAT(sys);
            //"nic" CALC_STAT(nic);
            //"idle" CALC_STAT(idle);
            //"iowait" CALC_STAT(iowait);
            //"irq" CALC_STAT(irq);
            //"softirq" CALC_STAT(softirq);
	
	    char buff[255]={0};
        //返回的数据
	    sprintf(&buff[4],"memery:\nused:%d\nfree:%d\nshrd:%d\nbuff:%d\ncached:%d\n\ncpu:\nusr:%f\nsys:%f\nnic:%f\nidle:%f\niowait:%f\nirq:%f\nsoftirq:%f\n"
		,(int)(meminfo[MI_MEMTOTAL] - meminfo[MI_MEMFREE])
		,(int)meminfo[MI_MEMFREE]
		,(int)(meminfo[MI_MEMSHARED] + meminfo[MI_SHMEM])
		,(int)meminfo[MI_BUFFERS]
		,(int)meminfo[MI_CACHED]

		,(float)CALC_STAT(usr)
		,(float)CALC_STAT(sys)
		,(float)CALC_STAT(nic)
		,(float)CALC_STAT(idle)
		,(float)CALC_STAT(iowait)
		,(float)CALC_STAT(irq)
		,(float)CALC_STAT(softirq));
       
        int len = strlen(&buff[4]);//数据的长度
        buff[0] = len+3;
        buff[1] = CMD_AGING; 
        buff[2] = 1;
        buff[3] = len;  
        send(m_iconnet_accept, buff, 255,0);
        }
}


