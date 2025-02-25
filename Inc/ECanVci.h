#ifndef _ECANVCI_H_
#define _ECANVCI_H_

#include <stdint.h>

#define DWORD unsigned int
#define UINT unsigned int
#define USHORT unsigned short int
#define CHAR signed char
#define UCHAR unsigned char
#define BYTE unsigned char
#define ULONG unsigned int
#define INT signed int
#define PVOID void *
#define LPVOID void *
#define HANDLE void *
#define BOOL BYTE

typedef DWORD ECAN_RESULT;
typedef DWORD ECAN_HANDLE;

typedef struct {
  int32_t id;          /* CAN ID */
  uint8_t len;         /* Length of message: 0-8 */
  uint8_t msg_lost;    /* Count of lost RX messages */
  uint8_t reserved[2]; /* Reserved */
  uint8_t data[8];     /* 8 data bytes */
} CMSG;

typedef struct {
  int32_t evid;        /* Event ID: possible range: EV_BASE...EV_LAST */
  uint8_t len;         /* Length of message: 0-8 */
  uint8_t reserved[3]; /* Reserved */
  union {
    uint8_t c[8];
    uint16_t s[4];
    uint32_t l[2];
  } evdata;
} EVMSG;

typedef struct {
  int32_t id;          /* CAN ID */
  uint8_t len;         /* Length of message: 0-8 */
  uint8_t msg_lost;    /* Count of lost RX messages */
  uint8_t reserved[2]; /* Reserved */
  uint8_t data[8];     /* 8 data bytes */
  uint64_t timestamp;  /* Timestamp of this message */
} CMSG_T;

typedef struct _BOARD_INFO {
  USHORT hw_Version;       /* Hardware version */
  USHORT fw_Version;       /* Firmware version */
  USHORT dr_Version;       /* Driver version */
  USHORT in_Version;       /* Interface version */
  USHORT irq_Num;          /* IRQ number */
  BYTE can_Num;            /* CAN number */
  CHAR str_Serial_Num[20]; /* Serial number */
  CHAR str_hw_Type[40];    /* Hardware type */
  USHORT Reserved[4];      /* Reserved */
} BOARD_INFO, *P_BOARD_INFO;

typedef struct _CAN_OBJ {
  UINT ID;          /* CAN ID */
  UINT TimeStamp;   /* Timestamp */
  BYTE TimeFlag;    /* Time flag */
  BYTE SendType;    /* Send type */
  BYTE RemoteFlag;  /* Remote flag */
  BYTE ExternFlag;  /* External flag */
  BYTE DataLen;     /* Data length */
  BYTE Data[8];     /* Data */
  BYTE Reserved[3]; /* Reserved */
} CAN_OBJ, *P_CAN_OBJ;

typedef struct _CAN_STATUS {
  UCHAR ErrInterrupt; /* Error interrupt */
  UCHAR regMode;      /* Register mode */
  UCHAR regStatus;    /* Register status */
  UCHAR regALCapture; /* Register AL capture */
  UCHAR regECCapture; /* Register EC capture */
  UCHAR regEWLimit;   /* Register EW limit */
  UCHAR regRECounter; /* Register RE counter */
  UCHAR regTECounter; /* Register TE counter */
  DWORD Reserved;     /* Reserved */
} CAN_STATUS, *P_CAN_STATUS;

typedef struct _ERR_INFO {
  UINT ErrCode;            /* Error code */
  BYTE Passive_ErrData[3]; /* Passive error data */
  BYTE ArLost_ErrData;     /* Arbitration lost error data */
} ERR_INFO, *P_ERR_INFO;

typedef struct _INIT_CONFIG {
  DWORD AccCode;  /* Acceptance code */
  DWORD AccMask;  /* Acceptance mask */
  DWORD Reserved; /* Reserved */
  UCHAR Filter;   /* Filter */
  UCHAR Timing0;  /* Timing 0 */
  UCHAR Timing1;  /* Timing 1 */
  UCHAR Mode;     /* Mode */
} INIT_CONFIG, *P_INIT_CONFIG;

typedef struct _FILTER_RECORD {
  DWORD ExtFrame; /* Extended frame */
  DWORD Start;    /* Start */
  DWORD End;      /* End */
} FILTER_RECORD, *P_FILTER_RECORD;

// Status values
#define STATUS_OK 1
#define STATUS_ERR 0
#define ECAN_ERR 0

#define ECAN_SUCCESS 1

// CAN error codes
#define ERR_CAN_OVERFLOW 0x0001
#define ERR_CAN_ERRALARM 0x0002
#define ERR_CAN_PASSIVE 0x0004
#define ERR_CAN_LOSE 0x0008
#define ERR_CAN_BUSERR 0x0010
#define ERR_CAN_REG_FULL 0x0020
#define ERR_CAN_REG_OVER 0x0040
#define ERR_CAN_ZHUDONG 0x0080

// General error codes
#define ERR_DEVICEOPENED 0x0100
#define ERR_DEVICEOPEN 0x0200
#define ERR_DEVICENOTOPEN 0x0400
#define ERR_BUFFEROVERFLOW 0x0800
#define ERR_DEVICENOTEXIST 0x1000
#define ERR_LOADKERNELDLL 0x2000
#define ERR_CMDFAILED 0x4000 // Command failed

#define ECAN_RX_TIMEOUT 0xE0000001
#define ECAN_TX_TIMEOUT 0xE0000002
#define ECAN_TX_ERROR 0xE0000004

/*------------------ Defines ------------------------------------------------*/

#define ECAN_EV_BASE 0x40000000
#define ECAN_EV_USER 0x40000080
#define ECAN_EV_LAST 0x400000FF

#define ECAN_EV_CAN_ERROR ECAN_EV_BASE
#define ECAN_EV_BAUD_CHANGE (ECAN_EV_BASE + 0x1)

#define ECAN_EV_ADD_ID_ZERO (ECAN_EV_BASE + 0x2)
#define ECAN_EV_ADD_ID (ECAN_EV_BASE + 0x3)
#define ECAN_EV_ADD_ID_SA (ECAN_EV_BASE + 0x4)
#define ECAN_EV_ADD_ID_RANG (ECAN_EV_BASE + 0x5)
#define ECAN_EV_RESET (ECAN_EV_BASE + 0x6)
#define ECAN_EV_ADD_MASK (ECAN_EV_BASE + 0x7)
#define ECAN_EV_Mode (ECAN_EV_BASE + 0xc)

#define ECAN_IOCTL_GET_TIMESTAMP_FREQ                                          \
  0x0007                                /* Get timestamp frequency in Hz       \
                                         */
#define ECAN_IOCTL_GET_TIMESTAMP 0x0008 /* Get timestamp counter */

#ifdef __cplusplus
extern "C" {
#endif

/// @brief 打开设备
/// @param DeviceType 设备类型
/// @param DeviceInd 设备索引
/// @param Reserved 保留参数
/// @return 操作结果
DWORD OpenDevice(DWORD DeviceType, DWORD DeviceInd, DWORD Reserved);

/// @brief 关闭设备
/// @param DeviceType 设备类型
/// @param DeviceInd 设备索引
/// @return 操作结果
DWORD CloseDevice(DWORD DeviceType, DWORD DeviceInd);

/// @brief 初始化CAN
/// @param DeviceType 设备类型
/// @param DeviceInd 设备索引
/// @param CANInd CAN索引
/// @param pInitConfig 初始化配置
/// @return 操作结果
DWORD InitCAN(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd,
              P_INIT_CONFIG pInitConfig);

/// @brief 读取板卡信息
/// @param DeviceType 设备类型
/// @param DeviceInd 设备索引
/// @param pInfo 板卡信息结构体指针
/// @return 操作结果
DWORD ReadBoardInfo(DWORD DeviceType, DWORD DeviceInd, P_BOARD_INFO pInfo);

/// @brief 读取错误信息
/// @param DeviceType 设备类型
/// @param DeviceInd 设备索引
/// @param CANInd CAN索引
/// @param pErrInfo 错误信息结构体指针
/// @return 操作结果
DWORD ReadErrInfo(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd,
                  P_ERR_INFO pErrInfo);

/// @brief 读取CAN状态
/// @param DeviceType 设备类型
/// @param DeviceInd 设备索引
/// @param CANInd CAN索引
/// @param pCANStatus CAN状态结构体指针
/// @return 操作结果
DWORD ReadCANStatus(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd,
                    P_CAN_STATUS pCANStatus);

/// @brief 获取参考信息
/// @param DeviceType 设备类型
/// @param DeviceInd 设备索引
/// @param CANInd CAN索引
/// @param RefType 参考类型
/// @param pData 数据指针
/// @return 操作结果
DWORD GetReference(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd,
                   DWORD RefType, PVOID pData);

/// @brief 设置参考信息
/// @param DeviceType 设备类型
/// @param DeviceInd 设备索引
/// @param CANInd CAN索引
/// @param RefType 参考类型
/// @param pData 数据指针
/// @return 操作结果
DWORD SetReference(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd,
                   DWORD RefType, PVOID pData);

/// @brief 获取接收数量
/// @param DeviceType 设备类型
/// @param DeviceInd 设备索引
/// @param CANInd CAN索引
/// @return 接收数量
DWORD GetReceiveNum(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd);

/// @brief 清空缓冲区
/// @param DeviceType 设备类型
/// @param DeviceInd 设备索引
/// @param CANInd CAN索引
/// @return 操作结果
DWORD ClearBuffer(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd);

/// @brief 启动CAN
/// @param DeviceType 设备类型
/// @param DeviceInd 设备索引
/// @param CANInd CAN索引
/// @return 操作结果
DWORD StartCAN(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd);

/// @brief 重置CAN
/// @param DeviceType 设备类型
/// @param DeviceInd 设备索引
/// @param CANInd CAN索引
/// @return 操作结果
DWORD ResetCAN(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd);

/// @brief 发送数据
/// @param DeviceType 设备类型
/// @param DeviceInd 设备索引
/// @param CANInd CAN索引
/// @param pSend 发送数据结构体指针
/// @param Len 数据长度
/// @return 操作结果
DWORD Transmit(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, P_CAN_OBJ pSend,
               ULONG Len);

/// @brief 接收数据
/// @param DeviceType 设备类型
/// @param DeviceInd 设备索引
/// @param CANInd CAN索引
/// @param pReceive 接收数据结构体指针
/// @param Len 数据长度
/// @param WaitTime 等待时间
/// @return 操作结果
DWORD Receive(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd,
              P_CAN_OBJ pReceive, ULONG Len, INT WaitTime);

#ifdef __cplusplus
}
#endif

#endif
