/*
 * SHA1 hash algorithm. Used in SSH-2 as a MAC, and the transform is
 * also used as a `stirring' function for the PuTTY random number
 * pool. Implemented directly from the specification by Simon
 * Tatham.
 */
#include <string.h>
#include "sha.h"


/* ----------------------------------------------------------------------
 * Core SHA algorithm: processes 16-word blocks into a message digest.
 */

#define rol(x,y) ( ((x) << (y)) | (((uint32)x) >> (32-y)) )
#define PUT_32BIT_MSB_FIRST(cp, value) ( \
  (cp)[0] = (unsigned char)((value) >> 24), \
  (cp)[1] = (unsigned char)((value) >> 16), \
  (cp)[2] = (unsigned char)((value) >> 8), \
  (cp)[3] = (unsigned char)(value) )

static void SHA_Core_Init(uint32 h[5])
{
    h[0] = 0x67452301;
    h[1] = 0xefcdab89;
    h[2] = 0x98badcfe;
    h[3] = 0x10325476;
    h[4] = 0xc3d2e1f0;
}

void SHATransform(word32 * digest, word32 * block)
{
    word32 w[80];
    word32 a, b, c, d, e;
    int t;

#ifdef RANDOM_DIAGNOSTICS
    {
        extern int random_diagnostics;
        if (random_diagnostics) {
            int i;
            printf("SHATransform:");
            for (i = 0; i < 5; i++)
                printf(" %08x", digest[i]);
            printf(" +");
            for (i = 0; i < 16; i++)
                printf(" %08x", block[i]);
        }
    }
#endif

    for (t = 0; t < 16; t++)
        w[t] = block[t];

    for (t = 16; t < 80; t++) {
        word32 tmp = w[t - 3] ^ w[t - 8] ^ w[t - 14] ^ w[t - 16];
        w[t] = rol(tmp, 1);
    }

    a = digest[0];
    b = digest[1];
    c = digest[2];
    d = digest[3];
    e = digest[4];

    for (t = 0; t < 20; t++) {
        word32 tmp =
            rol(a, 5) + ((b & c) | (d & ~b)) + e + w[t] + 0x5a827999;
        e = d;
        d = c;
        c = rol(b, 30);
        b = a;
        a = tmp;
    }
    for (t = 20; t < 40; t++) {
        word32 tmp = rol(a, 5) + (b ^ c ^ d) + e + w[t] + 0x6ed9eba1;
        e = d;
        d = c;
        c = rol(b, 30);
        b = a;
        a = tmp;
    }
    for (t = 40; t < 60; t++) {
        word32 tmp = rol(a,
                         5) + ((b & c) | (b & d) | (c & d)) + e + w[t] +
            0x8f1bbcdc;
        e = d;
        d = c;
        c = rol(b, 30);
        b = a;
        a = tmp;
    }
    for (t = 60; t < 80; t++) {
        word32 tmp = rol(a, 5) + (b ^ c ^ d) + e + w[t] + 0xca62c1d6;
        e = d;
        d = c;
        c = rol(b, 30);
        b = a;
        a = tmp;
    }

    digest[0] += a;
    digest[1] += b;
    digest[2] += c;
    digest[3] += d;
    digest[4] += e;

#ifdef RANDOM_DIAGNOSTICS
    {
        extern int random_diagnostics;
        if (random_diagnostics) {
            int i;
            printf(" =");
            for (i = 0; i < 5; i++)
                printf(" %08x", digest[i]);
            printf("\n");
        }
    }
#endif
}

/* ----------------------------------------------------------------------
 * Outer SHA algorithm: take an arbitrary length byte string,
 * convert it into 16-word blocks with the prescribed padding at
 * the end, and pass those blocks to the core SHA algorithm.
 */

void SHA_Init(SHA_State * s)
{
    SHA_Core_Init(s->h);
    s->blkused = 0;
    s->lenhi = s->lenlo = 0;
}

void SHA_Bytes(SHA_State * s, const void *p, int len)
{
    const unsigned char *q = (const unsigned char *) p;
    uint32 wordblock[16];
    uint32 lenw = len;
    int i;

    /*
     * Update the length field.
     */
    s->lenlo += lenw;
    s->lenhi += (s->lenlo < lenw);

    if (s->blkused && s->blkused + len < 64) {
        /*
         * Trivial case: just add to the block.
         */
        memcpy(s->block + s->blkused, q, len);
        s->blkused += len;
    } else {
        /*
         * We must complete and process at least one block.
         */
        while (s->blkused + len >= 64) {
            memcpy(s->block + s->blkused, q, 64 - s->blkused);
            q += 64 - s->blkused;
            len -= 64 - s->blkused;
            /* Now process the block. Gather bytes big-endian into words */
            for (i = 0; i < 16; i++) {
                wordblock[i] =
                    (((uint32) s->block[i * 4 + 0]) << 24) |
                    (((uint32) s->block[i * 4 + 1]) << 16) |
                    (((uint32) s->block[i * 4 + 2]) << 8) |
                    (((uint32) s->block[i * 4 + 3]) << 0);
            }
            SHATransform(s->h, wordblock);
            s->blkused = 0;
        }
        memcpy(s->block, q, len);
        s->blkused = len;
    }
}

void SHA_Final(SHA_State * s, unsigned char *output)
{
    int i;
    int pad;
    unsigned char c[64];
    uint32 lenhi, lenlo;

    if (s->blkused >= 56)
        pad = 56 + 64 - s->blkused;
    else
        pad = 56 - s->blkused;

    lenhi = (s->lenhi << 3) | (s->lenlo >> (32 - 3));
    lenlo = (s->lenlo << 3);

    memset(c, 0, pad);
    c[0] = 0x80;
    SHA_Bytes(s, &c, pad);

    c[0] = (lenhi >> 24) & 0xFF;
    c[1] = (lenhi >> 16) & 0xFF;
    c[2] = (lenhi >> 8) & 0xFF;
    c[3] = (lenhi >> 0) & 0xFF;
    c[4] = (lenlo >> 24) & 0xFF;
    c[5] = (lenlo >> 16) & 0xFF;
    c[6] = (lenlo >> 8) & 0xFF;
    c[7] = (lenlo >> 0) & 0xFF;

    SHA_Bytes(s, &c, 8);

    for (i = 0; i < 5; i++) {
        output[i * 4] = (s->h[i] >> 24) & 0xFF;
        output[i * 4 + 1] = (s->h[i] >> 16) & 0xFF;
        output[i * 4 + 2] = (s->h[i] >> 8) & 0xFF;
        output[i * 4 + 3] = (s->h[i]) & 0xFF;
    }
}

void SHA_Simple(const void *p, int len, unsigned char *output)
{
    SHA_State s;

    SHA_Init(&s);
    SHA_Bytes(&s, p, len);
    SHA_Final(&s, output);
}
