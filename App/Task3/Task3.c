/******************************************************************************
* File       : Task3.c
* Function   : It is No. 3 task which is just a demo task
* Description: To be done.           
* Version    : V1.00
* Author     : Ian
* Date       : 3rd May 2016
* History    :  No.  When           Who           What
*               1    3/May/2016     Ian           Create
******************************************************************************/

#include "../../Pub/type_def.h"
#include "../../Pub/common_head.h"
#include "project_config.h"
#include "../../Core/MOE_Core.h"
#include "../../Core/MOE_Event.h"
#include "../../Core/MOE_Timer.h"
#include "../../Core/MOE_Msg.h"
#include "./Task3.h"
#include "../../Pub/debug.h"


static uint8 sg_u8TaskID = TASK_NO_TASK;
/******************************************************************************
* Name       : uint8 Task3_Process(uint8 u8Evt, void *pPara)
* Function   : Task 3 process
* Input      : uint8  u8Evt  1~254     Event for the task
*              void  *pPara            Pointer of parameter
* Output:    : None
* Return     : SW_OK   Successful operation
*            : SW_ERR  Failed operation
*              1~254   Event which is not processed.
* Description: To be done
* Version    : V1.00
* Author     : Ian
* Date       : 3rd May 2016
******************************************************************************/
uint8 Task3_Process(uint8 u8Evt, void *pPara)
{   
    /* Check which event should be processed */
    switch (u8Evt)
    {
        /* If it is a timer event */
        case EVENT_PERIODIC:       
        {
            DBG_PRINT("I am task 3 and I am working!!\n");
            return SW_OK;     /* Return SW_OK to indicate event is processed */
            
        }

        /* If it is a message event */
        case EVENT_MSG:       
        {
            uint8  u8MsgType = ((T_MSG_HEAD*)pPara)->u8MsgType;
            static uint8 sg_u8Cnt = 0;

            u8MsgType = ((T_MSG_HEAD*)pPara)->u8MsgType;

            switch(u8MsgType)
            {
                case MSG_TYPE_TEST:
                {
                    T_TEST_MSG *ptTestMsg = (T_TEST_MSG*)MOE_MSG_CONTENT(pPara);
                    DBG_PRINT("\n\nThis is task %d\n",sg_u8TaskID);
                    DBG_PRINT("Get %d messages\n",sg_u8Cnt++);
                    DBG_PRINT("I get a uint32 data 0x%x!\n",(ptTestMsg->DATA.u32Data));
                    DBG_PRINT("I get a uint16 data 0x%x,0x%x!\n",(ptTestMsg->DATA.au16Data[0]),(ptTestMsg->DATA.au16Data[1]));
                    DBG_PRINT("I get a uint8 data 0x%x,0x%x,0x%x,0x%x!\n\n",(ptTestMsg->DATA.au8Data[0]),(ptTestMsg->DATA.au8Data[1]),(ptTestMsg->DATA.au8Data[2]),(ptTestMsg->DATA.au8Data[3]));
                }
                default:
                {
                    break;
                }

            }
            return SW_OK;     /* Return SW_OK to indicate event is processed */
        }

        /* If it is a test event */
        case EVENT_TEST:       
        {
            
            return SW_OK;     /* Return SW_OK to indicate event is processed */
        }

        /* If it is a timer event */
        case EVENT_TIMER:       
        {
            DBG_PRINT("I am a restart timer!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
            return SW_OK;     /* Return SW_OK to indicate event is processed */
        }

        /* If it is a timer event */
        case EVENT_DELAY:       
        {
            DBG_PRINT("I am a second delay!!!~~~~~~~~~~~~~~~~~~~~~\n");
            return SW_OK;     /* Return SW_OK to indicate event is processed */
        }

        /* If it is a message event */
        case EVENT_INIT:       
        {
            /******************************************************************/
            MOE_MANDATORY_INIT();  /* Mandatory init, shout call it here only */
            /******************************************************************/

            /*--------------------   Add your init code here   ----------------------*/
            Moe_Timer_Periodic(5000);
            /*-------------------   The end of your init code   ---------------------*/
            return SW_OK;     /* Return SW_OK to indicate event is processed */
        }

        /* If it is other event */
        default:       
        {
            return u8Evt;     /* Return event to indicate event is NOT processed */
        }
    }
}

/* End of file */



