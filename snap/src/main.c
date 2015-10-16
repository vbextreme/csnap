#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <csnap.h>
#include "optex.h"

static int32_t extendprint = 0;
static int32_t snapread = 0;

struct argdef myarg[] = { { 'd', 'd',   "dstadr", OPT_ARG, NULL, "Destination address (val)"},
						  { 's', 's',   "srcadr", OPT_ARG, NULL, "Source address(val)"},
						  { 'a', 'a',      "ack", OPT_ARG, NULL, "Ack (no,req,res,err)"},
                          { 'e', 'e',      "edm", OPT_ARG, NULL, "Error detection mode(none,chk,3tim,crc8,crc16,crc32)"},
                          { 'M', 'M',  "msgsize", OPT_ARG, NULL, "Message size(strval)"},
                          { 'm', 'm',  "message", OPT_ARG, NULL, "Message(str)"},
                          { 'x', 'x',   "extend", OPT_NOARG, &extendprint, "print human version"},
                          { 'r', 'r',     "read", OPT_NOARG, &snapread, "read from std input"},
						  { 'h', 'h',     "help", OPT_NOARG, NULL, "view help"},
						  {   0,   0,       NULL, OPT_ARG, NULL, NULL}
						};

uint8_t bufs[1024];
struct snap s;

int8_t snapw(uint8_t w)
{
    if ( !extendprint ) putchar(w);
    return 1;
}

int8_t snapr(uint8_t* r)
{
    int rc = getchar();
    *r = rc;
    return (rc < 0 ) ? -1 : 1;
}

void fsnapvaw(FILE* f, struct snapva* v, uint16_t sz)
{
    uint8_t i;
    for ( i = 0; i < sz; ++i )
        fprintf(f,"0x%X ",v->d[i]);
}

void fsnapw(FILE* f, struct snap* s)
{
    fprintf(f,"0x%X\n",SNAP_SYNC);
    fprintf(f,"hdb2     :: 0x%X\n",s->hdb2);
    fprintf(f,"hdb1     :: 0x%X\n",s->hdb1);
    fprintf(f,"dest(%3d):: ",snap_dst_get(s)); fsnapvaw(f,&s->dst,snap_dst_get(s));puts("");
    fprintf(f,"src (%3d):: ",snap_src_get(s)); fsnapvaw(f,&s->src,snap_src_get(s));puts("");
    fprintf(f,"prof(%3d):: ",snap_pro_get(s)); fsnapvaw(f,&s->pro,snap_pro_get(s));puts("");
    fprintf(f,"ack      :: 0x%X\n",snap_ack_get(s));
    fprintf(f,"cmd      :: 0x%X\n",snap_cmd_get(s));
    fprintf(f,"chk (%3d):: ",snap_edm_get(s)); fsnapvaw(f,&s->chk,s->chk.n);puts("");
    fprintf(f,"data(%3d):: ",snap_numbyte(s)); fsnapvaw(f,&s->dat,snap_numbyte(s));puts("");
}

int main(int argc, char** argv)
{
    opt_init(myarg,argv,argc);
    
    uint8_t dstcount = 0;
    uint8_t dstadr[3] = {0,0,0};
    uint8_t srccount = 0;
    uint8_t srcadr[3] = {0,0,0};
    uint8_t ack = SNAP_ACK_NO;
    uint8_t edm = SNAP_EDM_NONE;
    uint16_t msgsz = 0;
    uint8_t realmsgsz = SNAP_DATA_0;
    uint8_t msg[1024];
    uint16_t msgcount = 0;
    
    int32_t ret;
    char* carg;
    
    while ( (ret = opt_parse(&carg)) >= 0 )
    {
        switch ( ret )
        {
            case 'd':
                if ( dstcount >= 3 ) { fprintf(stderr,"error destination address max 3 byte\n"); return -1;}
                dstadr[dstcount++] = *carg;
            break;
            
            case 's':
                if ( srccount >= 3 ) { fprintf(stderr,"error source address max 3 byte\n"); return -1;}
                srcadr[srccount++] = *carg;
            break;
            
            case 'a':
                if ( !strcmp(carg,"no") )
                    ack = SNAP_ACK_NO;
                else if ( !strcmp(carg,"req") )
                    ack = SNAP_ACK_REQ;
                else if ( !strcmp(carg,"res") )
                    ack = SNAP_ACK_RES;
                else if ( !strcmp(carg,"err") )
                    ack = SNAP_ACK_ERR;
                else
                {
                    fprintf(stderr,"error ack not definited\n"); 
                    return -1;
                }
            break;
            
            case 'e':
                if ( !strcmp(carg,"none") )
                    edm = SNAP_EDM_NONE;
                else if ( !strcmp(carg,"3tim") )
                    edm = SNAP_EDM_3TIM;
                else if ( !strcmp(carg,"chk") )
                    edm = SNAP_EDM_CHKS;
                else if ( !strcmp(carg,"crc8") )
                    edm = SNAP_EDM_8CRC;
                else if ( !strcmp(carg,"crc16") )
                    edm = SNAP_EDM_16CR;
                else if ( !strcmp(carg,"crc32") )
                    edm = SNAP_EDM_32CR;
                else
                {
                    fprintf(stderr,"error error detection mode not definited\n"); 
                    return -1;
                }
            break;
            
            case 'M':
                msgsz = strtol(carg,NULL,10);
                if ( msgsz < 9 )
                    realmsgsz = msgsz;
                else if ( msgsz == 16 )
                    realmsgsz = SNAP_DATA_16;
                else if ( msgsz == 32 )
                    realmsgsz = SNAP_DATA_32;
                else if ( msgsz == 64 )
                    realmsgsz = SNAP_DATA_64;
                else if ( msgsz == 128 )
                    realmsgsz = SNAP_DATA_128;
                else if ( msgsz == 256 )
                    realmsgsz = SNAP_DATA_256;
                else if ( msgsz == 512 )
                    realmsgsz = SNAP_DATA_512;
                else
                { 
                    fprintf(stderr,"error message size (0,1,2,3,4,5,6,7,8,16,32,64,128,512)\n"); 
                    return -1;
                }
            break;
            
            case 'm':
                while ( *carg )
                {
                    if ( msgcount >= msgsz ) { fprintf(stderr,"error message > message size\n"); return -1;}
                    msg[msgcount++] = *carg++;
                }    
            break;
            
            case 'h':
                opt_help();
            return 0;
            
            default:
                 fprintf(stderr,"error argument\n"); 
            return -1;
        }
    }
    
    if ( snapread )
    {
        snap_init(bufs,&s,3,3,3,edm,SNAP_DATA_512);
        snap_ack_set(&s,ack);
        
        uint8_t bufch[1024];
        struct snap ch;
        snap_init(bufch,&ch,3,3,3,SNAP_EDM_3TIM,SNAP_DATA_512);
        int8_t er;
        
        while ( !(er = snap_read(&s, snapr)) )
        {
            if ( (er = snap_check(&s,&ch,snapr)) )
            {
                snap_err_print(er);
                continue;
            }
            
            if ( extendprint ) 
            {
                printf("S.N.A.P.\n");
                fsnapw(stdout,&s);
            }
            else
            {
                if ( (er = snap_write(&s,snapw)) );
                {
                    snap_err_print(er);
                    return er;
                }
            }
        }
        if ( er != -SNAP_ERR_GET_SYNC ) 
        {
            snap_err_print(er);
            return er;
        }
        return 0;
    }
    
    snap_init(bufs,&s,dstcount,srccount,0,edm,realmsgsz);
    snap_ack_set(&s,ack);
    
    memcpy(s.dst.d,dstadr,dstcount);
    memcpy(s.src.d,srcadr,srccount);
        
    if (msgsz)
    {
        memset(s.dat.d,0,s.dat.n);
        memcpy(s.dat.d,msg,msgsz);
    }
    
    snap_write(&s, snapw);
    
    if ( extendprint )
    {
        printf("S.N.A.P.\n");
        fsnapw(stdout,&s);
    }
    
    return 0;
}
