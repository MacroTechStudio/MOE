/******************************************************************************
* File       : Task_PT_Demo2.h
* Function   : Another demo task for PT application
* Description: To be done.           
* Version    : V1.00
* Author     : Ian
* Date       : 8th Jul 2016
* History    :  No.  When           Who           What
*               1    08/Jul/2016    Ian           Create
******************************************************************************/


#ifndef _TASK_PT_DEMO2_H_
#define _TASK_PT_DEMO2_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Check if specified option is set for debugging */
#ifndef __DEBUG_MODE_PT_DEMO2                    
#define __DEBUG_MODE      __DEBUG_NONE                /* Default: None debugging info            */
#else
#ifdef __DEBUG_MODE
#undef __DEBUG_MODE
#endif
#define __DEBUG_MODE      __DEBUG_MODE_PT_DEMO2       /* According the set from project_config.h */
#endif



/******************************************************************************
* Name       : uint8 Task_PT_Demo2_Process(uint8 u8Evt, void *pPara)
* Function   : A demo task for PT application
* Input      : uint8  u8Evt  1~254     Event for the task
*              void  *pPara            Pointer of parameter
* Output:    : None
* Return     : SW_OK   Successful operation
*            : SW_ERR  Failed operation
*              1~254   Event which is not processed.
* Description: To be done
* Version    : V1.00
* Author     : Ian
* Date       : 8th Jul 2016
******************************************************************************/
uint8 Task_PT_Demo2_Process(uint8 u8Evt, void *pPara);




#ifdef __cplusplus
}
#endif

#endif /* _TASK_PT_DEMO2_H */

/* End of file */

