#ifndef TEST_XOODYAK_XOODYAK_H
#define TEST_XOODYAK_XOODYAK_H

#include <stdint.h>

#define UP 0
#define DOWN 1

#define HASH 0
#define ENCRYPT 1

#define ZERO 0x00
#define AB_KEY 0x02
#define ABSORB 0x03
#define RATCHET 0x10
#define SQUEEZE_KEY 0x20
#define SQUEEZE 0x40
#define CRYPT 0x80

struct keykack
{
    uint8_t state[48];
    uint8_t phase;
    uint8_t mode;
    uint8_t rate_absorb;
    uint8_t rate_squeeze;
} xoodyak;

void init_xoodyak(struct keykack *x);
void absorb(struct keykack *x, uint8_t *data, uint16_t data_len);
void squeeze(struct keykack *x, uint8_t *data, uint16_t data_len);

#endif
