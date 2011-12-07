//BF*Light* 
#define BF_ROUNDS   4
#define BF_BLOCK    8
#define BF_LONG unsigned int

#define BF_ENC(LL,R,KEY,Pi) (\
    LL^=KEY[Pi], \
    t=  KEY[BF_ROUNDS+2 +   0 + ((R>>24)&0xFF)], \
    t+= KEY[BF_ROUNDS+2 + 256 + ((R>>16)&0xFF)], \
    t^= KEY[BF_ROUNDS+2 + 512 + ((R>>8 )&0xFF)], \
    t+= KEY[BF_ROUNDS+2 + 768 + ((R    )&0xFF)], \
    LL^=t \
    )


typedef struct bf_key_st {
	BF_LONG P[BF_ROUNDS + 2];
	BF_LONG S[4 * 256];
} BF_KEY;

//Crypt2 (Gameauth and ingame packets)
typedef struct
{	
	BF_KEY key;
	unsigned char md5[0x10]; // MD5
	unsigned char K[0x40]; //Backup of K
}TABULACRYPT2;

void Tabula_CryptInit2(TABULACRYPT2 *tbc2, unsigned char *InputK);
void Tabula_Encrypt2(TABULACRYPT2 *tbc2, unsigned int *PacketData, unsigned int Len);
void Tabula_Decrypt2(TABULACRYPT2 *tbc2, unsigned int *PacketData, unsigned int Len);

/**** MD5 *****/


#ifndef md5_INCLUDED
#  define md5_INCLUDED

/*
 * This package supports both compile-time and run-time determination of CPU
 * byte order.  If ARCH_IS_BIG_ENDIAN is defined as 0, the code will be
 * compiled to run only on little-endian CPUs; if ARCH_IS_BIG_ENDIAN is
 * defined as non-zero, the code will be compiled to run only on big-endian
 * CPUs; if ARCH_IS_BIG_ENDIAN is not defined, the code will be compiled to
 * run on either big- or little-endian CPUs, but will run slightly less
 * efficiently on either one than if ARCH_IS_BIG_ENDIAN is defined.
 */

typedef unsigned char md5_byte_t; /* 8-bit byte */
typedef unsigned int md5_word_t; /* 32-bit word */

/* Define the state of the MD5 Algorithm. */
typedef struct md5_state_s {
    md5_word_t count[2];	/* message length in bits, lsw first */
    md5_word_t abcd[4];		/* digest buffer */
    md5_byte_t buf[64];		/* accumulate block */
} md5_state_t;

#ifdef __cplusplus
extern "C" 
{
#endif

/* Initialize the algorithm. */
void md5_init(md5_state_t *pms);

/* Append a string to the message. */
void md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes);

/* Finish the message and return the digest. */
void md5_finish(md5_state_t *pms, md5_byte_t digest[16]);

#ifdef __cplusplus
}  /* end extern "C" */
#endif

#endif /* md5_INCLUDED */
