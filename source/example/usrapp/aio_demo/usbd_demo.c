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
#include "lv_conf.h"

/*============================ MACROS ========================================*/
/*============================ MACROFIED FUNCTIONS ===========================*/

#define GENERATE_HEX(value)             TPASTE2(0x, value)

#define USB_DESC_WORD(__value)                                                  \
            ((__value) >> 0) & 0xFF, ((__value) >> 8) & 0xFF

#define USB_DESC_DWORD(__value)                                                 \
            ((__value) >> 0) & 0xFF, ((__value) >> 8) & 0xFF,                   \
            ((__value) >> 16) & 0xFF, ((__value) >> 24) & 0xFF



// UI configuration
#define SCREEN_WIDTH                    LV_HOR_RES
#define SCREEN_HEIGHT                   LV_VER_RES
#define SCREEN_FPS                      30

#if LV_COLOR_DEPTH == 24
// RGB888
#define SCREEN_PIXEL_FORMAT_GUID                                                \
            0x7D, 0xEB, 0x36, 0xE4, 0x4F, 0x52, 0xCE, 0x11,                     \
            0x9F, 0x53, 0x00, 0x20, 0xAF, 0x0B, 0xA7, 0x70
#elif LV_COLOR_DEPTH == 16
// RGB565
#define SCREEN_PIXEL_FORMAT_GUID                                                \
            0x7B, 0xEB, 0x36, 0xE4, 0x4F, 0x52, 0xCE, 0x11,                     \
            0x9F, 0x53, 0x00, 0x20, 0xAF, 0x0B, 0xA7, 0x70
#elif LV_COLOR_DEPTH == 8
// RGB8
#define SCREEN_PIXEL_FORMAT_GUID                                                \
            0x7A, 0xEB, 0x36, 0xE4, 0x4F, 0x52, 0xCE, 0x11,                     \
            0x9F, 0x53, 0x00, 0x20, 0xAF, 0x0B, 0xA7, 0x70
#endif

#define SCREEN_PIXEL_BITLEN             LV_COLOR_DEPTH
#define SCREEN_FRAME_SIZE               (SCREEN_WIDTH * SCREEN_HEIGHT * SCREEN_PIXEL_BITLEN / 8)

/*============================ TYPES =========================================*/

struct usbd_demo_const_t {
    struct {
        uint8_t dev_desc[18];
        uint8_t config_desc[75 + 66 + 158];
        uint8_t str_lanid[4];
        uint8_t str_vendor[20];
        uint8_t str_product[14];
        uint8_t str_cdc0[16];
        uint8_t str_cdc1[16];
        uint8_t str_uvc[14];
        vsf_usbd_desc_t std_desc[8];

        struct {
            struct {
                usb_UVC_ct_roi_t roi_def;
                vsf_usbd_UVC_control_info_t control_info[1];
            } camera;

            struct {
                usb_UVC_vs_t probe_commit;
                vsf_usbd_UVC_control_info_t control_info[2];
            } stream;
        } uvc;
    } usbd;
};
typedef struct usbd_demo_const_t usbd_demo_const_t;

struct usbd_demo_t {
    struct {
        struct {
            vsf_usbd_CDCACM_t param;
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
            struct {
                vsf_fifo_stream_t tx;
                vsf_fifo_stream_t rx;
                uint8_t tx_buffer[4 * 1024];
                uint8_t rx_buffer[4 * 1024];
            } stream;
#elif VSF_USE_SERVICE_STREAM == ENABLED
#endif
        } cdc[2];

        struct {
            struct {
                usb_UVC_ct_roi_t roi_cur;
                vsf_usbd_UVC_control_t control[1];
            } camera;
            struct {
                usb_UVC_vs_t probe_commit;
                vsf_usbd_UVC_control_t control[2];
            } stream;
            vsf_usbd_UVC_entity_t entity[3];
            vsf_usbd_UVC_t param;
            vsf_teda_t teda;
            uint32_t frame_pos;
            uint16_t cur_size;
            bool stream_started;
            vsf_eda_t *eda_host;
        } uvc;

        vsf_usbd_ifs_t ifs[6];
        vsf_usbd_cfg_t config[1];
        vsf_usbd_dev_t dev;

        vsf_callback_timer_t connect_timer;
    } usbd;
};
typedef struct usbd_demo_t usbd_demo_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/

static const usbd_demo_const_t usbd_demo_const = {
    .usbd                       = {
        .dev_desc               = {
            USB_DT_DEVICE_SIZE,
            USB_DT_DEVICE,
            0x00, 0x02,     // bcdUSB
            0xEF,           // device class: IAD
            0x02,           // device sub class
            0x01,           // device protocol
            64,             // max packet size
            USB_DESC_WORD(GENERATE_HEX(APP_CFG_USBD_VID)),
                            // vendor
            USB_DESC_WORD(GENERATE_HEX(APP_CFG_USBD_PID)),
                            // product
            USB_DESC_WORD(0x2000),
                            // bcdDevice
            1,              // manu facturer
            2,              // product
            0,              // serial number
            1,              // number of configuration
        },
        .config_desc            = {
            USB_DT_CONFIG_SIZE,
            USB_DT_CONFIG,
            USB_DESC_WORD(sizeof(usbd_demo_const.usbd.config_desc)),
                            // wTotalLength
            0x06,           // bNumInterfaces: 6 interfaces
            0x01,           // bConfigurationValue: Configuration value
            0x00,           // iConfiguration: Index of string descriptor describing the configuration
            0x80,           // bmAttributes: bus powered
            0x64,           // MaxPower

            // IDA for CDC
            USB_DT_INTERFACE_ASSOCIATION_SIZE,
            USB_DT_INTERFACE_ASSOCIATION,
            0,              // bFirstInterface
            2,              // bInterfaceCount
            0x02,           // bFunctionClass
            0x02,           // bFunctionSubClass
            0x01,           // bFunctionProtocol
            0x04,           // iFunction

            USB_DT_INTERFACE_SIZE,
            USB_DT_INTERFACE,
            0x00,           // bInterfaceNumber: Number of Interface
            0x00,           // bAlternateSetting: Alternate setting
            0x01,           // bNumEndpoints
            0x02,           // bInterfaceClass:
            0x02,           // bInterfaceSubClass:
            0x01,           // nInterfaceProtocol:
            0x04,           // iInterface:

            // Header Functional Descriptor
            0x05,           // bLength: Endpoint Descriptor size
            0x24,           // bDescriptorType: CS_INTERFACE
            0x00,           // bDescriptorSubtype: Header Func Desc
            0x10,           // bcdCDC: spec release number
            0x01,

            // Call Managment Functional Descriptor
            0x05,           // bFunctionLength
            0x24,           // bDescriptorType: CS_INTERFACE
            0x01,           // bDescriptorSubtype: Call Management Func Desc
            0x00,           // bmCapabilities: D0+D1
            0x01,           // bDataInterface: 1

            // ACM Functional Descriptor
            0x04,           // bFunctionLength
            0x24,           // bDescriptorType: CS_INTERFACE
            0x02,           // bDescriptorSubtype: Abstract Control Management desc
            0x02,           // bmCapabilities

            // Union Functional Descriptor
            0x05,           // bFunctionLength
            0x24,           // bDescriptorType: CS_INTERFACE
            0x06,           // bDescriptorSubtype: Union func desc
            0,              // bMasterInterface: Communication class interface
            1,              // bSlaveInterface0: Data Class Interface

            USB_DT_ENDPOINT_SIZE,
            USB_DT_ENDPOINT,
            0x81,           // bEndpointAddress: (IN1)
            0x03,           // bmAttributes: Interrupt
            USB_DESC_WORD(8),
                            // wMaxPacketSize:
#if defined(APP_CFG_USBD_HIGHSPEED)
            0x10,           // bInterval:
#else
            0xFF,           // bInterval:
#endif

            USB_DT_INTERFACE_SIZE,
            USB_DT_INTERFACE,
            0x01,           // bInterfaceNumber: Number of Interface
            0x00,           // bAlternateSetting: Alternate setting
            0x02,           // bNumEndpoints
            0x0A,           // bInterfaceClass
            0x00,           // bInterfaceSubClass
            0x00,           // nInterfaceProtocol
            0x04,           // iInterface:

            USB_DT_ENDPOINT_SIZE,
            USB_DT_ENDPOINT,
            0x82,           // bEndpointAddress: (IN2)
            0x02,           // bmAttributes: Bulk
#if defined(APP_CFG_USBD_HIGHSPEED)
            USB_DESC_WORD(512),
                            // wMaxPacketSize:
#else
            USB_DESC_WORD(64),
                            // wMaxPacketSize:
#endif
            0x00,           // bInterval:

            USB_DT_ENDPOINT_SIZE,
            USB_DT_ENDPOINT,
            0x02,           // bEndpointAddress: (OUT2)
            0x02,           // bmAttributes: Bulk
#if defined(APP_CFG_USBD_HIGHSPEED)
            USB_DESC_WORD(512),
                            // wMaxPacketSize:
#else
            USB_DESC_WORD(64),
                            // wMaxPacketSize:
#endif
            0x00,           // bInterval:

            // IDA for another CDC
            USB_DT_INTERFACE_ASSOCIATION_SIZE,
            USB_DT_INTERFACE_ASSOCIATION,
            2,              // bFirstInterface
            2,              // bInterfaceCount
            0x02,           // bFunctionClass
            0x02,           // bFunctionSubClass
            0x01,           // bFunctionProtocol
            0x05,           // iFunction

            USB_DT_INTERFACE_SIZE,
            USB_DT_INTERFACE,
            0x02,           // bInterfaceNumber: Number of Interface
            0x00,           // bAlternateSetting: Alternate setting
            0x01,           // bNumEndpoints
            0x02,           // bInterfaceClass:
            0x02,           // bInterfaceSubClass:
            0x01,           // nInterfaceProtocol:
            0x05,           // iInterface:

            // Header Functional Descriptor
            0x05,           // bLength: Endpoint Descriptor size
            0x24,           // bDescriptorType: CS_INTERFACE
            0x00,           // bDescriptorSubtype: Header Func Desc
            0x10,           // bcdCDC: spec release number
            0x01,

            // Call Managment Functional Descriptor
            0x05,           // bFunctionLength
            0x24,           // bDescriptorType: CS_INTERFACE
            0x01,           // bDescriptorSubtype: Call Management Func Desc
            0x00,           // bmCapabilities: D0+D1
            0x01,           // bDataInterface: 1

            // ACM Functional Descriptor
            0x04,           // bFunctionLength
            0x24,           // bDescriptorType: CS_INTERFACE
            0x02,           // bDescriptorSubtype: Abstract Control Management desc
            0x02,           // bmCapabilities

            // Union Functional Descriptor
            0x05,           // bFunctionLength
            0x24,           // bDescriptorType: CS_INTERFACE
            0x06,           // bDescriptorSubtype: Union func desc
            2,              // bMasterInterface: Communication class interface
            3,              // bSlaveInterface0: Data Class Interface

            USB_DT_ENDPOINT_SIZE,
            USB_DT_ENDPOINT,
            0x83,           // bEndpointAddress: (IN3)
            0x03,           // bmAttributes: Interrupt
            USB_DESC_WORD(8),
                            // wMaxPacketSize:
#if defined(APP_CFG_USBD_HIGHSPEED)
            0x10,           // bInterval:
#else
            0xFF,           // bInterval:
#endif

            USB_DT_INTERFACE_SIZE,
            USB_DT_INTERFACE,
            0x03,           // bInterfaceNumber: Number of Interface
            0x00,           // bAlternateSetting: Alternate setting
            0x02,           // bNumEndpoints
            0x0A,           // bInterfaceClass
            0x00,           // bInterfaceSubClass
            0x00,           // nInterfaceProtocol
            0x05,           // iInterface:

            USB_DT_ENDPOINT_SIZE,
            USB_DT_ENDPOINT,
            0x84,           // bEndpointAddress: (IN4)
            0x02,           // bmAttributes: Bulk
#if defined(APP_CFG_USBD_HIGHSPEED)
            USB_DESC_WORD(512),
                            // wMaxPacketSize:
#else
            USB_DESC_WORD(64),
                            // wMaxPacketSize:
#endif
            0x00,           // bInterval:

            USB_DT_ENDPOINT_SIZE,
            USB_DT_ENDPOINT,
            0x04,           // bEndpointAddress: (OUT4)
            0x02,           // bmAttributes: Bulk
#if defined(APP_CFG_USBD_HIGHSPEED)
            USB_DESC_WORD(512),
                            // wMaxPacketSize:
#else
            USB_DESC_WORD(64),
                            // wMaxPacketSize:
#endif
            0x00,           // bInterval:

            // IDA for UVC
            USB_DT_INTERFACE_ASSOCIATION_SIZE,
            USB_DT_INTERFACE_ASSOCIATION,
            4,                      // bFirstInterface
            2,                      // bInterfaceCount
            0x0E,                   // bFunctionClass: CC_VIDEO
            0x03,                   // bFunctionSubClass: SC_VIDEO_INTERFACE_COLLECTION
            0x00,                   // bFunctionProtocol: PC_PROTOCOL_UNDEFINED
            0x06,                   // iFunction

            USB_DT_INTERFACE_SIZE,
            USB_DT_INTERFACE,
            0x04,                   // bInterfaceNumber: Number of Interface
            0x00,                   // bAlternateSetting: Alternate setting
            0,                      // bNumEndpoints
            0x0E,                   // bInterfaceClass: CC_VIDEO
            0x01,                   // bInterfaceSubClass: SC_VIDEOCONTROL
            0x00,                   // nInterfaceProtocol: PC_PROTOCOL_UNDEFINED
            0x06,                   // iInterface:

            // Class-specific VC Interface Descriptor
            13,                     // bLength: Endpoint Descriptor size
            0x24,                   // bDescriptorType: CS_INTERFACE
            0x01,                   // bDescriptorSubtype: VC_HEADER
            USB_DESC_WORD(0x0150),  // bcdUVC: 1.5
            USB_DESC_WORD(39),      // wTotalLength
            USB_DESC_DWORD(SYSTEM_FREQ),
                                    // dwClockFrequency: deprecated, MUST not be 0
            0x01,                   // bInCollection: Number of streaming interfaces
            0x05,                   // baInterfaceNr[1]:

            // Input Terminal Descriptor
            17,                     // bLength
            0x24,                   // bDescriptorType: CS_INTERFACE
            0x02,                   // bDescriptorSubtype: VC_INPUT_TERMINAL
            1,                      // bTerminalID
            USB_DESC_WORD(0x0201),  // wTerminalType: ITT_CAMERA
            0x00,                   // bAssocTerminal
            0x00,                   // iTerminal
            USB_DESC_WORD(0),       // wObjectiveFocalLengthMin
            USB_DESC_WORD(0),       // wObjectiveFocalLengthMax
            USB_DESC_WORD(0),       // wOcularFocalLength
            0x02,                   // bControlSize
            USB_DESC_WORD(0),       // bmControls

            // Output Terminal Descriptor
            9,                      // bLength
            0x24,                   // bDescriptorType: CS_INTERFACE
            0x03,                   // bDescriptorSubtype: VC_OUTPUT_TERMINAL
            2,                      // bTerminalID
            USB_DESC_WORD(0x0101),  // wTerminalType: TT_STREAMING
            0x00,                   // bAssocTerminal
            1,                      // bSourceID
            0x00,                   // iTerminal

            // Standard VS Interrupt Descriptor for 0-bandwidth alt
            9,                      // bLength
            0x04,                   // bDescriptorType: INTERFACE
            0x05,                   // bInterfaceNumber
            0x00,                   // bAlternateSetting
            0,                      // bNumEndpoints
            0x0E,                   // bInterfaceClass: CC_VIDEO
            0x02,                   // bInterfaceSubClass: SC_VIDEOSTREAMING
            0x00,                   // bInterfaceProtocol: PC_PROTOCOL_UNDEFINED
            0x00,                   // iInterface

            // Class-specific VS Header Descriptor
            14,                     // bLength
            0x24,                   // bDescriptorType: CS_INTERFACE
            0x01,                   // bDescriptorSubType: VS_INPUT_HEADER
            1,                      // bNumFormats
            USB_DESC_WORD(77),      // wTotalLength
            0x85,                   // bEndpointAddress
            0x00,                   // bmInfo
            2,                      // bTerminalLink
            0x00,                   // bStillCaptureMethod
            0x00,                   // bTriggerSupport
            0x00,                   // bTriggerUsage
            0x01,                   // bControlSize
            0x00,                   // bmaControls

            // Class-specific VS Format Descriptor
            27,                     // bLength
            0x24,                   // bDescriptorType: CS_INTERFACE
            0x04,                   // bDescriptorSubType: CS_FORMAT_UNCOMPRESSED
            0x01,                   // bFormatIndex
            1,                      // bNumFrameDescriptors
#ifndef SCREEN_PIXEL_FORMAT_GUID
            SCREEN_PIXEL_FORMAT,
            0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71,
#else
            SCREEN_PIXEL_FORMAT_GUID,
#endif
                                    // guidFormat
            SCREEN_PIXEL_BITLEN,    // bBitsPerPixel
            0x01,                   // bDefaultFrameIndex
            0x00,                   // bAspectRationX
            0x00,                   // bAspectRatioY
            0x00,                   // bmInterfaceFlags
            0x00,                   // bCopyProtect

            // Class-specific VS Frame Descriptor
            30,                     // bLength
            0x24,                   // bDescriptorType: CS_INTERFACE
            0x05,                   // bDescriptorSubType: VS_FRAME_UNCOMPRESSED
            0x01,                   // bFrameIndex
            0x00,                   // bmCapabilities
            USB_DESC_WORD(SCREEN_WIDTH),
                                    // wWidth
            USB_DESC_WORD(SCREEN_HEIGHT),
                                    // wHeight
            USB_DESC_DWORD(SCREEN_FPS * SCREEN_WIDTH * SCREEN_HEIGHT * SCREEN_PIXEL_BITLEN),
                                    // dwMinBitRate
            USB_DESC_DWORD(SCREEN_FPS * SCREEN_WIDTH * SCREEN_HEIGHT * SCREEN_PIXEL_BITLEN),
                                    // dwMaxBitRate
            USB_DESC_DWORD(SCREEN_FRAME_SIZE),
                                    // dwMaxVideoFrameBufferSize
            USB_DESC_DWORD(10000000 / SCREEN_FPS),
                                    // dwDefaultFrameInterval
            0x01,                   // bFrameIntervalType: Continuous frame interval
            USB_DESC_DWORD(10000000 / SCREEN_FPS),
                                    // dwFrameInterval

            // Class-specific Color Matching Descriptor
            6,                      // bLength
            0x24,                   // bDescriptorType: CS_INTERFACE
            0x0D,                   // bDescriptorSubType: VS_COLORFORMAT
            0x01,                   // bColorPrimaries
            0x01,                   // bTransferCharacteristics
            0x04,                   // bMatrixCoefficients

            // Standard VS Interrupt Descriptor
            9,                      // bLength
            0x04,                   // bDescriptorType: INTERFACE
            0x05,                   // bInterfaceNumber
            0x01,                   // bAlternateSetting
            1,                      // bNumEndpoints
            0x0E,                   // bInterfaceClass: CC_VIDEO
            0x02,                   // bInterfaceSubClass: SC_VIDEOSTREAMING
            0x00,                   // bInterfaceProtocol: PC_PROTOCOL_UNDEFINED
            0x00,                   // iInterface

            7,                      // bLength
            0x05,                   // bDescriptorType: ENDPOINT
            0x85,                   // bEndpointAddress: IN1
            0x05,                   // bmAttributes: Iso
            USB_DESC_WORD(1024),    // wMaxPacketSize: 1K
            0x01,                   // bInterval
        },
        .str_lanid              = {
            4,
            USB_DT_STRING,
            0x09,
            0x04,
        },
        .str_vendor             = {
            20,
            USB_DT_STRING,
            'S', 0, 'i', 0, 'm', 0, 'o', 0, 'n', 0, 'Q', 0, 'i', 0, 'a', 0,
            'n', 0,
        },
        .str_product            = {
            14,
            USB_DT_STRING,
            'V', 0, 'S', 0, 'F', 0, 'A', 0, 'I', 0, 'O', 0,
        },
        .str_cdc0               = {
            16,
            USB_DT_STRING,
            'V', 0, 'S', 0, 'F', 0, 'C', 0, 'D', 0, 'C', 0, '0', 0,
        },
        .str_cdc1               = {
            16,
            USB_DT_STRING,
            'V', 0, 'S', 0, 'F', 0, 'C', 0, 'D', 0, 'C', 0, '1', 0,
        },
        .str_uvc                = {
            14,
            USB_DT_STRING,
            'V', 0, 'S', 0, 'F', 0, 'U', 0, 'V', 0, 'C', 0,
        },
        .std_desc               = {
            VSF_USBD_DESC_DEVICE(0, usbd_demo_const.usbd.dev_desc, sizeof(usbd_demo_const.usbd.dev_desc)),
            VSF_USBD_DESC_CONFIG(0, 0, usbd_demo_const.usbd.config_desc, sizeof(usbd_demo_const.usbd.config_desc)),
            VSF_USBD_DESC_STRING(0, 0, usbd_demo_const.usbd.str_lanid, sizeof(usbd_demo_const.usbd.str_lanid)),
            VSF_USBD_DESC_STRING(0x0409, 1, usbd_demo_const.usbd.str_vendor, sizeof(usbd_demo_const.usbd.str_vendor)),
            VSF_USBD_DESC_STRING(0x0409, 2, usbd_demo_const.usbd.str_product, sizeof(usbd_demo_const.usbd.str_product)),
            VSF_USBD_DESC_STRING(0x0409, 4, usbd_demo_const.usbd.str_cdc0, sizeof(usbd_demo_const.usbd.str_cdc0)),
            VSF_USBD_DESC_STRING(0x0409, 5, usbd_demo_const.usbd.str_cdc1, sizeof(usbd_demo_const.usbd.str_cdc1)),
            VSF_USBD_DESC_STRING(0x0409, 6, usbd_demo_const.usbd.str_uvc, sizeof(usbd_demo_const.usbd.str_uvc)),
        },

        // camera_control
        .uvc.camera             = {
            .roi_def            = {
                .wROI_Top       = SCREEN_HEIGHT - 1,
                .wROI_Left      = 0,
                .wROI_Bottom    = 0,
                .wROI_Right     = SCREEN_WIDTH - 1,
                .bmAutoControls = 0x0000,
            },
            .control_info       = {
                [0]             = {
                    .selector   = 0x14,     // CT_REGION_OF_INTEREST_CONTROL
                    .size       = sizeof(usb_UVC_ct_roi_t),
                    .def.buffer = (void *)&usbd_demo_const.usbd.uvc.camera.roi_def,
                },
            },
        },
        .uvc.stream.probe_commit= {
            .bFormatIndex       = 1,
            .bFrameIndex        = 1,
            .dwFrameInterval    = 10000000 / SCREEN_FPS,
            .dwMaxVideoFrameSize= SCREEN_FRAME_SIZE,
        },
        .uvc.stream.control_info= {
            [0]                 = {
                .selector       = 0x01,     // VS_PROBE_CONTROL
                .size           = sizeof(usb_UVC_vs_t),
                .def.buffer     = (void *)&usbd_demo_const.usbd.uvc.stream.probe_commit,
                .max.buffer     = (void *)&usbd_demo_const.usbd.uvc.stream.probe_commit,
                .min.buffer     = (void *)&usbd_demo_const.usbd.uvc.stream.probe_commit,
            },
            [1]                 = {
                .selector       = 0x02,     // VS_COMMIT_CONTROL
                .size           = sizeof(usb_UVC_vs_t),
                .def.buffer     = (void *)&usbd_demo_const.usbd.uvc.stream.probe_commit,
                .max.buffer     = (void *)&usbd_demo_const.usbd.uvc.stream.probe_commit,
                .min.buffer     = (void *)&usbd_demo_const.usbd.uvc.stream.probe_commit,
            },
        },
    },
};

static usbd_demo_t usbd_demo = {
    .usbd                       = {
        .cdc[0]                 = {
            .param              = {
                .ep = {
                    .notify     = 1,
                    .out        = 2,
                    .in         = 2,
                },
                .line_coding    = {
                    .bitrate    = 115200,
                    .stop       = 0,
                    .parity     = 0,
                    .datalen    = 8,
                },
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
                .stream.tx.stream = (vsf_stream_t *)&usbd_demo.usbd.cdc[0].stream.tx,
                .stream.rx.stream = (vsf_stream_t *)&usbd_demo.usbd.cdc[0].stream.rx,
            },
            .stream             = {
                .tx.op              = &vsf_fifo_stream_op,
                .tx.buffer          = (uint8_t *)&usbd_demo.usbd.cdc[0].stream.tx_buffer,
                .tx.size            = sizeof(usbd_demo.usbd.cdc[0].stream.tx_buffer),
                .rx.op              = &vsf_fifo_stream_op,
                .rx.buffer          = (uint8_t *)&usbd_demo.usbd.cdc[0].stream.rx_buffer,
                .rx.size            = sizeof(usbd_demo.usbd.cdc[0].stream.rx_buffer),
            },
#else
        },
#endif
        },

        .cdc[1]                 = {
            .param              = {
                .ep = {
                    .notify     = 3,
                    .out        = 4,
                    .in         = 4,
                },
                .line_coding    = {
                    .bitrate    = 115200,
                    .stop       = 0,
                    .parity     = 0,
                    .datalen    = 8,
                },
#if VSF_USE_SERVICE_VSFSTREAM == ENABLED
                .stream.tx.stream = (vsf_stream_t *)&usbd_demo.usbd.cdc[1].stream.tx,
                .stream.rx.stream = (vsf_stream_t *)&usbd_demo.usbd.cdc[1].stream.rx,
            },
            .stream             = {
                .tx.op              = &vsf_fifo_stream_op,
                .tx.buffer          = (uint8_t *)&usbd_demo.usbd.cdc[1].stream.tx_buffer,
                .tx.size            = sizeof(usbd_demo.usbd.cdc[1].stream.tx_buffer),
                .rx.op              = &vsf_fifo_stream_op,
                .rx.buffer          = (uint8_t *)&usbd_demo.usbd.cdc[1].stream.rx_buffer,
                .rx.size            = sizeof(usbd_demo.usbd.cdc[1].stream.rx_buffer),
            },
#else
        },
#endif
        },

        .uvc                    = {
            // input terminal: Camera
            .camera.control     = {
                [0]             = {
                    .info       = &usbd_demo_const.usbd.uvc.camera.control_info[0],
                    .cur.buffer = (void *)&usbd_demo.usbd.uvc.camera.roi_cur,
                },
            },
            .stream.control     = {
                [0]             = {
                    .info       = &usbd_demo_const.usbd.uvc.stream.control_info[0],
                    .cur.buffer = (void *)&usbd_demo.usbd.uvc.stream.probe_commit,
                },
                [1]             = {
                    .info       = &usbd_demo_const.usbd.uvc.stream.control_info[1],
                    .cur.buffer = (void *)&usbd_demo.usbd.uvc.stream.probe_commit,
                },
            },
            .entity[0]          = {
                .id             = 0,            // for video streaming
                .control_num    = dimof(usbd_demo.usbd.uvc.stream.control),
                .control        = usbd_demo.usbd.uvc.stream.control,
            },
            // input terminal: Camera
            .entity[1]          = {
                .id             = 1,
                .control_num    = dimof(usbd_demo.usbd.uvc.camera.control),
                .control        = usbd_demo.usbd.uvc.camera.control,
            },
            // output terminal: USB
            .entity[2]          = {
                .id             = 2,
            },
            .param              = {
                .ep_in          = 5,

                .entity_num     = dimof(usbd_demo.usbd.uvc.entity),
                .entity         = usbd_demo.usbd.uvc.entity,
            },
        },

        // CDC0
        .ifs[0].class_op        = &vsf_usbd_CDCACM_control,
        .ifs[0].class_param     = &usbd_demo.usbd.cdc[0].param,
        .ifs[1].class_op        = &vsf_usbd_CDCACM_data,
        .ifs[1].class_param     = &usbd_demo.usbd.cdc[0].param,

        // CDC1
        .ifs[2].class_op        = &vsf_usbd_CDCACM_control,
        .ifs[2].class_param     = &usbd_demo.usbd.cdc[1].param,
        .ifs[3].class_op        = &vsf_usbd_CDCACM_data,
        .ifs[3].class_param     = &usbd_demo.usbd.cdc[1].param,

        // UVC
        .ifs[4].class_op        = &vsf_usbd_UVC_control_class,
        .ifs[4].class_param     = &usbd_demo.usbd.uvc.param,
        .ifs[5].class_op        = &vsf_usbd_UVC_stream_class,
        .ifs[5].class_param     = &usbd_demo.usbd.uvc.param,

        .config[0].num_of_ifs   = dimof(usbd_demo.usbd.ifs),
        .config[0].ifs          = usbd_demo.usbd.ifs,

        .dev.num_of_config      = dimof(usbd_demo.usbd.config),
        .dev.config             = usbd_demo.usbd.config,
        .dev.num_of_desc        = dimof(usbd_demo_const.usbd.std_desc),
        .dev.desc               = (vsf_usbd_desc_t *)usbd_demo_const.usbd.std_desc,

        .dev.speed              = USB_DC_SPEED_HIGH,
        .dev.drv                = &VSF_USB_DC0,//&VSF_USB.DC[0],
    },
};

/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

static void usbd_demo_on_timer(vsf_callback_timer_t *timer)
{
    vsf_usbd_connect(&usbd_demo.usbd.dev);
}

void usbd_demo_start(void)
{
    usbd_demo.usbd.uvc.stream.probe_commit = usbd_demo_const.usbd.uvc.stream.probe_commit;
    vsf_usbd_init(&usbd_demo.usbd.dev);
    vsf_usbd_disconnect(&usbd_demo.usbd.dev);

    usbd_demo.usbd.connect_timer.on_timer = usbd_demo_on_timer;
    vsf_callback_timer_add_ms(&usbd_demo.usbd.connect_timer, 200);
}

// UVC
WEAK void usbd_demo_uvc_on_ready(void) {}

void vsf_usbd_UVC_stop_stream(vsf_usbd_UVC_t *uvc, uint_fast8_t ifs)
{
    if (usbd_demo.usbd.uvc.stream_started) {
        usbd_demo.usbd.uvc.stream_started = false;
    }
}

void vsf_usbd_UVC_start_stream(vsf_usbd_UVC_t *uvc, uint_fast8_t ifs)
{
    if (!usbd_demo.usbd.uvc.stream_started) {
        usbd_demo.usbd.uvc.stream_started = true;
        usbd_demo_uvc_on_ready();
    }
}

void usbd_demo_uvc_trans_disp_line(uint8_t *buffer, uint_fast32_t size)
{
    vsf_usbd_UVC_send_packet(&usbd_demo.usbd.uvc.param, buffer, size);
}
