#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>           /* Definition of AT_* constants */
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <dirent.h>
#include <stdbool.h>
#include "sample_help.h"
#include "buffer_manager.h"
#include "dlist_help.h"
typedef buffer_info_t SampleHelp_BuffInfo_t;

static char *mesg_buff;
static char log_name[128];
static FILE *log_file = NULL;
/*********************************************************/
//for log comsumer
static SampleHelp_BuffInfo_t local_buff_info;
static char local_buff[LOCAL_BUFF_SIZE];

pthread_mutex_t mutex_log_list;
static dlist_node_t *log_list_head;

static SampleHelp_Buff_t temp_var_info;
static SampleHelp_BuffInfo_t *circle_buff_info;

static int SampleHelp_DirCheck(void);
static int SampleHelp_BuffPull(void);
static int SampleHelp_BuffPush(char *push_ptr,int length);
/*--------------------------------------------------------------------
 * 函数名称:static int SampleHelp_DirCheck(void)
 * 函数输入:
 * 函数输出:
 * 函数备注:检查日志文件目录
 * ------------------------------------------------------------------*/
static int SampleHelp_DirCheck(void)
{
    if(access("/tmp/", F_OK) == 0) return 0;

    if(system("mkdir -p /tmp/"))
    {
        perror("system");
        exit(0);
    }
}
/*--------------------------------------------------------------------
 * 函数名称:static int SampleHelp_NewFile(void)
 * 函数输入:
 * 函数输出:
 * 函数备注:创建新的日志文件
 * ------------------------------------------------------------------*/
int SampleHelp_NewFile(void)
{
    char file_time[32] = "";
    char file_name[128] = "/tmp/dynamic";

    time_t t = time(NULL);
    struct tm *t_now = localtime(&t);
    strftime(file_time, sizeof(file_time), "%Y%m%d%H%M%S.log", t_now);
    strcat(file_name, file_time);

    SampleHelp_DirCheck();
    log_file = fopen(file_name, "a+");
    if(NULL == log_file)
    {
        perror("fopen");
        exit(0);
    }
    strcpy(log_name, file_name);
    return 0;
}
/*--------------------------------------------------------------------
 * 函数名称:static void SampleHelp_CloseFile()
 * 函数输入:
 * 函数输出:
 * 函数备注:关闭日志文件
 * ------------------------------------------------------------------*/
void SampleHelp_CloseFile()
{
    fflush(log_file);
    fclose(log_file);
}

/*--------------------------------------------------------------------
 * 函数名称:int32_t SampleHelp_BuffPush(void *push_ptr,int32_t length)
 * 函数输入:
 * 函数输出:
 * 函数备注:写数据
 * ------------------------------------------------------------------*/
static int SampleHelp_BuffPush(char *push_ptr,int length)
{
    int ret_val = 0;
    ret_val = push_circle_buff_item(circle_buff_info,mesg_buff,push_ptr);
	return ret_val;
}
/*--------------------------------------------------------------------
 * 函数名称:void SampleHelp_BuffPull(void)
 * 函数输入:
 * 函数输出:
 * 函数备注:读数据
 * ------------------------------------------------------------------*/
static int SampleHelp_BuffPull(void)
{
	int res;
    //take items from communication buffer to local buffer
    res = pull_circle_buff_bundle(circle_buff_info,mesg_buff,&local_buff_info,local_buff);
	return res;
}

/*--------------------------------------------------------------------
 * 函数名称:void SampleHelp_Debug_Init(void)
 * 函数输入:
 * 函数输出:
 * 函数备注:初始化
 * ------------------------------------------------------------------*/
void SampleHelp_Init(void *ctr_ptr,void* msg_ptr)
{
	//circle_buff_info = (SampleHelp_DEBUG_RINGS_BUFF_STRUCT *)(LOG_MSG_CONTROL_ADDR);
	//mesg_buff = (char *)(LOG_MSG_ADDR);

	circle_buff_info = (SampleHelp_BuffInfo_t *)(ctr_ptr);
	mesg_buff = (char *)(msg_ptr);
    
	circle_buff_info->semaphore = START_SIG;
	circle_buff_info->tail_index_offset = 0;
    circle_buff_info->head_index_offset = 0;
	circle_buff_info->element_length = sizeof(SampleHelp_Buff_t);
	circle_buff_info->safety_resevd = 2*circle_buff_info->element_length;
    circle_buff_info->buff_length = LOCAL_BUFF_SIZE;
	
	log_list_head = circle_dlist_init();
	pthread_mutex_init(&mutex_log_list, NULL);
}

/*--------------------------------------------------------------------
 * 函数名称:void SampleHelp_BuffProcess(void)
 * 函数输入:
 * 函数输出:
 * 函数备注:打印数据
 * ------------------------------------------------------------------*/
void *SampleHelp_ReadProcess(void *p)
{
    SampleHelp_Buff_t *temp_item_ptr = NULL;
	dlist_node_t temp_log_ptr;
	char *pdat = NULL;
	int read_cnt = 0;
	pthread_detach(pthread_self());
	while(1)
	{
		read_cnt = pull_circle_buff_all(circle_buff_info,mesg_buff,local_buff,5);
		pthread_mutex_lock(&mutex_log_list);
		for(int i=0; i < read_cnt; i++)
		{
	        temp_item_ptr = (SampleHelp_Buff_t*)(local_buff+circle_buff_info->element_length*i);
			pdat = SampleHelp_Get(temp_item_ptr);	
			circle_dlist_tail_insert(log_list_head, pdat);						
		} 
		pthread_mutex_unlock(&mutex_log_list);
		usleep(2000);
	}
	return NULL;
}

void *SampleHelp_WriteProcess(void *p)
{
	DEBUG_INFO_t *pdat[5] = {NULL};
	int i = 0, j = 0, cnt = 0;
    SampleHelp_NewFile();
	//pthread_detach(pthread_self());
	while(1)
	{
		pthread_mutex_lock(&mutex_log_list);
        for (i = 0; i < 6; i++)
		{
		    if((pdat[i] = circle_dlist_head_eat(log_list_head))==NULL)
                break;
        }
		pthread_mutex_unlock(&mutex_log_list);

        
        if(pdat[0] == NULL && (SampleHelp_Signal()==FINISH_SIG))
        {
            break;
        }
        j = 0;
		while(j < i)
		{
            //printf("%s\n",pdat);
            fprintf(log_file,"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
                                pdat[j]->pos[0],pdat[j]->vel[0],pdat[j]->iq[0],
                                pdat[j]->pos[1],pdat[j]->vel[1],pdat[j]->iq[1],
                                pdat[j]->pos[2],pdat[j]->vel[2],pdat[j]->iq[2],
                                pdat[j]->pos[3],pdat[j]->vel[3],pdat[j]->iq[3],
                                pdat[j]->pos[4],pdat[j]->vel[4],pdat[j]->iq[4],
                                pdat[j]->pos[5],pdat[j]->vel[5],pdat[j]->iq[5]);
            free(pdat[j]);
            j++;
        }
      
		if(ftell(log_file) > LOG_FILE_LEN)// 1MB
	    {
	        SampleHelp_CloseFile();
	        SampleHelp_NewFile();
	    }
		usleep(2000);
	}
}

char *SampleHelp_Get(SampleHelp_Buff_t* item)
{
    SampleHelp_Buff_t *info = item;

	char *res = (char*)calloc(sizeof(SampleHelp_Buff_t),1);
    memcpy(res, item, sizeof(SampleHelp_Buff_t));
    return res;
}

void *SampleHelp_FileMange_Service(void *p)// 10 files check
{
    DIR *dir;
    struct dirent * ptr;
    int total = 0;
    char *root = "/tmp/";

    char latestFile[32];
    char rmFile[64];
    int flag = 0;
    pthread_detach(pthread_self());
    while(1)
    {
        flag=0;

        dir = opendir(root);
        if(dir == NULL){
            perror("fail to open dir");
            exit(1);
        }

        while((ptr = readdir(dir)) != NULL)
        {
            if(strcmp(ptr->d_name,".") == 0 || strcmp(ptr->d_name,"..") == 0)
            {
                continue;
            }

            if(ptr->d_type == DT_REG)
            {
                if(flag == 0)
                {
                    strcpy(latestFile,ptr->d_name);
                    flag=1;
                }else{
                    if(strcmp(ptr->d_name,latestFile)<0)
                    {
                        strcpy(latestFile,ptr->d_name);
                    }
                }
                total++;
            }
        }
        if(total>10)
        {
            sprintf(rmFile,"%s%s",root,latestFile);
            printf("%s\n",rmFile);
            if(unlink(rmFile)<0)
            //if(system(rmFile)<0)
            {
                printf("system() %s %s!\n",rmFile, strerror(errno));
            }
        }
        total=0;

        bzero(rmFile,64);
        bzero(latestFile,32);
        closedir(dir);
        usleep(2000000);
    }
    return NULL;
}

int SampleHelp_Signal()
{
    return circle_buff_info->semaphore;
}
