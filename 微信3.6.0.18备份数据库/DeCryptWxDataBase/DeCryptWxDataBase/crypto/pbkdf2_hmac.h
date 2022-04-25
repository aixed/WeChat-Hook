#include<stdio.h>
#include "sha.h"
#ifndef _CIPHER_PKCS5_PBKDF2_HMAC_H
#define _CIPHER_PKCS5_PBKDF2_HMAC_H
#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

#define KEY_IOPAD_SIZE 64
#define PUT_32BIT_MSB_FIRST(cp, value) ( \
    (cp)[0] = (unsigned char)((value) >> 24), \
    (cp)[1] = (unsigned char)((value) >> 16), \
    (cp)[2] = (unsigned char)((value) >> 8), \
    (cp)[3] = (unsigned char)(value))
	typedef struct {
		SHA_State ctx;
		unsigned char ipad[KEY_IOPAD_SIZE]; /*!< HMAC: inner padding */
		unsigned char opad[KEY_IOPAD_SIZE]; /*!< HMAC: outer padding */
	} sha1_context;

    void PKCS5_PBKDF2_HMAC(const unsigned char *password, size_t plen,
        const unsigned char *salt, size_t slen,
        const unsigned long iteration_count, const unsigned long key_length,
        unsigned char *output);

	void sha1_hmac_starts(sha1_context * ctx, const unsigned char *key, int keylen);
	void sha1_hmac_update(sha1_context * ctx, const unsigned char *input, int ilen);
	void sha1_hmac_finish(sha1_context * ctx, unsigned char output[20]);

#ifdef  __cplusplus
}
#endif /* __cplusplus */
#endif // _CIPHER_PKCS5_PBKDF2_HMAC_H
