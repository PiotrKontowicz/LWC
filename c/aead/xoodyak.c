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
       block[i] = x->state[i]; // Unsupported memory access on variable block
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

void absorb_key(struct keykack *x, uint8_t *key, uint8_t key_len,
                uint8_t * id, uint16_t id_len,
                uint8_t *cnt, uint16_t cnt_len) {

    uint8_t data[44];
    uint32_t total_len = key_len + id_len + cnt_len + 1;

    x->rate_squeeze = 24;
    x->rate_absorb = 44;

    x->mode = ENCRYPT;

    uint8_t i;
    for(i = 0; i < key_len; i++) {
        data[i] = key[i];
    }

    for(i = 0; i < id_len; i++) {
        data[i + key_len] = id[i];
    }

    data[key_len + id_len] = (uint8_t)(id_len & 0xFF);

    absorb_any(x, &data[0], total_len, x->rate_absorb, AB_KEY);

    if(cnt_len > 0) {
        absorb_any(x, cnt, cnt_len, 1, ZERO);
    }
}

void init_xoodyak(struct keykack *x, uint8_t *key, uint8_t key_len,
                  uint8_t * id, uint8_t id_len,
                  uint8_t *cnt, uint8_t cnt_len) {

    x->mode = HASH;
    x->phase = UP;

    int i = 0;
    for(i = 0; i < 48; i++) {
        x->state[i] = 0;
    }
    //memset(x->state, 0, 48);
    x->rate_squeeze = 16;
    x->rate_absorb = 16;
    if(key_len > 0) {
        absorb_key(x, key, key_len, id, id_len, cnt, cnt_len);
    }
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

 void crypt(struct keykack *x, uint8_t *data, uint16_t data_len, uint8_t decrypt) {
    uint8_t block_len;
    uint8_t flag = CRYPT;
    uint16_t acc = 0;

    uint8_t up_out[24];
    uint8_t down_in[24];
    uint8_t i = 0;
    for(i = 0; i < 24; i++){
        up_out[i] = 0;
        down_in[i] = 0;
    }

    do{
        block_len = min(data_len - acc, 24);
        up(x, &up_out[0], block_len, flag);
        flag = ZERO;

        if(decrypt){
            for(i = 0; i < block_len; i++) {
                data[i + acc] = data[i + acc] ^ up_out[i];
                down_in[i] = data[i + acc];
            }
        } else {
            for(i = 0; i < block_len; i++) {
                down_in[i] = data[i + acc];
                data[acc + i] = data[acc + i] ^ up_out[i];
            }
        }

        down(x, down_in, block_len, ZERO);
        acc = acc + block_len;
    }while(data_len != acc);
 }

void absorb(struct keykack *x, uint8_t *data, uint16_t data_len) {
    absorb_any(x, data, data_len, x->rate_absorb, ABSORB);
}

void encrypt(struct keykack *x, uint8_t *data, uint16_t data_len) {
    crypt(x, data, data_len, 0);
}

void decrypt(struct keykack *x, uint8_t *data, uint16_t data_len) {
    crypt(x, data, data_len, 1);
}

void squeeze(struct keykack *x, uint8_t *data, uint16_t data_len) {
    squeeze_any(x, data, data_len, SQUEEZE);
}

