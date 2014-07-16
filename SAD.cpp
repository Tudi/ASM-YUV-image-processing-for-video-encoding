#include "main.h"

UINT SAD_16x16( const BYTE * address1, const INT stride1, const BYTE * address2,const INT stride2 )
{
	//Hoi Ming x64
	 __declspec(align(16)) __m128i l0, l1, line_sad, acc_sad;

	acc_sad = _mm_setzero_si128();
	//acc_sad = _mm_xor_si128(acc_sad, acc_sad);

	l0 = _mm_loadu_si128((__m128i*)address1);
	l1 = _mm_loadu_si128((__m128i*)address2);
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1+stride1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2+stride2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	address1 += (stride1<<2);
	address2 += (stride2<<2);

	l0 = _mm_loadu_si128((__m128i*)address1);
	l1 = _mm_loadu_si128((__m128i*)address2);
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1+stride1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2+stride2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	address1 += (stride1<<2);
	address2 += (stride2<<2);

	l0 = _mm_loadu_si128((__m128i*)address1);
	l1 = _mm_loadu_si128((__m128i*)address2);
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1+stride1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2+stride2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	address1 += (stride1<<2);
	address2 += (stride2<<2);

	l0 = _mm_loadu_si128((__m128i*)address1);
	l1 = _mm_loadu_si128((__m128i*)address2);
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1+stride1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2+stride2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	unsigned short sad_array[8];
	_mm_storeu_si128((__m128i*)(&sad_array[0]), acc_sad);
	return (sad_array[0]+sad_array[4]);
}

UINT SAD_16x8( const BYTE * __restrict address1, const INT stride1, const BYTE * __restrict address2, const INT stride2 )
{
	//Hoi Ming x64
	unsigned short sad_array[8];
	__m128i l0, l1, line_sad, acc_sad;

	acc_sad = _mm_setzero_si128();
	//acc_sad = _mm_xor_si128(acc_sad, acc_sad);

	l0 = _mm_loadu_si128((__m128i*)address1);
	l1 = _mm_loadu_si128((__m128i*)address2);
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1+stride1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2+stride2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	address1 += (stride1<<2);
	address2 += (stride2<<2);

	l0 = _mm_loadu_si128((__m128i*)address1);
	l1 = _mm_loadu_si128((__m128i*)address2);
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadu_si128((__m128i*)(address1+stride1+stride1+stride1));
	l1 = _mm_loadu_si128((__m128i*)(address2+stride2+stride2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	_mm_storeu_si128((__m128i*)(&sad_array[0]), acc_sad);

	return (sad_array[0]+sad_array[4]);
}

