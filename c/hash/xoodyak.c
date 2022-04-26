#include "xoodyak.h"
#include "xoodoo.h"

static inline int min(uint32_t a, uint32_t b) {
    if(a > b)
        return b;
    return a;
}

void up(struct keykack *x, uint8_t *block, uint8_t count, uint8_t flag) {
    x->phase = UP;
    if(x->mode != HASH) {
        x->state[47] = x->state[47] ^ flag;
    }
    permute((uint32_t*)x->state);
    int i = 0;
    for(i = 0; i < count; i++) {
       block[i] = x->state[i]; 
    }
}

void down(struct keykack *x, uint8_t *block, uint8_t count, uint8_t flag) {
    x->phase = DOWN;
    uint8_t i = 0;
    for(i = 0; i < count; i++) {
        x->state[i] = x->state[i] ^ block[i];
    }

    x->state[count] = x->state[count] ^ 0x01;

    if(x->mode == HASH) {
        x->state[47] = x->state[47] ^ (flag & 0x01);
    } else {
        x->state[47] = x->state[47] ^ flag;
    }
}

void absorb_any(struct keykack *x, uint8_t *block, uint16_t block_len, uint16_t rate, uint8_t flag) {
    uint8_t tmp[1];
    uint16_t acc = 0;
    uint8_t len;
    do{
        if(x->phase != UP) {
            up(x, tmp, 0, ZERO);
        }
        len = min((block_len - acc), rate);
        down(x, &block[acc], len, flag);
        acc = acc + len;
        flag = ZERO;
    }while(block_len != acc);
}

void init_xoodyak(struct keykack *x) {
    x->mode = HASH;
    x->phase = UP;

    int i = 0;
    for(i = 0; i < 48; i++) {
        x->state[i] = 0;
    }
    x->rate_squeeze = 16;
    x->rate_absorb = 16;
}

void squeeze_any(struct keykack *x, uint8_t *out, uint16_t out_len, uint8_t flag) {
    uint8_t block_len = min(out_len, x->rate_squeeze);
    uint16_t end = 0;

    uint8_t tmp[1];
    up(x, out, block_len, flag);
    end = end + block_len;
    while(out_len > end) {
        down(x, tmp, 0, ZERO);
        block_len = min(out_len - end, x->rate_squeeze);
        up(x, &out[end], block_len, ZERO);
        end = end + block_len;
    }
}
void absorb(struct keykack *x, uint8_t *data, uint16_t data_len) {
    absorb_any(x, data, data_len, x->rate_absorb, ABSORB);
}

void squeeze(struct keykack *x, uint8_t *data, uint16_t data_len) {
    squeeze_any(x, data, data_len, SQUEEZE);
}