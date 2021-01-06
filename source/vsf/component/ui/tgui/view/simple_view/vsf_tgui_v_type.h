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

#ifndef __VSF_TINY_GUI_V_TMPLATE_TYPE_H__
#define __VSF_TINY_GUI_V_TMPLATE_TYPE_H__

/*============================ INCLUDES ======================================*/
#include "../../vsf_tgui_cfg.h"

#if     VSF_USE_TINY_GUI == ENABLED                                             \
    &&  VSF_TGUI_CFG_RENDERING_TEMPLATE_SEL == VSF_TGUI_V_TEMPLATE_SIMPLE_VIEW

#include "./vsf_tgui_sv_color.h"

/*============================ MACROS ========================================*/
#ifndef VSF_TGUI_CFG_SV_CONTROL_BACKGROUND_COLOR
#   define VSF_TGUI_CFG_SV_CONTROL_BACKGROUND_COLOR             VSF_TGUI_COLOR_DEF(0xB4, 0xC7, 0xE7)
#endif

#ifndef VSF_TGUI_CFG_SV_LABEL_BACKGROUND_COLOR
#   define VSF_TGUI_CFG_SV_LABEL_BACKGROUND_COLOR               VSF_TGUI_COLOR_DEF(0xB4, 0xC7, 0xE7)
#endif

#ifndef VSF_TGUI_CFG_SV_BUTTON_BACKGROUND_COLOR
#   define VSF_TGUI_CFG_SV_BUTTON_BACKGROUND_COLOR              VSF_TGUI_COLOR_DEF(0xB4, 0xC7, 0xE7)
#endif

#ifndef VSF_TGUI_CFG_SV_BUTTON_SUPPORT_CLICKED_BACKGROUND_COLOR
#   define VSF_TGUI_CFG_SV_BUTTON_SUPPORT_CLICKED_BACKGROUND_COLOR  ENABLED
#endif

#if VSF_TGUI_CFG_SV_BUTTON_SUPPORT_CLICKED_BACKGROUND_COLOR == ENABLED
#   ifndef VSF_TGUI_CFG_SV_BUTTON_CLICKED_BACKGROUND_COLOR
#       define VSF_TGUI_CFG_SV_BUTTON_CLICKED_BACKGROUND_COLOR  VSF_TGUI_COLOR_RGB(0x18, 0x53, 0x9C)
#   endif

#   define VSF_TGUI_V_BUTTON_STATIC_INIT_CLICKED_DEFAULT        .tClickedBackgroundColor = VSF_TGUI_CFG_SV_BUTTON_CLICKED_BACKGROUND_COLOR,
#else
#   define VSF_TGUI_V_BUTTON_STATIC_INIT_CLICKED_DEFAULT
#endif

#ifndef VSF_TGUI_CFG_SV_CONTAINER_BACKGROUND_COLOR
#   define VSF_TGUI_CFG_SV_CONTAINER_BACKGROUND_COLOR   VSF_TGUI_COLOR_DEF(0x44, 0x72, 0xC4)
#endif

#ifndef VSF_TGUI_CFG_SV_TEXT_LIST_BACKGROUND_COLOR
#   define VSF_TGUI_CFG_SV_TEXT_LIST_BACKGROUND_COLOR           VSF_TGUI_COLOR_DEF(0xB4, 0xC7, 0xE7)
#endif

#ifndef VSF_TGUI_CFG_SV_TEXT_LIST_INDICATOR_COLOR
#   define VSF_TGUI_CFG_SV_TEXT_LIST_INDICATOR_COLOR            VSF_TGUI_COLOR_DEF(0x44, 0x72, 0xC4)
#endif


#define VSF_TGUI_V_TEST_LIST_STATIC_INIT_DEFAULT                                \
            .bIsNoBackgroundColor = false,                                      \
            .tBackgroundColor = VSF_TGUI_CFG_SV_TEXT_LIST_BACKGROUND_COLOR,

#define VSF_TGUI_V_LIST_STATIC_INIT_DEFAULT

#define VSF_TGUI_V_CONTROL_STATIC_INIT_DEFAULT                                  \
            .bIsNoBackgroundColor = false,                                      \
            .tBackgroundColor = VSF_TGUI_CFG_SV_CONTROL_BACKGROUND_COLOR,

#define VSF_TGUI_V_CONTAINER_STATIC_INIT_DEFAULT                                \
            .bIsNoBackgroundColor = false,                                      \
            .tBackgroundColor = VSF_TGUI_CFG_SV_CONTAINER_BACKGROUND_COLOR,

#define VSF_TGUI_V_LABEL_STATIC_INIT_DEFAULT                                    \
            .bIsNoBackgroundColor = false,                                      \
            .tBackgroundColor = VSF_TGUI_CFG_SV_LABEL_BACKGROUND_COLOR,

#define VSF_TGUI_V_BUTTON_STATIC_INIT_DEFAULT                                   \
            .bIsNoBackgroundColor = false,                                      \
            .tBackgroundColor = VSF_TGUI_CFG_SV_BUTTON_BACKGROUND_COLOR,        \
            VSF_TGUI_V_BUTTON_STATIC_INIT_CLICKED_DEFAULT

#define VSF_TGUI_V_PANEL_STATIC_INIT_DEFAULT                                    \
            .bIsShowCornerTile = true,

#define VSF_TGUI_V_TEST_LIST_STATIC_INIT_OVERRIDE                               \
            .tList.tContent.bIsUseRawView = true,                               \
            .tList.tContent.bIsNoBackgroundColor = true,                        \
            .tList.bIsNoBackgroundColor = true,

#define VSF_TGUI_V_LIST_STATIC_INIT_OVERRIDE
#define VSF_TGUI_V_CONTROL_STATIC_INIT_OVERRIDE
#define VSF_TGUI_V_CONTAINER_STATIC_INIT_OVERRIDE
#define VSF_TGUI_V_LABEL_STATIC_INIT_OVERRIDE
#define VSF_TGUI_V_BUTTON_STATIC_INIT_OVERRIDE
#define VSF_TGUI_V_PANEL_STATIC_INIT_OVERRIDE

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/

declare_structure(vsf_tgui_v_control_t)

def_structure(vsf_tgui_v_control_t)
    union {
        uint8_t ControlAttributes;
        struct {
            uint8_t bIsTileTransparency: 1;
            uint8_t bIsNoBackgroundColor: 1;
        };
    };
    uint8_t chTileTransparencyRate;
    vsf_tgui_sv_color_t tBackgroundColor;
end_def_structure(vsf_tgui_v_control_t)

declare_structure(vsf_tgui_v_container_t)

def_structure(vsf_tgui_v_container_t)
    uint8_t bIsShowCornerTile;
end_def_structure(vsf_tgui_v_container_t)

declare_structure(vsf_tgui_v_label_t)
def_structure(vsf_tgui_v_label_t)
    uint8_t bIsUseRawView;
    uint8_t chFontIndex;
    vsf_tgui_sv_color_t tFontColor;
end_def_structure(vsf_tgui_v_label_t)

declare_structure(vsf_tgui_v_button_t)
def_structure(vsf_tgui_v_button_t)
#if VSF_TGUI_CFG_SV_BUTTON_SUPPORT_CLICKED_BACKGROUND_COLOR == ENABLED
    vsf_tgui_sv_color_t tClickedBackgroundColor;
#endif
#if __IS_COMPILER_IAR__ || (!defined( __STDC_VERSION__ ) || __STDC_VERSION__ < 199901L)
    //please remove this if your structure is not emplty
    REG_RSVD_U8
#endif
end_def_structure(vsf_tgui_v_button_t)

declare_structure(vsf_tgui_v_panel_t)
def_structure(vsf_tgui_v_panel_t)
#if __IS_COMPILER_IAR__ || (!defined( __STDC_VERSION__ ) || __STDC_VERSION__ < 199901L)
    //please remove this if your structure is not emplty
    REG_RSVD_U8
#endif
end_def_structure(vsf_tgui_v_panel_t)

declare_structure(vsf_tgui_v_list_t)
def_structure(vsf_tgui_v_list_t)
#if __IS_COMPILER_IAR__ || (!defined( __STDC_VERSION__ ) || __STDC_VERSION__ < 199901L)
    //please remove this if your structure is not emplty
    REG_RSVD_U8
#endif
end_def_structure(vsf_tgui_v_list_t)

declare_structure(vsf_tgui_v_text_list_t)
def_structure(vsf_tgui_v_text_list_t)
#if __IS_COMPILER_IAR__ || (!defined( __STDC_VERSION__ ) || __STDC_VERSION__ < 199901L)
    //please remove this if your structure is not emplty
    REG_RSVD_U8
#endif
end_def_structure(vsf_tgui_v_text_list_t)

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

#endif

#endif
/* EOF */
