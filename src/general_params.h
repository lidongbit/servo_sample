#ifndef GENERAL_PARAMS_H_
#define GENERAL_PARAMS_H_

#ifdef __cplusplus
extern "C" {
#endif
#define LOG_FILE_LEN (10*1024*1024)
#define DOF_AXIS 6
#define CORE_COMM_BASE_ADDR	0x7b000000
// #define DYNAMIC_SAMPLE_START 		(*((volatile unsigned int*)(0x7b000020)))
#define DYNAMIC_BUFF_INFO_ADDR 		(0x7b008000)  //0x7b004000
#define DYNAMIC_BUFF_ADDR 			(0x7b500000)//0x7b100000

#define START_SIG           0xaabbccdd
#define FINISH_SIG          0xdeadbeef

typedef struct
{
    int32_t pos[DOF_AXIS];// 24
    int32_t vel[DOF_AXIS];// 24
    int32_t iq[DOF_AXIS];// 24
}DEBUG_INFO_t;

#define LOCAL_BUFF_SIZE (1024*sizeof(DEBUG_INFO_t))
#ifdef __cplusplus
}
#endif
#endif
