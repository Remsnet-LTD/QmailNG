/* digest_sha1.h for QLDAP checkpassword.c */

/*        */
/*  SHA1  */
/*        */

/*
 * SHA-1 in C
 * By Steve Reid <steve@edmweb.com>
 * 100% Public Domain
 */

#ifndef _SHA1_H
#define _SHA1_H

typedef struct {
    u_int32_t state[5];
    u_int32_t count[2];
    unsigned char buffer[64];
} SHA1_CTX;

void SHA1Transform __P((u_int32_t state[5], const unsigned char buffer[64]));
void SHA1Init __P((SHA1_CTX *context));
void SHA1Update __P((SHA1_CTX *context, const unsigned char *data, unsigned int len));
void SHA1Final __P((unsigned char digest[20], SHA1_CTX *context));
char *SHA1End __P((SHA1_CTX *, char *));
char *SHA1File __P((char *, char *));
char *SHA1Data __P((const unsigned char *, size_t, char *));

char *SHA1DataBase64 __P((const unsigned char *, size_t, char *, size_t));

#endif /* _SHA1_H */