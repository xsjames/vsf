/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/
/*============================ INCLUDES ======================================*/
#include "vsf.h"
#include <stdio.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
declare_vsf_pt(user_pt_task_t)
declare_vsf_pt(user_pt_sub_t)

#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
declare_vsf_task(user_sub_task_t)
#endif

def_vsf_pt(user_pt_sub_t,
    def_params(
        uint32_t cnt;
    ));

#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
def_vsf_task(user_sub_task_t,
    def_params(      
        uint32_t cnt;
    ));
#endif

#if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
declare_vsf_thread(user_thread_b_t)
def_vsf_thread(user_thread_b_t, 1024);


#endif

def_vsf_pt(user_pt_task_t,
    def_params(
        vsf_sem_t *sem_ptr;
        uint32_t cnt;
        
        vsf_pt(user_pt_sub_t)       print_task;
    #if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
        vsf_task(user_sub_task_t)   progress_task;
    #endif
    #if     VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED                            \
        &&  VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
        vsf_thread(user_thread_b_t) thread_task;
    #endif
    ));
    


    
#if VSF_OS_CFG_MAIN_MODE != VSF_OS_CFG_MAIN_MODE_THREAD
declare_vsf_pt(user_pt_task_b_t)
def_vsf_pt(user_pt_task_b_t,
    def_params(
        uint32_t cnt;
        vsf_sem_t *sem_ptr;
    ));
#else
declare_vsf_thread(user_thread_a_t)

def_vsf_thread(user_thread_a_t, 1024,

    features_used(
        mem_sharable( )
        mem_nonsharable( )
    )
    
    def_params(
        vsf_sem_t *sem_ptr;
    ));
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
static NO_INIT vsf_sem_t __user_sem;
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
implement_vsf_thread(user_thread_b_t)
{
    printf("run thread...delay 100ms...");
    vsf_delay_ms(100);
    printf("cpl\r\n");
}
#endif


private implement_vsf_pt(user_pt_sub_t) 
{
    vsf_pt_begin();
   
    printf("receive semaphore...[%08x]\r\n", this.cnt++);
     
    vsf_pt_end();
}

#define RESET_FSM()     do {vsf_task_state = 0;} while(0)

#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
private implement_vsf_task(user_sub_task_t)
{
    vsf_task_begin();
    enum {
        START,
        PRINT_PROGRESS,
    };
    
    /*! \note when you want to use fsm_rt_asyn, you need a dedicated chState
     *        rather than using vsf_task_state by default.
     */
    switch (vsf_task_state) {
        case START:
            this.cnt = 0;
            vsf_task_state = PRINT_PROGRESS;
            printf("\r\n[");
            //break;
        case PRINT_PROGRESS:
            if (this.cnt >= 100) {
                printf("]\r\n");
                RESET_FSM();
                return fsm_rt_cpl;
            }
            printf(".");
            this.cnt += 5;
            if (0 == (this.cnt % 25)) {
                return fsm_rt_asyn;
            }
            break;
    }
    
    vsf_task_end();
}
#endif

private implement_vsf_pt(user_pt_task_t) 
{
    vsf_pt_begin();

    this.cnt = 0;
    while(1) {
        vsf_pt_wait_until(vsf_sem_pend(this.sem_ptr));

    #if VSF_KERNEL_CFG_EDA_SUPPORT_SUB_CALL == ENABLED
        this.print_task.cnt = this.cnt;                                         //!< Pass parameter
        vsf_pt_call_pt(user_pt_sub_t, &this.print_task);
        //! pt call complete
        this.cnt = this.print_task.cnt;                                         //!< read parameter
    #else
        printf("receive semaphore...[%08x]\r\n", this.cnt++);
    #endif
    
    #if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
        prepare_vsf_task(user_sub_task_t,&this.progress_task);
        do {
            fsm_rt_t ret;
            vsf_pt_call_task(user_sub_task_t, &this.progress_task, &ret);
            if (fsm_rt_cpl == ret) {
                break;
            } /* else if (fsm_rt_asyn == ret ) */
            printf("%2d%%", this.progress_task.cnt);
        } while(true);
    #endif
    
    #if     VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED                            \
        &&  VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
        vsf_pt_call_thread(user_thread_b_t, &(this.thread_task));
    #endif
        
    }

    vsf_pt_end();
}

#if VSF_OS_CFG_MAIN_MODE != VSF_OS_CFG_MAIN_MODE_THREAD
private implement_vsf_pt(user_pt_task_b_t) 
{
    vsf_pt_begin();
    
    while(1) {
        vsf_pt_wait_until( vsf_delay_ms(10000) );                               //!< wait 10s
        printf("post semaphore...   [%08x]\r\n", this.cnt++);
        vsf_sem_post(this.sem_ptr);                                                //!< post a semaphore
    }
    
    vsf_pt_end();
}
#else
implement_vsf_thread(user_thread_a_t) 
{
    uint32_t cnt = 0;
    while (1) {
        vsf_delay_ms(10000);
        printf("post semaphore...   [%08x]\r\n", cnt++);
        vsf_sem_post(&__user_sem);            //!< post a semaphore
    }
}

#endif

void vsf_kernel_pt_simple_demo(void)
{  
    //! initialise semaphore
    vsf_sem_init(&__user_sem, 0); 
    
    //! start a user task
    {
        static NO_INIT user_pt_task_t __user_pt;
        __user_pt.param.sem_ptr = &__user_sem;
        init_vsf_pt(user_pt_task_t, &__user_pt, vsf_prio_inherit);
    };

#if     VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED                                \
    &&  VSF_OS_CFG_MAIN_MODE == VSF_OS_CFG_MAIN_MODE_THREAD
    
    //! start the user task a
    {
        static NO_INIT user_thread_a_t __user_task_a;
        __user_task_a.param.sem_ptr = &__user_sem;
        init_vsf_thread(user_thread_a_t, &__user_task_a, vsf_prio_0);
    }
#else
    //! in this case, we only use main to initialise vsf_tasks

    //! start a user task b
    {
        static NO_INIT user_pt_task_b_t __user_pt_task_b;
        __user_pt_task_b.param.sem_ptr = &__user_sem;
        __user_pt_task_b.param.cnt = 0;
        init_vsf_pt(user_pt_task_b_t, &__user_pt_task_b, vsf_prio_0);
    }
#endif
}

#if VSF_PROJ_CFG_USE_CUBE != ENABLED
int main(void)
{
    static_task_instance(
        features_used(
            mem_sharable( )
            mem_nonsharable( )
        )
    )

    vsf_stdio_init();
    
    vsf_kernel_pt_simple_demo();
    
#if     VSF_OS_CFG_MAIN_MODE == VSF_OS_CFG_MAIN_MODE_THREAD                     \
    &&  VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
    while(1) {
        printf("hello world! \r\n");
        vsf_delay_ms(1000);
    }
#else
    return 0;
#endif
}

#endif

