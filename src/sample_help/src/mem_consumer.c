#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h> //mmap head file
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/shm.h>
#include <pthread.h>
#include "buffer_manager.h"
#include "log_help.h"

#define LOG_BUFF_INFO_ADDR (0x3b007000)
#define LOG_BUFF_ADDR (0x3b100000)

#define LOG_BUFF_INFO_ADDR_SERVO (0x4e000000)
#define LOG_BUFF_ADDR_SERVO (0x4e100000)

#define TEST_MMAP
//#define TEST_SHM
#define TEST_IN_PC

buffer_info_t *map_ctrl;
char *map_msg;

static void log_end(void)
{
#ifdef TEST_SHM
   if(shmdt(shm_ctrl) == -1 || shmdt(shm_msg) == -1)
   {
      fprintf(stderr, "shmdt failed\n");
      exit(EXIT_FAILURE);
   }
#endif

#ifdef TEST_MMAP
   munmap( map_ctrl, sizeof(buffer_info_t) );
   munmap( map_msg, 1024*1024);
   printf( "umap ok \n" );
#endif
   SampleHelp_CloseFile();
}

static void init(int argc, char **argv)
{
    unsigned int buff_info_addr;
    unsigned int buff_ptr;
    SampleHelp_NewFile();

 #if 0 //TEST_SHM
    int i;
    //char *ctr,*msg;
    void *shm_ctrl = NULL;
    void *shm_msg = NULL;
    buffer_info_t *shared_ctrl = NULL;
    int shmid_ctrl,shmid_msg;  //创建共享内存

    shmid_ctrl = shmget((key_t)1234, sizeof(buffer_info_t), 0666|IPC_CREAT);
    shmid_msg = shmget((key_t)2345, 1024*1024, 0666|IPC_CREAT);
    if( shmid_msg == -1 || shmid_ctrl == -1)
    {
       fprintf(stderr, "shmget failed\n");
       exit(EXIT_FAILURE);
    }

    //将共享内存连接到当前进程的地址空间
    shm_ctrl = shmat(shmid_ctrl, (void*)0, 0);
    shm_msg = shmat(shmid_msg,(void*)0,0);
    if(shm_ctrl == (void*)-1 || shm_msg == (void*)-1)
    {
       fprintf(stderr, "shmat failed\n");
       exit(EXIT_FAILURE);
    }
    printf("Memory attached at 0x%X,0x%X\n", (int)shm_ctrl,(int)shm_msg);    //设置共享内存

    buff_init(shm_ctrl,shm_msg);force_diagnose
 #endif
 
 #ifdef TEST_MMAP
 #ifdef TEST_IN_PC //run in pc
   int fd_ctrl,fd_msg;
    if(argc!=3)
    {
        printf("Usage:...\n");
        exit(0);
    }

    fd_ctrl = open(argv[1],O_RDWR,00777);
    if(fd_ctrl<0)
    {
        printf("open fd one error!\n");
    }
    fd_msg = open(argv[2],O_RDWR,00777);
    if(fd_msg<0)
    {
        printf("open fd two error!\n");
    }

    lseek(fd_ctrl,sizeof(buffer_info_t),SEEK_SET);
    write(fd_ctrl,"1",1);
    lseek(fd_msg,1024*1024,SEEK_SET);
    write(fd_msg,"1",1);
    map_ctrl = (buffer_info_t*) mmap( NULL,sizeof(buffer_info_t),PROT_READ|PROT_WRITE,MAP_SHARED,fd_ctrl,0 );
    map_msg = (char*) mmap( NULL,1024*1024,PROT_READ|PROT_WRITE,MAP_SHARED,fd_msg,0 );
    close(fd_ctrl);
    close(fd_msg);
 #endif

 #if 0 // run in arm
    int fd;
    fd = open("/dev/mem", O_RDWR|O_SYNC);
    if(fd<0)
    {
        printf("open error!\n");
    }
    if(strcmp(argv[1],"servo")==0)
    {
        buff_info_addr = LOG_BUFF_INFO_ADDR_SERVO;
        buff_ptr = LOG_BUFF_ADDR_SERVO;
    }else if(strcmp(argv[1],"force")==0)
    {
        buff_info_addr = LOG_BUFF_INFO_ADDR;
        buff_ptr = LOG_BUFF_ADDR;
    }else{
        printf("exit!\n");
        exit(0);
    }
    map_ctrl = (buffer_info_t*) mmap(NULL,sizeof(buffer_info_t),PROT_READ|PROT_WRITE,MAP_SHARED,fd,buff_info_addr );
    if(map_ctrl==NULL)
    {
        printf("map ctrl error!\n");
    }
    map_msg = (char*) mmap( NULL,1024*1024,PROT_READ|PROT_WRITE,MAP_SHARED,fd,buff_ptr );
    if(map_msg==NULL)
    {
        printf("map msg error!\n");
    }

    close(fd);
    atexit(log_end);
 #endif
    SampleHelp_Init(map_ctrl,map_msg);
    printf("consumer init ok!\n");
 #endif
}

int main (int argc, char *argv[])
{
   init(argc, argv);

   pthread_t tid[0];
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
   exit(EXIT_SUCCESS);
   return 0;
}
