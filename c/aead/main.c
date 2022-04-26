#define test

#ifdef test
    #include "test.h"
    int main() {
        run_test();
    }
#endif

#ifndef test
    #include <string.h>
    #include <stdint.h>
    #include "xoodyak.h"
    #include "assert.h"
    #include <stdio.h>
void aead_encrypt(uint8_t *key, uint8_t key_len,
                  uint8_t *nonce, uint8_t nonce_len,
                  uint8_t *ad, uint8_t ad_len,
                  uint8_t *pt, uint8_t pt_len, uint8_t *buffer) {
    struct keykack x;
    uint8_t tmp[1];
    init_xoodyak(&x, key, key_len, tmp, 0, tmp, 0);
    absorb(&x, nonce, nonce_len);
    absorb(&x, ad, ad_len);
    
    encrypt(&x, pt, pt_len);
    uint8_t tag[16];
    squeeze(&x, tag, 16);

    int i = 0;
    for(i = 0; i < pt_len; i++) {
        buffer[i] = pt[i];
    }

    for(i = 0; i < 16; i++) {
        buffer[i + pt_len] = tag[i];
    }
}

void aead_decrypt(uint8_t *key, uint8_t key_len,
                  uint8_t *nonce, uint8_t nonce_len,
                  uint8_t *ad, uint8_t ad_len,
                  uint8_t *pt, uint8_t ct_len, uint8_t *buffer) {
    struct keykack x;
    uint8_t tmp[1];
    int i = 0;
    uint8_t pt_len = ct_len - 16;
    
    init_xoodyak(&x, key, key_len, tmp, 0, tmp, 0);
    absorb(&x, nonce, nonce_len);
    absorb(&x, ad, ad_len);
    
    decrypt(&x, buffer, pt_len);
    uint8_t tag[16];
    squeeze(&x, tag, 16);

    for(i = 0; i < 16; i++) {
        if(buffer[i + pt_len] != tag[i]) {
            printf("%d\t%d\n", (uint8_t)buffer[i + pt_len], (uint8_t)tag[i]);
        }
        assert(buffer[i + pt_len] == tag[i]);
    }
}

void help()
{
    printf("-m\tPlaintext message to encrypt\n");
    printf("-ad\tAssociated data to authenticate along with the packet but which does not need to be encrypted\n");
    printf("-npub\tPublic nonce for the packet which must be 16 bytes in lenght\n");
    printf("-k\t16 bytes key\n");
    printf("-decrypt\tSet to decrypt data.\n");
    printf("-encrypt\tSet to encrypt data.\n");
    printf("-f\tFilepath to write ciphertext\n");
}

void read_encrypted_data(uint8_t *c, const char *filepath, unsigned long long *clen) {
    FILE *fp;
    printf("Read ciphertext from: %s\n", filepath);
    fp = fopen(filepath, "rb");
    if(fp != NULL){
        fseek(fp, 0L, SEEK_END);
        *clen = ftell(fp);
        fseek(fp, 0L, SEEK_SET);
        fread(c, sizeof(uint8_t), *clen, fp);
        fclose(fp);
    } else {
        printf("Error. Can not read file.\n");
    }
}

void write_encrypted_data(unsigned char *c, const char *filepath, unsigned long long clen) {
	FILE *fp;
    printf("Save file in: %s\n", filepath);
	fp = fopen(filepath, "wb");
    if(fp!=NULL){
        fwrite(c, sizeof(unsigned char), clen, fp);
        fclose(fp);
    }
    else    
        printf("Error: Can not write file.\n");
}


int main(int argc, char **argv){
    char filepath[1000];
    

    uint8_t m[1000];
    unsigned long long mlen;

    uint8_t ad[1000];
    unsigned long long adlen;

    uint8_t npub[16];

    uint8_t k[16];

    uint8_t c[1000];
    unsigned long long clen;

    int enc = 1;


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
        } else if(strcmp(argv[i], "-ad")==0){
            cnt += 1;
            strcpy((char*)ad, argv[i+1]);
            adlen = strlen((char*)ad);
        } else if(strcmp(argv[i], "-npub")==0) {
            cnt += 1;
            strcpy((char*)npub, argv[i+1]);
        } else if (strcmp(argv[i], "-k")==0) {
            cnt += 1;
            strcpy((char*)k, argv[i+1]);
            if(strlen((char*)argv[i+1])!= 16){
                printf("Error: Key must be 16 bytes long!\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-f")==0) {
            strcpy((char*)filepath, argv[i+1]);
            path = 1;
        } else if (strcmp(argv[i], "-decrypt")==0) {
            enc = 1;
        } else if (strcmp(argv[i], "-encrypt")==0) {
            enc = 0;
        }

    }
    
    if(path != 1){
        strcpy(filepath, "./default.enc");
    }
    
    if(enc == 0){
        if(cnt == 4){
            printf("Encryption...\n");
            uint8_t buff[1024];
            aead_encrypt(k, 16, npub, 16, ad, adlen, m, mlen, buff);
            write_encrypted_data(buff, filepath, mlen + 16);
            printf("Encryption done. Ciphertext saved in: %s\n", filepath);

        } 
    } else {
        printf("Decrypt...\n");
        uint8_t buff[1024];
        printf("Read ciphertext from: %s\n", filepath);
        read_encrypted_data(buff, filepath, &clen);
        printf("Ciphertext length %lld bytes (With 16 bytes of aead tag).\n", clen);

        int i = 0;

        aead_decrypt(k, 16, npub, 16, ad, adlen, c, clen, buff);
        printf("Message:\n");
        for(i = 0; i < clen - 16; i++){
            printf("%c", buff[i]);
        }

        printf("\n");
    }

    return 0;
}
#endif

