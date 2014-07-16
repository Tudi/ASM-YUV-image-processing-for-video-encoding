#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <assert.h>
#include <conio.h>
#include <emmintrin.h>
//#include <tmmintrin.h>

#define MB_X_REF 68
#define MB_Y_REF 45
#define MB_SIZE 16
#define REF_STRIDE (MB_X_REF*MB_SIZE)
#ifdef _DEBUG
	#define LOOP_TEST_COUNT	1	//to have a larger runtime
	#define LOOP_TEST_COUNT_INTRA	1	//to have a larger runtime
	#define LOOP_TEST_COUNT_INTERP	1	//to have a larger runtime
#else
	#define LOOP_TEST_COUNT	10	//to have a larger runtime
	#define LOOP_TEST_COUNT_INTERP	10	//to have a larger runtime
#endif

void read_luma_inter_pred_MxN_half_full_tmp_Encoder( unsigned char *src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
{
	int i,j;
#define CLIP3(a,b,c)	((c) < (a) ? (a) : ((c) > (b) ? (b) : (c))) // MIN, MAX, val
#define CLIP1(x)		(CLIP3( 0, 255, (x) ))
	//BYTE *src = m_pred_src;
	INT src_stride_2 = 2 * m_src_stride;
	INT src_stride_3 = 3 * m_src_stride;

	for( i = 0; i < m_height; i++ )
	{
		for( j = 0; j < m_width; j++ )
		{
			INT v = src[j-src_stride_2] + src[j+src_stride_3] - 5*( src[j-m_src_stride] + src[j+src_stride_2] ) +
					20*( src[j] + src[j+m_src_stride] );
			
			dst[j] = CLIP1( (v+16)>>5 );
		}

		src += m_src_stride;
		dst += m_dst_stride;
	}
}

void read_luma_inter_pred_MxN_half_full_tmp_Encoder_intr( unsigned char *m_pred_src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
{
	BYTE *dst_backup = dst; 
	int j, i;

	const unsigned char* src = m_pred_src;
	const unsigned char* src_ptr;
	unsigned char* dst_ptr;
	int src_stride = m_src_stride;
	int dst_stride = m_dst_stride;
	int theight = m_height;
	int twidth = m_width;
	int stride1, stride2, stride3;
	stride1 = src_stride;
	stride2 = src_stride<<1;
	stride3 = (src_stride<<1)+src_stride;

	__declspec(align(16)) short temp_debug[8];

	for( i = 0; i < (twidth); i+=8){

		__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5, r0_b, r1_b, r2_b, r3_b, r4_b, r5_b, offset16, temp, zero;
		zero = _mm_setzero_si128();
		//zero = _mm_xor_si128(zero, zero);
		offset16 = _mm_set1_epi16(16);

		src_ptr = src;
		dst_ptr = dst;
		r0_b = _mm_loadl_epi64((__m128i*)(src_ptr-stride2));
		r1_b = _mm_loadl_epi64((__m128i*)(src_ptr-stride1));
		r2_b = _mm_loadl_epi64((__m128i*)(src_ptr));
		r3_b = _mm_loadl_epi64((__m128i*)(src_ptr+stride1));
		r4_b = _mm_loadl_epi64((__m128i*)(src_ptr+stride2));
		r0_b = _mm_unpacklo_epi8(r0_b, zero);
		r1_b = _mm_unpacklo_epi8(r1_b, zero);
		r2_b = _mm_unpacklo_epi8(r2_b, zero);
		r3_b = _mm_unpacklo_epi8(r3_b, zero);
		r4_b = _mm_unpacklo_epi8(r4_b, zero);
		for( j = 0; j < theight; j++){
			r5_b = _mm_loadl_epi64((__m128i*)(src_ptr+stride3));
			r5_b = _mm_unpacklo_epi8(r5_b, zero);
			r0 = r0_b;
			r1 = r1_b;
			r2 = r2_b;
			r3 = r3_b;
			r4 = r4_b;
			r5 = r5_b;
			r0 = _mm_adds_epi16(r0, r5);
			r1 = _mm_adds_epi16(r1, r4);
			r2 = _mm_adds_epi16(r2, r3);
			temp = r1;
			r1 = _mm_slli_epi16(r1, 2);
			r1 = _mm_adds_epi16(r1, temp);
			temp = _mm_slli_epi16(r2, 2);
			r2 = _mm_slli_epi16(r2, 4);
			r2 = _mm_adds_epi16(r2, temp);
			r0 = _mm_adds_epi16(r0, r2);
			r0 = _mm_subs_epi16(r0, r1);
			r0 = _mm_adds_epi16(r0, offset16);
			r0 = _mm_srai_epi16(r0, 5);
			r0 = _mm_packus_epi16(r0, zero);
			_mm_storel_epi64((__m128i*)dst_ptr, r0);
			r0_b = r1_b;
			r1_b = r2_b;
			r2_b = r3_b;
			r3_b = r4_b;
			r4_b = r5_b;
			src_ptr += stride1;
			dst_ptr += dst_stride;
		}
		src += 8;
		dst += 8;
	}
} 

void read_luma_inter_pred_MxN_half_full_tmp_Encoder_intr2( unsigned char *m_pred_src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
{
	BYTE *dst_backup = dst; 
	int j, i;

	const unsigned char* src = m_pred_src;
	const unsigned char* src_ptr;
	unsigned char* dst_ptr;
	int src_stride = m_src_stride;
	int dst_stride = m_dst_stride;
	int theight = m_height;
	int twidth = m_width;
	int stride1, stride2, stride3;
	stride1 = src_stride;
	stride2 = src_stride<<1;
	stride3 = (src_stride<<1)+src_stride;

	__declspec(align(16)) __m128i offset16, zero;
	zero = _mm_setzero_si128();
	offset16 = _mm_set1_epi16(16);

	for( i = 0; i < (twidth); i+=8)
	{
		__declspec(align(16)) __m128i r0_b, r1_b, r2_b, r3_b, r4_b;

		src_ptr = src;
		dst_ptr = dst;
		r0_b = _mm_loadl_epi64((__m128i*)(src_ptr-stride2));
		r1_b = _mm_loadl_epi64((__m128i*)(src_ptr-stride1));
		r2_b = _mm_loadl_epi64((__m128i*)(src_ptr));
		r3_b = _mm_loadl_epi64((__m128i*)(src_ptr+stride1));
		r4_b = _mm_loadl_epi64((__m128i*)(src_ptr+stride2));
		r0_b = _mm_unpacklo_epi8(r0_b, zero);
		r1_b = _mm_unpacklo_epi8(r1_b, zero);
		r2_b = _mm_unpacklo_epi8(r2_b, zero);
		r3_b = _mm_unpacklo_epi8(r3_b, zero);
		r4_b = _mm_unpacklo_epi8(r4_b, zero);
		for( j = 0; j < theight; j++)
		{
			__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5, r5_b, temp;
			r5_b = _mm_loadl_epi64((__m128i*)(src_ptr+stride3));
			r5_b = _mm_unpacklo_epi8(r5_b, zero);
			r0 = r0_b;
			r1 = r1_b;
			r2 = r2_b;
			r3 = r3_b;
			r4 = r4_b;
			r5 = r5_b;
			r0 = _mm_adds_epi16(r0, r5);
			r1 = _mm_adds_epi16(r1, r4);
			r2 = _mm_adds_epi16(r2, r3);
			temp = r1;
			r1 = _mm_slli_epi16(r1, 2);
			r1 = _mm_adds_epi16(r1, temp);
			temp = _mm_slli_epi16(r2, 2);
			r2 = _mm_slli_epi16(r2, 4);
			r2 = _mm_adds_epi16(r2, temp);
			r0 = _mm_adds_epi16(r0, r2);
			r0 = _mm_subs_epi16(r0, r1);
			r0 = _mm_adds_epi16(r0, offset16);
			r0 = _mm_srai_epi16(r0, 5);
			r0 = _mm_packus_epi16(r0, zero);
			_mm_storel_epi64((__m128i*)dst_ptr, r0);
			r0_b = r1_b;
			r1_b = r2_b;
			r2_b = r3_b;
			r3_b = r4_b;
			r4_b = r5_b;
			src_ptr += stride1;
			dst_ptr += dst_stride;
		}
		src += 8;
		dst += 8;
	}
} 

void read_luma_inter_pred_MxN_half_full_tmp_Encoder_intr3( unsigned char *m_pred_src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
{
	BYTE *dst_backup = dst; 
	int j, i;

	const unsigned char* src = m_pred_src;
	const unsigned char* src_ptr;
	unsigned char* dst_ptr;
	int src_stride = m_src_stride;
	int dst_stride = m_dst_stride;
	int theight = m_height;
	int twidth = m_width;
	int stride1, stride2, stride3;
	stride1 = src_stride;
	stride2 = src_stride<<1;
	stride3 = (src_stride<<1)+src_stride;

	__declspec(align(16)) __m128i offset16, zero;
	zero = _mm_setzero_si128();
	offset16 = _mm_set1_epi16(16);

	__declspec(align(16)) __m128i mulm5;
	mulm5 = _mm_set1_epi16(5);

	__declspec(align(16)) __m128i mulm20;
	mulm20 = _mm_set1_epi16(20);

	for( i = 0; i < (twidth); i+=8)
	{
		__declspec(align(16)) __m128i r0_b, r1_b, r2_b, r3_b, r4_b;

		src_ptr = src;
		dst_ptr = dst;
		r0_b = _mm_loadl_epi64((__m128i*)(src_ptr-stride2));
		r1_b = _mm_loadl_epi64((__m128i*)(src_ptr-stride1));
		r2_b = _mm_loadl_epi64((__m128i*)(src_ptr));
		r3_b = _mm_loadl_epi64((__m128i*)(src_ptr+stride1));
		r4_b = _mm_loadl_epi64((__m128i*)(src_ptr+stride2));
		r0_b = _mm_unpacklo_epi8(r0_b, zero);
		r1_b = _mm_unpacklo_epi8(r1_b, zero);
		r2_b = _mm_unpacklo_epi8(r2_b, zero);
		r3_b = _mm_unpacklo_epi8(r3_b, zero);
		r4_b = _mm_unpacklo_epi8(r4_b, zero);
		for( j = 0; j < theight; j++)
		{
			__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5, r5_b, temp;
			r5_b = _mm_loadl_epi64((__m128i*)(src_ptr+stride3));
			r5_b = _mm_unpacklo_epi8(r5_b, zero);

			r0 = _mm_adds_epi16(r0_b, r5_b);
			r1 = _mm_adds_epi16(r1_b, r4_b);
			r2 = _mm_adds_epi16(r2_b, r3_b);

			r1 =  _mm_mullo_epi16( r1, mulm5 );
			r2 =  _mm_mullo_epi16( r2, mulm20 );

			r0 = _mm_subs_epi16(r0, r1);
			r0 = _mm_adds_epi16(r0, r2);

			r0 = _mm_adds_epi16(r0, offset16);
			r0 = _mm_srai_epi16(r0, 5);
			r0 = _mm_packus_epi16(r0, zero);

			_mm_storel_epi64((__m128i*)dst_ptr, r0);

			r0_b = r1_b;
			r1_b = r2_b;
			r2_b = r3_b;
			r3_b = r4_b;
			r4_b = r5_b;
			src_ptr += stride1;
			dst_ptr += dst_stride;
		}
		src += 8;
		dst += 8;
	}
} 

void read_luma_inter_pred_MxN_half_full_tmp_Encoder_intr4( unsigned char *m_pred_src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
{
	BYTE *dst_backup = dst; 
	int j, i;

	const unsigned char* src = m_pred_src;
	const unsigned char* src_ptr;
	unsigned char* dst_ptr;
	int src_stride = m_src_stride;
	int dst_stride = m_dst_stride;
	int theight = m_height;
	int twidth = m_width;
	int stride1, stride2, stride3;
	stride1 = src_stride;
	stride2 = src_stride<<1;
	stride3 = (src_stride<<1)+src_stride;

	__declspec(align(16)) __m128i offset16, zero;
	zero = _mm_setzero_si128();
	offset16 = _mm_set1_epi16(16);

	__declspec(align(16)) __m128i mulm5;
	mulm5 = _mm_set1_epi16(5);

	__declspec(align(16)) __m128i mulm20;
	mulm20 = _mm_set1_epi16(20);

	for( i = 0; i < (twidth); i+=16)
	{
		__declspec(align(16)) __m128i r0_b, r1_b, r2_b, r3_b, r4_b;
		__declspec(align(16)) __m128i r0_l, r1_l, r2_l, r3_l, r4_l;
		__declspec(align(16)) __m128i r0_h, r1_h, r2_h, r3_h, r4_h;

		src_ptr = src;
		dst_ptr = dst;
		r0_b = _mm_load_si128((__m128i*)(src_ptr-stride2));
		r1_b = _mm_load_si128((__m128i*)(src_ptr-stride1));
		r2_b = _mm_load_si128((__m128i*)(src_ptr));
		r3_b = _mm_load_si128((__m128i*)(src_ptr+stride1));
		r4_b = _mm_load_si128((__m128i*)(src_ptr+stride2));

		r0_l = _mm_unpacklo_epi8(r0_b, zero);
		r1_l = _mm_unpacklo_epi8(r1_b, zero);
		r2_l = _mm_unpacklo_epi8(r2_b, zero);
		r3_l = _mm_unpacklo_epi8(r3_b, zero);
		r4_l = _mm_unpacklo_epi8(r4_b, zero);
		r0_h = _mm_unpackhi_epi8(r0_b, zero);
		r1_h = _mm_unpackhi_epi8(r1_b, zero);
		r2_h = _mm_unpackhi_epi8(r2_b, zero);
		r3_h = _mm_unpackhi_epi8(r3_b, zero);
		r4_h = _mm_unpackhi_epi8(r4_b, zero);

		for( j = 0; j < theight; j++)
		{
			__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5, r5_b, temp;
			__declspec(align(16)) __m128i r5_l, r5_h, res_l,res_h;

			r5_b = _mm_load_si128((__m128i*)(src_ptr+stride3));
			r5_l = _mm_unpacklo_epi8(r5_b, zero);
			r5_h = _mm_unpackhi_epi8(r5_b, zero);

			r0 = _mm_adds_epi16(r0_l, r5_l);
			r1 = _mm_adds_epi16(r1_l, r4_l);
			r2 = _mm_adds_epi16(r2_l, r3_l);

			r1 =  _mm_mullo_epi16( r1, mulm5 );
			r2 =  _mm_mullo_epi16( r2, mulm20 );

			r0 = _mm_subs_epi16(r0, r1);
			r0 = _mm_adds_epi16(r0, r2);

			r0 = _mm_adds_epi16(r0, offset16);
			res_l = _mm_srai_epi16(r0, 5);

			r0 = _mm_adds_epi16(r0_h, r5_h);
			r1 = _mm_adds_epi16(r1_h, r4_h);
			r2 = _mm_adds_epi16(r2_h, r3_h);

			r1 =  _mm_mullo_epi16( r1, mulm5 );
			r2 =  _mm_mullo_epi16( r2, mulm20 );

			r0 = _mm_subs_epi16(r0, r1);
			r0 = _mm_adds_epi16(r0, r2);

			r0 = _mm_adds_epi16(r0, offset16);
			res_h = _mm_srai_epi16(r0, 5);

			r0 = _mm_packus_epi16(res_l, res_h);

			_mm_storeu_si128((__m128i*)dst_ptr, r0);

			r0_l = r1_l;
			r1_l = r2_l;
			r2_l = r3_l;
			r3_l = r4_l;
			r4_l = r5_l;

			r0_h = r1_h;
			r1_h = r2_h;
			r2_h = r3_h;
			r3_h = r4_h;
			r4_h = r5_h;

			src_ptr += stride1;
			dst_ptr += dst_stride;
		}
		src += 16;
		dst += 16;
	}
} 

void read_luma_inter_pred_MxN_half_full_tmp_Encoder_intr5( unsigned char *m_pred_src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
{
	BYTE *dst_backup = dst; 
	int j, i;

	const unsigned char* src = m_pred_src;
	const unsigned char* src_ptr;
	unsigned char* dst_ptr;
	int src_stride = m_src_stride;
	int dst_stride = m_dst_stride;
	int theight = m_height;
	int twidth = m_width;
	int stride1, stride2, stride3;
	stride1 = src_stride;
	stride2 = src_stride<<1;
	stride3 = (src_stride<<1)+src_stride;

	__declspec(align(16)) __m128i offset16, zero;
	zero = _mm_setzero_si128();
	offset16 = _mm_set1_epi16(16);

	__declspec(align(16)) __m128i mulm5;
	mulm5 = _mm_set1_epi16(5);

	__declspec(align(16)) __m128i mulm20;
	mulm20 = _mm_set1_epi16(20);

	for( j = 0; j < theight; j++)
	{
		src_ptr = src;
		dst_ptr = dst;
		for( i = 0; i < twidth; i+=16)
		{
			__declspec(align(16)) __m128i r0_b, r1_b, r2_b, r3_b, r4_b, r5_b;
			__declspec(align(16)) __m128i r0_l, r1_l, r2_l, r3_l, r4_l, r5_l;
			__declspec(align(16)) __m128i r0_h, r1_h, r2_h, r3_h, r4_h, r5_h;

			r0_b = _mm_load_si128((__m128i*)(src_ptr-stride2));
			r1_b = _mm_load_si128((__m128i*)(src_ptr-stride1));
			r2_b = _mm_load_si128((__m128i*)(src_ptr));
			r3_b = _mm_load_si128((__m128i*)(src_ptr+stride1));
			r4_b = _mm_load_si128((__m128i*)(src_ptr+stride2));
			r5_b = _mm_load_si128((__m128i*)(src_ptr+stride3));

			r0_l = _mm_unpacklo_epi8(r0_b, zero);
			r1_l = _mm_unpacklo_epi8(r1_b, zero);
			r2_l = _mm_unpacklo_epi8(r2_b, zero);
			r3_l = _mm_unpacklo_epi8(r3_b, zero);
			r4_l = _mm_unpacklo_epi8(r4_b, zero);
			r5_l = _mm_unpacklo_epi8(r5_b, zero);

			r0_h = _mm_unpackhi_epi8(r0_b, zero);
			r1_h = _mm_unpackhi_epi8(r1_b, zero);
			r2_h = _mm_unpackhi_epi8(r2_b, zero);
			r3_h = _mm_unpackhi_epi8(r3_b, zero);
			r4_h = _mm_unpackhi_epi8(r4_b, zero);
			r5_h = _mm_unpackhi_epi8(r5_b, zero);

			__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5;
			r0 = _mm_adds_epi16(r0_l, r5_l);
			r1 = _mm_adds_epi16(r1_l, r4_l);
			r2 = _mm_adds_epi16(r2_l, r3_l);

			r1 =  _mm_mullo_epi16( r1, mulm5 );
			r2 =  _mm_mullo_epi16( r2, mulm20 );

			r0 = _mm_subs_epi16(r0, r1);
			r0 = _mm_adds_epi16(r0, r2);

			r0 = _mm_adds_epi16(r0, offset16);

			__declspec(align(16)) __m128i res_l, res_h;

			res_l = _mm_srai_epi16(r0, 5);

			r0 = _mm_adds_epi16(r0_h, r5_h);
			r1 = _mm_adds_epi16(r1_h, r4_h);
			r2 = _mm_adds_epi16(r2_h, r3_h);

			r1 =  _mm_mullo_epi16( r1, mulm5 );
			r2 =  _mm_mullo_epi16( r2, mulm20 );

			r0 = _mm_subs_epi16(r0, r1);
			r0 = _mm_adds_epi16(r0, r2);

			r0 = _mm_adds_epi16(r0, offset16);
			res_h = _mm_srai_epi16(r0, 5);

			r0 = _mm_packus_epi16(res_l, res_h);

			_mm_storeu_si128((__m128i*)dst_ptr, r0);

			src += 16;
			dst += 16;
		}
		src_ptr += stride1;
		dst_ptr += dst_stride;
	}
} 

void read_luma_inter_pred_MxN_half_full_tmp_Encoder_intr6( unsigned char *m_pred_src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
{
	BYTE *dst_backup = dst; 
	int j, i;

	const unsigned char* src = m_pred_src;
	const unsigned char* src_ptr;
	unsigned char* dst_ptr;
	int src_stride = m_src_stride;
	int dst_stride = m_dst_stride;
	int theight = m_height;
	int twidth = m_width;
	int stride1, stride2, stride3;
	stride1 = src_stride;
	stride2 = src_stride<<1;
	stride3 = (src_stride<<1)+src_stride;

	__declspec(align(16)) __m128i offset16, zero;
	zero = _mm_setzero_si128();
	offset16 = _mm_set1_epi16(16);

	__declspec(align(16)) __m128i mulm5;
	mulm5 = _mm_set1_epi16(5);

	__declspec(align(16)) __m128i mulm20;
	mulm20 = _mm_set1_epi16(20);

	for( i = 0; i < (twidth); i+=8)
	{
		__declspec(align(16)) __m128i r0_b, r1_b, r2_b, r3_b, r4_b;

		src_ptr = src;
		dst_ptr = dst;
		r0_b = _mm_loadl_epi64((__m128i*)(src_ptr-stride2));
		r1_b = _mm_loadl_epi64((__m128i*)(src_ptr-stride1));
		r2_b = _mm_loadl_epi64((__m128i*)(src_ptr));
		r3_b = _mm_loadl_epi64((__m128i*)(src_ptr+stride1));
		r4_b = _mm_loadl_epi64((__m128i*)(src_ptr+stride2));
		r0_b = _mm_unpacklo_epi8(r0_b, zero);
		r1_b = _mm_unpacklo_epi8(r1_b, zero);
		r2_b = _mm_unpacklo_epi8(r2_b, zero);
		r3_b = _mm_unpacklo_epi8(r3_b, zero);
		r4_b = _mm_unpacklo_epi8(r4_b, zero);
		for( j = 0; j < theight; j+=4 )
		{
			__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5, r5_b;
			r5_b = _mm_loadl_epi64((__m128i*)(src_ptr+stride3));
			r5_b = _mm_unpacklo_epi8(r5_b, zero);

			r0 = _mm_adds_epi16(r0_b, r5_b);
			r1 = _mm_adds_epi16(r1_b, r4_b);
			r2 = _mm_adds_epi16(r2_b, r3_b);

			r1 =  _mm_mullo_epi16( r1, mulm5 );
			r2 =  _mm_mullo_epi16( r2, mulm20 );

			r0 = _mm_subs_epi16(r0, r1);
			r0 = _mm_adds_epi16(r0, r2);

			r0 = _mm_adds_epi16(r0, offset16);
			r0 = _mm_srai_epi16(r0, 5);
			r0 = _mm_packus_epi16(r0, zero);

			_mm_storel_epi64((__m128i*)dst_ptr, r0);

			__declspec(align(16)) __m128i r6_b;

			r6_b = _mm_loadl_epi64((__m128i*)(src_ptr+stride2*2));
			r6_b = _mm_unpacklo_epi8(r6_b, zero);

			r0 = _mm_adds_epi16(r1_b, r6_b);
			r1 = _mm_adds_epi16(r2_b, r5_b);
			r2 = _mm_adds_epi16(r3_b, r4_b);

			r1 =  _mm_mullo_epi16( r1, mulm5 );
			r2 =  _mm_mullo_epi16( r2, mulm20 );

			r0 = _mm_subs_epi16(r0, r1);
			r0 = _mm_adds_epi16(r0, r2);

			r0 = _mm_adds_epi16(r0, offset16);
			r0 = _mm_srai_epi16(r0, 5);
			r0 = _mm_packus_epi16(r0, zero);

			_mm_storel_epi64((__m128i*)(dst_ptr+dst_stride), r0);

			__declspec(align(16)) __m128i r7_b;

			r7_b = _mm_loadl_epi64((__m128i*)(src_ptr+stride2+stride3));
			r7_b = _mm_unpacklo_epi8(r7_b, zero);

			r0 = _mm_adds_epi16(r2_b, r7_b);
			r1 = _mm_adds_epi16(r3_b, r6_b);
			r2 = _mm_adds_epi16(r4_b, r5_b);

			r1 =  _mm_mullo_epi16( r1, mulm5 );
			r2 =  _mm_mullo_epi16( r2, mulm20 );

			r0 = _mm_subs_epi16(r0, r1);
			r0 = _mm_adds_epi16(r0, r2);

			r0 = _mm_adds_epi16(r0, offset16);
			r0 = _mm_srai_epi16(r0, 5);
			r0 = _mm_packus_epi16(r0, zero);

			_mm_storel_epi64((__m128i*)(dst_ptr+dst_stride*2), r0);

			__declspec(align(16)) __m128i r8_b;

			r8_b = _mm_loadl_epi64((__m128i*)(src_ptr+stride3*2));
			r8_b = _mm_unpacklo_epi8(r8_b, zero);

			r0 = _mm_adds_epi16(r3_b, r8_b);
			r1 = _mm_adds_epi16(r4_b, r7_b);
			r2 = _mm_adds_epi16(r5_b, r6_b);

			r1 =  _mm_mullo_epi16( r1, mulm5 );
			r2 =  _mm_mullo_epi16( r2, mulm20 );

			r0 = _mm_subs_epi16(r0, r1);
			r0 = _mm_adds_epi16(r0, r2);

			r0 = _mm_adds_epi16(r0, offset16);
			r0 = _mm_srai_epi16(r0, 5);
			r0 = _mm_packus_epi16(r0, zero);

			_mm_storel_epi64((__m128i*)(dst_ptr+dst_stride*3), r0);

			r0_b = r4_b;
			r1_b = r5_b;
			r2_b = r6_b;
			r3_b = r7_b;
			r4_b = r8_b;

			src_ptr += stride2*2;
			dst_ptr += dst_stride*4;
		}
		src += 8;
		dst += 8;
	}
} 



void read_luma_inter_pred_MxN_half_full_tmp_Encoder_intr7( unsigned char *m_pred_src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
{
	BYTE *dst_backup = dst; 
	int j, i;

	const unsigned char* src = m_pred_src;
	const unsigned char* src_ptr;
	unsigned char* dst_ptr;
	int src_stride = m_src_stride;
	int dst_stride = m_dst_stride;
	int theight = m_height;
	int twidth = m_width;
	int stride1, stride2, stride3;
	stride1 = src_stride;
	stride2 = src_stride<<1;
	stride3 = (src_stride<<1)+src_stride;

	__declspec(align(16)) __m128i offset16, zero;
	zero = _mm_setzero_si128();
	offset16 = _mm_set1_epi16(16);

	__declspec(align(16)) __m128i mulm5;
	mulm5 = _mm_set1_epi16(5);

	__declspec(align(16)) __m128i mulm20;
	mulm20 = _mm_set1_epi16(20);

	for( i = 0; i < (twidth); i+=8)
	{
		__declspec(align(16)) __m128i r0_b, r1_b, r2_b, r3_b, r4_b;

		src_ptr = src;
		dst_ptr = dst;
		r0_b = _mm_loadl_epi64((__m128i*)(src_ptr-stride2));
		r1_b = _mm_loadl_epi64((__m128i*)(src_ptr-stride1));
		r2_b = _mm_loadl_epi64((__m128i*)(src_ptr));
		r3_b = _mm_loadl_epi64((__m128i*)(src_ptr+stride1));
		r4_b = _mm_loadl_epi64((__m128i*)(src_ptr+stride2));
		r0_b = _mm_unpacklo_epi8(r0_b, zero);
		r1_b = _mm_unpacklo_epi8(r1_b, zero);
		r2_b = _mm_unpacklo_epi8(r2_b, zero);
		r3_b = _mm_unpacklo_epi8(r3_b, zero);
		r4_b = _mm_unpacklo_epi8(r4_b, zero);
		for( j = 0; j < theight; j+=8 )
		{
			__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5, r5_b;
			r5_b = _mm_loadl_epi64((__m128i*)(src_ptr+stride3));
			r5_b = _mm_unpacklo_epi8(r5_b, zero);

			r0 = _mm_adds_epi16(r0_b, r5_b);
			r1 = _mm_adds_epi16(r1_b, r4_b);
			r2 = _mm_adds_epi16(r2_b, r3_b);

			r1 =  _mm_mullo_epi16( r1, mulm5 );
			r2 =  _mm_mullo_epi16( r2, mulm20 );

			r0 = _mm_subs_epi16(r0, r1);
			r0 = _mm_adds_epi16(r0, r2);

			r0 = _mm_adds_epi16(r0, offset16);
			r0 = _mm_srai_epi16(r0, 5);
			r0 = _mm_packus_epi16(r0, zero);

			_mm_storel_epi64((__m128i*)dst_ptr, r0);

			__declspec(align(16)) __m128i r6_b;

			r6_b = _mm_loadl_epi64((__m128i*)(src_ptr+stride2*2));
			r6_b = _mm_unpacklo_epi8(r6_b, zero);

			r0 = _mm_adds_epi16(r1_b, r6_b);
			r1 = _mm_adds_epi16(r2_b, r5_b);
			r2 = _mm_adds_epi16(r3_b, r4_b);

			r1 =  _mm_mullo_epi16( r1, mulm5 );
			r2 =  _mm_mullo_epi16( r2, mulm20 );

			r0 = _mm_subs_epi16(r0, r1);
			r0 = _mm_adds_epi16(r0, r2);

			r0 = _mm_adds_epi16(r0, offset16);
			r0 = _mm_srai_epi16(r0, 5);
			r0 = _mm_packus_epi16(r0, zero);

			_mm_storel_epi64((__m128i*)(dst_ptr+dst_stride), r0);

			__declspec(align(16)) __m128i r7_b;

			r7_b = _mm_loadl_epi64((__m128i*)(src_ptr+stride2+stride3));
			r7_b = _mm_unpacklo_epi8(r7_b, zero);

			r0 = _mm_adds_epi16(r2_b, r7_b);
			r1 = _mm_adds_epi16(r3_b, r6_b);
			r2 = _mm_adds_epi16(r4_b, r5_b);

			r1 =  _mm_mullo_epi16( r1, mulm5 );
			r2 =  _mm_mullo_epi16( r2, mulm20 );

			r0 = _mm_subs_epi16(r0, r1);
			r0 = _mm_adds_epi16(r0, r2);

			r0 = _mm_adds_epi16(r0, offset16);
			r0 = _mm_srai_epi16(r0, 5);
			r0 = _mm_packus_epi16(r0, zero);

			_mm_storel_epi64((__m128i*)(dst_ptr+dst_stride*2), r0);

			__declspec(align(16)) __m128i r8_b;

			r8_b = _mm_loadl_epi64((__m128i*)(src_ptr+stride3*2));
			r8_b = _mm_unpacklo_epi8(r8_b, zero);

			r0 = _mm_adds_epi16(r3_b, r8_b);
			r1 = _mm_adds_epi16(r4_b, r7_b);
			r2 = _mm_adds_epi16(r5_b, r6_b);

			r1 =  _mm_mullo_epi16( r1, mulm5 );
			r2 =  _mm_mullo_epi16( r2, mulm20 );

			r0 = _mm_subs_epi16(r0, r1);
			r0 = _mm_adds_epi16(r0, r2);

			r0 = _mm_adds_epi16(r0, offset16);
			r0 = _mm_srai_epi16(r0, 5);
			r0 = _mm_packus_epi16(r0, zero);

			_mm_storel_epi64((__m128i*)(dst_ptr+dst_stride*3), r0);

			__declspec(align(16)) __m128i r9_b;

			r9_b = _mm_loadl_epi64((__m128i*)(src_ptr+stride3*2+stride1));
			r9_b = _mm_unpacklo_epi8(r9_b, zero);

			r0 = _mm_adds_epi16(r4_b, r9_b);
			r1 = _mm_adds_epi16(r5_b, r8_b);
			r2 = _mm_adds_epi16(r6_b, r7_b);

			r1 =  _mm_mullo_epi16( r1, mulm5 );
			r2 =  _mm_mullo_epi16( r2, mulm20 );

			r0 = _mm_subs_epi16(r0, r1);
			r0 = _mm_adds_epi16(r0, r2);

			r0 = _mm_adds_epi16(r0, offset16);
			r0 = _mm_srai_epi16(r0, 5);
			r0 = _mm_packus_epi16(r0, zero);

			_mm_storel_epi64((__m128i*)(dst_ptr+dst_stride*4), r0);

			__declspec(align(16)) __m128i r10_b;

			r10_b = _mm_loadl_epi64((__m128i*)(src_ptr+stride2*4));
			r10_b = _mm_unpacklo_epi8(r10_b, zero);

			r0 = _mm_adds_epi16(r5_b, r10_b);
			r1 = _mm_adds_epi16(r6_b, r9_b);
			r2 = _mm_adds_epi16(r7_b, r8_b);

			r1 =  _mm_mullo_epi16( r1, mulm5 );
			r2 =  _mm_mullo_epi16( r2, mulm20 );

			r0 = _mm_subs_epi16(r0, r1);
			r0 = _mm_adds_epi16(r0, r2);

			r0 = _mm_adds_epi16(r0, offset16);
			r0 = _mm_srai_epi16(r0, 5);
			r0 = _mm_packus_epi16(r0, zero);

			_mm_storel_epi64((__m128i*)(dst_ptr+dst_stride*5), r0);

			__declspec(align(16)) __m128i r11_b;

			r11_b = _mm_loadl_epi64((__m128i*)(src_ptr+stride2*4+stride1));
			r11_b = _mm_unpacklo_epi8(r11_b, zero);

			r0 = _mm_adds_epi16(r6_b, r11_b);
			r1 = _mm_adds_epi16(r7_b, r10_b);
			r2 = _mm_adds_epi16(r8_b, r9_b);

			r1 =  _mm_mullo_epi16( r1, mulm5 );
			r2 =  _mm_mullo_epi16( r2, mulm20 );

			r0 = _mm_subs_epi16(r0, r1);
			r0 = _mm_adds_epi16(r0, r2);

			r0 = _mm_adds_epi16(r0, offset16);
			r0 = _mm_srai_epi16(r0, 5);
			r0 = _mm_packus_epi16(r0, zero);

			_mm_storel_epi64((__m128i*)(dst_ptr+dst_stride*6), r0);

			__declspec(align(16)) __m128i r12_b;

			r12_b = _mm_loadl_epi64((__m128i*)(src_ptr+stride3*3+stride1));
			r12_b = _mm_unpacklo_epi8(r12_b, zero);

			r0 = _mm_adds_epi16(r7_b, r12_b);
			r1 = _mm_adds_epi16(r8_b, r11_b);
			r2 = _mm_adds_epi16(r9_b, r10_b);

			r1 =  _mm_mullo_epi16( r1, mulm5 );
			r2 =  _mm_mullo_epi16( r2, mulm20 );

			r0 = _mm_subs_epi16(r0, r1);
			r0 = _mm_adds_epi16(r0, r2);

			r0 = _mm_adds_epi16(r0, offset16);
			r0 = _mm_srai_epi16(r0, 5);
			r0 = _mm_packus_epi16(r0, zero);

			_mm_storel_epi64((__m128i*)(dst_ptr+dst_stride*7), r0);

			r0_b = r8_b;
			r1_b = r9_b;
			r2_b = r10_b;
			r3_b = r11_b;
			r4_b = r12_b;

			src_ptr += stride1*8;
			dst_ptr += dst_stride*8;
		}
		src += 8;
		dst += 8;
	}
} 

void DoInterpolateTest(unsigned char* address1,int stride1, unsigned char* address2,int stride2,unsigned int* orio,unsigned int* newo)
{
	UINT so,eo,sn,en;
	UINT diffo,diffn,diffnc;
	diffo = 0;
	diffn = 0;
	diffnc = 0;
/*	memset( address1, 0, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( char ) );
	memset( address2, 0, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( char ) );
	for(int y=0;y<MB_Y_REF * MB_SIZE;y++)
		for(int i=0;i<MB_X_REF * MB_SIZE;i++)
		{
//			*(signed short*)&address1[y*REF_STRIDE+i] = ( 21 * 512 / 2) - (( y + i ) * 20) % ( 21 * 512);
//			*(signed short*)&address1[y*REF_STRIDE+i] = ( 21 * 512 / 2) - (( y + i ) * 256) % ( 21 * 512);
//			*(signed short*)&address2[y*REF_STRIDE+i] = ( i ) % ( 21 * 512 );
		}*/

	for( int loop=0;loop<LOOP_TEST_COUNT;loop++)
	{
		so = GetTickCount();
		//do a full search motion estimation. This does not require to be correct. It is a speed test
		for( int local_loop=0;local_loop<LOOP_TEST_COUNT_INTERP;local_loop++)
		{
			for( int mby = 1; mby < MB_Y_REF-2; mby++ )
				for( int mbx = 1; mbx < MB_X_REF-2; mbx++ )
				{
					BYTE* refa = address1 + ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE;
					BYTE* cura = (BYTE*)orio + ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE;
//					read_luma_inter_pred_MxN_half_full_tmp_Encoder( refa, stride1, cura, stride2, 16, REF_STRIDE );
					read_luma_inter_pred_MxN_half_full_tmp_Encoder_intr( refa, stride1, cura, stride2, 16, REF_STRIDE );
				}
		}
		eo = GetTickCount();
		diffo += eo - so;

		sn = GetTickCount();
		for( int local_loop=0;local_loop<LOOP_TEST_COUNT_INTERP;local_loop++)
		{
			for( int mby = 1; mby < MB_Y_REF-2; mby++ )
				for( int mbx = 1; mbx < MB_X_REF-2; mbx++ )
				{
					BYTE* refa = address1 + ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE;
					BYTE* cura = (BYTE*)newo + ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE;
//					read_luma_inter_pred_MxN_half_full_tmp_Encoder_intr2( refa, stride1, cura, stride2, 16, REF_STRIDE ); //ver3 is better
//					read_luma_inter_pred_MxN_half_full_tmp_Encoder_intr3( refa, stride1, cura, stride2, 16, REF_STRIDE ); //ver 6 is better
//					read_luma_inter_pred_MxN_half_full_tmp_Encoder_intr4( refa, stride1, cura, stride2, 16, REF_STRIDE ); //ver 3 is better
//					read_luma_inter_pred_MxN_half_full_tmp_Encoder_intr5( refa, stride1, cura, stride2, 16, REF_STRIDE ); //worst version
					read_luma_inter_pred_MxN_half_full_tmp_Encoder_intr6( refa, stride1, cura, stride2, 16, REF_STRIDE );  
//					read_luma_inter_pred_MxN_half_full_tmp_Encoder_intr7( refa, stride1, cura, stride2, 16, REF_STRIDE ); //ver 6 is randomly better
				}
		}
/*
{
BYTE* ta = (BYTE*)orio + ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE;
for(int ty=0;ty<MB_SIZE;ty++)
	for(int tx=0;tx<MB_SIZE;tx++)
		if( cura[ ty * REF_STRIDE + tx ] != ta [ty * REF_STRIDE + tx] )
			printf("error detected here");
}
*/
		en = GetTickCount();
		diffn += en - sn;
/**/
	}

	printf("Interpolate NxN: MS for old algorithm : %u\n",diffo);
	printf("Interpolate NxN: MS for new algorithm : %u\n",diffn);
//	printf("speed diff PCT : %u\n",(diffn)*100/(diffo));
	printf("Interpolate: Results match : %s \n", memcmp( newo,orio, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) ) == 0 ? "true":"false");
	//dump results in text file for investigation
	{
		FILE *f=fopen("old.txt","wt");
		if( f )
		{
			for(int y=MB_SIZE;y<MB_Y_REF * MB_SIZE;y++)
			{
				for(int x=MB_SIZE;x<MB_Y_REF * MB_SIZE;x++)
					fprintf(f,"%06d ",*(short*)&((unsigned char*)orio)[y*REF_STRIDE+x]);
				fprintf(f,"\n");
			}
			fclose(f);
		}
		f=fopen("new.txt","wt");
		if( f )
		{
			for(int y=MB_SIZE;y<MB_Y_REF * MB_SIZE;y++)
			{
				for(int x=MB_SIZE;x<MB_Y_REF * MB_SIZE;x++)
					fprintf(f,"%06d ",*(short*)&((unsigned char*)newo)[y*REF_STRIDE+x]);
				fprintf(f,"\n");
			}
			fclose(f);
		}
		f=fopen("in.txt","wt");
		if( f )
		{
			for(int y=MB_SIZE;y<MB_Y_REF * MB_SIZE;y++)
			{
				for(int x=MB_SIZE;x<MB_Y_REF * MB_SIZE;x+=2)
				{
					unsigned char *base = &address1[y*REF_STRIDE+x];
					fprintf(f,"%06d ",*(signed short*)base);
				}
				fprintf(f,"\n");
			}
			fclose(f);
		}
	}
}

int main()
{
	volatile int stride = MB_X_REF * MB_SIZE;
	BYTE* address1 = (BYTE*)malloc( MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) );
	BYTE* address2 = (BYTE*)malloc( MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) );
	UINT* orio = (UINT*)malloc( MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) );
	UINT* newo = (UINT*)malloc( MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) );
	memset( orio, 0, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) );
	memset( newo, 0, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) );
	for(int i=0;i<MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE;i++)
	{
		address1[i] = i+1;
		address2[i] = i;
	}

	DoInterpolateTest( address1, stride, address2, stride, orio, newo );

	getch();
	free( address1 );
	free( address2 );
	free( orio );
	free( newo );
	return 0;

}