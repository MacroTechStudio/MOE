/******************************************************************************
* File       : MOE_Msg.h
* Function   : Provide message services.
* Description: To be done.           
* Version    : V1.00
* Author     : Ian
* Date       : 26th May 2016
* History    :  No.  When           Who           What
*               1    26/May/2016    Ian           Create
******************************************************************************/

#ifndef _MOE_MSG_H_
#define _MOE_MSG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Check if specified option is set for debugging */
#ifndef __DEBUG_MODE_MOE_MSG                    
#define __DEBUG_MODE      __DEBUG_NONE                /* Default: None debugging info            */
#else
#ifdef __DEBUG_MODE
#undef __DEBUG_MODE
#endif
#define __DEBUG_MODE      __DEBUG_MODE_MOE_MSG        /* According the set from project_config.h */
#endif

#define MOE_MSG_POLL_NONE          (0)                /* Unnecessary to poll message process     */
#define MOE_MSG_POLL               (1)                /* Need to poll message process            */

#define MOE_MSG_RCV_DONE           (1)                /* Task has received the message           */
#define MOE_MSG_RCV_DONE_NONE      (0)                /* Task has NOT received the message       */

#define MSG_TYPE_TEST              (1)                /* The message type for test               */
#define MSG_TYPE_QUEUE             (2)                /* The message type for queue              */

#define MOE_MSG_SEND(DstTsk, MsgType, Msg)     Moe_Msg_Send(DstTsk, MsgType, sizeof(Msg), (void*)(&Msg))

#ifdef __FLEXIBLE_ARRAY_NOT_SUPPORTED
#define MOE_MSG_CONTENT(p)  ((T_MSG_HEAD*)p + 1);                 /* Return the data of message         */
#else
#define MOE_MSG_CONTENT(p)  (((T_MSG_HEAD*)p)->au8Data);          /* Return the data of message         */
#endif      


/*******************************************************************************
* Structure  : T_MSG_HEAD
* Description: Structure of message head.
* Memebers   : uint8                 u8DestTask   1~254     The Destination task number
*              uint8                 u8SrcTask    1~254     The task which sends this message
*              uint8                 u8MsgType    0~255     Kind of message types
*              uint8                 u8CopyCnt    0~255     Count for message copy
*              uint8                 au8Data[0];            Data information of the message           
*******************************************************************************/
typedef struct _T_MSG_HEAD
{
    uint8   u8DestTask;             /* The Destination task number               */    
    uint8   u8SrcTask;              /* The task which sends this message         */
    uint8   u8MsgType;              /* Kind of message types                     */
    uint8   u8CopyCnt;              /* Count for message copy                    */
#ifndef __FLEXIBLE_ARRAY_NOT_SUPPORTED
    uint8   au8Data[0];             /* Data information of the message           */
#endif
}T_MSG_HEAD;


/*******************************************************************************
* Structure  : T_TEST_MSG
* Description: Structure of test message.
* Memebers   : uint32 u32Data                 4-byte format data
*              uint16 au16Data[2]             2-byte format data
*              uint8  au8Data[4]              1-byte format data
*******************************************************************************/
typedef struct _T_TEST_MSG
{
    union
    {
        uint32 u32Data;                       /* 4-byte format data */
        uint16 au16Data[2];                   /* 2-byte format data */
        uint8  au8Data[4];                    /* 1-byte format data */
    }DATA;
}T_TEST_MSG;


/******************************************************************************
* Name       : void Moe_Msg_Init(void)
* Function   : Init message function
* Input      : None
* Output:    : None
* Return     : None
* Description: None
* Version    : V1.00
* Author     : Ian
* Date       : 27 Jun 2016
******************************************************************************/
void Moe_Msg_Init(void);

/******************************************************************************
* Name       : uint8 Moe_Msg_Send(uint8 u8DestTask,uint8 u8MsgType,uint16 u16Size,void *ptMsg)
* Function   : Send the message to the destination task.
* Input      : uint8  u8DestTask    1~254     The destination task number
*                                   255       It is a to-all-task message
*              uint8  u8MsgType     0~255     Type of message
*              uint16 u16Size       0~65535   Length of the Message
*              void *ptMsg                    Pointer of user message information
* Output:    : None
* Return     : SW_OK   Successful.
*              SW_ERR  Failed.
* Description: To be done.
* Version    : V1.00
* Author     : Ian
* Date       : 28th May 2016
******************************************************************************/
uint8 Moe_Msg_Send(uint8 u8DestTask, uint8 u8MsgType, uint16 u16Size, void *ptMsg);


/******************************************************************************
* Name       : uint8 Moe_Msg_Forward(void *ptMsg, uint8 u8NextTask)
* Function   : Forward a message
* Input      : void *ptMsg                    The pointer of the message
*              uint8  u8NextTask    0~254     The next task number which receives 
*                                             such forwarded message
* Output:    : None
* Return     : SW_OK   Successful.
*              SW_ERR  Failed.
* Description: Note: If the u8NextTask is the same with u8DestTask, the next task
*                    will be set as TASK_NO_TASK, forward the same message to 
*                    itself is meaningless.
* Version    : V1.00
* Author     : Ian
* Date       : 5th Jun 2016
******************************************************************************/
uint8 Moe_Msg_Forward(T_MSG_HEAD *ptMsg, uint8 u8NextTask);


/******************************************************************************
* Name       : uint8 Moe_Msg_Process(void)
* Function   : Message process function, distribute "all task message" to each
*              task, and delete useless message.
* Input      : None
* Output:    : None
* Return     : SW_OK   Successful.
*              SW_ERR  Failed.
* Description: To be done.
* Version    : V1.00
* Author     : Ian
* Date       : 31st May 2016
******************************************************************************/
uint8 Moe_Msg_Process(T_MSG_HEAD *ptMsg);

 
#ifdef __cplusplus
}
#endif

#endif /* _MOE_MSG_H_ */

/* End of file */

