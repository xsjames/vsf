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
#include "app_cfg.h"
#include <stdio.h>

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
declare_vsf_task(user_task_t)
declare_vsf_task(user_sub_task_t)

#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
def_vsf_task(user_sub_task_t,
    def_params(
        uint32_t cnt;
    ));
#endif

def_vsf_task(user_task_t,
    def_params(
        vsf_sem_t *sem_ptr;
        uint32_t cnt;
    #if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
        vsf_task(user_sub_task_t) print_task;
    #endif
    ));
                                                       


#if VSF_KERNEL_CFG_SUPPORT_THREAD != ENABLED
declare_vsf_task(user_task_b_t)
def_vsf_task(user_task_b_t,
    def_params(
        vsf_sem_t *sem_ptr;
        uint32_t cnt;
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
#if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
implement_vsf_task(user_sub_task_t) 
{
    vsf_task_begin();
    printf("receive semaphore...[%08x]\r\n", this.cnt++);
    return fsm_rt_cpl;                  //!< return to caller
    vsf_task_end();
}
#endif

#define USER_TASK_RESET_FSM()   do { vsf_task_state = 0;} while(0)

implement_vsf_task(user_task_t) 
{
    vsf_task_begin();
    enum {
        WAIT_FOR_SEM = 0,
        CALL_SUB_TO_PRINT,
    };
    
    on_vsf_task_init() {
        this.cnt = 0;
    }

    switch (vsf_task_state) {
        case WAIT_FOR_SEM:    
            
            vsf_task_wait_until(vsf_sem_pend(this.sem_ptr));                       //!< wait for semaphore forever  
        #if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
            prepare_vsf_task(user_sub_task_t, &this.print_task);
            this.print_task.cnt = this.cnt;                                     //!< passing parameter
        #endif
            vsf_task_state = CALL_SUB_TO_PRINT;                                 //!< tranfer to next state
            break;
            
        case CALL_SUB_TO_PRINT:
        #if VSF_KERNEL_CFG_EDA_SUPPORT_FSM == ENABLED
            if (fsm_rt_cpl == vsf_task_call_task(user_sub_task_t, 
                                            &this.print_task)) {
                //! task complete
                this.cnt = this.print_task.cnt;                                 //!< read param value
                USER_TASK_RESET_FSM();                                          //!< reset fsm
            }
        #else
            printf("receive semaphore...[%08x]\r\n", this.cnt++);
            USER_TASK_RESET_FSM();
        #endif
            break;
    }
    vsf_task_end();
}

#if VSF_KERNEL_CFG_SUPPORT_THREAD != ENABLED
implement_vsf_task(user_task_b_t) 
{
    vsf_task_begin();
    enum {
        DELAY = 0,
        PRINT = 1,
    };
    
    switch(vsf_task_state) {
        case DELAY:
            vsf_task_wait_until(vsf_delay_ms(3000));                           //!< wait 10s 
            vsf_task_state = PRINT;
            break;
            
        case PRINT:
            printf("post semaphore...   [%08x]\r\n", this.cnt++);
            vsf_sem_post(this.sem_ptr);                                            //!< post a semaphore
            USER_TASK_RESET_FSM();                                              //!< reset fsm
            break;
        
    }
    
    vsf_task_end();
}
#else
implement_vsf_thread(user_thread_a_t) 
{
    uint32_t cnt = 0;
    while (1) {
        vsf_delay_ms(3000);
        printf("post semaphore...   [%08x]\r\n", cnt++);
        vsf_sem_post(&__user_sem);            //!< post a semaphore
    }
}

#endif

void vsf_kernel_task_simple_demo(void)
{   
    //! initialise semaphore
    vsf_sem_init(&__user_sem, 0); 
    
    //! start a user task
    {
        static NO_INIT user_task_t __user_task;
        __user_task.param.sem_ptr = &__user_sem;
        init_vsf_task(user_task_t, &__user_task, vsf_prio_0);
    }

#if VSF_KERNEL_CFG_SUPPORT_THREAD == ENABLED
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
        static NO_INIT user_task_b_t __user_task_b;
        __user_task_b.param.sem_ptr = &__user_sem;
        __user_task_b.param.cnt = 0;
        init_vsf_task(user_task_b_t, &__user_task_b, vsf_prio_0);
    }
#endif
}

#if VSF_PROJ_CFG_USE_CUBE != ENABLED
#if VSF_OS_CFG_MAIN_MODE == VSF_OS_CFG_MAIN_MODE_THREAD

int main(void)
{
    static_task_instance(
        features_used(
            mem_sharable( )
            mem_nonsharable( )
        )
    )

    vsf_stdio_init();
    
    vsf_kernel_task_simple_demo();
    
    while(1) {
        printf("hello world! \r\n");
        vsf_delay_ms(1000);
    }
}
#elif   VSF_OS_CFG_MAIN_MODE == VSF_OS_CFG_MAIN_MODE_EDA                        \
    ||  (   VSF_OS_CFG_MAIN_MODE == VSF_OS_CFG_MAIN_MODE_IDLE                   \
        &&  VSF_OS_CFG_ADD_EVTQ_TO_IDLE == ENABLED)
void main(void)
{
    static_task_instance(
        features_used(
            mem_sharable( )
            mem_nonsharable( )
        )
        def_params(
            uint32_t cnt;
        )
    )

    vsf_pt_begin()

    vsf_stdio_init();
    
    vsf_kernel_task_simple_demo();
    
    this.cnt = 0;
    while(1) {
        printf("hello world! \r\n");
        vsf_pt_wait_until(vsf_delay_ms(1000));
    }
    vsf_pt_end()
}
#else 
int main(void)
{
    vsf_stdio_init();
    
    vsf_kernel_task_simple_demo();
    
    return 0;
}

#endif
#endif
