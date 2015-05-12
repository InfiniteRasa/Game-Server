#include<Windows.h>
#include<stdio.h>

#include"framework.h"
#include"Bignum.h"

//**************************Bignumfunktionen**************************

//Setzt einen Bignum auf 0
sint32 Bignum_Reset(BIGNUM *b)
{
	sint32 i;
	//Setze alle Ziffern auf 0
	for(i=0;i<BignumSize;i++)
		b->d[i]=0;
	//Return Erfolg
	return 1;
}

//Setzt einen Bignum auf einen uint32eger Wert
sint32 Bignum_SetUsint32(BIGNUM *a, uint32 usint32val)
{
	sint32 i;

	*(uint32 *)&a->d[0] = usint32val;
	//Setze alle restlichen Ziffern auf 0
	for(i=4;i<BignumSize;i++)
		a->d[i]=0;
	//Return erfolg
	return 1;
}

//Kopiert einen Bignum (src = dest)
sint32 Bignum_Copy(BIGNUM *src, BIGNUM *dest)
{
	sint32 i;
	//Kopiere Ziffern
	for(i=0;i<BignumSize;i++)
		dest->d[i] = src->d[i];
	//Return erfolg
	return 1;
}

//Addiert zwei Bignums (A = B + C), gibt den Überlauf zurück
sint32 Bignum_Add(BIGNUM *a, BIGNUM *b, BIGNUM *c)
{
	uint32 overflow;
	sint32 i;
	//Setze overflow Wert auf 0
	overflow = 0;
	for(i=0;i<BignumSize;i++)
	{
		//Addiere zu overflow die beiden Bignum Ziffern
		overflow += (uint32)b->d[i] + (uint32)c->d[i];
		//Speichere aktuellen Ziffernwert
		a->d[i] = (uint8)(overflow&0xFF);
		//Rücke overflow Wert
		overflow = overflow >> 8;
	}
	return overflow;
}

//Subtrahiert zwei Bignums (Result = B - C), gibt den Überlauf zurück
sint32 Bignum_Sub(BIGNUM *result, BIGNUM *b, BIGNUM *c)
{
	uint32 overflow;
	sint32 i;
	//Setze overflow Wert auf 1
	overflow = 1; //Invertierter Wert für Subtraktion
	for(i=0;i<BignumSize;i++)
	{
		//Addiere zu overflow die beiden Bignum Ziffern b + ~d
		overflow += (uint32)b->d[i] + (uint32)((~c->d[i])&0xFF);
		//Speichere aktuellen Ziffernwert
		result->d[i] = (uint8)(overflow&0xFF);
		//Rücke overflow Wert
		overflow = overflow >> 8;
	}
	return overflow;
}

//Multipliziert zwei Bignums (result = a * b)
sint32 Bignum_Mul(BIGNUM *result, BIGNUM *a, BIGNUM *b)
{
	//Zwischenspeicher für Ziffern-Multiplikationsergebniss
	BIGNUM storer;
	BIGNUM resultnew;

	BIGNUM o1,o2;

	Bignum_Copy(a,&o1);
	Bignum_Copy(b,&o2);

	//Ziffernwertspeicher
	uint32 overflow;
	sint32 i;
	sint32 i2;

	//Setze result auf 0
	Bignum_Reset(&resultnew);

	for(i=0;i<BignumSize;i++)
	{
		//Setze storer und overflow auf 0
		Bignum_Reset(&storer);
		overflow = 0;
		//Berechne Ziffernmultiplikation
		for(i2=0;i2<(BignumSize-i);i2++)
		{
			//Multipliziere Ziffern
			overflow += (o1.d[i] * o2.d[i2]);
			//Speichere Ziffer
			storer.d[i+i2] = (uint8)(overflow & 0xFF);
			//Schiebe Ziffern
			overflow = overflow >> 8;
		}
		//Addiere Ziffernprodukt zu result
		Bignum_Add(&resultnew, &resultnew, &storer);
	}
	//Kopiere Ergebniss
	Bignum_Copy(&resultnew, result);
	//Return erfolg
	return 1;
}

//Dividiert einen Bignum (result = a / c)
sint32 Bignum_Div(BIGNUM *result, BIGNUM *a, BIGNUM *b)
{
	BIGNUM o1,o2;
	sint32 ShiftCount;
	Bignum_Reset(result);
	Bignum_Copy(a,&o1);
	Bignum_Copy(b,&o2);

	//Fange falsche Werte ab
	if( Bignum_IsZero(&o1) || Bignum_IsZero(&o2) )
		return 0;

	//Suche maximum
	ShiftCount = 0;
	while( Bignum_Compare(&o1,&o2)>=0 && (o2.d[BignumSize-1]&0x80)==0 )
	{
		ShiftCount++;
		Bignum_Double(&o2);
	}
	//Mache den letzten Verdoppler Rückgängig
	Bignum_Half(&o2);
	//Solange b ist nicht Null
	while(ShiftCount>0)
	{
		//Rücke result
		Bignum_Double(result);
		//Passt b in a?
		if( Bignum_Compare(&o1,&o2)>=0 ) // (a-b)>=0
		{
			//Subtrahiere von a
			Bignum_Sub(&o1,&o1,&o2);
			//Setze Result
			result->d[0] |= 1;
		}
		//Halbiere b
		Bignum_Half(&o2);
		//Zähle shiftanzahl
		ShiftCount--;
	};
	//Return erfolg
	return 1;
}

//Berechnet den Modulo einer Bignumdivision (result = A%B)
sint32 Bignum_Mod(BIGNUM *result, BIGNUM *a, BIGNUM *b)
{
	BIGNUM factor;
	//Runde Factor auf a/b*b
	Bignum_Div(&factor, a, b);
	Bignum_Mul(&factor, &factor, b);
	//Berechne Rest
	Bignum_Sub(result,a,&factor);
	//Return Erfolg
	return 1;
}

//Modulare Exponentiation (result = (B^E)%N)
sint32 Bignum_ModExp(BIGNUM *result, BIGNUM *b, BIGNUM *e, BIGNUM *n)
{
	BIGNUM p;
	BIGNUM exponent;

	sint32 BitCount = Bignum_CountBits(e);
	//Setze p auf 1
	Bignum_Reset(&p);
	p.d[0] = 0x01;
	//Kopiere Exponent
	Bignum_Copy(e, &exponent);
	//Beginne schleife
	while( BitCount>=0 )
	{
		Bignum_Mul(&p,&p,&p);
		Bignum_Mod(&p,&p,n);
		//Falls ungerade multiplizierte mit Basis
		if( (exponent.d[BitCount/8]&(1<<(BitCount%8))) )
		{
			Bignum_Mul(&p,&p,b);
			Bignum_Mod(&p,&p,n);
		}
		//Halbiere exponent
		//Bignum_Half(&exponent);
		BitCount--;
	}
	//Kopiere Ergebniss
	Bignum_Copy(&p, result);
	//Return erfolg
	return 1;
}

//Gibt einen Bignum hexadezimal aus
void Bignum_Prsint32(BIGNUM *b)
{
	sint8 HexDigit[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	sint8 Output[(2*BignumSize)+2] = {0};
	sint32 i;

	memset((void*)Output, 0x00, (2)*BignumSize+2);
	for(i=0;i<BignumSize;i++)
	{
		Output[(BignumSize-1)*2-(i*2)+1] = HexDigit[b->d[i] & 0x0F];
		Output[(BignumSize-1)*2-(i*2)] = HexDigit[(b->d[i] & 0xF0)>>4];
	}
	Output[2*BignumSize] = '\0';

	puts(Output);
	
}

//Halbiert einen Bignum (a = a / 2)
sint32 Bignum_Half(BIGNUM *a)
{
	uint8 shiftbit;
	uint8 newvalue;
	sint32 i;
	//Shiftbit ist 0
	shiftbit = 0;
	for(i=BignumSize-1;i>=0;i--)
	{
		//Errechne neuen Ziffernwert
		newvalue = (a->d[i]>>1) | (shiftbit<<7);
		//Merke shiftbit
		shiftbit = (a->d[i] & 0x01);
		//Setze neuen Ziffernwert
		a->d[i] = newvalue;
	}
	//Return erfolg
	return 1;
}

//Verdoppelt einen Bignum (a = a * 2)
sint32 Bignum_Double(BIGNUM *a)
{
	uint8 shiftbit;
	uint8 newvalue;
	sint32 i;
	//Shiftbit ist 0
	shiftbit = 0;
	for(i=0;i<BignumSize;i++)
	{
		//Errechne neuen Ziffernwert
		newvalue = (a->d[i]<<1) | (shiftbit>>7);
		//Merke shiftbit
		shiftbit = (a->d[i] & 0x80);
		//Setze neuen Ziffernwert
		a->d[i] = newvalue;
	}
	//Return erfolg
	return 1;
}

//Bignumvergleiche

//Vergleicht zwei Bignums (-1,0,1)
sint32 Bignum_Compare(BIGNUM *a, BIGNUM *b)
{
	uint32 vergleich;
	sint32 i;
	for(i=(BignumSize-1);i>=0;i--)
	{
		vergleich = a->d[i] - b->d[i];
		//Falls unterschiedlich, gib Unterschied zurück
		if( vergleich )
			return vergleich;
	}
	//Return gleich
	return 0;
}

//Vergleicht einen Bignum mit 0
sint32 Bignum_IsZero(BIGNUM *a)
{
	sint32 i;
	//Für alle Ziffern
	for(i=0;i<BignumSize;i++)
		if( a->d[i] )
			return 0; //Return false falls Ziffer ungleich 0
	//Return true
	return 1;
}

//Gibt die Stelle des ersten Bits zurück(Bsp: 0x100 wäre 8)
sint32 Bignum_CountBits(BIGNUM *a)
{
	sint32 i;
	
	for(i=(BignumSize-1)*8;i>=0;i--)
		if( a->d[i/8]&(1<<(i%8)) )
			return i;
	return 0;
}

//Liest einen Bignum aus dem Speicher
sint32 Bignum_Read(BIGNUM *a, uint8 *p, sint32 DigitCount)
{
	sint32 i;
	Bignum_Reset(a);

	for(i=0;i<DigitCount;i++)
	{
		a->d[i] = *p;
		p++;
	}
	//Return Erfolg
	return 1;
}

//Liest einen Bignum aus dem Speicher im Big Endian Format.
sint32 Bignum_Read_BigEndian(BIGNUM *a, uint8 *p, sint32 DigitCount)
{
	sint32 i;
	Bignum_Reset(a);

	for(i=0;i<DigitCount;i++)
	{
		a->d[DigitCount-i-1] = *p;
		p++;
	}
	//Return Erfolg
	return 1;
}

//Schreibt einen Bignum in den Speicher
sint32 Bignum_Write(BIGNUM *a, uint8 *p, sint32 DigitCount)
{
	sint32 i;

	memset((void*)p, 0x00, DigitCount);

	for(i=0;i<DigitCount;i++)
	{
		*p = a->d[i];
		p++;
	}
	//Return Erfolg
	return 1;
}

//Schreibt einen Bignum in den Speicher im Big Endian Format
//Achtung: Funktioniert nicht ganz fehlerfrei wenn DigitCount > Eigentliche Anzahl der Stellen
sint32 Bignum_Write_BigEndian(BIGNUM *a, uint8 *p, sint32 DigitCount)
{
	sint32 i;
	memset((void*)p, 0x00, DigitCount);

	for(i=0;i<DigitCount;i++)
	{
		*p = a->d[DigitCount-i-1];
		p++;
	}
	//Return Erfolg
	return 1;
}

//Gibt die Länge eines Bignum in Byte-Stellen zurück
sint32 Bignum_GetLen(BIGNUM *a)
{
	sint32 i;
	
	for(i=(BignumSize-1);i>=0;i--)
		if( a->d[i] )
			return i+1;
	return 0;
}