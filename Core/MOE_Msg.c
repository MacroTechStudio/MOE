/******************************************************************************
* File       : MOE_Msg.c
* Function   : Provide message services.
* description: To be done.          
* Version    : V1.00
* Author     : Ian
* Date       : 26th May 2016
* History    :  No.  When           Who           What
*               1    26/May/2016    Ian           Create
******************************************************************************/

#include "type_def.h"
#include "common_head.h"
#include "project_config.h"
#include "MOE_Core.h"
#include "MOE_Event.h"
#include "MOE_Msg.h"
#include "debug.h"


static T_MSG_HEAD* Moe_Msg_Create(uint8 u8DestTask, uint8 u8MsgType, uint16 u16Size, void *ptMsg);
static T_MSG_HEAD* Moe_Msg_Del(T_MSG_HEAD *ptMsg);
static uint16 Moe_Msg_Max_Cnt(void);


static uint16       sg_u16MsgPollFlag = MOE_MSG_POLL_NONE;        /* Message poll request flag  */

static uint8        sg_au8RcvDone[MAX_TASK_NUM] = {0};

/******************************************************************************
* Name       : void Moe_Msg_Init(void)
* Function   : Init message function
* Input      : None
* Output:    : None
* Return     : None
* description: This function init the receive_done_flag array with all 0
* Version    : V1.00
* Author     : Ian
* Date       : 27 Jun 2016
******************************************************************************/
void Moe_Msg_Init(void)
{
    Moe_Memset(sg_au8RcvDone, 0, sizeof(sg_au8RcvDone));
    return;
}

/******************************************************************************
* Name       : T_MSG_HEAD* Moe_Msg_Create(uint8 *pu8DestTask,uint8 u8MsgType,uint16 u16Size,void *ptMsg)
* Function   : Create a message
* Input      : uint8  pu8DestTask   1~254     The destination task number
*              uint8  u8MsgType     0~255     Type of message
*              uint16 u16Size       0~65535   Length of the Message
*              void *ptMsg                    Pointer of user message information
* Output:    : uint8  pu8DestTask   1~254     The destination task number
* Return     : Pointer of the message data struct.
*              NULL:  Failed.
* description: To be done.
* Version    : V1.00
* Author     : Ian
* Date       : 26th May 2016
******************************************************************************/
static T_MSG_HEAD* Moe_Msg_Create(uint8 u8DestTask, uint8 u8MsgType, uint16 u16Size, void *ptMsg)
{
    uint32 u32IntSt;
    T_MSG_HEAD *ptMsgHead;
    uint16 u16Idx;

    ENTER_CRITICAL_ZONE(u32IntSt);  /* Enter the critical zone to prevent event updating unexpectedly */
    /**************************************************************************************************/
    ptMsgHead = (T_MSG_HEAD*)MOE_MALLOC(u16Size + sizeof(T_MSG_HEAD));
    /**************************************************************************************************/
    EXIT_CRITICAL_ZONE(u32IntSt);   /* Exit the critical zone                                         */

    if(NULL != ptMsgHead)
    {
        DBG_PRINT("Create a message successfully!!\n");      
        ptMsgHead->u8MsgType  = u8MsgType;
        ptMsgHead->u8SrcTask  = TASK_CURRENT_TASK;
        if(TASK_ALL_TASK == u8DestTask)
        {
            ptMsgHead->u8CopyCnt  = MAX_TASK_NUM - 1;     /* Message for all tasks                           */
#ifdef __WANTED_A_LIVE_FOX
            ptMsgHead->u8DestTask = TASK_CURRENT_TASK % TASK_LAST_TASK + 1; /* Start with the next task      */
            DBG_PRINT("Fox is ready to kill the next one!!\n");
#else
            /* Check if it is the first task which sends a message to other tasks */
            /* If so, start with the second task                                  */
            /* Otherwise, start with the first task                               */
            ptMsgHead->u8DestTask = TASK_FIRST_TASK + (!(TASK_CURRENT_TASK - TASK_FIRST_TASK));
#endif        
        }
        else
        {            
            ptMsgHead->u8CopyCnt  = 0;                    /* Message for single tasks                        */
            ptMsgHead->u8DestTask = u8DestTask;
        }
#ifdef __FLEXIBLE_ARRAY_NOT_SUPPORTED                     /* If the complier does NOT support flexible array */
        {
            uint8 *pu8Data;
            pu8Data = (uint8*)(ptMsgHead + 1);            /* Calculate the address of the field for data     */
            /* Copy the data from user information to the message */
            for(u16Idx = 0; u16Idx < u16Size; u16Idx++)
            {
                pu8Data[u16Idx] = ((uint8*)ptMsg)[u16Idx];
            }
        }
#else                                                     /* If the complier DO support flexible array       */
        /* Copy the data from user information to the message */
        for(u16Idx = 0; u16Idx < u16Size; u16Idx++)
        {
            ptMsgHead->au8Data[u16Idx] = ((uint8*)ptMsg)[u16Idx];
        }
#endif        
        return ptMsgHead;
    }

    DBG_PRINT("Can NOT create the message!! the operation of malloc is failed!!\n");
    return NULL;   
}

/******************************************************************************
* Name       : uint8 Moe_Msg_Send(uint8 u8DestTask,uint8 u8MsgType,uint16 u16Size,void *ptMsg)
* Function   : Send the message to the destination task.
* Input      : uint8  u8DestTask    1~254     The destination task number
*              uint8  u8MsgType     0~255     Type of message
*              uint16 u16Size       0~65535   Length of the Message
*              void *ptMsg                    Pointer of user message information
* Output:    : None
* Return     : SW_OK   Successful.
*              SW_ERR  Failed.
* description: To be done.
* Version    : V1.00
* Author     : Ian
* Date       : 28th May 2016
******************************************************************************/
uint8 Moe_Msg_Send(uint8 u8DestTask, uint8 u8MsgType, uint16 u16Size, void *ptMsg)
{    
    T_MSG_HEAD *ptMsgNode;
       
    /* Check if the destination task is valid or NOT */
    MOE_CHECK_IF_RET_ST(((MAX_TASK_NUM  >  u8DestTask)\
                      && (TASK_ALL_TASK != u8DestTask)\
                      || (TASK_NO_TASK  == u8DestTask)),\
                          "The destination task of the sending message should be valid!!\n");

    /* If the length of message is less then a message head */
    MOE_CHECK_IF_RET_ST((0 == u16Size),"The length of creating message is 0\n");

    /* Check if the message pointer is valid or NOT */
    MOE_CHECK_IF_RET_ST((ptMsg == NULL),"The message pointer should NOT be NULL\n");

    ptMsgNode = Moe_Msg_Create(u8DestTask, u8MsgType, u16Size, ptMsg);
    
    MOE_CHECK_IF_RET_ST((NULL == ptMsgNode), "Message is NOT created!!\n");

    Moe_Event_Set(ptMsgNode->u8DestTask, EVENT_MSG, MOE_EVENT_NORMAL, ptMsgNode);/* Set a message event to call destination task      */
    
    DBG_PRINT("Message is sent successfully!!\n");
    return SW_OK;
}


/******************************************************************************
* Name       : uint8 Moe_Msg_Forward(void *ptMsg, uint8 u8NextTask)
* Function   : Forward a message
* Input      : void *ptMsg                    The pointer of the message
*              uint8  u8NextTask    0~254     The next task number which receives 
*                                             such forwarded message
* Output:    : None
* Return     : SW_OK   Successful.
*              SW_ERR  Failed.
* description: Note: If the u8NextTask is the same with u8DestTask, the next task
*                    will be set as TASK_NO_TASK, forward the same message to 
*                    itself is meaningless.
* Version    : V1.00
* Author     : Ian
* Date       : 5th Jun 2016
******************************************************************************/
uint8 Moe_Msg_Forward(T_MSG_HEAD *ptMsg, uint8 u8NextTask)
{
    /* Check if the pointer is invalid or NOT */
    if(NULL == ptMsg)
    {
        DBG_PRINT("Invalid pointer for message!!\n");
        return SW_ERR;
    }

    /* The next task should NOT be bigger than the max task number                          */
    /* The next task should NOT be TASK_CURRENT_TASK, can NOT forward the message to itself */
    /* The next task should NOT be TASK_NO_TASK, it is meaningless                          */
    if((u8NextTask > MAX_TASK_NUM) || (TASK_CURRENT_TASK == u8NextTask) || (TASK_NO_TASK == u8NextTask))
    {
        DBG_PRINT("Invalid task number when forwarding a message!!\n");
        return SW_ERR;
    }

    /* If it is a message to all task, do NOT forward */
    if(0 != ptMsg->u8CopyCnt)
    {
        DBG_PRINT("It is a message to all task, do NOT forward!!\n");
        return SW_ERR;
    }

    /* Set the next task to receive such message */
    ptMsg->u8DestTask = u8NextTask;
    ptMsg->u8SrcTask  = TASK_CURRENT_TASK;
    Moe_Event_Set(u8NextTask, EVENT_MSG, MOE_EVENT_NORMAL, ptMsg); /* Call next task to receive message  */
    DBG_PRINT("The message is forwarded to task %d\n",u8NextTask);
    return SW_OK;
}


/******************************************************************************
* Name       : static T_MSG_HEAD* Moe_Msg_Del(T_MSG_HEAD *ptMsg)
* Function   : Detele a message
* Input      : T_MSG_HEAD *ptMsg  The message to be deteled.
* Output:    : None
* Return     : Pointer of the message data struct.
*              NULL:  Failed.
* description: To be done.
* Version    : V1.00
* Author     : Ian
* Date       : 31st May 2016
******************************************************************************/
static T_MSG_HEAD* Moe_Msg_Del(T_MSG_HEAD *ptMsg)
{
    /* Check if the pointer of message to be deteled is invalid or NOT */
    MOE_CHECK_IF_RET_VAL((ptMsg == NULL), NULL, "The pointer of message to be deteled is invalid!!\n");
    
    DBG_PRINT("Delete the message now.\n");
    MOE_FREE(ptMsg);
    return ptMsg;
}

/******************************************************************************
* Name       : uint8 Moe_Msg_Process(void)
* Function   : Message process function, distribute "all task message" to each
*              task, and delete useless message.
* Input      : None
* Output:    : None
* Return     : SW_OK   Successful.
*              SW_ERR  Failed.
* description: To be done.
* Version    : V1.00
* Author     : Ian
* Date       : 31st May 2016
******************************************************************************/
uint8 Moe_Msg_Process(T_MSG_HEAD *ptMsg)
{
    if(ptMsg->u8CopyCnt)
    {
        ptMsg->u8CopyCnt--;
        if(ptMsg->u8CopyCnt)
        {
#ifdef __WANTED_A_LIVE_FOX         
            ptMsg->u8DestTask = TASK_CURRENT_TASK % TASK_LAST_TASK + 1;/* Stop forward such message  */
            DBG_PRINT("Fox just killed %d!!\n", TASK_CURRENT_TASK);
#else
            /* If the next task is the one which sent this message */
            if(ptMsg->u8DestTask == ptMsg->u8SrcTask)
            {
                ptMsg->u8DestTask += 2;                      /* Forward to the next task   */
                DBG_PRINT("It is a self-message send by %d, ignore!!\n", TASK_CURRENT_TASK);
            }
            else
            {
                ptMsg->u8DestTask++;                         /* Forward to the next task   */  
            }                     
#endif
            Moe_Event_Set(ptMsg->u8DestTask, EVENT_MSG, MOE_EVENT_NORMAL, ptMsg);/* Set MSG event to next task */
            DBG_PRINT("The message to all tasks is processed by task %d, ready for next forwarding!!\n", TASK_CURRENT_TASK);
        }
                        
        /* If such message is for all task and this is the last forwarding */
        /* Or it is a message for a single task                            */
        else
        {        
            /* If it is a message for all tasks */
            if(0 != ptMsg->u8CopyCnt)
            {
#ifdef __WANTED_A_LIVE_FOX
                DBG_PRINT("Fox killed all except herself %d!!\n", ptMsg->u8SrcTask);                    
#endif
                DBG_PRINT("All tasks have received the message!!\n");
            } 
            MOE_FREE(ptMsg);
        }
        return SW_OK;
    }

    return SW_ERR;
}


/******************************************************************************
* Name       : void Moe_Msg_Never_Rcv_Check(uint8 u8Task, uint8 u8Evt)
* Function   : Check is the message is received or NOT after the EVENT_MSG task
*              process. 
* Input      : uint8     u8Task   1~254      Task number
*              uint8     u8Evt    0~255      Event number
* Output:    : None
* Return     : None
* description: To be done
* Version    : V1.00
* Author     : Ian
* Date       : 27th Jun 2016
******************************************************************************/
void Moe_Msg_Never_Rcv_Check(uint8 u8Task, uint8 u8Evt)
{   
    uint8 u8Temp;
 
    /* If it is a message event */
    if((EVENT_MSG == u8Evt)&&((TASK_NO_TASK != u8Task)&&(TASK_ALL_TASK != u8Task)))
    {   /* If the message has NOT been received after task process */
        if(MOE_MSG_RCV_DONE_NONE == sg_au8RcvDone[u8Task - 1])
        {   /* Received the message as a dummy reading */
            Moe_Msg_Receive(u8Task,&u8Temp);
        }
        /* Clear the flag of received */
        sg_au8RcvDone[u8Task - 1] = MOE_MSG_RCV_DONE_NONE;
    }    

    return;
}


/******************************************************************************
* Name       : static uint16 Moe_Msg_Max_Cnt(void)
* Function   : Get the max number of messages which can be created
* Input      : None
* Output:    : None
* Return     : The max number of messages
* description: To be done.
* Version    : V1.00
* Author     : Ian
* Date       : 6th Jun 2016
******************************************************************************/
static uint16 Moe_Msg_Max_Cnt(void)
{
    uint16      u16Cnt = 0;
    uint8       u8Type = 0;
    T_TEST_MSG  tMsg;
    T_MSG_HEAD *ptMsg  = (T_MSG_HEAD*)Moe_Msg_Send(TASK_FIRST_TASK, MSG_TYPE_TEST, sizeof(T_TEST_MSG), (void*)&tMsg);
    while(!ptMsg)
    {
         u16Cnt++;
         ptMsg  = (T_MSG_HEAD*)Moe_Msg_Send(TASK_FIRST_TASK, MSG_TYPE_TEST, sizeof(T_TEST_MSG), (void*)&tMsg);
    }
    DBG_PRINT("Max_count of message is %d!\n",u16Cnt);

    ptMsg = (T_MSG_HEAD*)Moe_Msg_Receive(TASK_FIRST_TASK, &u8Type);
    while(ptMsg)
    {
        ptMsg = (T_MSG_HEAD*)Moe_Msg_Receive(TASK_FIRST_TASK, &u8Type);
    }

    DBG_PRINT("%d messages are ready to be deleted!!\n", sg_u16MsgPollFlag);
    Moe_Msg_Process();
    if(NULL == sg_ptMsgListHead)
    {   
        DBG_PRINT("Max_count of message is %d!\n",u16Cnt);
        DBG_PRINT("All messages are deleted!!\n");
    }
    else
    {
        DBG_PRINT("All messages are NOT deleted!!\n");
    }
    return u16Cnt;
}

/******************************************************************************
* Name       : uint16 Moe_Msg_Total_Cnt(void)
* Function   : Get the max number of total messages in message link list.
* Input      : None
* Output:    : None
* Return     : The max number of total messages in message link list.
* description: To be done.
* Version    : V1.00
* Author     : Ian
* Date       : 6th Jun 2016
******************************************************************************/
uint16 Moe_Msg_Total_Cnt(void)
{
    uint16      u16Cnt = 0;
    T_MSG_HEAD *ptMsg  = sg_ptMsgListHead;
    
    /* Check if message exist or NOT  */
    while(ptMsg)
    {
        u16Cnt++;              /* Increase the count */
        ptMsg = ptMsg->ptNext; /* Check next one     */
    }
    DBG_PRINT("There are %d messages!\n", u16Cnt);
    return u16Cnt;
}


/******************************************************************************
* Name       : uint16 Moe_Msg_Read_Cnt(void)
* Function   : Get the max number of read messages
* Input      : None
* Output:    : None
* Return     : The max number of read messages
* description: To be done.
* Version    : V1.00
* Author     : Ian
* Date       : 8th Jun 2016
******************************************************************************/
uint16 Moe_Msg_Read_Cnt(void)
{
    DBG_PRINT("There are %d read messages!\n", sg_u16MsgPollFlag);
    return sg_u16MsgPollFlag;
}


/******************************************************************************
* Name       : uint16 Moe_Msg_Unread_Cnt(void)
* Function   : Get the max number of unread messages
* Input      : None
* Output:    : None
* Return     : The max number of unread messages
* description: To be done.
* Version    : V1.00
* Author     : Ian
* Date       : 6th Jun 2016
******************************************************************************/
uint16 Moe_Msg_Unread_Cnt(void)
{
    uint16      u16Cnt = 0;    
    
    u16Cnt = Moe_Msg_Total_Cnt() - Moe_Msg_Read_Cnt();

    DBG_PRINT("There are %d unread messages!\n", u16Cnt);
    return u16Cnt;
}

/******************************************************************************
* Name       : void Moe_Msg_Test_General(void)
* Function   : General test for message
* Input      : None
* Output:    : None
* Return     : None
* description: To be done.
* Version    : V1.00
* Author     : Ian
* Date       : 6th Jun 2016
******************************************************************************/
void Moe_Msg_Test_General(void)
{  
    uint8       u8Type = 0;
    T_TEST_MSG  tMsg;

    Moe_Msg_Max_Cnt();
    
    for (uint8 u8Idx = 0; u8Idx < 10; u8Idx++)
    {
        Moe_Msg_Send(TASK_FIRST_TASK, MSG_TYPE_TEST, sizeof(T_TEST_MSG), (void*)&tMsg);
    }
    
    if(10 != Moe_Msg_Total_Cnt())
    {
        DBG_PRINT("Total message count is wrong!!\n");
    }

    for (uint8 u8Idx = 0; u8Idx < 6; u8Idx++)
    {
        Moe_Msg_Receive(TASK_FIRST_TASK, &u8Type);
    }

    if(6 != Moe_Msg_Read_Cnt())
    {
        DBG_PRINT("Read message count is wrong!!\n");
    }

    if(4 != Moe_Msg_Unread_Cnt())
    {
        DBG_PRINT("Unead message count is wrong!!\n");
    }

    Moe_Msg_Process();
    
    if(4 != Moe_Msg_Total_Cnt())
    {
        DBG_PRINT("Total message count after deteling is wrong!!\n");
    }
    
    for (uint8 u8Idx = 0; u8Idx < 4; u8Idx++)
    {
        Moe_Msg_Receive(TASK_FIRST_TASK, &u8Type);
    }
    Moe_Msg_Process();
    if(0 != Moe_Msg_Total_Cnt())
    {
        DBG_PRINT("Total message count after deteling is wrong!!\n");
    }
    return;
}



/* end of file */

