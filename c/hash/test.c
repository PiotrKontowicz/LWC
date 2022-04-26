#include "test.h"
#include "stdio.h"
#include "xoodoo.h"
#include "xoodyak.h"
#include "assert.h"
#include "stdlib.h"
#include "string.h"

void test_xoodoo() {
    uint8_t state[48] = { 0 };
    int i = 0;
    for(i = 0; i < 384; i++) {
        permute((uint32_t*)state);
    }
    uint8_t expected_data[48] = { 0xb0, 0xfa, 0x04, 0xfe, 0xce, 0xd8, 0xd5, 0x42,
                                  0xe7, 0x2e, 0xc6, 0x29, 0xcf, 0xe5, 0x7a, 0x2a,
                                  0xa3, 0xeb, 0x36, 0xea, 0x0a, 0x9e, 0x64, 0x14,
                                  0x1b, 0x52, 0x12, 0xfe, 0x69, 0xff, 0x2e, 0xfe,
                                  0xa5, 0x6c, 0x82, 0xf1, 0xe0, 0x41, 0x4c, 0xfc,
                                  0x4f, 0x39, 0x97, 0x15, 0xaf, 0x2f, 0x09, 0xeb };
    for(i = 0; i < 48; i++) {
        if(state[i] != expected_data[i])
            printf("Error in postion: %d\n", i);

        assert(state[i] == expected_data[i]);
    }
}

uint8_t* datahex(char* string) {

   if(string == NULL)
       return NULL;

   size_t slength = strlen(string);
   if((slength % 2) != 0) // must be even
       return NULL;

   size_t dlength = slength / 2;

   uint8_t* data = malloc(dlength);
   memset(data, 0, dlength);

   size_t index = 0;
   while (index < slength) {
       char c = string[index];
       int value = 0;
       if(c >= '0' && c <= '9')
           value = (c - '0');
       else if (c >= 'A' && c <= 'F')
           value = (10 + (c - 'A'));
       else if (c >= 'a' && c <= 'f')
           value = (10 + (c - 'a'));
       else {
           free(data);
           return NULL;
       }
       data[(index/2)] += value << (((index + 1) % 2) * 4);
       index++;
   }
   return data;
}

void test_xoodyak_hash(char *path) {
   FILE * fp;
   char * msg = NULL;
   char *md = NULL;
   size_t msg_len = 0;
   size_t md_len = 0;
   size_t  read;

   fp = fopen(path, "r");
   if (fp == NULL)
       exit(EXIT_FAILURE);

   int i = 0;
   
   while ((msg_len = getline(&msg, &read, fp)) != -1) {
       if((md_len = getline(&md, &read, fp)) != -1) {
           struct keykack x;
           init_xoodyak(&x);
           msg[msg_len - 1] = '\0';
           md[md_len - 1] = '\0';
           uint8_t *tmptmp = datahex(msg);
           absorb(&x, tmptmp, ((msg_len - 1) / 2));
           uint8_t tag[32] = {0};
           squeeze(&x, tag, 32);
           uint8_t *new_md = datahex(md);

           for(i = 0; i < 32; i++)
               assert(tag[i] == new_md[i]);
       }
   }

   fclose(fp);
   if (msg)
       free(msg);
}

void run_test() {
    printf("Test Xoodoo...\n");
    test_xoodoo();
    printf("Test Xoodoo succes.\nTest Xoodyak hash...\n");
    test_xoodyak_hash("/home/test/Desktop/test_xoodyak/vectors/hash.raw");
    printf("Test Xoodyak hash succes.\n");
}
