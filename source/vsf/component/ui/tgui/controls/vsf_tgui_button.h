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

/****************************************************************************
*  Copyright 2020 by Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)    *
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

#ifndef __VSF_TGUI_CONTROLS_BUTTON_H__
#define __VSF_TGUI_CONTROLS_BUTTON_H__

/*============================ INCLUDES ======================================*/
#include "./__vsf_tgui_controls_common.h"
#include "./vsf_tgui_label.h"

/*! \NOTE: Make sure #include "utilities/ooc_class.h" is close to the class
 *!        definition and there is NO ANY OTHER module-interface-header file
 *!        included in this file
 */
#if     defined(__VSF_TGUI_CONTROLS_BUTTON_CLASS_IMPLEMENT)
#   define __PLOOC_CLASS_IMPLEMENT__
#   undef __VSF_TGUI_CONTROLS_BUTTON_CLASS_IMPLEMENT
#elif   defined(__VSF_TGUI_CONTROLS_BUTTON_CLASS_INHERIT)
#   define __PLOOC_CLASS_INHERIT__
#   undef __VSF_TGUI_CONTROLS_BUTTON_CLASS_INHERIT
#endif

#include "utilities/ooc_class.h"

/*============================ MACROS ========================================*/


#define __VSF_TGUI_INTERFACE_CONTROLS_BUTTON         {                          \
            .msg_handler = {                                                    \
                VSF_MSGT_NODE_HANDLER_TYPE_FSM,                                 \
                (vsf_msgt_method_fsm_t *)&vsf_tgui_button_msg_handler,          \
            },                                                                  \
            .Status = (vsf_msgt_method_status_t *)                              \
                        &vsf_tgui_control_status_get,                           \
            .Shoot = (vsf_msgt_method_shoot_t *)&vsf_tgui_control_shoot,        \
        }


/*============================ MACROFIED FUNCTIONS ===========================*/


#define __tgui_button(__NAME, __PARENT_ADDR, __PREVIOUS, __NEXT, ...)           \
        tgui_control_base(   __NAME,                                            \
                            VSF_TGUI_COMPONENT_ID_BUTTON,                       \
                            vsf_tgui_button_t,                                  \
                            (__PARENT_ADDR),                                    \
                            __PREVIOUS,                                         \
                            __NEXT,                                             \
                            VSF_TGUI_V_BUTTON_STATIC_INIT_DEFAULT               \
                            __VA_ARGS__                                         \
                            VSF_TGUI_V_BUTTON_STATIC_INIT_OVERRIDE)

#define tgui_button(__NAME, __PARENT_ADDR, __PREVIOUS, __NEXT, ...)             \
            __tgui_button(  __NAME,                                             \
                            (__PARENT_ADDR),                                    \
                            __PREVIOUS,                                         \
                            __NEXT,                                             \
                            __VA_ARGS__)

#define __tgui_set_internal_button( __OWNER_ADDR,                               \
                                __MEMBER,                                       \
                                __PREVIOUS,                                     \
                                __NEXT, ...)                                    \
            (*__OWNER_ADDR) tgui_button(                                        \
                                __MEMBER,                                       \
                                (__OWNER_ADDR),                                 \
                                __PREVIOUS,                                     \
                                __NEXT,                                         \
                                __VA_ARGS__)

#define tgui_set_internal_button( __OWNER_ADDR,                                 \
                                __MEMBER,                                       \
                                __PREVIOUS,                                     \
                                __NEXT, ...)                                    \
        __tgui_set_internal_button(   (__OWNER_ADDR),                           \
                                    __MEMBER,                                   \
                                    __PREVIOUS,                                 \
                                    __NEXT,                                     \
                                    __VA_ARGS__)


#define tgui_set_priv_button( __OWNER_ADDR ,                                    \
                                __MEMBER,                                       \
                                ...)                                            \
        __tgui_set_internal_button(   (__OWNER_ADDR),                           \
                                    __MEMBER,                                   \
                                    __MEMBER,                                   \
                                    __MEMBER,                                   \
                                    __VA_ARGS__)


/*============================ TYPES =========================================*/

declare_class(vsf_tgui_button_t)

def_class(vsf_tgui_button_t,
    which(
        implement(vsf_tgui_label_t)
        implement(vsf_tgui_v_button_t)
    )

    public_member(
        implement_ex(
            struct {
                uint8_t     bIsCheckButton      : 1;
                uint8_t     bIsChecked          : 1;
                uint8_t     bIsAllowEmphasize   : 1;
                uint8_t     bIsEmphasized       : 1;
            },
            _
        )
    )
)
end_def_class(vsf_tgui_button_t)


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern
fsm_rt_t vk_tgui_button_init(vsf_tgui_button_t* ptButton);

extern
fsm_rt_t vk_tgui_button_update(vsf_tgui_button_t* ptButton);

extern
fsm_rt_t vsf_tgui_button_msg_handler(vsf_tgui_button_t* control_ptr,
                                        vsf_tgui_msg_t* ptMSG);

#endif
/* EOF */

