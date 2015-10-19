#ifndef __SNAP_PROTOCOL_H_INCLUDE__
#define __SNAP_PROTOCOL_H_INCLUDE__


/* S.N.A.P. 
 * -----------------
 * |     SYNC      -
 * -----------------
 * |0|1|0|1|0|1|0|0|
 * -----------------
 * 
 * ----------------- ----------------- 
 * |      HDB2     | |      HDB1     | 
 * ----------------- -----------------
 * |D|D|S|S|P|P|A|A| |C|E|E|E|N|N|N|N|
 *  s   r   r   c     m   r   u
 *  t   c   o   k     d   r   m
 *  A   A   t             o   D
 *  d   d   o             r   a
 *  r   r   F                 t
 * 
 *  -----------------
 *  | Dest Address  | 0/3
 *  -----------------
 * 
 *  -----------------
 *  | Src  Address  | 0/3
 *  -----------------
 * 
 *  -----------------
 *  | Protocol Flags| 0/3
 *  -----------------
 *  |R|E|S|E|R|V|E|D| |R|E|S|E|R|V|E|D| |R|E|S|E|R|V|E|D|
 *
 * ----------------------------------------------------------------
 * |SYNC|HDB2|HDB1|DST 0-3|SRC 0-3|PRO 0-3|DATA 0-512-USER|CRC 0-4|
 * ----------------------------------------------------------------
*/

#define SNAP_SHIFT_SRCADR 6
#define SNAP_SHIFT_DSTADR 4
#define SNAP_SHIFT_PROTOF 2
#define SNAP_SHIFT_ACK    0
#define SNAP_SHIFT_CMD    7
#define SNAP_SHIFT_EDM    4
#define SNAP_SHIFT_NUMDAT 0
#define SNAP_MASK_SRCADR  0x03
#define SNAP_MASK_DSTADR  0x03
#define SNAP_MASK_PROTOF  0x03
#define SNAP_MASK_ACK     0x03
#define SNAP_MASK_CMD     0x01
#define SNAP_MASK_EDM     0x07
#define SNAP_MASK_NUMDAT  0x0F

#define SNAP_STR_ERROR "snap ok", \
                       "snap error synch", "snap wait data", "snap error data corrupt", \
                       "snap error r/w synch","snap error r/w hdb2","snap error r/w hdb1", \
                       "snap error r/w dst address","snap error r/w src address", \
                       "snap error r/w protocol flags", \
                       "snap error r/w data","snap error r/w check", \
                       "snap error number error", \
                       "snap error command"
                       
#define SNAP_GET_DSTADR(H2) ( (H2 >> SNAP_SHIFT_DSTADR) & SNAP_MASK_DSTADR )
#define SNAP_GET_SRCADR(H2) ( (H2 >> SNAP_SHIFT_SRCADR) & SNAP_MASK_SRCADR )
#define SNAP_GET_PROTOF(H2) ( (H2 >> SNAP_SHIFT_PROTOF) & SNAP_MASK_PROTOF )
#define SNAP_GET_ACK(H2)    ( (H2 >> SNAP_SHIFT_ACK) & SNAP_MASK_PROTOF )
#define SNAP_GET_CMD(H1)    ( (H1 >> SNAP_SHIFT_CMD) & SNAP_MASK_CMD )
#define SNAP_GET_EDM(H1)    ( (H1 >> SNAP_SHIFT_EDM) & SNAP_MASK_EDM )
#define SNAP_GET_NUMDAT(H1) ( (H1 >> SNAP_SHIFT_NUMDAT) & SNAP_MASK_NUMDAT )
#define SNAP_CLR_DSTADR(H2)   H2 &= ~(SNAP_MASK_DSTADR << SNAP_SHIFT_DSTADR)
#define SNAP_CLR_SRCADR(H2)   H2 &= ~(SNAP_MASK_SRCADR << SNAP_SHIFT_SRCADR)
#define SNAP_CLR_PROTOF(H2)   H2 &= ~(SNAP_MASK_PROTOF << SNAP_SHIFT_PROTOF)
#define SNAP_CLR_ACK(H2)      H2 &= ~(SNAP_MASK_ACK << SNAP_SHIFT_ACK)
#define SNAP_CLR_CMD(H1)      H1 &= ~(SNAP_MASK_CMD << SNAP_SHIFT_CMD)
#define SNAP_CLR_EDM(H1)      H1 &= ~(SNAP_MASK_EDM << SNAP_SHIFT_EDM)
#define SNAP_CLR_NUMDAT(H1)   H1 &= ~(SNAP_MASK_NUMDAT << SNAP_SHIFT_NUMDAT)

#define SNAP_SET_DSTADR(H2,B) H2 |= (((B) << SNAP_SHIFT_DSTADR) & (SNAP_MASK_DSTADR << SNAP_SHIFT_DSTADR))
#define SNAP_SET_SRCADR(H2,B) H2 |= (((B) << SNAP_SHIFT_SRCADR) & (SNAP_MASK_SRCADR << SNAP_SHIFT_SRCADR))
#define SNAP_SET_PROTOF(H2,B) H2 |= (((B) << SNAP_SHIFT_PROTOF) & (SNAP_MASK_PROTOF << SNAP_SHIFT_PROTOF))
#define SNAP_SET_ACK(H2,B)    H2 |= (((B) << SNAP_SHIFT_ACK) & (SNAP_MASK_ACK << SNAP_SHIFT_ACK))
#define SNAP_SET_CMD(H1,B)    H1 |= (((B) << SNAP_SHIFT_CMD) & (SNAP_MASK_CMD << SNAP_SHIFT_CMD))
#define SNAP_SET_EDM(H1,B)    H1 |= (((B) << SNAP_SHIFT_EDM) & (SNAP_MASK_EDM << SNAP_SHIFT_EDM))
#define SNAP_SET_NUMDAT(H1,B) H1 |= (((B) << SNAP_SHIFT_NUMDAT) & (SNAP_MASK_NUMDAT << SNAP_SHIFT_NUMDAT))



#endif
