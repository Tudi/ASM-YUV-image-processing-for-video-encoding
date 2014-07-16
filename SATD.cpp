#include <emmintrin.h>
#include "dep_libCPU/libcpuid.h"

#ifndef abs
	#define abs(a) a>=0?a:-a
#endif 

extern "C" unsigned int satd_4x4_sse3_YASM( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 );
extern "C" unsigned int satd_4x4_sse4_YASM( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 );

unsigned char CPU_HAS_SSE4_1()
{
	struct cpu_raw_data_t raw;
	struct cpu_id_t id;
	if (cpuid_get_raw_data(&raw) == 0 && cpu_identify(&raw, &id) == 0) 
	{
		if (id.flags[CPU_FEATURE_SSE4_1]) 
			return 1;
	} 
	return 0;
}

unsigned char CPU_HAS_SSSE3()
{
	struct cpu_raw_data_t raw;
	struct cpu_id_t id;
	if (cpuid_get_raw_data(&raw) == 0 && cpu_identify(&raw, &id) == 0) 
	{
		if (id.flags[CPU_FEATURE_SSSE3]) 
			return 1;
	} 
	return 0;
}
/*
unsigned char CPU_HAS_SSE3()
{
	struct cpu_raw_data_t raw;
	struct cpu_id_t id;
	if (cpuid_get_raw_data(&raw) == 0 && cpu_identify(&raw, &id) == 0) 
	{
		if (id.flags[CPU_FEATURE_PNI]) 
			return 1;
	} 
	return 0;
}*/

unsigned char CPU_HAS_SSE2()
{
	struct cpu_raw_data_t raw;
	struct cpu_id_t id;
	if (cpuid_get_raw_data(&raw) == 0 && cpu_identify(&raw, &id) == 0) 
	{
		if (id.flags[CPU_FEATURE_SSE2]) 
			return 1;
	} 
	return 0;
}
unsigned int satd_16x16_sse3_YASM_4x4( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 )
{
	unsigned int res;
	res = 0;
	for(int y=0;y<16;y+=4)
		for(int x=0;x<16;x+=4)
			res += satd_4x4_sse3_YASM( address1 + x + y * stride1, stride1, address2 + x + y * stride2, stride2 );
	return res;
}

unsigned int satd_16x16_sse4_YASM_4x4( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 )
{
	unsigned int res;
	res = 0;
	for(int y=0;y<16;y+=4)
		for(int x=0;x<16;x+=4)
			res += satd_4x4_sse4_YASM( address1 + x + y * stride1, stride1, address2 + x + y * stride2, stride2 );
	return res;
}

unsigned int satd_8x8_sse3_YASM_4x4( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 )
{
	unsigned int res;
	res = satd_4x4_sse3_YASM( address1, stride1, address2, stride2 );
	res += satd_4x4_sse3_YASM( address1 + 4, stride1, address2 + 4, stride2 );
	res += satd_4x4_sse3_YASM( address1 + stride1*4, stride1, address2 + stride2*4, stride2 );
	res += satd_4x4_sse3_YASM( address1 + 4 + stride1*4, stride1, address2 + 4 + stride2*4, stride2 );
	return res;
}

unsigned int satd_8x8_sse4_YASM_4x4( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 )
{
	unsigned int res;
	res = satd_4x4_sse4_YASM( address1, stride1, address2, stride2 );
	res += satd_4x4_sse4_YASM( address1 + 4, stride1, address2 + 4, stride2 );
	res += satd_4x4_sse4_YASM( address1 + stride1*4, stride1, address2 + stride2*4, stride2 );
	res += satd_4x4_sse4_YASM( address1 + 4 + stride1*4, stride1, address2 + 4 + stride2*4, stride2 );
	return res;
}

unsigned int SATD_16x16_SSE2_Intrinsic( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 )
{
	const unsigned char *pSrc1;
	int src1Step;
	const unsigned char *pSrc2;
	int src2Step;

	pSrc1 = address1;
	pSrc2 = address2;
	src1Step = stride1;
	src2Step = stride2;
		 
	int x, y;
    unsigned int satd;
    __declspec(align(16)) __m128i  _p_0, _p_1, _p_2, _p_3, _p_4, _p_5, _p_7, _p_zero;
    const unsigned char *pS1, *pS2;
    __declspec(align(16)) __m128i  _r_0, _r_1, _r_2, _r_3, _r_5, _r_7, _p_satd;
    static __declspec(align(16)) short _p_one[8] = {1, 1, 1, 1, 1, 1, 1, 1};
    _p_zero = _mm_setzero_si128();
    _p_satd = _mm_setzero_si128();
    for (y = 0; y < 16; y += 4) {
        for (x = 0; x < 16; x += 8) {
            pS1 = pSrc1 + x;
            pS2 = pSrc2 + x;
            _p_0 = _mm_loadl_epi64((__m128i*)(pS1));
            _p_4 = _mm_loadl_epi64((__m128i*)(pS2));
            _p_1 = _mm_loadl_epi64((__m128i*)(pS1+src1Step));
            _p_5 = _mm_loadl_epi64((__m128i*)(pS2+src2Step));
            _p_0 = _mm_unpacklo_epi8(_p_0, _p_zero);
            _p_4 = _mm_unpacklo_epi8(_p_4, _p_zero);
            _p_1 = _mm_unpacklo_epi8(_p_1, _p_zero);
            _p_5 = _mm_unpacklo_epi8(_p_5, _p_zero);
            _p_0 = _mm_sub_epi16(_p_0, _p_4);
            _p_1 = _mm_sub_epi16(_p_1, _p_5);
            pS1 += 2 * src1Step;
            pS2 += 2 * src2Step;
            _p_2 = _mm_loadl_epi64((__m128i*)(pS1));
            _p_4 = _mm_loadl_epi64((__m128i*)(pS2));
            _p_3 = _mm_loadl_epi64((__m128i*)(pS1+src1Step));
            _p_5 = _mm_loadl_epi64((__m128i*)(pS2+src2Step));
            _p_2 = _mm_unpacklo_epi8(_p_2, _p_zero);
            _p_4 = _mm_unpacklo_epi8(_p_4, _p_zero);
            _p_3 = _mm_unpacklo_epi8(_p_3, _p_zero);
            _p_5 = _mm_unpacklo_epi8(_p_5, _p_zero);
            _p_2 = _mm_sub_epi16(_p_2, _p_4);
            _p_3 = _mm_sub_epi16(_p_3, _p_5);

            _p_5 = _mm_sub_epi16(_p_0, _p_1);
            _p_0 = _mm_add_epi16(_p_0, _p_1);

            _p_7 = _mm_sub_epi16(_p_2, _p_3);
            _p_2 = _mm_add_epi16(_p_2, _p_3);

            _p_1 = _mm_sub_epi16(_p_0, _p_2);
            _p_0 = _mm_add_epi16(_p_0, _p_2);

            _p_3 = _mm_add_epi16(_p_5, _p_7);
            _p_5 = _mm_sub_epi16(_p_5, _p_7);

            _r_0 = _mm_unpackhi_epi16(_p_0, _p_1);
            _r_5 = _mm_unpackhi_epi16(_p_5, _p_3);

            _p_0 = _mm_unpacklo_epi16(_p_0, _p_1);
            _p_5 = _mm_unpacklo_epi16(_p_5, _p_3);
            _p_7 = _mm_unpackhi_epi32(_p_0, _p_5);
            _p_0 = _mm_unpacklo_epi32(_p_0, _p_5);
            _p_1 = _mm_srli_si128(_p_0, 8);
            _p_3 = _mm_srli_si128(_p_7, 8);
            _p_5 = _mm_sub_epi16(_p_0, _p_1);
            _p_0 = _mm_add_epi16(_p_0, _p_1);
            _p_2 = _mm_sub_epi16(_p_7, _p_3);
            _p_7 = _mm_add_epi16(_p_7, _p_3);
            _p_1 = _mm_sub_epi16(_p_0, _p_7);
            _p_0 = _mm_add_epi16(_p_0, _p_7);
            _p_3 = _mm_add_epi16(_p_5, _p_2);
            _p_5 = _mm_sub_epi16(_p_5, _p_2);
            _p_0 = _mm_unpacklo_epi16(_p_0, _p_1);
            _p_5 = _mm_unpacklo_epi16(_p_5, _p_3);
            _p_2 = _mm_unpackhi_epi32(_p_0, _p_5);
            _p_0 = _mm_unpacklo_epi32(_p_0, _p_5);
            _p_3 = _mm_srai_epi16(_p_2, 15);
            _p_1 = _mm_srai_epi16(_p_0, 15);
            _p_2 = _mm_xor_si128(_p_2, _p_3);
            _p_0 = _mm_xor_si128(_p_0, _p_1);
            _p_2 = _mm_sub_epi16(_p_2, _p_3);
            _p_0 = _mm_sub_epi16(_p_0, _p_1);
            _p_0 = _mm_add_epi16(_p_0, _p_2);
            _p_0 = _mm_madd_epi16(_p_0, *(__m128i*)_p_one);
            _p_satd = _mm_add_epi32(_p_satd, _p_0);

            _r_7 = _mm_unpackhi_epi32(_r_0, _r_5);
            _r_0 = _mm_unpacklo_epi32(_r_0, _r_5);
            _r_1 = _mm_srli_si128(_r_0, 8);
            _r_3 = _mm_srli_si128(_r_7, 8);
            _r_5 = _mm_sub_epi16(_r_0, _r_1);
            _r_0 = _mm_add_epi16(_r_0, _r_1);
            _r_2 = _mm_sub_epi16(_r_7, _r_3);
            _r_7 = _mm_add_epi16(_r_7, _r_3);
            _r_1 = _mm_sub_epi16(_r_0, _r_7);
            _r_0 = _mm_add_epi16(_r_0, _r_7);
            _r_3 = _mm_add_epi16(_r_5, _r_2);
            _r_5 = _mm_sub_epi16(_r_5, _r_2);
            _r_0 = _mm_unpacklo_epi16(_r_0, _r_1);
            _r_5 = _mm_unpacklo_epi16(_r_5, _r_3);
            _r_2 = _mm_unpackhi_epi32(_r_0, _r_5);
            _r_0 = _mm_unpacklo_epi32(_r_0, _r_5);
            _r_3 = _mm_srai_epi16(_r_2, 15);
            _r_1 = _mm_srai_epi16(_r_0, 15);
            _r_2 = _mm_xor_si128(_r_2, _r_3);
            _r_0 = _mm_xor_si128(_r_0, _r_1);
            _r_2 = _mm_sub_epi16(_r_2, _r_3);
            _r_0 = _mm_sub_epi16(_r_0, _r_1);
            _r_0 = _mm_add_epi16(_r_0, _r_2);
            _r_0 = _mm_madd_epi16(_r_0, *(__m128i*)_p_one);
            _p_satd = _mm_add_epi32(_p_satd, _r_0);
        }
        pSrc1 += 4 * src1Step;
        pSrc2 += 4 * src2Step;
    }
    _p_1 = _mm_srli_si128(_p_satd, 8);
    _p_satd = _mm_add_epi32(_p_satd, _p_1);
    _p_1 = _mm_srli_si128(_p_satd, 4);
    _p_satd = _mm_add_epi32(_p_satd, _p_1);
    satd = _mm_cvtsi128_si32(_p_satd);

//	printf("%d\n", (int)satd);

    return (satd>>1);
}

unsigned int SATD_4x4_Intrinsic( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 )
{
	const unsigned char *pSrc1;
	int src1Step;
	const unsigned char *pSrc2;
	int src2Step;

	pSrc1 = address1;
	pSrc2 = address2;
	src1Step = stride1;
	src2Step = stride2;

	__declspec(align(16)) __m128i  _p_0, _p_1, _p_2, _p_3, _p_4, _p_5, _p_7, _p_zero;
    const unsigned char *pS1, *pS2;
    int s;
    unsigned int satd;

    pS1 = pSrc1;
    pS2 = pSrc2;
    _p_zero = _mm_setzero_si128();
    _p_0 = _mm_cvtsi32_si128(*(int*)(pS1));
    _p_4 = _mm_cvtsi32_si128(*(int*)(pS2));
    _p_1 = _mm_cvtsi32_si128(*(int*)(pS1+src1Step));
    _p_5 = _mm_cvtsi32_si128(*(int*)(pS2+src2Step));
    _p_0 = _mm_unpacklo_epi8(_p_0, _p_zero);
    _p_4 = _mm_unpacklo_epi8(_p_4, _p_zero);
    _p_1 = _mm_unpacklo_epi8(_p_1, _p_zero);
    _p_5 = _mm_unpacklo_epi8(_p_5, _p_zero);
    _p_0 = _mm_sub_epi16(_p_0, _p_4);
    _p_1 = _mm_sub_epi16(_p_1, _p_5);
    pS1 += 2 * src1Step;
    pS2 += 2 * src2Step;
    _p_2 = _mm_cvtsi32_si128(*(int*)(pS1));
    _p_4 = _mm_cvtsi32_si128(*(int*)(pS2));
    _p_3 = _mm_cvtsi32_si128(*(int*)(pS1+src1Step));
    _p_5 = _mm_cvtsi32_si128(*(int*)(pS2+src2Step));
    _p_2 = _mm_unpacklo_epi8(_p_2, _p_zero);
    _p_4 = _mm_unpacklo_epi8(_p_4, _p_zero);
    _p_3 = _mm_unpacklo_epi8(_p_3, _p_zero);
    _p_5 = _mm_unpacklo_epi8(_p_5, _p_zero);
    _p_2 = _mm_sub_epi16(_p_2, _p_4);
    _p_3 = _mm_sub_epi16(_p_3, _p_5);

	//do calc on columns
    _p_5 = _mm_subs_epi16(_p_0, _p_1);
    _p_0 = _mm_adds_epi16(_p_0, _p_1);
    _p_7 = _mm_subs_epi16(_p_2, _p_3);
    _p_2 = _mm_adds_epi16(_p_2, _p_3);
    _p_1 = _mm_subs_epi16(_p_0, _p_2);
    _p_0 = _mm_adds_epi16(_p_0, _p_2);
    _p_3 = _mm_adds_epi16(_p_5, _p_7);
    _p_5 = _mm_subs_epi16(_p_5, _p_7);

	//swap rows to columns
    _p_0 = _mm_unpacklo_epi16(_p_0, _p_1);
    _p_5 = _mm_unpacklo_epi16(_p_5, _p_3);
    _p_7 = _mm_unpackhi_epi32(_p_0, _p_5);
    _p_0 = _mm_unpacklo_epi32(_p_0, _p_5);
    _p_1 = _mm_srli_si128(_p_0, 8);
    _p_3 = _mm_srli_si128(_p_7, 8);

	//do calc on columns
    _p_5 = _mm_subs_epi16(_p_0, _p_1);
    _p_0 = _mm_adds_epi16(_p_0, _p_1);
    _p_2 = _mm_subs_epi16(_p_7, _p_3);
    _p_7 = _mm_adds_epi16(_p_7, _p_3);
    _p_1 = _mm_subs_epi16(_p_0, _p_7);
    _p_0 = _mm_adds_epi16(_p_0, _p_7);
    _p_3 = _mm_adds_epi16(_p_5, _p_2);
    _p_5 = _mm_subs_epi16(_p_5, _p_2);

	//swap rows to columns
	_p_0 = _mm_unpacklo_epi16(_p_0, _p_1);
    _p_5 = _mm_unpacklo_epi16(_p_5, _p_3);
    _p_2 = _mm_unpackhi_epi32(_p_0, _p_5);
    _p_0 = _mm_unpacklo_epi32(_p_0, _p_5);
    _p_3 = _mm_srai_epi16(_p_2, 15);
    _p_1 = _mm_srai_epi16(_p_0, 15);

	//abs the values
    _p_2 = _mm_xor_si128(_p_2, _p_3);
    _p_0 = _mm_xor_si128(_p_0, _p_1);
    _p_2 = _mm_sub_epi16(_p_2, _p_3);
    _p_0 = _mm_sub_epi16(_p_0, _p_1);
    _p_0 = _mm_add_epi16(_p_0, _p_2);
    _p_2 = _mm_srli_si128(_p_0, 8);
    _p_0 = _mm_add_epi16(_p_0, _p_2);
    _p_2 = _mm_srli_si128(_p_0, 4);
    _p_0 = _mm_add_epi16(_p_0, _p_2);
    s = _mm_cvtsi128_si32(_p_0);
    satd = (s >> 16) + (short)s;

    return satd >> 1;
}

unsigned int SATD_4x4_CACHED( unsigned char *address1, unsigned char *address2 )
{
	const unsigned char *pSrc1;
	const unsigned char *pSrc2;

	pSrc1 = address1;
	pSrc2 = address2;

	__declspec(align(16)) __m128i  _p_0, _p_1, _p_2, _p_3, _p_4, _p_5, _p_7, _p_zero;
    const unsigned char *pS1, *pS2;
    int s;
    unsigned int satd;

    pS1 = pSrc1;
    pS2 = pSrc2;
    _p_zero = _mm_setzero_si128();
    _p_0 = _mm_cvtsi32_si128(*(int*)(pS1));
    _p_4 = _mm_cvtsi32_si128(*(int*)(pS2));
    _p_1 = _mm_cvtsi32_si128(*(int*)(pS1+16));
    _p_5 = _mm_cvtsi32_si128(*(int*)(pS2+16));
    _p_0 = _mm_unpacklo_epi8(_p_0, _p_zero);
    _p_4 = _mm_unpacklo_epi8(_p_4, _p_zero);
    _p_1 = _mm_unpacklo_epi8(_p_1, _p_zero);
    _p_5 = _mm_unpacklo_epi8(_p_5, _p_zero);
    _p_0 = _mm_sub_epi16(_p_0, _p_4);
    _p_1 = _mm_sub_epi16(_p_1, _p_5);
    _p_2 = _mm_cvtsi32_si128(*(int*)(pS1+32));
    _p_4 = _mm_cvtsi32_si128(*(int*)(pS2+32));
    _p_3 = _mm_cvtsi32_si128(*(int*)(pS1+48));
    _p_5 = _mm_cvtsi32_si128(*(int*)(pS2+48));
    _p_2 = _mm_unpacklo_epi8(_p_2, _p_zero);
    _p_4 = _mm_unpacklo_epi8(_p_4, _p_zero);
    _p_3 = _mm_unpacklo_epi8(_p_3, _p_zero);
    _p_5 = _mm_unpacklo_epi8(_p_5, _p_zero);
    _p_2 = _mm_sub_epi16(_p_2, _p_4);
    _p_3 = _mm_sub_epi16(_p_3, _p_5);

	//do calc on columns
    _p_5 = _mm_subs_epi16(_p_0, _p_1);
    _p_0 = _mm_adds_epi16(_p_0, _p_1);
    _p_7 = _mm_subs_epi16(_p_2, _p_3);
    _p_2 = _mm_adds_epi16(_p_2, _p_3);
    _p_1 = _mm_subs_epi16(_p_0, _p_2);
    _p_0 = _mm_adds_epi16(_p_0, _p_2);
    _p_3 = _mm_adds_epi16(_p_5, _p_7);
    _p_5 = _mm_subs_epi16(_p_5, _p_7);

	//swap rows to columns
    _p_0 = _mm_unpacklo_epi16(_p_0, _p_1);
    _p_5 = _mm_unpacklo_epi16(_p_5, _p_3);
    _p_7 = _mm_unpackhi_epi32(_p_0, _p_5);
    _p_0 = _mm_unpacklo_epi32(_p_0, _p_5);
    _p_1 = _mm_srli_si128(_p_0, 8);
    _p_3 = _mm_srli_si128(_p_7, 8);

	//do calc on columns
    _p_5 = _mm_subs_epi16(_p_0, _p_1);
    _p_0 = _mm_adds_epi16(_p_0, _p_1);
    _p_2 = _mm_subs_epi16(_p_7, _p_3);
    _p_7 = _mm_adds_epi16(_p_7, _p_3);
    _p_1 = _mm_subs_epi16(_p_0, _p_7);
    _p_0 = _mm_adds_epi16(_p_0, _p_7);
    _p_3 = _mm_adds_epi16(_p_5, _p_2);
    _p_5 = _mm_subs_epi16(_p_5, _p_2);

	//swap rows to columns
	_p_0 = _mm_unpacklo_epi16(_p_0, _p_1);
    _p_5 = _mm_unpacklo_epi16(_p_5, _p_3);
    _p_2 = _mm_unpackhi_epi32(_p_0, _p_5);
    _p_0 = _mm_unpacklo_epi32(_p_0, _p_5);
    _p_3 = _mm_srai_epi16(_p_2, 15);
    _p_1 = _mm_srai_epi16(_p_0, 15);

	//abs the values
    _p_2 = _mm_xor_si128(_p_2, _p_3);
    _p_0 = _mm_xor_si128(_p_0, _p_1);
    _p_2 = _mm_sub_epi16(_p_2, _p_3);
    _p_0 = _mm_sub_epi16(_p_0, _p_1);
    _p_0 = _mm_add_epi16(_p_0, _p_2);
    _p_2 = _mm_srli_si128(_p_0, 8);
    _p_0 = _mm_add_epi16(_p_0, _p_2);
    _p_2 = _mm_srli_si128(_p_0, 4);
    _p_0 = _mm_add_epi16(_p_0, _p_2);
    s = _mm_cvtsi128_si32(_p_0);
    satd = (s >> 16) + (short)s;

    return satd >> 1;
}

unsigned int SATD16x16_C(unsigned char *pSrc1, unsigned int src1Step, unsigned char *pSrc2, unsigned int src2Step)
{
    signed short tmpBuff[4][4];
    signed short diffBuff[4][4];
    signed int x, y;
    unsigned int satd = 0;

    for (y = 0; y < 16; y += 4) 
	{
        for (x = 0; x < 16; x += 4)
		{
            signed int b;
			for(int ty=0;ty<4;ty++)
				for(int tx=0;tx<4;tx++)
					diffBuff[ty][tx] = (int)(pSrc1[x+ty*src1Step + tx])-(int)(pSrc2[x+ty*src2Step + tx]);
            for (b = 0; b < 4; b ++) 
			{
                signed int a01, a23, b01, b23;

                a01 = diffBuff[b][0] + diffBuff[b][1];
                a23 = diffBuff[b][2] + diffBuff[b][3];
                b01 = diffBuff[b][0] - diffBuff[b][1];
                b23 = diffBuff[b][2] - diffBuff[b][3];
                tmpBuff[b][0] = a01 + a23;
                tmpBuff[b][1] = a01 - a23;
                tmpBuff[b][2] = b01 - b23;
                tmpBuff[b][3] = b01 + b23;
            }
            for (b = 0; b < 4; b ++)
			{
                signed int a01, a23, b01, b23;

                a01 = tmpBuff[0][b] + tmpBuff[1][b];
                a23 = tmpBuff[2][b] + tmpBuff[3][b];
                b01 = tmpBuff[0][b] - tmpBuff[1][b];
                b23 = tmpBuff[2][b] - tmpBuff[3][b];
                satd += abs(a01 + a23) + abs(a01 - a23) + abs(b01 - b23) + abs(b01 + b23);
            }
        }
        pSrc1 += 4 * src1Step;
        pSrc2 += 4 * src2Step;
    }
	return satd >> 1;
}

unsigned int SATD4x4_INTEL_Intrinsic(unsigned char *pSrc1, unsigned int src1Step, unsigned char *pSrc2, unsigned int src2Step)
{
    __declspec(align(16)) __m128i  _p_0, _p_1, _p_2, _p_3, _p_4, _p_5, _p_7, _p_zero;
    const unsigned char *pS1, *pS2;
    signed int s;
    unsigned int satd;

    pS1 = pSrc1;
    pS2 = pSrc2;
    _p_zero = _mm_setzero_si128();
    _p_0 = _mm_cvtsi32_si128(*(int*)(pS1));
    _p_4 = _mm_cvtsi32_si128(*(int*)(pS2));
    _p_1 = _mm_cvtsi32_si128(*(int*)(pS1+src1Step));
    _p_5 = _mm_cvtsi32_si128(*(int*)(pS2+src2Step));
    _p_0 = _mm_unpacklo_epi8(_p_0, _p_zero);
    _p_4 = _mm_unpacklo_epi8(_p_4, _p_zero);
    _p_1 = _mm_unpacklo_epi8(_p_1, _p_zero);
    _p_5 = _mm_unpacklo_epi8(_p_5, _p_zero);
    _p_0 = _mm_sub_epi16(_p_0, _p_4);
    _p_1 = _mm_sub_epi16(_p_1, _p_5);
    pS1 += 2 * src1Step;
    pS2 += 2 * src2Step;
    _p_2 = _mm_cvtsi32_si128(*(int*)(pS1));
    _p_4 = _mm_cvtsi32_si128(*(int*)(pS2));
    _p_3 = _mm_cvtsi32_si128(*(int*)(pS1+src1Step));
    _p_5 = _mm_cvtsi32_si128(*(int*)(pS2+src2Step));
    _p_2 = _mm_unpacklo_epi8(_p_2, _p_zero);
    _p_4 = _mm_unpacklo_epi8(_p_4, _p_zero);
    _p_3 = _mm_unpacklo_epi8(_p_3, _p_zero);
    _p_5 = _mm_unpacklo_epi8(_p_5, _p_zero);
    _p_2 = _mm_sub_epi16(_p_2, _p_4);
    _p_3 = _mm_sub_epi16(_p_3, _p_5);

    _p_5 = _mm_subs_epi16(_p_0, _p_1);
    _p_0 = _mm_adds_epi16(_p_0, _p_1);
    _p_7 = _mm_subs_epi16(_p_2, _p_3);
    _p_2 = _mm_adds_epi16(_p_2, _p_3);
    _p_1 = _mm_subs_epi16(_p_0, _p_2);
    _p_0 = _mm_adds_epi16(_p_0, _p_2);
    _p_3 = _mm_adds_epi16(_p_5, _p_7);
    _p_5 = _mm_subs_epi16(_p_5, _p_7);
    _p_0 = _mm_unpacklo_epi16(_p_0, _p_1);
    _p_5 = _mm_unpacklo_epi16(_p_5, _p_3);
    _p_7 = _mm_unpackhi_epi32(_p_0, _p_5);
    _p_0 = _mm_unpacklo_epi32(_p_0, _p_5);
    _p_1 = _mm_srli_si128(_p_0, 8);
    _p_3 = _mm_srli_si128(_p_7, 8);
    _p_5 = _mm_subs_epi16(_p_0, _p_1);
    _p_0 = _mm_adds_epi16(_p_0, _p_1);
    _p_2 = _mm_subs_epi16(_p_7, _p_3);
    _p_7 = _mm_adds_epi16(_p_7, _p_3);
    _p_1 = _mm_subs_epi16(_p_0, _p_7);
    _p_0 = _mm_adds_epi16(_p_0, _p_7);
    _p_3 = _mm_adds_epi16(_p_5, _p_2);
    _p_5 = _mm_subs_epi16(_p_5, _p_2);
    _p_0 = _mm_unpacklo_epi16(_p_0, _p_1);
    _p_5 = _mm_unpacklo_epi16(_p_5, _p_3);
    _p_2 = _mm_unpackhi_epi32(_p_0, _p_5);
    _p_0 = _mm_unpacklo_epi32(_p_0, _p_5);
    _p_3 = _mm_srai_epi16(_p_2, 15);
    _p_1 = _mm_srai_epi16(_p_0, 15);
    _p_2 = _mm_xor_si128(_p_2, _p_3);
    _p_0 = _mm_xor_si128(_p_0, _p_1);
    _p_2 = _mm_sub_epi16(_p_2, _p_3);
    _p_0 = _mm_sub_epi16(_p_0, _p_1);
    _p_0 = _mm_add_epi16(_p_0, _p_2);
    _p_2 = _mm_srli_si128(_p_0, 8);
    _p_0 = _mm_add_epi16(_p_0, _p_2);
    _p_2 = _mm_srli_si128(_p_0, 4);
    _p_0 = _mm_add_epi16(_p_0, _p_2);
    s = _mm_cvtsi128_si32(_p_0);
    satd = (s >> 16) + (signed short)s;
    return satd >> 1;
}


unsigned int SATD4x4_INTEL_C(unsigned char *pSrc1, unsigned int src1Step, unsigned char *pSrc2, unsigned int src2Step)
{
    __declspec(align(16)) signed short tmpBuff[4][4];
    __declspec(align(16)) signed short diffBuff[4][4];
    unsigned int satd = 0;
    unsigned int b;

	for(int ty=0;ty<4;ty++)
		for(int tx=0;tx<4;tx++)
			diffBuff[ty][tx] = (int)(pSrc1[ty*src1Step + tx])-(int)(pSrc2[ty*src2Step + tx]);

    for (b = 0; b < 4; b ++)
	{
        signed int a01, a23, b01, b23;

        a01 = diffBuff[b][0] + diffBuff[b][1];
        a23 = diffBuff[b][2] + diffBuff[b][3];
        b01 = diffBuff[b][0] - diffBuff[b][1];
        b23 = diffBuff[b][2] - diffBuff[b][3];
        tmpBuff[b][0] = a01 + a23;
        tmpBuff[b][1] = a01 - a23;
        tmpBuff[b][2] = b01 - b23;
        tmpBuff[b][3] = b01 + b23;
    }
    for (b = 0; b < 4; b ++)
	{
        signed int a01, a23, b01, b23;

        a01 = tmpBuff[0][b] + tmpBuff[1][b];
        a23 = tmpBuff[2][b] + tmpBuff[3][b];
        b01 = tmpBuff[0][b] - tmpBuff[1][b];
        b23 = tmpBuff[2][b] - tmpBuff[3][b];
        satd += abs(a01 + a23) + abs(a01 - a23) + abs(b01 - b23) + abs(b01 + b23);
    }
	return satd >> 1;
}


unsigned int SATD16x16_INTEL_8x8_C(unsigned char *pSrc1, unsigned int src1Step, unsigned char *pSrc2, unsigned int src2Step)
{
    signed short diff[8][8];
    signed int x, y;
    unsigned int satd = 0;

    for (y = 0; y < 16; y += 8) 
	{
        for (x = 0; x < 16; x += 8)
		{
			for(int ty=0;ty<8;ty++)
				for(int tx=0;tx<8;tx++)
					diff[ty][tx] = (int)(pSrc1[x+ty*src1Step + tx])-(int)(pSrc2[x+ty*src2Step + tx]);
			for(int i = 0; i < 8; i++)
			{
				signed int t0 = diff[i][0] + diff[i][4];
				signed int t4 = diff[i][0] - diff[i][4];
				signed int t1 = diff[i][1] + diff[i][5];
				signed int t5 = diff[i][1] - diff[i][5];
				signed int t2 = diff[i][2] + diff[i][6];
				signed int t6 = diff[i][2] - diff[i][6];
				signed int t3 = diff[i][3] + diff[i][7];
				signed int t7 = diff[i][3] - diff[i][7];
				signed int s0 = t0 + t2;
				signed int s2 = t0 - t2;
				signed int s1 = t1 + t3;
				signed int s3 = t1 - t3;
				signed int s4 = t4 + t6;
				signed int s6 = t4 - t6;
				signed int s5 = t5 + t7;
				signed int s7 = t5 - t7;
				diff[i][0] = s0 + s1;
				diff[i][1] = s0 - s1;
				diff[i][2] = s2 + s3;
				diff[i][3] = s2 - s3;
				diff[i][4] = s4 + s5;
				diff[i][5] = s4 - s5;
				diff[i][6] = s6 + s7;
				diff[i][7] = s6 - s7;
			}
			for(int i = 0; i < 8; i++)
			{
				signed int t0 = diff[0][i] + diff[4][i];
				signed int t4 = diff[0][i] - diff[4][i];
				signed int t1 = diff[1][i] + diff[5][i];
				signed int t5 = diff[1][i] - diff[5][i];
				signed int t2 = diff[2][i] + diff[6][i];
				signed int t6 = diff[2][i] - diff[6][i];
				signed int t3 = diff[3][i] + diff[7][i];
				signed int t7 = diff[3][i] - diff[7][i];
				signed int s0 = t0 + t2;
				signed int s2 = t0 - t2;
				signed int s1 = t1 + t3;
				signed int s3 = t1 - t3;
				signed int s4 = t4 + t6;
				signed int s6 = t4 - t6;
				signed int s5 = t5 + t7;
				signed int s7 = t5 - t7;
				satd += abs(s0 + s1);
				satd += abs(s0 - s1);
				satd += abs(s2 + s3);
				satd += abs(s2 - s3);
				satd += abs(s4 + s5);
				satd += abs(s4 - s5);
				satd += abs(s6 + s7);
				satd += abs(s6 - s7);
			}
        }
        pSrc1 += 8 * src1Step;
        pSrc2 += 8 * src2Step;
    }
	return satd >> 1;
}

unsigned int SATD16x16_16x16_C(unsigned char *pSrc1, unsigned int src1Step, unsigned char *pSrc2, unsigned int src2Step)
{
    signed short diff[16][16];
    signed int x, y;
    unsigned int satd = 0;

    for (y = 0; y < 16; y += 16) 
	{
        for (x = 0; x < 16; x += 16)
		{
			for(int ty=0;ty<16;ty++)
				for(int tx=0;tx<16;tx++)
					diff[ty][tx] = (int)(pSrc1[x+ty*src1Step + tx])-(int)(pSrc2[x+ty*src2Step + tx]);
			for(int i = 0; i < 16; i++)
			{
				signed int t0 = diff[i][0] + diff[i][8];
				signed int t8 = diff[i][0] - diff[i][8];
				signed int t1 = diff[i][1] + diff[i][9];
				signed int t9 = diff[i][1] - diff[i][9];
				signed int t2 = diff[i][2] + diff[i][10];
				signed int t10 = diff[i][2] - diff[i][10];
				signed int t3 = diff[i][3] + diff[i][11];
				signed int t11 = diff[i][3] - diff[i][11];
				signed int t4 = diff[i][4] + diff[i][12];
				signed int t12 = diff[i][4] - diff[i][12];
				signed int t5 = diff[i][5] + diff[i][13];
				signed int t13 = diff[i][5] - diff[i][13];
				signed int t6 = diff[i][6] + diff[i][14];
				signed int t14 = diff[i][6] - diff[i][14];
				signed int t7 = diff[i][7] + diff[i][15];
				signed int t15 = diff[i][7] - diff[i][15];
				signed int s0 = t0 + t4;
				signed int s4 = t0 - t4;
				signed int s1 = t1 + t5;
				signed int s5 = t1 - t5;
				signed int s2 = t2 + t6;
				signed int s6 = t2 - t6;
				signed int s3 = t3 + t7;
				signed int s7 = t3 - t7;
				signed int s8 = t8 + t12;
				signed int s12 = t8 - t12;
				signed int s9 = t9 + t13;
				signed int s13 = t9 - t13;
				signed int s10 = t10 + t14;
				signed int s14 = t10 - t14;
				signed int s11 = t11 + t15;
				signed int s15 = t11 - t15;

				signed int d0 = s0 + s2;
				signed int d2 = s0 - s2;
				signed int d1 = s1 + s3;
				signed int d3 = s1 - s3;
				signed int d4 = s4 + s6;
				signed int d6 = s4 - s6;
				signed int d5 = s5 + s7;
				signed int d7 = s5 - s7;
				signed int d8 = s8 + s10;
				signed int d10 = s8 - s10;
				signed int d9 = s9 + s11;
				signed int d11 = s9 - s11;
				signed int d12 = s12 + s14;
				signed int d14 = s12 - s14;
				signed int d13 = s13 + s15;
				signed int d15 = s13 - s15;

				diff[i][0] = d0 + d1;
				diff[i][1] = d0 - d1;
				diff[i][2] = d2 + d3;
				diff[i][3] = d2 - d3;
				diff[i][4] = d4 + d5;
				diff[i][5] = d4 - d5;
				diff[i][6] = d6 + d7;
				diff[i][7] = d6 - d7;
				diff[i][8] = d8 + d9;
				diff[i][9] = d8 - d9;
				diff[i][10] = d10 + d11;
				diff[i][11] = d10 - d11;
				diff[i][12] = d12 + d13;
				diff[i][13] = d12 - d13;
				diff[i][14] = d14 + d15;
				diff[i][15] = d14 - d15;
			}
			for(int i = 0; i < 16; i++)
			{
				signed int t0 = diff[0][i] + diff[8][i];
				signed int t8 = diff[0][i] - diff[8][i];
				signed int t1 = diff[1][i] + diff[9][i];
				signed int t9 = diff[1][i] - diff[9][i];
				signed int t2 = diff[2][i] + diff[10][i];
				signed int t10 = diff[2][i] - diff[10][i];
				signed int t3 = diff[3][i] + diff[11][i];
				signed int t11 = diff[3][i] - diff[11][i];
				signed int t4 = diff[4][i] + diff[12][i];
				signed int t12 = diff[4][i] - diff[12][i];
				signed int t5 = diff[5][i] + diff[13][i];
				signed int t13 = diff[5][i] - diff[13][i];
				signed int t6 = diff[6][i] + diff[14][i];
				signed int t14 = diff[6][i] - diff[14][i];
				signed int t7 = diff[7][i] + diff[15][i];
				signed int t15 = diff[7][i] - diff[15][i];
				signed int s0 = t0 + t4;
				signed int s4 = t0 - t4;
				signed int s1 = t1 + t5;
				signed int s5 = t1 - t5;
				signed int s2 = t2 + t6;
				signed int s6 = t2 - t6;
				signed int s3 = t3 + t7;
				signed int s7 = t3 - t7;
				signed int s8 = t8 + t12;
				signed int s12 = t8 - t12;
				signed int s9 = t9 + t13;
				signed int s13 = t9 - t13;
				signed int s10 = t10 + t14;
				signed int s14 = t10 - t14;
				signed int s11 = t11 + t15;
				signed int s15 = t11 - t15;

				signed int d0 = s0 + s2;
				signed int d2 = s0 - s2;
				signed int d1 = s1 + s3;
				signed int d3 = s1 - s3;
				signed int d4 = s4 + s6;
				signed int d6 = s4 - s6;
				signed int d5 = s5 + s7;
				signed int d7 = s5 - s7;
				signed int d8 = s8 + s10;
				signed int d10 = s8 - s10;
				signed int d9 = s9 + s11;
				signed int d11 = s9 - s11;
				signed int d12 = s12 + s14;
				signed int d14 = s12 - s14;
				signed int d13 = s13 + s15;
				signed int d15 = s13 - s15;

				satd += abs(d0 + d1);
				satd += abs(d0 - d1);
				satd += abs(d2 + d3);
				satd += abs(d2 - d3);
				satd += abs(d4 + d5);
				satd += abs(d4 - d5);
				satd += abs(d6 + d7);
				satd += abs(d6 - d7);
				satd += abs(d8 + d9);
				satd += abs(d8 - d9);
				satd += abs(d10 + d11);
				satd += abs(d10 - d11);
				satd += abs(d12 + d13);
				satd += abs(d12 - d13);
				satd += abs(d14 + d15);
				satd += abs(d14 - d15);
			}
        }
//        pSrc1 += 8 * src1Step;
//        pSrc2 += 8 * src2Step;
    }
	return satd >> 1;
}

unsigned int SAD_16x16_intrinsic( unsigned char * address1, unsigned int stride1, unsigned char * address2,unsigned int stride2 )
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

unsigned int SATD_16x16_INTRA_intrinsic( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 )
{
	const unsigned char *pSrc1;
	int src1Step;
	const unsigned char *pSrc2;
	int src2Step;

	pSrc1 = address1;
	pSrc2 = address2;
	src1Step = stride1;
	src2Step = stride2;

	int x, y;
    unsigned int satd;
    __declspec(align(16)) __m128i  _p_0, _p_1, _p_2, _p_3, _p_4, _p_5, _p_7, _p_zero;
    const unsigned char *pS1, *pS2;
    __declspec(align(16)) __m128i  _r_0, _r_1, _r_2, _r_3, _r_5, _r_7, _p_satd;
    static __declspec(align(16)) short _p_one[8] = {1, 1, 1, 1, 1, 1, 1, 1};
    _p_zero = _mm_setzero_si128();
    _p_satd = _mm_setzero_si128();
    for (y = 0; y < 16; y += 4) {
        for (x = 0; x < 16; x += 8) {
            pS1 = pSrc1 + x;
            pS2 = pSrc2 + x;
            _p_0 = _mm_loadl_epi64((__m128i*)(pS1));
            _p_4 = _mm_loadl_epi64((__m128i*)(pS2));
            _p_1 = _mm_loadl_epi64((__m128i*)(pS1+src1Step));
            _p_5 = _mm_loadl_epi64((__m128i*)(pS2+src2Step));
            _p_0 = _mm_unpacklo_epi8(_p_0, _p_zero);
            _p_4 = _mm_unpacklo_epi8(_p_4, _p_zero);
            _p_1 = _mm_unpacklo_epi8(_p_1, _p_zero);
            _p_5 = _mm_unpacklo_epi8(_p_5, _p_zero);
            _p_0 = _mm_sub_epi16(_p_0, _p_4);
            _p_1 = _mm_sub_epi16(_p_1, _p_5);
            pS1 += 2 * src1Step;
            pS2 += 2 * src2Step;
            _p_2 = _mm_loadl_epi64((__m128i*)(pS1));
            _p_4 = _mm_loadl_epi64((__m128i*)(pS2));
            _p_3 = _mm_loadl_epi64((__m128i*)(pS1+src1Step));
            _p_5 = _mm_loadl_epi64((__m128i*)(pS2+src2Step));
            _p_2 = _mm_unpacklo_epi8(_p_2, _p_zero);
            _p_4 = _mm_unpacklo_epi8(_p_4, _p_zero);
            _p_3 = _mm_unpacklo_epi8(_p_3, _p_zero);
            _p_5 = _mm_unpacklo_epi8(_p_5, _p_zero);
            _p_2 = _mm_sub_epi16(_p_2, _p_4);
            _p_3 = _mm_sub_epi16(_p_3, _p_5);
            _p_5 = _mm_subs_epi16(_p_0, _p_1);
            _p_0 = _mm_adds_epi16(_p_0, _p_1);
            _p_7 = _mm_subs_epi16(_p_2, _p_3);
            _p_2 = _mm_adds_epi16(_p_2, _p_3);
            _p_1 = _mm_subs_epi16(_p_0, _p_2);
            _p_0 = _mm_adds_epi16(_p_0, _p_2);
            _p_3 = _mm_adds_epi16(_p_5, _p_7);
            _p_5 = _mm_subs_epi16(_p_5, _p_7);

            _r_0 = _mm_unpackhi_epi16(_p_0, _p_1);
            _r_5 = _mm_unpackhi_epi16(_p_5, _p_3);

            _p_0 = _mm_unpacklo_epi16(_p_0, _p_1);
            _p_5 = _mm_unpacklo_epi16(_p_5, _p_3);
            _p_7 = _mm_unpackhi_epi32(_p_0, _p_5);
            _p_0 = _mm_unpacklo_epi32(_p_0, _p_5);
            _p_1 = _mm_srli_si128(_p_0, 8);
            _p_3 = _mm_srli_si128(_p_7, 8);
            _p_5 = _mm_subs_epi16(_p_0, _p_1);
            _p_0 = _mm_adds_epi16(_p_0, _p_1);
            _p_2 = _mm_subs_epi16(_p_7, _p_3);
            _p_7 = _mm_adds_epi16(_p_7, _p_3);
            _p_1 = _mm_subs_epi16(_p_0, _p_7);
            _p_0 = _mm_adds_epi16(_p_0, _p_7);
            _p_3 = _mm_adds_epi16(_p_5, _p_2);
            _p_5 = _mm_subs_epi16(_p_5, _p_2);
            _p_0 = _mm_unpacklo_epi16(_p_0, _p_1);
            _p_5 = _mm_unpacklo_epi16(_p_5, _p_3);
            _p_2 = _mm_unpackhi_epi32(_p_0, _p_5);
            _p_0 = _mm_unpacklo_epi32(_p_0, _p_5);
            _p_3 = _mm_srai_epi16(_p_2, 15);
            _p_1 = _mm_srai_epi16(_p_0, 15);
            _p_2 = _mm_xor_si128(_p_2, _p_3);
            _p_0 = _mm_xor_si128(_p_0, _p_1);
            _p_2 = _mm_sub_epi16(_p_2, _p_3);
            _p_0 = _mm_sub_epi16(_p_0, _p_1);
            _p_0 = _mm_add_epi16(_p_0, _p_2);
            _p_0 = _mm_madd_epi16(_p_0, *(__m128i*)_p_one);
            _p_satd = _mm_add_epi32(_p_satd, _p_0);

            _r_7 = _mm_unpackhi_epi32(_r_0, _r_5);
            _r_0 = _mm_unpacklo_epi32(_r_0, _r_5);
            _r_1 = _mm_srli_si128(_r_0, 8);
            _r_3 = _mm_srli_si128(_r_7, 8);
            _r_5 = _mm_subs_epi16(_r_0, _r_1);
            _r_0 = _mm_adds_epi16(_r_0, _r_1);
            _r_2 = _mm_subs_epi16(_r_7, _r_3);
            _r_7 = _mm_adds_epi16(_r_7, _r_3);
            _r_1 = _mm_subs_epi16(_r_0, _r_7);
            _r_0 = _mm_adds_epi16(_r_0, _r_7);
            _r_3 = _mm_adds_epi16(_r_5, _r_2);
            _r_5 = _mm_subs_epi16(_r_5, _r_2);
            _r_0 = _mm_unpacklo_epi16(_r_0, _r_1);
            _r_5 = _mm_unpacklo_epi16(_r_5, _r_3);
            _r_2 = _mm_unpackhi_epi32(_r_0, _r_5);
            _r_0 = _mm_unpacklo_epi32(_r_0, _r_5);
            _r_3 = _mm_srai_epi16(_r_2, 15);
            _r_1 = _mm_srai_epi16(_r_0, 15);
            _r_2 = _mm_xor_si128(_r_2, _r_3);
            _r_0 = _mm_xor_si128(_r_0, _r_1);
            _r_2 = _mm_sub_epi16(_r_2, _r_3);
            _r_0 = _mm_sub_epi16(_r_0, _r_1);
            _r_0 = _mm_add_epi16(_r_0, _r_2);
            _r_0 = _mm_madd_epi16(_r_0, *(__m128i*)_p_one);
            _p_satd = _mm_add_epi32(_p_satd, _r_0);
        }
        pSrc1 += 4 * src1Step;
        pSrc2 += 4 * src2Step;
    }
    _p_1 = _mm_srli_si128(_p_satd, 8);
    _p_satd = _mm_add_epi32(_p_satd, _p_1);
    _p_1 = _mm_srli_si128(_p_satd, 4);
    _p_satd = _mm_add_epi32(_p_satd, _p_1);
    satd = _mm_cvtsi128_si32(_p_satd);

//	printf("%d\n", (int)satd);

    return (satd);
}

unsigned int SATD_16x16_INTRA_C( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 )
{
	int i, j, k;
	unsigned int sad = 0;
	int z0, z1, z2, z3;
	int diff[16], *d;
	unsigned char *a1 = address1;
	unsigned char *a2 = address2;
	int dc[16];

	for( i = 0; i < 4; i++ )
	{
		for( j = 0; j < 4; j++ )
		{
			d = &diff[0];

			// horizontal
			for( k = 0; k < 4; k++ )
			{
				z0 = (int)(*a1 - *a2) + (int)(*(a1+3) - *(a2+3));
				z1 = (int)(*(a1+1) - *(a2+1)) + (int)(*(a1+2) - *(a2+2)); 
				z2 = (int)(*(a1+1) - *(a2+1)) - (int)(*(a1+2) - *(a2+2));
				z3 = (int)(*a1 - *a2) - (int)(*(a1+3) - *(a2+3));	
				
				*d		= z0 + z1;
				*(d+1)	= z3 + z2;
				*(d+2)	= z0 - z1;
				*(d+3)	= z3 - z2;
				
				a1 += stride1;
				a2 += stride2;
				d += 4;
			}
			
			d = &diff[0];
			
			// vertical
			for( k = 0; k < 4; k++ )
			{
				z0 = *d + *(d + 12);
				z1 = *(d + 4) + *(d + 8);
				z2 = *(d + 4) - *(d + 8);
				z3 = *d - *(d + 12);
				
				*d        = (z0 + z1);
				*(d + 4)  = (z3 + z2);
				*(d + 8)  = (z0 - z1);
				*(d + 12) = (z3 - z2);
				
				d++;
			}
			
			d = &diff[1];
			
			for( k = 1; k < 4*4; k++, d++ )
			{
				sad += abs( *d );
			}

			dc[j*4 + i] = diff[0] / 4;								// store DC for later processing
		}

		a1 = address1 + (i+1)*4;
		a2 = address2 + (i+1)*4;
	}

	d = &dc[0];
	
	// Hadamard of DC coeff
	for( i = 0; i < 4; i++ )
	{
		z0 = *d + *(d+3);
		z1 = *(d+1) + *(d+2);
		z2 = *(d+1) - *(d+2);
		z3 = *d - *(d+3);

		*d		= z0 + z1;
		*(d+1)	= z2 + z3;
		*(d+2)	= z0 - z1;
		*(d+3)	= z3 - z2;

		d += 4;
	}

	d = &dc[0];
	for( i = 0; i < 4; i++ )	
	{
		z0 = *d + *(d+12);
		z1 = *(d+4) + *(d+8);
		z2 = *(d+4) - *(d+8);
		z3 = *d - *(d+12);

		*d		= z0 + z1;
		*(d+4)	= z2 + z3;
		*(d+8)	= z0 - z1;
		*(d+12) = z3 - z2;
	
		d++;
	}

	d = &dc[0];
	
	for( k = 0; k < 4*4; k++, d++ )
	{
		sad += abs( *d );
	}

	return sad;
}

//yes it is the same function as in the h264 encoder SSE2 version. Using it in a separate lib to be able to use other then VS compiler
void read_luma_inter_pred_MxN_half_half_Encoder_INTRINSIC( short *src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
{
	int j, i;
	const short* src_ptr;
	unsigned char* dst_ptr;
	int src_stride = m_dst_stride;
	int dst_stride = m_dst_stride;
	int theight = m_height;
	int twidth = m_width;
	int stride1, stride2, stride3;
	stride1 = src_stride;
	stride2 = src_stride<<1;
	stride3 = (src_stride<<1)+src_stride;
	__declspec(align(16)) __m128i zero, offset512;
	zero = _mm_setzero_si128();
	offset512 = _mm_set1_epi32(512);

	for( i = 0; i < (twidth); i+=8)
	{
		__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5, r0_b, r1_b, r2_b, r3_b, r4_b, r5_b;
		src_ptr = src;
		dst_ptr = dst;
		r0_b = _mm_loadu_si128((__m128i*)(src_ptr));
		r1_b = _mm_loadu_si128((__m128i*)(src_ptr+stride1));
		r2_b = _mm_loadu_si128((__m128i*)(src_ptr+stride2));
		r3_b = _mm_loadu_si128((__m128i*)(src_ptr+stride3));
		r4_b = _mm_loadu_si128((__m128i*)(src_ptr+(stride2<<1)));
		for( j = 0; j < theight; j++)
		{
			__declspec(align(16)) __m128i temp, rResult1;
			r5_b = _mm_loadu_si128((__m128i*)(src_ptr+stride3+stride2));
			//first 64 bits
			r0 = _mm_unpacklo_epi16(r0_b, zero);
			r1 = _mm_unpacklo_epi16(r1_b, zero);
			r2 = _mm_unpacklo_epi16(r2_b, zero);
			r3 = _mm_unpacklo_epi16(r3_b, zero);
			r4 = _mm_unpacklo_epi16(r4_b, zero);
			r5 = _mm_unpacklo_epi16(r5_b, zero);

			r0 = _mm_add_epi32(r0, r5);
			r1 = _mm_add_epi32(r1, r4);
			r2 = _mm_add_epi32(r2, r3);
			temp = r1;
			r1 = _mm_slli_epi32(r1, 2);
			r1 = _mm_add_epi32(r1, temp);
			temp = _mm_slli_epi32(r2, 2);
			r2 = _mm_slli_epi32(r2, 4);
			r2 = _mm_add_epi32(r2, temp);
			r0 = _mm_add_epi32(r0, r2);
			r0 = _mm_sub_epi32(r0, r1);
			r0 = _mm_add_epi32(r0, offset512);
			rResult1 = _mm_srai_epi32(r0, 10);
			//end

			//first 64 bits
			r0 = _mm_unpackhi_epi16(r0_b, zero);
			r1 = _mm_unpackhi_epi16(r1_b, zero);
			r2 = _mm_unpackhi_epi16(r2_b, zero);
			r3 = _mm_unpackhi_epi16(r3_b, zero);
			r4 = _mm_unpackhi_epi16(r4_b, zero);
			r5 = _mm_unpackhi_epi16(r5_b, zero);

			r0 = _mm_add_epi32(r0, r5);
			r1 = _mm_add_epi32(r1, r4);
			r2 = _mm_add_epi32(r2, r3);
			temp = r1;
			r1 = _mm_slli_epi32(r1, 2);
			r1 = _mm_add_epi32(r1, temp);
			temp = _mm_slli_epi32(r2, 2);
			r2 = _mm_slli_epi32(r2, 4);
			r2 = _mm_add_epi32(r2, temp);
			r0 = _mm_add_epi32(r0, r2);
			r0 = _mm_sub_epi32(r0, r1);
			r0 = _mm_add_epi32(r0, offset512);
			r0 = _mm_srai_epi32(r0, 10);
			r0 = _mm_packs_epi32(rResult1, r0);

			rResult1 = _mm_packus_epi16(r0, zero);
			//end

			_mm_storel_epi64((__m128i*)dst_ptr, rResult1);
			r0_b = r1_b;
			r1_b = r2_b;
			r2_b = r3_b;
			r3_b = r4_b;
			r4_b = r5_b;
			src_ptr += stride1;
			dst_ptr += dst_stride;
		}
		src+=8;
		dst+=8;
	}
}


unsigned int SAD_16x8_intrinsic( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 )
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

unsigned int SAD_8x16_intrinsic( unsigned char *address1, unsigned int stride1, unsigned char *address2, unsigned int stride2 )
{
	unsigned short sad_array[8];
	__m128i l0, l1, line_sad, acc_sad;

	acc_sad = _mm_setzero_si128();
	//acc_sad = _mm_xor_si128(acc_sad, acc_sad);

	l0 = _mm_loadl_epi64((__m128i*)address1);
	l1 = _mm_loadl_epi64((__m128i*)address2);
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadl_epi64((__m128i*)(address1+stride1));
	l1 = _mm_loadl_epi64((__m128i*)(address2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadl_epi64((__m128i*)(address1+stride1+stride1));
	l1 = _mm_loadl_epi64((__m128i*)(address2+stride2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadl_epi64((__m128i*)(address1+stride1+stride1+stride1));
	l1 = _mm_loadl_epi64((__m128i*)(address2+stride2+stride2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	address1 += (stride1<<2);
	address2 += (stride2<<2);

	l0 = _mm_loadl_epi64((__m128i*)address1);
	l1 = _mm_loadl_epi64((__m128i*)address2);
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadl_epi64((__m128i*)(address1+stride1));
	l1 = _mm_loadl_epi64((__m128i*)(address2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadl_epi64((__m128i*)(address1+stride1+stride1));
	l1 = _mm_loadl_epi64((__m128i*)(address2+stride2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadl_epi64((__m128i*)(address1+stride1+stride1+stride1));
	l1 = _mm_loadl_epi64((__m128i*)(address2+stride2+stride2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	address1 += (stride1<<2);
	address2 += (stride2<<2);

	l0 = _mm_loadl_epi64((__m128i*)address1);
	l1 = _mm_loadl_epi64((__m128i*)address2);
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadl_epi64((__m128i*)(address1+stride1));
	l1 = _mm_loadl_epi64((__m128i*)(address2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadl_epi64((__m128i*)(address1+stride1+stride1));
	l1 = _mm_loadl_epi64((__m128i*)(address2+stride2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadl_epi64((__m128i*)(address1+stride1+stride1+stride1));
	l1 = _mm_loadl_epi64((__m128i*)(address2+stride2+stride2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	address1 += (stride1<<2);
	address2 += (stride2<<2);

	l0 = _mm_loadl_epi64((__m128i*)address1);
	l1 = _mm_loadl_epi64((__m128i*)address2);
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadl_epi64((__m128i*)(address1+stride1));
	l1 = _mm_loadl_epi64((__m128i*)(address2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadl_epi64((__m128i*)(address1+stride1+stride1));
	l1 = _mm_loadl_epi64((__m128i*)(address2+stride2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	l0 = _mm_loadl_epi64((__m128i*)(address1+stride1+stride1+stride1));
	l1 = _mm_loadl_epi64((__m128i*)(address2+stride2+stride2+stride2));
	line_sad = _mm_sad_epu8(l0, l1);
	acc_sad = _mm_add_epi16(acc_sad, line_sad);

	_mm_storeu_si128((__m128i*)(&sad_array[0]), acc_sad);

	return (sad_array[0]);
}
