#ifndef _CIPHER_SHASHA_H
#define _CIPHER_SHASHA_H
#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */
    typedef unsigned int uint32;
    typedef uint32 word32;
    typedef struct {
        uint32 h[5];
        unsigned char block[64];
        int blkused;
        uint32 lenhi, lenlo;
    } SHA_State;
    void SHA_Init(SHA_State * s);
    void SHA_Bytes(SHA_State * s, const void *p, int len);
    void SHA_Final(SHA_State * s, unsigned char *output);
    void SHA_Simple(const void *p, int len, unsigned char *output);
#ifdef  __cplusplus
}
#endif /* __cplusplus */
#endif /* _CIPHER_SHASHA_H */
