#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h> //mmap head file
#include <sys/shm.h>
#include <sched.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>

#include "general_params.h"
#include "sample_help.h"
#include "buffer_manager.h"
#include "dlist_help.h"

int main()
{
   int fd;
   buffer_info_t *map_ctrl;
   char *map_msg;
   fd = open("/dev/mem", O_RDWR|O_SYNC);
   if(fd<0)
   {
       printf("open error!\n");
   }else{
      printf("open /dev/mem succeed!\n");
   }

   map_ctrl = (buffer_info_t*) mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_SHARED,fd,(void*)DYNAMIC_BUFF_INFO_ADDR);
   if(map_ctrl==NULL)
   {
      printf("map ctrl error!\n");
   }else{
      printf("map ctrl succeed!\n");
   }
   map_msg = (char*) mmap( NULL,1024*1024,PROT_READ|PROT_WRITE,MAP_SHARED,fd,(void*)DYNAMIC_BUFF_ADDR);

   if(map_msg==NULL)
   {
      printf("map msg error!\n");
   }else{
      printf("map area succeed!\n");
   }
   close(fd);

   SampleHelp_Init(map_ctrl,map_msg);

   pthread_t tid[3];
   if(pthread_create(&tid[0], NULL, SampleHelp_FileMange_Service, NULL))
   {
        printf("Create log clean thread failed !\n");
        exit(0);
   }
   if(pthread_create(&tid[1], NULL, SampleHelp_ReadProcess, NULL))
   {
        printf("Create log clean thread failed !\n");
        exit(0);
   }
   if(pthread_create(&tid[2], NULL, SampleHelp_WriteProcess, NULL))
   {
        printf("Create log clean thread failed !\n");
        exit(0);
   }
   
   pthread_join(tid[2],NULL); 
}



