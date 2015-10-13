#ifndef __CSNAP_H_INCLUDE__
#define __CSNAP_H_INCLUDE__

/***************/
/*** CONFIG: ***/
/******************************************************/
/*** SNAP_FORCE_8BIT         force use all 8bit     ***/
/*** SNAP_DISABLE_16BIT      disable 16bit function ***/
/*** SNAP_DISABLE_32BIT      disable 32bit function ***/
/*** SNAP_DISABLE_PRINTERROR disable function error ***/
/******************************************************/

#include <stdint.h>

#ifdef SNAP_FORCE_8BIT
    #ifndef SNAP_DISABLE_16BIT 
        #define SNAP_DISABLE_16BIT
    #endif
    #ifndef SNAP_DISABLE_32BIT 
        #define SNAP_DISABLE_32BIT
    #endif
#endif

#ifdef SNAP_FORCE_8BIT
    typedef uint8_t szdata_t;
#else
    typedef uint16_t szdata_t;
#endif

typedef int8_t(*SnapRead)(uint8_t* buf);
typedef int8_t(*SnapWrite)(uint8_t buf);

struct snapva
{
    szdata_t n;
    uint8_t* d;
};

struct snap
{
    uint8_t hdb2;
    uint8_t hdb1;
    struct snapva dst;
    struct snapva src;
    struct snapva pro;
    struct snapva dat;
    struct snapva chk;
};

#define SNAP_SYNC     0x54
#define SNAP_ACK_NO   0x00
#define SNAP_ACK_REQ  0x01
#define SNAP_ACK_RES  0x02
#define SNAP_ACK_ERR  0x03
#define SNAP_EDM_NONE 0x00
#define SNAP_EDM_3TIM 0x01
#define SNAP_EDM_CHKS 0x02
#define SNAP_EDM_8CRC 0x03
#ifndef SNAP_DISABLE_16BIT
    #define SNAP_EDM_16CR 0x04
#endif
#ifndef SNAP_DISABLE_32BIT
    #define SNAP_EDM_32CR 0x05
#endif
#define SNAP_EDM_SFEC 0x06
#define SNAP_EDM_USER 0x07

#define SNAP_DATA_0    0x00
#define SNAP_DATA_1    0x01
#define SNAP_DATA_2    0x02
#define SNAP_DATA_3    0x03
#define SNAP_DATA_4    0x04
#define SNAP_DATA_5    0x05
#define SNAP_DATA_6    0x06
#define SNAP_DATA_7    0x07
#define SNAP_DATA_8    0x08
#define SNAP_DATA_16   0x09
#define SNAP_DATA_32   0x0A
#define SNAP_DATA_64   0x0B
#define SNAP_DATA_128  0x0C
#define SNAP_DATA_256  0x0D
#ifndef SNAP_FORCE_8BIT
    #define SNAP_DATA_512  0x0E
#endif
#define SNAP_DATA_USER 0x0F

#define SNAP_OK           0x00
#define SNAP_ERR_SYNC     0x01
#define SNAP_WAIT         0x02
#define SNAP_ERR_CORRUPT  0x03
#define SNAP_ERR_GET_SYNC 0x04
#define SNAP_ERR_GET_HDB2 0x05
#define SNAP_ERR_GET_HDB1 0x06
#define SNAP_ERR_GET_DADR 0x07
#define SNAP_ERR_GET_SADR 0x08
#define SNAP_ERR_GET_PROF 0x09
#define SNAP_ERR_GET_DATA 0x0A
#define SNAP_ERR_GET_ECHK 0x0B
#define SNAP_ERR_NOTERROR 0x0C
#define SNAP_ERR_COUNT    0x0D

uint8_t  snap_bitcount(struct snap* s);
uint8_t  snap_crc8(struct snap* s);
#ifndef SNAP_DISABLE_16BIT
    uint16_t snap_crc16(struct snap* s);
#endif
#ifndef SNAP_DISABLE_32BIT
    uint32_t snap_crc32(struct snap* s);
#endif
szdata_t snap_numbyte(struct snap* s);
int8_t   snap_read(struct snap* s, SnapRead fnc);
int8_t   snap_read_async(struct snap* s, SnapRead fnc);
uint8_t  snap_cmp(struct snap* s, struct snap* t);
uint8_t  snap_3time(struct snap* s, struct snap* t, SnapRead fnc);
int8_t   snap_check(struct snap* s, struct snap* t, SnapRead fnc);
int8_t   snap_write(struct snap* s, SnapWrite fnc);
uint8_t  snap_init(uint8_t* sbuf, struct snap* s, uint8_t szdst, uint8_t szsrc, uint8_t szpro, uint8_t errtype, szdata_t szdata);
uint8_t  snap_data_set(struct snap* s, szdata_t szdata);
uint16_t snap_data_get(struct snap* s);
uint8_t  snap_ack_set(struct snap* s, uint8_t ack);
uint8_t  snap_ack_get(struct snap* s);
uint8_t  snap_dst_set(struct snap* s, uint8_t dst);
uint8_t  snap_dst_get(struct snap* s);
uint8_t  snap_src_set(struct snap* s, uint8_t src);
uint8_t  snap_src_get(struct snap* s);
uint8_t  snap_pro_set(struct snap* s, uint8_t pro);
uint8_t  snap_pro_get(struct snap* s);
uint8_t  snap_cmd_set(struct snap* s, uint8_t cmd);
uint8_t  snap_cmd_get(struct snap* s);
uint8_t  snap_edm_set(struct snap* s, uint8_t erd);
uint8_t  snap_edm_get(struct snap* s);
#ifndef SNAP_DISABLE_ERROR_REPORTING
    void snap_err_print(int8_t e);
#endif

#endif

