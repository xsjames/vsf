/*****************************************************************************
 *   Copyright(C)2009-2019 by SimonQian                                      *
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

#ifndef __VSFIP_NETDRV_ADAPTER_H__
#define __VSFIP_NETDRV_ADAPTER_H__

/*============================ INCLUDES ======================================*/

#include "component/tcpip/vsf_tcpip_cfg.h"

#if VSF_USE_TCPIP == ENABLED && VSF_USE_VSFIP == ENABLED

#include "./vsfip_netif.h"
#include "./component/tcpip/netdrv/vsf_netdrv.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern void vsfip_netif_set_netdrv(vsfip_netif_t *netif, vk_netdrv_t *netdrv);
extern vk_netdrv_t * vsfip_netif_get_netdrv(vsfip_netif_t *netif);

#endif      // VSF_USE_TCPIP
#endif      // __VSFIP_NETDRV_ADAPTER_H__
