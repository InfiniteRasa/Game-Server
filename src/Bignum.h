//Maximale Bignum größe in Bytes
#define BignumSize 256	//Wert: 0 - 2^65536 
//Bignum Sturktur
typedef struct{
	uint8 d[BignumSize];
}BIGNUM;


//Funktionsprototypen
sint32 Bignum_Reset(BIGNUM *b);
sint32 Bignum_SetUsint32(BIGNUM *a, uint32 usint32val);
sint32 Bignum_Copy(BIGNUM *src, BIGNUM *dest);
sint32 Bignum_Add(BIGNUM *a, BIGNUM *b, BIGNUM *c);
sint32 Bignum_Sub(BIGNUM *result, BIGNUM *b, BIGNUM *c);
sint32 Bignum_Mul(BIGNUM *result, BIGNUM *b, BIGNUM *c);
sint32 Bignum_Div(BIGNUM *result, BIGNUM *a, BIGNUM *b);
sint32 Bignum_Mod(BIGNUM *result, BIGNUM *a, BIGNUM *b);
sint32 Bignum_ModExp(BIGNUM *result, BIGNUM *b, BIGNUM *e, BIGNUM *n);
sint32 Bignum_CountBits(BIGNUM *a);
sint32 Bignum_Half(BIGNUM *a);
sint32 Bignum_Double(BIGNUM *a);
sint32 Bignum_Compare(BIGNUM *a, BIGNUM *b);
sint32 Bignum_IsZero(BIGNUM *a);
void Bignum_Prsint32(BIGNUM *b);

sint32 Bignum_Read(BIGNUM *a, uint8 *p, sint32 DigitCount);
sint32 Bignum_Read_BigEndian(BIGNUM *a, uint8 *p, sint32 DigitCount);
sint32 Bignum_Write(BIGNUM *a, uint8 *p, sint32 DigitCount);
sint32 Bignum_Write_BigEndian(BIGNUM *a, uint8 *p, sint32 DigitCount);

sint32 Bignum_GetLen(BIGNUM *a);