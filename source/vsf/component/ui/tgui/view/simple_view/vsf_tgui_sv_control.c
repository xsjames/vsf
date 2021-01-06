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
#include "../../vsf_tgui_cfg.h"

#if     VSF_USE_TINY_GUI == ENABLED \
    &&  VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL == VSF_TGUI_V_TEMPLATE_SIMPLE_VIEW

#define __VSF_TGUI_CONTROLS_CONTROL_CLASS_INHERIT
declare_class(vsf_tgui_t)

#include "./vsf_tgui_sv_control.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ IMPLEMENTATION ================================*/

#if VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED
const char* vsf_tgui_control_get_node_name(vsf_tgui_control_t* control_ptr)
{
    return control_ptr->use_as__vsf_msgt_node_t.node_name_ptr;
}
#endif

fsm_rt_t vsf_tgui_control_v_init(vsf_tgui_control_t* control_ptr)
{
#if (VSF_TGUI_CFG_SV_RENDERING_LOG == ENABLED) && (VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED)
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]%s(%p) control view init" VSF_TRACE_CFG_LINEEND, vsf_tgui_control_get_node_name(control_ptr), control_ptr);
#endif

    /*
    vsf_tgui_status_t Status = vsf_tgui_control_status_get((vsf_tgui_control_t*)control_ptr);
    Status.chStatus |= VSF_TGUI_CTRL_STATUS_INITIALISED    |
                        VSF_TGUI_CTRL_STATUS_VISIBLE        |
                        VSF_TGUI_CTRL_STATUS_ENABLED;

    vsf_tgui_control_status_set((vsf_tgui_control_t *)control_ptr, Status);

    return vk_tgui_control_update(control_ptr);
    */
    return fsm_rt_cpl;
}



fsm_rt_t __vk_tgui_control_v_rendering(  vsf_tgui_control_t* control_ptr,
                                        vsf_tgui_region_t* ptDirtyRegion,       //!< you can ignore the tDirtyRegion for simplicity
                                        vsf_tgui_control_refresh_mode_t tMode,
                                        bool bIsIgnoreBackground)
{
    const vsf_tgui_tile_t* ptTile;

    VSF_TGUI_ASSERT(control_ptr != NULL);
    VSF_TGUI_ASSERT(ptDirtyRegion != NULL);

#if (VSF_TGUI_CFG_SV_RENDERING_LOG == ENABLED) && (VSF_TGUI_CFG_SUPPORT_NAME_STRING == ENABLED)
    VSF_TGUI_LOG(VSF_TRACE_INFO, "[Simple View]%s(%p) control view rendering" VSF_TRACE_CFG_LINEEND, vsf_tgui_control_get_node_name(control_ptr), control_ptr);
#endif

    if (!control_ptr->bIsNoBackgroundColor && !bIsIgnoreBackground) {
        vsf_tgui_region_t tRegion = { 0 };
        tRegion.tSize = *vsf_tgui_control_get_size(control_ptr);

        vsf_tgui_control_v_draw_rect(   control_ptr,
                                        ptDirtyRegion,
                                        &tRegion,
                                        control_ptr->use_as____vsf_tgui_control_core_t.use_as__vsf_tgui_v_control_t.tBackgroundColor);
    }

    ptTile = control_ptr->tBackground.ptTile;
    if (ptTile != NULL) {
        vsf_tgui_control_v_draw_tile(control_ptr, ptDirtyRegion, ptTile, control_ptr->tBackground.tAlign);
    }

    return fsm_rt_cpl;
}


fsm_rt_t vsf_tgui_control_v_rendering(  vsf_tgui_control_t* control_ptr,
                                        vsf_tgui_region_t* ptDirtyRegion,       //!< you can ignore the tDirtyRegion for simplicity
                                        vsf_tgui_control_refresh_mode_t tMode)
{
    return __vk_tgui_control_v_rendering(control_ptr, ptDirtyRegion, tMode, false);
}

fsm_rt_t vsf_tgui_control_v_depose(vsf_tgui_control_t* control_ptr)
{
    return fsm_rt_cpl;
}

fsm_rt_t vsf_tgui_control_v_update(vsf_tgui_control_t* control_ptr)
{
    return fsm_rt_cpl;
}

#endif


/* EOF */
