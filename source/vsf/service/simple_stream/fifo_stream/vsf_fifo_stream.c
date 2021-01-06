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

#if VSF_USE_SIMPLE_STREAM == ENABLED

#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#define __VSF_FIFO_STREAM_CLASS_IMPLEMENT
#include "../vsf_simple_stream.h"
#include "./vsf_fifo_stream.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/
/*============================ TYPES =========================================*/
/*============================ PROTOTYPES ====================================*/

static void __vsf_fifo_stream_init(vsf_stream_t *stream);
static uint_fast32_t __vsf_fifo_stream_write(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
static uint_fast32_t __vsf_fifo_stream_read(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size);
static uint_fast32_t __vsf_fifo_stream_get_buff_length(vsf_stream_t *stream);
static uint_fast32_t __vsf_fifo_stream_get_data_length(vsf_stream_t *stream);
static uint_fast32_t __vsf_fifo_stream_get_avail_length(vsf_stream_t *stream);
static uint_fast32_t __vsf_fifo_stream_get_wbuf(vsf_stream_t *stream, uint8_t **ptr);
static uint_fast32_t __vsf_fifo_stream_get_rbuf(vsf_stream_t *stream, uint8_t **ptr);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/

const vsf_stream_op_t vsf_fifo_stream_op = {
    .init               = __vsf_fifo_stream_init,
    .fini               = __vsf_fifo_stream_init,
    .write              = __vsf_fifo_stream_write,
    .read               = __vsf_fifo_stream_read,
    .get_buff_length    = __vsf_fifo_stream_get_buff_length,
    .get_data_length    = __vsf_fifo_stream_get_data_length,
    .get_avail_length   = __vsf_fifo_stream_get_avail_length,
    .get_wbuf           = __vsf_fifo_stream_get_wbuf,
    .get_rbuf           = __vsf_fifo_stream_get_rbuf,
};

/*============================ IMPLEMENTATION ================================*/

vsf_err_t vsf_byte_fifo_init(vsf_byte_fifo_t *fifo)
{
    VSF_SERVICE_ASSERT(fifo != NULL);
    fifo->head = fifo->tail = 0;
    return VSF_ERR_NONE;
}

uint_fast32_t vsf_byte_fifo_get_buff_length(vsf_byte_fifo_t *fifo)
{
    VSF_SERVICE_ASSERT(fifo != NULL);
    return fifo->size;
}

uint_fast32_t vsf_byte_fifo_get_data_length(vsf_byte_fifo_t *fifo)
{
    VSF_SERVICE_ASSERT(fifo != NULL);
    if (fifo->head >= fifo->tail) {
        return fifo->head - fifo->tail;
    } else {
        return fifo->size - (fifo->tail - fifo->head);
    }
}

uint_fast32_t vsf_byte_fifo_get_avail_length(vsf_byte_fifo_t *fifo)
{
    uint_fast32_t len;

    VSF_SERVICE_ASSERT(fifo != NULL);
    len = fifo->size - vsf_byte_fifo_get_data_length(fifo);
    if (len > 0) {
        len--;
    }
    return len;
}

uint_fast32_t vsf_byte_fifo_write(vsf_byte_fifo_t *fifo, uint8_t *buf, uint_fast32_t size)
{
    uint_fast32_t tmp32;

    VSF_SERVICE_ASSERT(fifo != NULL);
    if (size > vsf_byte_fifo_get_avail_length(fifo)) {
        return 0;
    }

    tmp32 = fifo->size - fifo->head;
    if (size > tmp32) {
        if (buf) {
            memcpy(&fifo->buffer[fifo->head], &buf[0], tmp32);
            memcpy(&fifo->buffer[0], &buf[tmp32], size - tmp32);
        }
        fifo->head = size - tmp32;
    } else {
        if (buf) {
            memcpy(&fifo->buffer[fifo->head], buf, size);
        }
        fifo->head += size;
        if (fifo->head == fifo->size) {
            fifo->head = 0;
        }
    }
    return size;
}

uint_fast32_t vsf_byte_fifo_get_rbuf(vsf_byte_fifo_t *fifo, uint8_t **buf)
{
    uint_fast32_t tmp32, avail_len = vsf_byte_fifo_get_data_length(fifo);

    VSF_SERVICE_ASSERT(fifo != NULL);
    if (buf != NULL) {
        *buf = &fifo->buffer[fifo->tail];
    }
    tmp32 = fifo->size - fifo->tail;
    return min(tmp32, avail_len);
}

uint_fast32_t vsf_byte_fifo_get_wbuf(vsf_byte_fifo_t *fifo, uint8_t **buf)
{
    uint_fast32_t tmp32, avail_len = vsf_byte_fifo_get_avail_length(fifo);

    VSF_SERVICE_ASSERT(fifo != NULL);
    if (buf != NULL) {
        *buf = &fifo->buffer[fifo->head];
    }
    tmp32 = fifo->size - fifo->head;
    return min(tmp32, avail_len);
}

uint_fast32_t vsf_byte_fifo_peek(vsf_byte_fifo_t *fifo, uint8_t *buf, uint_fast32_t size)
{
    uint_fast32_t tmp32;
    uint_fast32_t avail_len = vsf_byte_fifo_get_data_length(fifo);

    VSF_SERVICE_ASSERT(fifo != NULL);
    if (size > avail_len) {
        size = avail_len;
    }

    tmp32 = fifo->size - fifo->tail;
    if (buf) {
        if (size > tmp32) {
            memcpy(&buf[0], &fifo->buffer[fifo->tail], tmp32);
            memcpy(&buf[tmp32], &fifo->buffer[0], size - tmp32);
        } else {
            memcpy(buf, &fifo->buffer[fifo->tail], size);
        }
    }
    return size;
}

uint_fast32_t vsf_byte_fifo_read(vsf_byte_fifo_t *fifo, uint8_t *buf, uint_fast32_t size)
{
    uint_fast32_t tmp32;
    uint_fast32_t ret = vsf_byte_fifo_peek(fifo, buf, size);

    if (!ret) {
        return 0;
    }

    tmp32 = fifo->size - fifo->tail;
    if (ret > tmp32) {
        fifo->tail = ret - tmp32;
    } else {
        fifo->tail += ret;
        if (fifo->tail == fifo->size) {
            fifo->tail = 0;
        }
    }
    return ret;
}

static void __vsf_fifo_stream_init(vsf_stream_t *stream)
{
    vsf_fifo_stream_t *fifo_stream = (vsf_fifo_stream_t *)stream;
    vsf_byte_fifo_init(&fifo_stream->use_as__vsf_byte_fifo_t);
}

static uint_fast32_t __vsf_fifo_stream_get_buff_length(vsf_stream_t *stream)
{
    vsf_fifo_stream_t *fifo_stream = (vsf_fifo_stream_t *)stream;
    return vsf_byte_fifo_get_buff_length(&fifo_stream->use_as__vsf_byte_fifo_t);
}

static uint_fast32_t __vsf_fifo_stream_get_data_length(vsf_stream_t *stream)
{
    vsf_fifo_stream_t *fifo_stream = (vsf_fifo_stream_t *)stream;
    return vsf_byte_fifo_get_data_length(&fifo_stream->use_as__vsf_byte_fifo_t);
}

static uint_fast32_t __vsf_fifo_stream_get_avail_length(vsf_stream_t *stream)
{
    vsf_fifo_stream_t *fifo_stream = (vsf_fifo_stream_t *)stream;
    return vsf_byte_fifo_get_avail_length(&fifo_stream->use_as__vsf_byte_fifo_t);
}

static uint_fast32_t __vsf_fifo_stream_get_wbuf(vsf_stream_t *stream, uint8_t **ptr)
{
    vsf_fifo_stream_t *fifo_stream = (vsf_fifo_stream_t *)stream;
    return vsf_byte_fifo_get_wbuf(&fifo_stream->use_as__vsf_byte_fifo_t, ptr);
}

static uint_fast32_t __vsf_fifo_stream_get_rbuf(vsf_stream_t *stream, uint8_t **ptr)
{
    vsf_fifo_stream_t *fifo_stream = (vsf_fifo_stream_t *)stream;
    return vsf_byte_fifo_get_rbuf(&fifo_stream->use_as__vsf_byte_fifo_t, ptr);
}

static uint_fast32_t __vsf_fifo_stream_write(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size)
{
    vsf_fifo_stream_t *fifo_stream = (vsf_fifo_stream_t *)stream;
    return vsf_byte_fifo_write(&fifo_stream->use_as__vsf_byte_fifo_t, buf, size);
}

static uint_fast32_t __vsf_fifo_stream_read(vsf_stream_t *stream, uint8_t *buf, uint_fast32_t size)
{
    vsf_fifo_stream_t *fifo_stream = (vsf_fifo_stream_t *)stream;
    return vsf_byte_fifo_read(&fifo_stream->use_as__vsf_byte_fifo_t, buf, size);
}

#endif      // VSF_USE_SIMPLE_STREAM
