#include "xoodoo.h"
#include <string.h>
#define keccak


#ifdef keccak
#define ROTL32(a, offset)                       ((((uint32_t)a) << ((offset)%32)) ^ (((uint32_t)a) >> ((32-(offset))%32)))
#define    index(__x,__y)    ((((__y) % 3) * 4) + ((__x) % 4))

void pround(uint32_t *state, uint32_t rc){
    unsigned int x, y;
    uint32_t    b[12];
    uint32_t    p[4];
    uint32_t    e[4];

    /* Theta: Column Parity Mixer */
    for (x=0; x<4; ++x)
        p[x] = state[index(x,0)] ^ state[index(x,1)] ^ state[index(x,2)];
    for (x=0; x<4; ++x)
        e[x] = ROTL32(p[(x-1)%4], 5) ^ ROTL32(p[(x-1)%4], 14);
    for (x=0; x<4; ++x)
        for (y=0; y<3; ++y)
            state[index(x,y)] ^= e[x];


    /* Rho-west: plane shift */
    for (x=0; x<4; ++x) {
        b[index(x,0)] = state[index(x,0)];
        b[index(x,1)] = state[index(x-1,1)];
        b[index(x,2)] = ROTL32(state[index(x,2)], 11);
    }
    memcpy(state, b, sizeof(b) );
        
    /* Iota: round constant */
    state[0] ^= rc;

    /* Chi: non linear layer */
    for (x=0; x<4; ++x)
        for (y=0; y<3; ++y)
            b[index(x,y)] = state[index(x,y)] ^ (~state[index(x,y+1)] & state[index(x,y+2)]);
    memcpy(state, b, sizeof(b) );

    /* Rho-east: plane shift */
    for (x=0; x<4; ++x) {
        b[index(x,0)] = state[index(x,0)];
        b[index(x,1)] = ROTL32(state[index(x,1)], 1);
        b[index(x,2)] = ROTL32(state[index(x+2,2)], 8);
    }
    memcpy(state, b, sizeof(b) );
}

void permute( uint32_t * state)
{
    int i = 11;
    uint32_t r_const[12] = {0x00000058, 0x00000038, 0x000003C0, 0x000000D0, 0x00000120, 0x00000014, 0x00000060, 0x0000002C, 0x00000380, 0x000000F0, 0x000001A0, 0x00000012};
    for(i = 0; i < 12; i++){
        pround(state, r_const[i]);
    }
}
#endif

#ifndef keccak
static inline void swap (uint32_t *s, uint8_t a, uint8_t b)
{
    for (int i = 1 ; i ; i <<= 1)
    {
        if ((*(s + a) & i) != (*(s + b) & i))
        {
            *(s + a) ^= i;
            *(s + b) ^= i;
        }
    }
}

void permute(uint32_t *state) {
    uint32_t e[4], a, b, c, r;
    uint32_t round_constants[12] = { 0x058, 0x038, 0x3C0, 0x0D0, 0x120, 0x014, 0x060, 0x02C, 0x380, 0x0F0, 0x1A0, 0x012 };

    int i = 0;
    int j = 0;

    for(i = 0; i < 12; i++) {

        for(j = 0; j < 4; j++) {
            r = state[j] ^ state[j + 4] ^ state[j + 8];
            e[j] = (r >> 18) | (r << 14);
            r = e[j];
            e[j] ^= (r >> 9) | (r << 23);
        }

        for(j = 0; j < 12; j++) {
            state[j] ^= e[(j - 1) & 3];
        }

        swap(state, 7, 4);
        swap(state, 7, 5);
        swap(state, 7, 6);

        state[0] ^= round_constants[i];

        for(j = 0; j < 4; j++) {
            a = state[j];
            b = state[j + 4];
            r = state[j + 8];
            c = (r >> 21) | (r << 11);
            r = (b & ~a) ^ c;
            state[j + 8] = (r >> 24) | (r << 8);
            r = (a & ~c) ^ b;
            state[j + 4] = (r >> 31) | (r << 1);
            state[j] ^= c & ~b;
        }
        swap(state, 8, 10);
        swap(state, 9, 11);
    }
}

#endif