#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "snaproto.h"
#include "csnap.h"

uint8_t snap_checksum(struct snap* s)
{
    szdata_t i;
    uint8_t v = 0;
    v += s->hdb2;
    v += s->hdb1;
    i = SNAP_GET_DSTADR(s->hdb2);
    while( i-- ) v += s->dst.d[i];
    i = SNAP_GET_SRCADR(s->hdb2);
    while( i-- ) v += s->src.d[i];
    i = SNAP_GET_PROTOF(s->hdb2);
    while( i-- ) v += s->pro.d[i];
    i = snap_numbyte(s);
    while( i-- ) v += s->dat.d[i];
    
    return v;
}

//start 0
static uint8_t _crc8(uint8_t v, uint8_t crc)
{
    uint8_t fb;
    uint8_t i;
    
    crc ^= v;
    i = 8;
    while( i-- )
    {
        fb = crc & 1;
        crc >>= 1;
        if ( fb ) crc ^= 0x8C;
    }
    return crc;
}

uint8_t snap_crc8(struct snap* s)
{
    szdata_t i, n;
    uint8_t c = 0;
    c = _crc8(s->hdb2, 0);
    c = _crc8(s->hdb1, c);
    n = SNAP_GET_DSTADR(s->hdb2);
    for(i = 0; i < n; ++i) c = _crc8(s->dst.d[i], c);
    n = SNAP_GET_SRCADR(s->hdb2);
    for(i = 0; i < n; ++i) c = _crc8(s->src.d[i], c);
    n = SNAP_GET_PROTOF(s->hdb2);
    for(i = 0; i < n; ++i) c = _crc8(s->pro.d[i], c);
    n = snap_numbyte(s);
    for(i = 0; i < n; ++i) c = _crc8(s->dat.d[i], c);
    
    return c;
}


#ifndef SNAP_DISABLE_16BIT
//XMODEM start 0x0000
static uint16_t _crc16(uint8_t v, uint16_t crc)
{
    uint16_t x;
    
    x = crc >> 8 ^ v;
    x ^= x >> 4;
    crc = (crc << 8) ^ ((unsigned short)(x << 12)) ^ ((unsigned short)(x <<5)) ^ ((unsigned short)x);
    return crc;
}

uint16_t snap_crc16(struct snap* s)
{
    uint16_t i, n;
    uint16_t c = 0;
    c = _crc16(s->hdb2, 0x0000);
    c = _crc16(s->hdb1, c);
    n = SNAP_GET_DSTADR(s->hdb2);
    for(i = 0; i < n; ++i) c = _crc16(s->dst.d[i], c);
    n = SNAP_GET_SRCADR(s->hdb2);
    for(i = 0; i < n; ++i) c = _crc16(s->src.d[i], c);
    n = SNAP_GET_PROTOF(s->hdb2);
    for(i = 0; i < n; ++i) c = _crc16(s->pro.d[i], c);
    n = snap_numbyte(s);
    for(i = 0; i < n; ++i) c = _crc16(s->dat.d[i], c);
    
    return c;
}
#endif

#ifndef SNAP_DISABLE_32BIT
//start 0xFFFFFFFF;
static uint32_t _crc32(uint8_t v, uint32_t crc)
{
    uint32_t mask;
    uint8_t i = 8;
    
    crc ^= v;
    while ( i-- )
    { 
        mask = crc & 1;
        crc >>= 1;
        if ( mask ) crc ^= 0xEDB88320;
    }
    return crc;
}

uint32_t snap_crc32(struct snap* s)
{
    uint16_t i, n;
    uint16_t c = 0;
    c = _crc32(s->hdb2, 0xFFFFFFFF);
    c = _crc32(s->hdb1, c);
    n = SNAP_GET_DSTADR(s->hdb2);
    for(i = 0; i < n; ++i) c = _crc32(s->dst.d[i], c);
    n = SNAP_GET_SRCADR(s->hdb2);
    for(i = 0; i < n; ++i) c = _crc32(s->src.d[i], c);
    n = SNAP_GET_PROTOF(s->hdb2);
    for(i = 0; i < n; ++i) c = _crc32(s->pro.d[i], c);
    n = snap_numbyte(s);
    for(i = 0; i < n; ++i) c = _crc32(s->dat.d[i], c);
    
    return c ^ 0xFFFFFFFF;
}
#endif

static int8_t _snap_varead(struct snapva* v, szdata_t nb, SnapRead fnc)
{
    if ( !nb ) return 0;
    
    szdata_t n = v->n;
    uint8_t* d = v->d;
    int8_t ret;
    
    for(; nb && n; --nb, --n, ++d)
    {
        while( !(ret = fnc(d)) );
        if ( ret < 0 ) return -1;
    }
    
    for(; nb; --nb )
    {
        uint8_t disc;
        while( !(ret = fnc(&disc)) );
        if ( ret < 0 ) return -1;
    }
    
    return 0;
}

szdata_t snap_numbyte(struct snap* s)
{
    szdata_t d = SNAP_GET_NUMDAT(s->hdb1);
    
    if ( d <= 0x08 ) return d + SNAP_GET_CMD(s->hdb1);
    switch( d )
    {
        case 0x09: return 16 + SNAP_GET_CMD(s->hdb1);
        case 0x0A: return 32 + SNAP_GET_CMD(s->hdb1);
        case 0x0B: return 64 + SNAP_GET_CMD(s->hdb1);
        case 0x0C: return 128 + SNAP_GET_CMD(s->hdb1);
        case 0x0D: return 256 + SNAP_GET_CMD(s->hdb1);
        case 0x0E: return 512 + SNAP_GET_CMD(s->hdb1);
        case 0x0F: return s->dat.n + SNAP_GET_CMD(s->hdb1);
    }
    
    return -1;
}

int8_t snap_read(struct snap* s, SnapRead fnc)
{
    int8_t ret;
    uint8_t p;
    
    /**************************/
    /***     GET HEADER     ***/
    /**************************/
    
    ///get && cmp sync
    while( !(ret = fnc(&p)) );
    if ( ret < 0 ) return -SNAP_ERR_GET_SYNC;
    if ( p != SNAP_SYNC ) return -SNAP_ERR_SYNC;
    
    ///get hdb2
    while( !(ret = fnc(&s->hdb2)) );
    if ( ret < 0 ) return -SNAP_ERR_GET_HDB2;
    
    ///get hdb1
    while( !(ret = fnc(&s->hdb1)) );
    if ( ret < 0 ) return -SNAP_ERR_GET_HDB1;
    
    
    /**************************/
    /***     PARSE HDB2     ***/
    /**************************/
        
    ///get dst adr
    ret = _snap_varead( &s->dst, SNAP_GET_DSTADR(s->hdb2), fnc);
    if ( ret ) return -SNAP_ERR_GET_DADR;
    
    ///get src adr
    ret = _snap_varead( &s->src, SNAP_GET_SRCADR(s->hdb2), fnc);
    if ( ret ) return -SNAP_ERR_GET_SADR;
    
    ///get proto flags
    ret = _snap_varead( &s->pro, SNAP_GET_PROTOF(s->hdb2), fnc);
    if ( ret ) return -SNAP_ERR_GET_PROF;
    
    /**************************/
    /***     PARSE HDB1     ***/
    /**************************/
    
    ///get data
    ret = _snap_varead( &s->dat, snap_numbyte(s), fnc);
    if ( ret ) return -SNAP_ERR_GET_DATA;
    
    ///get error check
    switch ( SNAP_GET_EDM(s->hdb1) )
    {
        case SNAP_EDM_3TIM:
        case SNAP_EDM_NONE:
        case SNAP_EDM_SFEC:
            p = 0;
        break;
        
        case SNAP_EDM_8CRC:
        case SNAP_EDM_CHKS:
            p = 1;
        break;
        
        #ifndef SNAP_DISABLE_16BIT
        case SNAP_EDM_16CR:
            p = 2;
        break;
        #endif
        #ifndef SNAP_DISABLE_32BIT
        case SNAP_EDM_32CR:
            p = 4;
        break;
        #endif
        default:
            return -SNAP_ERR_GET_ECHK;
    }
    
    ret = _snap_varead( &s->chk, p, fnc);
    if ( ret ) return -SNAP_ERR_GET_ECHK;
    
    return 0;
}

int8_t snap_read_async(struct snap* s, SnapRead fnc)
{
    static uint8_t state = 0;
    int8_t ret;
    uint8_t p;
    
    switch ( state )
    {
        case 0:
            ret = fnc(&p);
            if ( ret < 0 ) { state = 0; return -SNAP_ERR_GET_SYNC;}
            if ( ret == 0 ) return -SNAP_WAIT;
            if ( p != SNAP_SYNC ) {state = 0; return -SNAP_ERR_SYNC;}
            ++state;
        
        case 1:
            ret = fnc(&s->hdb2);
            if ( ret < 0 ) { state = 0; return -SNAP_ERR_GET_HDB2;}
            if ( ret == 0 ) return -SNAP_WAIT;
            ++state;
        
        case 2:
            ret = fnc(&s->hdb1);
            if ( ret < 0 ) {state = 0; return -SNAP_ERR_GET_HDB1;}
            if ( ret == 0 ) return -SNAP_WAIT;
            ++state;
            
        case 3:
            ret = _snap_varead( &s->dst, SNAP_GET_DSTADR(s->hdb2), fnc);
            if ( ret ) return -SNAP_ERR_GET_DADR;
            ret = _snap_varead( &s->src, SNAP_GET_SRCADR(s->hdb2), fnc);
            if ( ret ) return -SNAP_ERR_GET_SADR;
            ret = _snap_varead( &s->pro, SNAP_GET_PROTOF(s->hdb2), fnc);
            if ( ret ) return -SNAP_ERR_GET_PROF;
            
            ret = _snap_varead( &s->dat, snap_numbyte(s), fnc);
            if ( ret ) return -SNAP_ERR_GET_DATA;
    
            switch ( SNAP_GET_EDM(s->hdb1) )
            {
                case SNAP_EDM_3TIM:
                case SNAP_EDM_NONE:
                case SNAP_EDM_SFEC:
                    p = 0;
                break;
        
                case SNAP_EDM_8CRC:
                case SNAP_EDM_CHKS:
                    p = 1;
                break;
        
                case SNAP_EDM_16CR:
                    p = 2;
                break;
        
                case SNAP_EDM_32CR:
                    p = 4;
                break;
            }
            ret = _snap_varead( &s->chk, p, fnc);
            if ( ret ) return -SNAP_ERR_GET_ECHK;
        break;
    }
    return SNAP_OK;
}


uint8_t snap_cmp(struct snap* s, struct snap* t)
{
    szdata_t i;
    szdata_t n;
    
    if ( s->hdb2 != t->hdb2 ) return s->hdb2 - t->hdb2;
    if ( s->hdb1 != t->hdb1 ) return s->hdb1 - t->hdb1;
    n = SNAP_GET_DSTADR(s->hdb2);
    for( i = 0; i < n; ++i) if ( s->dst.d[i] != t->dst.d[i] ) return s->dst.d[i] - t->dst.d[i];
    n = SNAP_GET_SRCADR(s->hdb2);
    for( i = 0; i < n; ++i) if ( s->src.d[i] != t->src.d[i] ) return s->src.d[i] - t->src.d[i];
    n = SNAP_GET_PROTOF(s->hdb2);
    for( i = 0; i < n; ++i) if ( s->pro.d[i] != t->pro.d[i] ) return s->pro.d[i] - t->pro.d[i];
    n = snap_numbyte(s);
    for( i = 0; i < n; ++i) if ( s->dat.d[i] != t->dat.d[i] ) return s->dat.d[i] - t->dat.d[i];
    n = SNAP_GET_EDM(s->hdb1);
    for( i = 0; i < n; ++i) if ( s->chk.d[i] != t->chk.d[i] ) return s->chk.d[i] - t->chk.d[i];
    
    return 0;
}

uint8_t snap_3time(struct snap* s, struct snap* t, SnapRead fnc)
{
    uint8_t i;
    int8_t ret;
    
    for ( i = 0; i < 2; ++i)
    {
        ret = snap_read(t, fnc);
        if ( ret ) return ret;
        if ( snap_cmp(s,t) ) return -SNAP_ERR_CORRUPT;
    }
    return SNAP_OK;
}

int8_t snap_check(struct snap* s, struct snap* t, SnapRead fnc)
{
    switch ( SNAP_GET_EDM(s->hdb1) )
    {
        case SNAP_EDM_NONE:
        return SNAP_OK;
        
        case SNAP_EDM_3TIM:
        return snap_3time(s, t, fnc);
        
        case SNAP_EDM_CHKS:
        {
            uint8_t c = snap_checksum(s);
            return (c == *s->chk.d) ? 0 : -SNAP_ERR_CORRUPT;
        }
        
        case SNAP_EDM_8CRC:
        {
            uint8_t c = snap_crc8(s);
            return (c == *s->chk.d) ? SNAP_OK : -SNAP_ERR_CORRUPT;
        }
        
        #ifndef SNAP_DISABLE_16BIT
        case SNAP_EDM_16CR:
        {
            uint16_t c = snap_crc16(s);
            if ( (c & 0xFF) != s->chk.d[0] ) return -SNAP_ERR_CORRUPT;
            if ( ((c >> 8) & 0xFF) != s->chk.d[1] ) return -SNAP_ERR_CORRUPT;
            return SNAP_OK;
        }
        #endif
        
        #ifndef SNAP_DISABLE_32BIT
        case SNAP_EDM_32CR:
        {
            uint32_t c = snap_crc32(s);
            if ( (c & 0xFF) != s->chk.d[0] ) return -SNAP_ERR_CORRUPT;
            if ( ((c >> 8) & 0xFF) != s->chk.d[1] ) return -SNAP_ERR_CORRUPT;
            if ( ((c >> 16) & 0xFF) != s->chk.d[2] ) return -SNAP_ERR_CORRUPT;
            if ( ((c >> 24) & 0xFF) != s->chk.d[3] ) return -SNAP_ERR_CORRUPT;
            return (c == *((int32_t*)(s->chk.d))) ? 0 : -SNAP_ERR_CORRUPT;
        }
        #endif
        
        default:
            return -SNAP_ERR_CORRUPT;
    }
    
    return  -SNAP_ERR_CORRUPT;
}


static int8_t _snap_vawrite(struct snapva* v, szdata_t nb, SnapWrite fnc)
{
    if ( !nb ) return 0;
    
    uint8_t* d = v->d;
    int8_t ret;
    
    for(; nb; --nb, ++d)
    {
        while( !(ret = fnc(*d)) );
        if ( ret < 0 ) return -1;
    }
    
    return 0;
}

int8_t snap_write(struct snap* s, SnapWrite fnc)
{
    int8_t ret;
    int8_t p;
    
    /**************************/
    /***     PUT HEADER     ***/
    /**************************/
    
    while ( !(ret = fnc(SNAP_SYNC)) );
    if ( ret < 0 ) return -SNAP_ERR_GET_SYNC;
    
    ///hdb2
    while( !(ret = fnc(s->hdb2)) );
    if ( ret < 0 ) return -SNAP_ERR_GET_HDB2;
    
    ///hdb1
    while( !(ret = fnc(s->hdb1)) );
    if ( ret < 0 ) return -SNAP_ERR_GET_HDB1;
    
    /**************************/
    /***     PARSE HDB2     ***/
    /**************************/
        
    ///put dst adr
    ret = _snap_vawrite( &s->dst, SNAP_GET_DSTADR(s->hdb2), fnc);
    if ( ret ) return -SNAP_ERR_GET_DADR;
    
    ///put src adr
    ret = _snap_vawrite( &s->src, SNAP_GET_SRCADR(s->hdb2), fnc);
    if ( ret ) return -SNAP_ERR_GET_SADR;
    
    ///put proto flags
    ret = _snap_vawrite( &s->pro, SNAP_GET_PROTOF(s->hdb2), fnc);
    if ( ret ) return -SNAP_ERR_GET_PROF;
    
    /**************************/
    /***     PARSE HDB1     ***/
    /**************************/
    
    ///get data
    ret = _snap_vawrite( &s->dat, snap_numbyte(s), fnc);
    if ( ret ) return -SNAP_ERR_GET_DATA;
    
    ///get error check
    switch ( SNAP_GET_EDM(s->hdb1) )
    {
        default:
        #ifndef SNAP_DISABLE_ERROR_REPORTING
            fprintf(stderr,"DEBUG ERROR!!!\n"); 
            exit(0);
        #endif
        case SNAP_EDM_3TIM:
        case SNAP_EDM_NONE:
        case SNAP_EDM_SFEC:
            p = 0;
        break;
        
        case SNAP_EDM_CHKS:
            p = 1;
            *((uint8_t*)(s->chk.d)) = snap_checksum(s);
        break;
        
        case SNAP_EDM_8CRC:
            p = 1;
            s->chk.d[0] = snap_crc8(s);
        break;
        
        #ifndef SNAP_DISABLE_16BIT
        case SNAP_EDM_16CR:
            p = 2;
            uint16_t crc16 = snap_crc16(s);
            s->chk.d[0] = crc16 & 0xFF;
            s->chk.d[1] = (crc16 >> 8 ) & 0xFF;
        break;
        #endif
        
        #ifndef SNAP_DISABLE_32BIT
        case SNAP_EDM_32CR:
            p = 4;
            uint32_t crc32 = snap_crc32(s);
            s->chk.d[0] = crc32 & 0xFF;
            s->chk.d[1] = (crc32 >> 8 ) & 0xFF;
            s->chk.d[2] = (crc32 >> 16 ) & 0xFF;
            s->chk.d[3] = (crc32 >> 24 ) & 0xFF;
        break;
        #endif
    }
    ret = _snap_vawrite( &s->chk, p, fnc);
    if ( ret ) return -SNAP_ERR_GET_ECHK;
    
    return 0;
    
}

uint8_t snap_init(uint8_t* sbuf, struct snap* s, uint8_t szdst, uint8_t szsrc, uint8_t szpro, uint8_t errtype, szdata_t szdata)
{
    s->hdb2 = 0;
    s->hdb1 = 0;
    
    s->dst.n = szdst;
    s->dst.d = ( szdst ) ? sbuf : NULL;
    sbuf += szdst;
    SNAP_SET_DSTADR(s->hdb2, szdst);
    
    s->src.n = szsrc;
    s->src.d = ( szsrc ) ? sbuf : NULL;
    sbuf += szsrc;
    SNAP_SET_SRCADR(s->hdb2, szsrc);
    
    s->pro.n = szpro;
    s->pro.d = ( szpro ) ? sbuf : NULL;
    sbuf += szpro;
    SNAP_SET_PROTOF(s->hdb2, szpro);
    
    switch ( errtype )
    {
        default:
        case SNAP_EDM_3TIM:
        case SNAP_EDM_NONE:
        case SNAP_EDM_SFEC:
            s->chk.n = 0;
            s->chk.d = NULL;
        break;
        
        case SNAP_EDM_8CRC:
        case SNAP_EDM_CHKS:
            s->chk.n = 1;
            s->chk.d = sbuf;
            ++sbuf;
        break;
        
        #ifndef SNAP_DISABLE_16BIT
        case SNAP_EDM_16CR:
            s->chk.n = 2;
            s->chk.d = sbuf;
            sbuf += 2;
        break;
        #endif 
        
        #ifndef SNAP_DISABLE_32BIT
        case SNAP_EDM_32CR:
            s->chk.n = 4;
            s->chk.d = sbuf;
            sbuf += 4;
        break;
        #endif
    }
    SNAP_SET_EDM(s->hdb1, errtype);
    
    snap_data_set(s, szdata);
    s->dat.d = sbuf;
    
    return 0;
}

uint8_t snap_data_set(struct snap* s, szdata_t szdata)
{
    if ( szdata < SNAP_DATA_USER )
    {
        SNAP_SET_NUMDAT(s->hdb1, szdata);
        s->dat.n = snap_numbyte(s);
        return 0;
    }
    
    SNAP_SET_NUMDAT(s->hdb1, SNAP_DATA_USER);
    s->dat.n = szdata;
    
    return 0;
}

szdata_t snap_data_get(struct snap* s)
{
    return snap_numbyte(s);
}

uint8_t snap_ack_set(struct snap* s, uint8_t ack)
{
    SNAP_SET_ACK(s->hdb2,ack);
    return 0;
}

uint8_t snap_ack_get(struct snap* s)
{
    return SNAP_GET_ACK(s->hdb2);
}

uint8_t snap_dst_set(struct snap* s, uint8_t dst)
{
    SNAP_SET_DSTADR(s->hdb2,dst);
    return 0;
}

uint8_t snap_dst_get(struct snap* s)
{
    return SNAP_GET_DSTADR(s->hdb2);
}

uint8_t snap_src_set(struct snap* s, uint8_t src)
{
    SNAP_SET_SRCADR(s->hdb2,src);
    return 0;
}

uint8_t snap_src_get(struct snap* s)
{
    return SNAP_GET_SRCADR(s->hdb2);
}

uint8_t snap_pro_set(struct snap* s, uint8_t pro)
{
    SNAP_SET_PROTOF(s->hdb2,pro);
    return 0;
}

uint8_t snap_pro_get(struct snap* s)
{
    return SNAP_GET_PROTOF(s->hdb2);
}

uint8_t snap_cmd_set(struct snap* s, uint8_t cmd)
{
    SNAP_SET_CMD(s->hdb1,cmd);
    return 0;
}

uint8_t snap_cmd_get(struct snap* s)
{
    return SNAP_GET_CMD(s->hdb1);
}

uint8_t snap_edm_set(struct snap* s, uint8_t erd)
{
    SNAP_SET_EDM(s->hdb1,erd);
    return 0;
}

uint8_t snap_edm_get(struct snap* s)
{
    return SNAP_GET_EDM(s->hdb1);
}

#ifndef SNAP_DISABLE_ERROR_REPORTIN
void snap_err_print(int8_t e)
{
    static const char* mper[] = { SNAP_STR_ERROR };
    if ( e < 0 ) e = -e;
    if ( e  < SNAP_ERR_COUNT )
        fprintf(stderr,"%s\n",mper[e & 0x0F]);
    else
        fprintf(stderr,"%s\n",mper[SNAP_ERR_NOTERROR]);
}
#endif

#ifdef _APP
void test()
{
    char *str = "SNAP";
    int l = strlen(str);
    
    int i;
    uint8_t crc8 = 0;
    uint16_t crc16 = 0;
    uint32_t crc32 = 0xFFFFFFFF;
    uint8_t cheks = 0;
    
    
    for( i = 0; i < l; ++i, ++str)
    {
        crc8 = _crc8(*str,crc8);
        crc16 = _crc16(*str,crc16);
        crc32 = _crc32(*str,crc32);
        cheks += *str;
    }
    crc32 ^= 0xFFFFFFFF;
    
    puts("SNAP");
    printf("checksum:0x%X\ncrc8: 0x%X\ncrc16: 0x%X\ncrc32: 0x%X\n",cheks,crc8,crc16,crc32);
    
    str = "snap";
    l = strlen(str);
    cheks = 0;
    crc8 = 0;
    crc16 = 0;
    crc32 = 0xFFFFFFFF;
    
    for( i = 0; i < l; ++i, ++str)
    {
        crc8 = _crc8(*str,crc8);
        crc16 = _crc16(*str,crc16);
        crc32 = _crc32(*str,crc32);
        cheks += *str;
    }
    crc32 ^= 0xFFFFFFFF;
    
    puts("snap");
    printf("checksum:0x%X\ncrc8: 0x%X\ncrc16: 0x%X\ncrc32: 0x%X\n",cheks,crc8,crc16,crc32);
    
}
#endif
