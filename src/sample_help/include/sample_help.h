#ifndef __LOGHELP_H__
#define __LOGHELP_H__
 
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "general_params.h"
#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
    int32_t pos[DOF_AXIS];// 24
    int32_t vel[DOF_AXIS];// 24
    int32_t iq[DOF_AXIS];// 24

}SampleHelp_Buff_t;

void SampleHelp_Init(void *ctr_ptr,void* msg_ptr);

extern void SampleHelp_Put(const char* str,...);

extern char *SampleHelp_Get(SampleHelp_Buff_t* item);

extern void *SampleHelp_ReadProcess(void *p);

extern void *SampleHelp_WriteProcess(void *p);

extern void *SampleHelp_FileMange_Service(void *p);

extern int SampleHelp_NewFile(void);

extern void SampleHelp_CloseFile();

#ifdef __cplusplus
}
#endif
#endif
