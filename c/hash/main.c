#define test

#ifdef test
    #include "test.h"
    int main() {
        run_test();
    }
#endif

#ifndef test
#include <stdint.h>
#include "xoodyak.h"
#include "assert.h"
#include <stdio.h>
#include <string.h>

void hash(uint8_t *message, uint16_t message_len, uint8_t *hash, uint16_t hash_len) {
    struct keykack x;
    init_xoodyak(&x);
    absorb(&x, message, message_len);
    squeeze(&x, hash, hash_len);
}

void write_hash_data(unsigned char *h, const char *filepath, unsigned long long hlen) {
	FILE *fp;
    printf("Save hash in: %s\n", filepath);
	fp = fopen(filepath, "wb");
    if(fp!=NULL){
        fwrite(h, sizeof(unsigned char), hlen, fp);
        fclose(fp);
    }
    else    
        printf("Error: Can not write file.\n");
}

void help()
{
    printf("-m\tMessage to calc hash.\n");
    printf("-f\tFilepath to write hash.\n");
}

int main(int argc, char **argv) {
    #ifdef t
        test();
    #else
        char filepath[1000];
        

        unsigned char m[1000];
        unsigned long long mlen;

        int i = 0;
        int cnt = 0;
        int path = 0;
        for(i = 1; i < argc; i=i+2){
            if(strcmp(argv[i], "-h")==0 || strcmp(argv[i], "-help")==0 || strcmp(argv[i], "--h")==0 ||strcmp(argv[i], "--help")==0){
                help();
                return 0;
            } else if(strcmp(argv[i], "-m")==0) {
                cnt += 1;
                strcpy((char*)m, argv[i+1]);
                mlen = strlen((char*)m);
            } else if (strcmp(argv[i], "-f")==0) {
                strcpy((char*)filepath, argv[i+1]);
                path = 1;
            } 
        }
        
        if(path != 1){
            strcpy(filepath, "./default.hash");
        }
        
        uint8_t out_hash_buff[32];
        hash(m, mlen, out_hash_buff, 32);
        printf("Hash:\n");
        for(i = 0; i < 32; i++) {
            printf("%02x", out_hash_buff[i]);
        }
        printf("\n");

        write_hash_data(out_hash_buff, filepath, 32);

        return 0;
    #endif
}

#endif