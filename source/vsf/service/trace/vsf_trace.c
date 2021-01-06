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

#include "service/vsf_service_cfg.h"

#if VSF_USE_TRACE == ENABLED
#include "hal/vsf_hal.h"

//! todo: remove this dependency
#include "kernel/vsf_kernel.h"

#include "./vsf_trace.h"
#include <stdio.h>

/*============================ MACROS ========================================*/

#if VSF_USE_SIMPLE_STREAM != ENABLED && VSF_USE_STREAMVSF_USE_STREAM != ENABLED
#   error stream MUST be enabled to use trace
#endif

#ifndef VSF_TRACE_CFG_BUFSIZE
#   define VSF_TRACE_CFG_BUFSIZE            1024
#endif

#define VSF_TRACE_LINEBUF_SIZE              128

#ifndef VSF_TRACE_CFG_PROTECT_LEVEL
/*! \note   By default, the driver tries to make all APIs scheduler-safe,
 *!
 *!         in the case when you want to disable it,
 *!         please use following macro:
 *!         #define VSF_TRACE_CFG_PROTECT_LEVEL none
 *!         
 *!         in the case when you want to use interrupt-safe,
 *!         please use following macro:
 *!         #define VSF_TRACE_CFG_PROTECT_LEVEL interrupt
 *!         
 *!         NOTE: This macro should be defined in vsf_usr_CFG.h
 */
#   define VSF_TRACE_CFG_PROTECT_LEVEL      scheduler
#endif

#define vsf_trace_protect                   vsf_protect(VSF_TRACE_CFG_PROTECT_LEVEL)
#define vsf_trace_unprotect                 vsf_unprotect(VSF_TRACE_CFG_PROTECT_LEVEL)

/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
#if VSF_USE_SIMPLE_STREAM == ENABLED
typedef struct vsf_trace_t {
    vsf_stream_t *stream;
    uint8_t print_buffer[VSF_TRACE_CFG_BUFSIZE];
} vsf_trace_t;
#elif VSF_USE_STREAM == ENABLED

#endif
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
#if VSF_USE_SIMPLE_STREAM == ENABLED
static vsf_trace_t __vsf_trace;
#elif VSF_USE_STREAM == ENABLED
NO_INIT static vsf_stream_writer_t __vsf_trace;
#endif

#if VSF_TRACE_CFG_COLOR_EN == ENABLED
static const char *__vsf_trace_color[VSF_TRACE_LEVEL_NUM] = {
    [VSF_TRACE_NONE]    = "",
    [VSF_TRACE_ERROR]   = "\033[1;31m",
    [VSF_TRACE_INFO]    = "\033[1;37m",
    [VSF_TRACE_WARNING] = "\033[1;33m",
    [VSF_TRACE_DEBUG]   = "\033[1;34m",
};
#endif

/*============================ PROTOTYPES ====================================*/

static uint_fast32_t __vsf_trace_output(const char* buff, uint_fast32_t size);

/*============================ IMPLEMENTATION ================================*/

void vsf_trace_output_string(const char* str)
{
    __vsf_trace_output(str, strlen(str));
}

static void __vsf_trace_set_level(vsf_trace_level_t level)
{
#if VSF_TRACE_CFG_COLOR_EN == ENABLED
    VSF_SERVICE_ASSERT(level < dimof(__vsf_trace_color));
    vsf_trace_output_string((char *)__vsf_trace_color[level]);
#endif
}

#if VSF_USE_SIMPLE_STREAM == ENABLED
static uint_fast32_t __vsf_trace_output(const char *buff, uint_fast32_t size)
{
    uint32_t ret = 0;

    if (__vsf_trace.stream != NULL) {
        vsf_protect_t origlevel = vsf_trace_protect();
            ret = vsf_stream_write(__vsf_trace.stream, (uint8_t *)buff, size);
        vsf_trace_unprotect(origlevel);
    }
    return ret;
}

void __vsf_trace_init(vsf_stream_t *stream)
{
    __vsf_trace.stream = stream;
    if (stream) {
        vsf_stream_connect_tx(stream);
    }
}

void vsf_trace_fini(void)
{
    if (__vsf_trace.stream) {
        vsf_stream_disconnect_tx(__vsf_trace.stream);
    }
    __vsf_trace.stream = NULL;
}

static void __vsf_trace_arg(const char *format, va_list *arg)
{
    vsf_protect_t origlevel = vsf_trace_protect();
    //__vsf_sched_safe(
        uint_fast32_t size = vsnprintf( (char *)__vsf_trace.print_buffer,
                                        sizeof(__vsf_trace.print_buffer),
                                        format, 
                                        *arg);
        __vsf_trace_output((const char *)__vsf_trace.print_buffer, size);
    //)
    vsf_trace_unprotect(origlevel);
}

#elif VSF_USE_STREAM == ENABLED
void __vsf_trace_init(vsf_stream_tx_t *ptTX)
{
    //VSF_SERVICE_ASSERT(NULL != ptTX);
    
    //! initialise stream source
    do {
        vsf_stream_src_cfg_t cfg = {
            .ptTX = ptTX,                                       //!< connect stream TX 
        };

        vsf_stream_writer_init(&__vsf_trace, &cfg);

    } while(0);
}

static uint_fast32_t __vsf_trace_output(const char *buff, uint_fast32_t size)
{
    uint_fast16_t length = size;
    uint8_t *src = (uint8_t *)buff;
    
    while(length) {
        uint_fast16_t written_size = vsf_stream_writer_write(
                                        &__vsf_trace,
                                        src,
                                        length);
        
        length -= written_size;
        src += written_size;

        if (0 == written_size) {
            size -= length;
            break;
        }
    }
    return size;
}

static void __vsf_trace_arg(const char *format, va_list *arg)
{
    vsf_pbuf_t *block = NULL; 
    __vsf_sched_safe(
    #if VSF_STREAM_CFG_SUPPORT_OPEN_CLOSE == ENABLED
        vsf_stream_tx_t *tx = vsf_stream_src_get_tx(&__vsf_trace.use_as__vsf_stream_src_t);
        if (!tx->piMethod->GetStatus(tx).IsOpen) {
            vsf_sched_safe_exit();
            return;
        }
    #endif
        block = vsf_stream_src_new_pbuf(&__vsf_trace.use_as__vsf_stream_src_t, 
                                        -1, 
                                        -1);
        if (NULL == block) {
            //! no enough buffer
            vsf_sched_safe_exit();
            return;
        }
        //vsf_pbuf_size_reset(block);
    )
    

    uint_fast32_t size = vsnprintf( (char *)vsf_pbuf_buffer_get(block),
                                        vsf_pbuf_size_get(block),
                                        format, 
                                        *arg);
    vsf_pbuf_size_set(block, size);
    vsf_stream_writer_send_pbuf(&__vsf_trace, block);
}
#endif

void vsf_trace_string(vsf_trace_level_t level, const char *str)
{
    __vsf_trace_set_level(level);
    vsf_trace_output_string(str);
}

SECTION(".text.vsf.trace.__vsf_trace_buffer")
void __vsf_trace_buffer(  vsf_trace_level_t level, 
                        void *buffer, 
                        uint_fast16_t len, 
                        uint_fast32_t flag)
{
    uint_fast8_t data_size = (flag >> 0) & 0xFF;
    uint_fast8_t data_per_line = (flag >> 8) & 0xFF;
    bool disp_addr = flag & VSF_TRACE_DF_ADDR;
    bool disp_char = flag & VSF_TRACE_DF_CHAR;
    bool disp_newline = flag & VSF_TRACE_DF_NEWLINE;
    uint8_t *buf_tmp = (uint8_t *)buffer, *pend = buf_tmp + len, *line;

    if (!data_size || (data_size > 4) || (data_size == 3))
        data_size = 1;
    if (!data_per_line || (data_per_line > 16))
        data_per_line = 16;

    __vsf_trace_set_level(level);
    if (len > 0) {
        static const char __map[16] = "0123456789ABCDEF";
        // line format 16 data max:
        //    XXXXXXXX: XXXXXXXX XXXXXXXX ....  | CHAR.....\r\n\0
        //              9 * 16                 3   4 * 16   3
        char linebuf[(8 + 1) * 16 + 3 + 64 + 3], *ptr, ch;

        for (uint_fast16_t i = 0; i < len; i += data_size * data_per_line) {
            if (disp_addr) {
                vsf_trace(level, "%08X: ", i);
            }

            ptr = linebuf;
            line = buf_tmp;
            for (uint_fast8_t j = 0; j < data_per_line; j++) {
                for (uint_fast8_t k = 0; k < data_size; k++, buf_tmp++) {
                    if (buf_tmp < pend) {
                        *ptr++ = __map[(*buf_tmp >> 4) & 0x0F];
                        *ptr++ = __map[(*buf_tmp >> 0) & 0x0F];
                    } else if (disp_char) {
                        *ptr++ = ' ';
                        *ptr++ = ' ';
                    }
                }
                *ptr++ = ' ';
                if (!disp_char && (buf_tmp >= pend)) {
                    break;
                }
            }

            if (disp_char) {
                memcpy(ptr, " | ", 3);
                ptr += 3;
                for (uint_fast16_t j = 0; j < data_size * data_per_line; j++, line++) {
                    if (line < pend) {
                        ch = *line;
                        *ptr++ = ((ch < 32) || (ch > 127)) ? '.' : ch;
                    } else {
                        *ptr++ = ' ';
                    }
                }
            }

            if (disp_newline) {
                strcpy(ptr, VSF_TRACE_CFG_LINEEND);
            } else {
                *ptr++ = '\0';
            }
            vsf_trace_output_string(linebuf);
        }
    }
}



void vsf_trace_arg(vsf_trace_level_t level, const char *format, va_list *arg)
{
    __vsf_trace_set_level(level);
    __vsf_trace_arg(format, arg);
}

void vsf_trace(vsf_trace_level_t level, const char *format, ...)
{
    va_list ap;

    __vsf_trace_set_level(level);
    va_start(ap, format);
    __vsf_trace_arg(format, &ap);
    va_end(ap);
}

#endif      // VSF_USE_TRACE
