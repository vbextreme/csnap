#ifdef _APP

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "csnap.h"

uint8_t bufw[530];
struct snap sw;
uint8_t bufr[530];
struct snap sr;

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

uint8_t fkbuf[1024];
uint16_t szfkb = 0;
uint16_t ifkbw = 0;
uint16_t ifkbr = 0;

int8_t snapw(uint8_t w)
{
    if ( ifkbw == 1024 ) return -1;
    fkbuf[ifkbw++] = w;
    ++szfkb;
    return 1;
}

int8_t snapr(uint8_t* r)
{
    if ( ifkbr == 1024 ) return -1;
    if ( ifkbr == szfkb ) return 0;
    
    *r = fkbuf[ifkbr++];
    return 1;
}

int main(int argc, char** argv)
{
    test_crc();
    return 0;
    
    int ret;
    
    printf("Send Data:: ");
    fflush(stdout);
    snap_init(bufw,&sw,3,3,3,SNAP_EDM_8CRC,SNAP_DATA_8);
    strcpy((char*)(sw.dat.d), "ciao");
    ret = snap_write(&sw, snapw);
    snap_err_print(ret);
    fsnapw(stdout,&sw);
    puts("");
    
    printf("Read Data:: ");
    fflush(stdout);
    snap_init(bufr,&sr,3,3,3,SNAP_EDM_8CRC,SNAP_DATA_8);
    ret = snap_read(&sr, snapr);
    snap_err_print(ret);
    fsnapw(stdout,&sr);
    puts("");
    
    printf("Check:: ");
    fflush(stdout);
    ret = snap_check(&sr,NULL,NULL);
    snap_err_print(ret);
    puts("");
    
    return 0;
}
#endif
