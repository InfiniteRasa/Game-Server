//Maximale Bignum größe in Bytes
#define BignumSize 256	//Wert: 0 - 2^65536 
//Bignum Sturktur
typedef struct{
	unsigned char d[BignumSize];
}BIGNUM;


//Funktionsprototypen
int Bignum_Reset(BIGNUM *b);
int Bignum_SetUInt(BIGNUM *a, unsigned int uintval);
int Bignum_Copy(BIGNUM *src, BIGNUM *dest);
int Bignum_Add(BIGNUM *a, BIGNUM *b, BIGNUM *c);
int Bignum_Sub(BIGNUM *result, BIGNUM *b, BIGNUM *c);
int Bignum_Mul(BIGNUM *result, BIGNUM *b, BIGNUM *c);
int Bignum_Div(BIGNUM *result, BIGNUM *a, BIGNUM *b);
int Bignum_Mod(BIGNUM *result, BIGNUM *a, BIGNUM *b);
int Bignum_ModExp(BIGNUM *result, BIGNUM *b, BIGNUM *e, BIGNUM *n);
int Bignum_CountBits(BIGNUM *a);
int Bignum_Half(BIGNUM *a);
int Bignum_Double(BIGNUM *a);
int Bignum_Compare(BIGNUM *a, BIGNUM *b);
int Bignum_IsZero(BIGNUM *a);
void Bignum_Print(BIGNUM *b);

int Bignum_Read(BIGNUM *a, unsigned char *p, int DigitCount);
int Bignum_Read_BigEndian(BIGNUM *a, unsigned char *p, int DigitCount);
int Bignum_Write(BIGNUM *a, unsigned char *p, int DigitCount);
int Bignum_Write_BigEndian(BIGNUM *a, unsigned char *p, int DigitCount);

int Bignum_GetLen(BIGNUM *a);