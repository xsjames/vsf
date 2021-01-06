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

#ifndef __VSF_TINY_GUI_V_LABEL_H__
#define __VSF_TINY_GUI_V_LABEL_H__

/*============================ INCLUDES ======================================*/
#include "../../vsf_tgui_cfg.h"

#if     VSF_USE_TINY_GUI == ENABLED                                             \
    &&  VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL == VSF_TGUI_V_TEMPLATE_EXAMPLE

#include "../../controls/vsf_tgui_controls.h"


/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
def_structure(vsf_tgui_v_label_t)
#if __IS_COMPILER_IAR__
//please remove this if your structure is not emplty
uint8_t : 8;
#endif
end_def_structure(vsf_tgui_v_label_t)


/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

extern fsm_rt_t vsf_tgui_label_v_init(vsf_tgui_label_t* ptLabel);

extern fsm_rt_t vsf_tgui_label_v_rendering(vsf_tgui_label_t* ptLabel,
    vsf_tgui_region_t* ptDirtyRegion);

extern fsm_rt_t vsf_tgui_label_v_depose(vsf_tgui_label_t* ptLabel);

#endif

#endif
/* EOF */
