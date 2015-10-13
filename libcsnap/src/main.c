#ifdef _APP

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "csnap.h"

/*
//ESEMPIO invio
//funzione che invia il dato al terminale
int8_t snapw(uint8_t w)
{
    putchar(w);
    return 1;//ok
}

int main(int argc, char** argv)
{
    uint8_t buf[3];
    struct snap s;
    
    int ret;
    
    snap_init(buf,&s,0,0,0,SNAP_EDM_8CRC,SNAP_DATA_0);
    ret = snap_write(&s, snapw);
    snap_err_print(ret);
    puts("");
    
    return 0;
}
*/

/*
//ESEMPIO ricezione
//funzione che legge il dato dal terminale
int8_t snapr(uint8_t* r)
{
    int ch = getchar();
    if ( ch < 0 ) return -1; //ritorno l'errore
    *r = ch;
    return 1;//ok
}

int main(int argc, char** argv)
{
    uint8_t buf[3];
    struct snap s;
    
    int ret;
    
    snap_init(buf,&s,0,0,0,SNAP_EDM_8CRC,SNAP_DATA_0);
    
    ret = snap_read(&s, snapr);
    snap_err_print(ret);
    ret = snap_check(&sr,NULL,NULL);
    snap_err_print(ret);
    puts("");
    
    return 0;
}
*/
#endif
