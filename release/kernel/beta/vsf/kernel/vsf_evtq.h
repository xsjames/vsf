/****************************************************************************
*   Copyright (C) 2009 - 2019 by Simon Qian <SimonQian@SimonQian.com>       *
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

#ifndef __VSF_EVTQ_H__
#define __VSF_EVTQ_H__

/*============================ INCLUDES ======================================*/
#include "kernel/vsf_kernel_cfg.h"
#if VSF_USE_KERNEL == ENABLED

#include "./vsf_eda.h"
/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/

struct vsf_evtq_ctx_t {
    vsf_eda_t *eda;
    vsf_evt_t evt;
    uintptr_t msg;
};
typedef struct vsf_evtq_ctx_t vsf_evtq_ctx_t;

#if VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED == ENABLED

typedef struct vsf_evt_node_t vsf_evt_node_t;
typedef struct vsf_evtq_t vsf_evtq_t;

#if VSF_KERNEL_CFG_SUPPORT_DYNAMIC_PRIOTIRY == ENABLED

struct vsf_evtq_t {
    vsf_dlist_t rdy_list;
    vsf_evtq_ctx_t cur;
};

struct vsf_evt_node_t {
    implement(vsf_slist_node_t)

#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
    vsf_evt_t evt;
    void *msg;
#else
    union {
        uintptr_t value;
        void *msg;
    } evt_union;
#endif
};

#else

struct vsf_evt_node_t {
    vsf_eda_t *eda;

#if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
    vsf_evt_t evt;
    void *msg;
#else
    union {
        uintptr_t value;
        void *msg;
    } evt_union;
#endif
};

struct vsf_evtq_t {
    vsf_evt_node_t *node;
    uint8_t bitsize;

    // private
    uint8_t head;
    uint8_t tail;
    vsf_evtq_ctx_t cur;
};

#endif
#endif

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#if VSF_KERNEL_CFG_ALLOW_KERNEL_BEING_PREEMPTED == ENABLED
extern void vsf_evtq_on_eda_init(vsf_eda_t *eda);
extern void vsf_evtq_on_eda_fini(vsf_eda_t *eda);

extern vsf_err_t vsf_evtq_init(vsf_evtq_t *pthis);
#   if VSF_KERNEL_CFG_SUPPORT_EVT_MESSAGE == ENABLED
extern vsf_err_t vsf_evtq_post_evt_msg(vsf_eda_t *eda, vsf_evt_t evt, void *msg);
#   endif
extern vsf_err_t vsf_evtq_post_evt_ex(vsf_eda_t *eda, vsf_evt_t evt, bool force);
extern vsf_err_t vsf_evtq_post_evt(vsf_eda_t *eda, vsf_evt_t evt);
extern vsf_err_t vsf_evtq_post_msg(vsf_eda_t *eda, void *msg);
extern vsf_err_t vsf_evtq_poll(vsf_evtq_t *pthis);
#endif

#endif
#endif
