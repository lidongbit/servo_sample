#ifndef __BUFFER_MANAGER_H__
#define __BUFFER_MANAGER_H__
#include "stdint.h"
#ifdef __cplusplus
extern "C" {
#endif
#include <pthread.h>

typedef struct
{
        int32_t tail_index_offset;
        int32_t head_index_offset;
        int32_t element_length;
        int32_t buff_length;
        int32_t safety_resevd;
        int32_t semaphore;//初始化需要置位1
        //int32_t write_lock;
		pthread_mutex_t mutex;
        //char * buff;
}buffer_info_t;

extern int32_t push_circle_buff_item(buffer_info_t *circle_buff_info, char *circle_buff, char *push_ptr);

extern int32_t push_circle_buff_bundle(buffer_info_t *circle_buff_info, char *circle_buff,
                                        buffer_info_t *local_buff_info, char *local_buff);

extern int32_t pull_circle_buff_item(buffer_info_t *circle_buff_info, char *circle_buff, char *pull_ptr);

extern int32_t pull_circle_buff_bundle(buffer_info_t *circle_buff_info, char *circle_buff,
                                        buffer_info_t *local_buff_info, char *local_buff);

extern int32_t pull_circle_buff_all(buffer_info_t *circle_buff_info, char *circle_buff, char *pull_ptr, int32_t req_nums);

extern int32_t get_circle_buff_occupied(buffer_info_t *circle_buff_info);

extern int buffer_clear(buffer_info_t *circle_buff_info);

extern int32_t is_buff_full(buffer_info_t*);
extern int32_t is_buff_empty(buffer_info_t*);

extern int buffer_lock(buffer_info_t*);
extern void buffer_unlock(buffer_info_t*);

#ifdef __cplusplus
}
#endif
#endif
