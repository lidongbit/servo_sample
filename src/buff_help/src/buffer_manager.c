#include "buffer_manager.h"
#include <string.h>

/*push one item per operation*/
int32_t push_circle_buff_item(buffer_info_t *circle_buff_info, char *circle_buff, char *push_ptr)
{
    if(is_buff_full(circle_buff_info))
    {
        return -1;
    }

    memcpy(circle_buff+circle_buff_info->tail_index_offset,push_ptr,circle_buff_info->element_length);
    circle_buff_info->tail_index_offset = (circle_buff_info->tail_index_offset + circle_buff_info->element_length)%circle_buff_info->buff_length;
    return circle_buff_info->element_length;
}

/*push all items per operation*/
int32_t push_circle_buff_bundle(buffer_info_t *circle_buff_info, char *circle_buff,
                                buffer_info_t *local_buff_info,  char *local_buff)
{
    int ir;
    int iw;
    int or;
    int ow;
/*
    ir = circle_buff_info->head_index_offset;
    iw = circle_buff_info->tail_index_offset;
    or = local_buff_info->head_index_offset;
    ow = local_buff_info->tail_index_offset;
    */
    //printf("ir,iw,or,ow:%d %d %d %d\r\n",ir,iw,or,ow);
    ir = local_buff_info->head_index_offset;
    iw = local_buff_info->tail_index_offset;
    or = circle_buff_info->head_index_offset;
    ow = circle_buff_info->tail_index_offset;
    if(ir<iw)
    {
        if((circle_buff_info->buff_length-ow)>(iw-ir))
        {
            /*
             * in      |    R------W |temp_var_info
             * local   |   W         |
             * res     |   -------W  |
             */
            //printf("buff_len:%d,data_len:%d\r\n",1024*1024,iw-ir);
            memcpy(&(circle_buff[ow]), &local_buff[ir], (iw-ir));

        }else{
            /*
             * in      |    R------W |
             * local   |        W    |
             * res     |-W      -----|
             */
            memcpy(&circle_buff[ow], &local_buff[ir], (circle_buff_info->buff_length-ow));
            memcpy(&circle_buff[0], &local_buff[ir+(circle_buff_info->buff_length-ow)], iw-ir-(circle_buff_info->buff_length-ow));

        }
        local_buff_info->head_index_offset = local_buff_info->tail_index_offset;
        circle_buff_info->tail_index_offset = (circle_buff_info->tail_index_offset+(iw-ir))%circle_buff_info->buff_length;
        return (iw-ir);

    }else{

        if((circle_buff_info->buff_length-ow)>=(local_buff_info->buff_length-ir+iw))
        {
            /*
             * in      |+++W     R---|
             * local   |      W      |
             * res     |      ---+++W|
             */
            memcpy(&circle_buff[ow], &local_buff[ir], (local_buff_info->buff_length-ir));
            memcpy(&circle_buff[ow+(local_buff_info->buff_length-ir)], &local_buff[0], iw);

        }else if((circle_buff_info->buff_length-ow)>=(local_buff_info->buff_length-ir)){
            /*
             * in      |+++W     R---|
             * local   |        W    |
             * res     |+W      ---++|
             */
            memcpy(&circle_buff[ow], &local_buff[ir], (local_buff_info->buff_length-ir));
            memcpy(&circle_buff[ow+(local_buff_info->buff_length-ir)], &local_buff[0], (circle_buff_info->buff_length-ow)-(local_buff_info->buff_length-ir));
            memcpy(&circle_buff[0], &local_buff[(circle_buff_info->buff_length-ow)-(local_buff_info->buff_length-ir)], (local_buff_info->buff_length-ir+iw)-(circle_buff_info->buff_length-ow));

        }else if((circle_buff_info->buff_length-ow)<(local_buff_info->buff_length-ir)){
            /*
             * in      |+++W     R---|
             * local   |           W |
             * res     |-+++W      --|
             */
            memcpy(&circle_buff[ow], &local_buff[ir], (circle_buff_info->buff_length-ow));
            memcpy(&circle_buff[0], &local_buff[ir+(circle_buff_info->buff_length-ow)], (local_buff_info->buff_length-ir)-(circle_buff_info->buff_length-ow));
            memcpy(&circle_buff[(local_buff_info->buff_length-ir)-(circle_buff_info->buff_length-ow)], &local_buff[0], iw);

        }
        local_buff_info->head_index_offset = local_buff_info->tail_index_offset;
        circle_buff_info->tail_index_offset = (circle_buff_info->tail_index_offset+(local_buff_info->buff_length-ir+iw))%circle_buff_info->buff_length;
        return (local_buff_info->buff_length-ir+iw);
    }
    return -1;
}


int32_t pull_circle_buff_item(buffer_info_t *circle_buff_info, char *circle_buff, char *pull_ptr)
{
    //printf("head:%d tail:%d\n",circle_buff_info->head_index_offset, circle_buff_info->tail_index_offset);
    
    if(circle_buff_info->head_index_offset != circle_buff_info->tail_index_offset)
    {
        //printf("local_buff_info.r:%d local_buff_info.w:%d\r\n",local_buff_info.r,local_buff_info.w);
        memcpy(pull_ptr,&circle_buff[circle_buff_info->head_index_offset],circle_buff_info->element_length);
        circle_buff_info->head_index_offset = (circle_buff_info->head_index_offset+circle_buff_info->element_length)%circle_buff_info->buff_length;
        //printf("head:%d tail:%d\n",circle_buff_info->head_index_offset, circle_buff_info->tail_index_offset);
        return 0;
    }
    return -1;//buffer empty
}

int32_t get_circle_buff_occupied(buffer_info_t *circle_buff_info)
{
	int32_t len = (circle_buff_info->tail_index_offset - circle_buff_info->head_index_offset + circle_buff_info->buff_length)%circle_buff_info->buff_length;
	return len/circle_buff_info->element_length;
}

int32_t pull_circle_buff_all(buffer_info_t *circle_buff_info, char *circle_buff, char *pull_ptr, int32_t req_nums)
{
	int32_t nums = 0;
	int32_t res = 0;
	int32_t cnt = 0;
    nums = get_circle_buff_occupied(circle_buff_info);
	if(nums < req_nums)
	{
		while(cnt < nums)
	    {
	        
	        memcpy(pull_ptr+cnt*circle_buff_info->element_length,&circle_buff[circle_buff_info->head_index_offset],circle_buff_info->element_length);
	        circle_buff_info->head_index_offset = (circle_buff_info->head_index_offset+circle_buff_info->element_length)%circle_buff_info->buff_length;
			cnt++;
	    }
		res = nums;
	}else{
		while(cnt < req_nums)
	    {
	        memcpy(pull_ptr+cnt*circle_buff_info->element_length,&circle_buff[circle_buff_info->head_index_offset],circle_buff_info->element_length);
	        circle_buff_info->head_index_offset = (circle_buff_info->head_index_offset+circle_buff_info->element_length)%circle_buff_info->buff_length;
			cnt++;
	    }
		res = req_nums;
	}
	return res;
}


/*pull all data per operation*/
int32_t pull_circle_buff_bundle(buffer_info_t *circle_buff_info, char *circle_buff,
                                buffer_info_t *local_buff_info, char *local_buff)
{
    int ir;
    int iw;
    int or;
    int ow;

    ir = circle_buff_info->head_index_offset;
    iw = circle_buff_info->tail_index_offset;
    or = local_buff_info->head_index_offset;
    ow = local_buff_info->tail_index_offset;
    //printf("ir,iw,or,ow:%d %d %d %d\r\n",ir,iw,or,ow);

    if(ir<iw)
    {
        if((local_buff_info->buff_length-ow)>(iw-ir))
        {
            /*
             * in      |    R------W |temp_var_info
             * local   |   W         |
             * res     |   -------W  |
             */
            //printf("buff_len:%d,data_len:%d\r\n",1024*1024,iw-ir);
            memcpy(&(local_buff[ow]), &circle_buff[ir], (iw-ir));

        }else{
            /*
             * in      |    R------W |
             * local   |        W    |
             * res     |-W      -----|
             */
            memcpy(&local_buff[ow], &circle_buff[ir], (local_buff_info->buff_length-ow));
            memcpy(&local_buff[0], &circle_buff[ir+(local_buff_info->buff_length-ow)], iw-ir-(local_buff_info->buff_length-ow));

        }
        circle_buff_info->head_index_offset = circle_buff_info->tail_index_offset;
        local_buff_info->tail_index_offset = (local_buff_info->tail_index_offset+(iw-ir))%local_buff_info->buff_length;
        return (iw-ir);

    }else{

        if((local_buff_info->buff_length-ow)>=(circle_buff_info->buff_length-ir+iw))
        {
            /*
             * in      |+++W     R---|
             * local   |      W      |
             * res     |      ---+++W|
             */
            memcpy(&local_buff[ow], &circle_buff[ir], (circle_buff_info->buff_length-ir));
            memcpy(&local_buff[ow+(circle_buff_info->buff_length-ir)], &circle_buff[0], iw);

        }else if((local_buff_info->buff_length-ow)>=(circle_buff_info->buff_length-ir)){
            /*
             * in      |+++W     R---|
             * local   |        W    |
             * res     |+W      ---++|
             */
            memcpy(&local_buff[ow], &circle_buff[ir], (circle_buff_info->buff_length-ir));
            memcpy(&local_buff[ow+(circle_buff_info->buff_length-ir)], &circle_buff[0], (local_buff_info->buff_length-ow)-(circle_buff_info->buff_length-ir));
            memcpy(&local_buff[0], &circle_buff[(local_buff_info->buff_length-ow)-(circle_buff_info->buff_length-ir)], (circle_buff_info->buff_length-ir+iw)-(local_buff_info->buff_length-ow));

        }else if((local_buff_info->buff_length-ow)<(circle_buff_info->buff_length-ir)){
            /*
             * in      |+++W     R---|
             * local   |           W |
             * res     |-+++W      --|
             */
            memcpy(&local_buff[ow], &circle_buff[ir], (local_buff_info->buff_length-ow));
            memcpy(&local_buff[0], &circle_buff[ir+(local_buff_info->buff_length-ow)], (circle_buff_info->buff_length-ir)-(local_buff_info->buff_length-ow));
            memcpy(&local_buff[(circle_buff_info->buff_length-ir)-(local_buff_info->buff_length-ow)], &circle_buff[0], iw);

        }
        circle_buff_info->head_index_offset = circle_buff_info->tail_index_offset;
        local_buff_info->tail_index_offset = (local_buff_info->tail_index_offset+(circle_buff_info->buff_length-ir+iw))%local_buff_info->buff_length;
        return (circle_buff_info->buff_length-ir+iw);
    }
    return -1;
}

int32_t is_buff_full(buffer_info_t *circle_buff_info)
{
    if(((circle_buff_info->tail_index_offset+circle_buff_info->element_length)%circle_buff_info->buff_length) == circle_buff_info->head_index_offset)
    {
        return 1;
    }
    return 0;
}

int32_t is_buff_empty(buffer_info_t *circle_buff_info)
{
    if(circle_buff_info->tail_index_offset
             == circle_buff_info->head_index_offset)
    {
        return 1;
    }
    return 0;
}

int buffer_clear(buffer_info_t *circle_buff_info)
{
	circle_buff_info->head_index_offset = 0;
	circle_buff_info->tail_index_offset = 0;
}

int buffer_lock(buffer_info_t *circle_buff_info)
{
	int res = 0;
	res = pthread_mutex_lock(&circle_buff_info->mutex);  // 为保证条件变量不会因为多线程混乱，所以先加锁
	return res;
}

void buffer_unlock(buffer_info_t *circle_buff_info)
{
	pthread_mutex_unlock(&circle_buff_info->mutex);
}

