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

void read_luma_inter_pred_MxN_half_full_tmp_Encoder( unsigned char *src, int src_stride, short *dst, int m_dst_stride, int m_height, int m_width)
{
	int i,j;
#define CLIP3(a,b,c)	((c) < (a) ? (a) : ((c) > (b) ? (b) : (c))) // MIN, MAX, val
#define CLIP1(x)		(CLIP3( 0, 255, (x) ))
	unsigned char *s=src+2;
	for( i = 0; i < m_height; i++ )
	{
		for( j = 0; j < m_width; j++ )
		{
			dst[j] = s[j-2] + s[j+3] - 5*( s[j-1] + s[j+2] ) + 20*( s[j] + s[j+1] );
		}
		s += m_dst_stride;
		dst += m_dst_stride;
	}

}

//this is wrong in many ways, do not use it !
void read_luma_inter_pred_MxN_half_full_tmp_Encoder_intr( unsigned char *m_pred_src, int m_src_stride, short *dst, int m_dst_stride, int m_height, int m_width)
{
	short *dst_backup = dst; 

	int j, i, k;

	const unsigned char* src = m_pred_src-(m_src_stride<<1);
	const unsigned char* src_ptr;
	short* dst_ptr;
	int src_stride = m_src_stride;
	int dst_stride = m_dst_stride;
	int theight = m_height;
	int twidth = m_width;
	int stride1, stride2, stride3;
	stride1 = src_stride;

	for( i = 0; i < (theight); i++){

		__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5, offset16, temp, zero;
		zero = _mm_setzero_si128();
		//zero = _mm_xor_si128(zero, zero);
		offset16 = _mm_set1_epi16(16);

		src_ptr = src;
		dst_ptr = dst;

		for( j = 0; j < (twidth); j+=8){

			r0 = _mm_loadu_si128((__m128i*)(src_ptr-2));
			r1 = _mm_srli_si128(r0, 1);
			r2 = _mm_srli_si128(r0, 2);
			r3 = _mm_srli_si128(r0, 3);
			r4 = _mm_srli_si128(r0, 4);
			r5 = _mm_srli_si128(r0, 5);
			r0 = _mm_unpacklo_epi8(r0, zero);
			r1 = _mm_unpacklo_epi8(r1, zero);
			r2 = _mm_unpacklo_epi8(r2, zero);
			r3 = _mm_unpacklo_epi8(r3, zero);
			r4 = _mm_unpacklo_epi8(r4, zero);
			r5 = _mm_unpacklo_epi8(r5, zero);

			r0 = _mm_adds_epi16(r0, r5);
			r1 = _mm_adds_epi16(r1, r4);
			r2 = _mm_adds_epi16(r2, r3);
			temp = r1;
			r1 = _mm_slli_epi16(r1, 2);
			r1 = _mm_adds_epi16(r1, temp);
			temp = _mm_slli_epi16(r2, 2);
			r2 = _mm_slli_epi16(r2, 4);
			r2 = _mm_adds_epi16(r2, temp);
			r0 = _mm_subs_epi16(r0, r1);
			r0 = _mm_adds_epi16(r0, r2);
			_mm_storeu_si128((__m128i*)dst_ptr, r0);

			src_ptr += 8;
			dst_ptr += 8;

		}
		src += stride1;
		dst += dst_stride;
	}
} 

//this is wrong in many ways, do not use it !
void read_luma_inter_pred_MxN_half_full_tmp_Encoder_intr2( unsigned char *m_pred_src, int m_src_stride, short *dst, int m_dst_stride, int m_height, int m_width)
{
	short *dst_backup = dst; 

	int j, i, k;

	__declspec(align(16)) short magic_mul[8] = {1,-5,20,20,-5,1,0,0};

	const unsigned char* src = m_pred_src-(m_src_stride<<1)-2;
	const unsigned char* src_ptr;
	short* dst_ptr;
	int src_stride = m_src_stride;
	int dst_stride = m_dst_stride;
	int theight = m_height;
	int twidth = m_width;
	int stride1, stride2, stride3;
	stride1 = src_stride;

	for( i = 0; i < (theight); i++){

		__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5, temp, zero;
		zero = _mm_setzero_si128();

		__declspec(align(16)) __m128i multi;
		multi = _mm_loadu_si128((__m128i*)(magic_mul));

		src_ptr = src;
		dst_ptr = dst;

		for( j = 0; j < (twidth); j+=2)
		{
			r0 = _mm_loadu_si128((__m128i*)(src_ptr));
			//convert from uint8 to uint 16
			r1 = _mm_unpacklo_epi8(r0, zero);
			r2 = _mm_srli_si128(r1, 1);
//			r2 = _mm_unpackhi_epi8(r0, zero);
			//multiply and add
			r3 = _mm_madd_epi16( r1, multi );
			r4 = _mm_madd_epi16( r2, multi );
			//convert result from 32 to 16 bits			
			r5 = _mm_packs_epi32(r3, r4);
			//add remaining sums
			r0 = _mm_srli_si128(r5, 2);
			r1 = _mm_srli_si128(r5, 4);
			r2 = _mm_add_epi16(r5, r0);
			r3 = _mm_add_epi16(r1, r2);
			_mm_storel_epi64((__m128i*)dst_ptr, r3);
			r3 = _mm_srli_si128(r3, 4);
			_mm_storel_epi64((__m128i*)dst_ptr+2, r3);

			src_ptr += 2;
			dst_ptr += 2;

		}
		src += stride1;
		dst += dst_stride;
	}
} 

//this is wrong in many ways, do not use it !
void read_luma_inter_pred_MxN_half_full_tmp_Encoder_intr3( unsigned char *m_pred_src, int m_src_stride, short *dst, int m_dst_stride, int m_height, int m_width)
{
	short *dst_backup = dst; 

	int j, i, k;

	const unsigned char* src = m_pred_src-(m_src_stride<<1);
	const unsigned char* src_ptr;
	short* dst_ptr;
	int src_stride = m_src_stride;
	int dst_stride = m_dst_stride;
	int theight = m_height;
	int twidth = m_width;
	int stride1, stride2, stride3;
	stride1 = src_stride;

	__declspec(align(16)) __m128i mulm5;
	mulm5 = _mm_set1_epi32(5);

	__declspec(align(16)) __m128i mulm20;
	mulm20 = _mm_set1_epi32(20);


	for( i = 0; i < (theight); i++){

		__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5, temp, zero;
		zero = _mm_setzero_si128();

		src_ptr = src;
		dst_ptr = dst;

		for( j = 0; j < (twidth); j+=4)
		{
			r0 = _mm_loadu_si128((__m128i*)(src_ptr-2));
			r1 = _mm_srli_si128(r0, 1);
			r2 = _mm_srli_si128(r0, 2);
			r3 = _mm_srli_si128(r0, 3);
			r4 = _mm_srli_si128(r0, 4);
			r5 = _mm_srli_si128(r0, 5);
			r0 = _mm_unpacklo_epi8(r0, zero);
			r1 = _mm_unpacklo_epi8(r1, zero);
			r2 = _mm_unpacklo_epi8(r2, zero);
			r3 = _mm_unpacklo_epi8(r3, zero);
			r4 = _mm_unpacklo_epi8(r4, zero);
			r5 = _mm_unpacklo_epi8(r5, zero);

			r1 = _mm_madd_epi16( r1, mulm5 );
			r4 = _mm_madd_epi16( r4, mulm5 );

			r2 = _mm_madd_epi16( r2, mulm20 );
			r3 = _mm_madd_epi16( r3, mulm20 );

			r1 = _mm_packs_epi32(r1, r1);
			r4 = _mm_packs_epi32(r4, r4);
			r2 = _mm_packs_epi32(r2, r2);
			r3 = _mm_packs_epi32(r3, r3);

			r1 = _mm_adds_epi16(r1, r2);
			r3 = _mm_adds_epi16(r3, r4);
			r5 = _mm_adds_epi16(r5, r1);
			r5 = _mm_adds_epi16(r5, r3);

			_mm_storel_epi64((__m128i*)dst_ptr, r5);

			src_ptr += 4;
			dst_ptr += 4;

		}
		src += stride1;
		dst += dst_stride;
	}
} 

//this is wrong in many ways, do not use it !
void read_luma_inter_pred_MxN_half_full_tmp_Encoder_intr4( unsigned char *m_pred_src, int m_src_stride, short *dst, int m_dst_stride, int m_height, int m_width)
{
	short *dst_backup = dst; 

	int j, i, k;

	const unsigned char* src = m_pred_src-(m_src_stride<<1)-2;
	const unsigned char* src_ptr;
	short* dst_ptr;
	int src_stride = m_src_stride;
	int dst_stride = m_dst_stride;
	int theight = m_height;
	int twidth = m_width;
	int stride1, stride2, stride3;
	stride1 = src_stride;

	__declspec(align(16)) __m128i mulm5;
	mulm5 = _mm_set1_epi16(-5);

	__declspec(align(16)) __m128i mulm20;
	mulm20 = _mm_set1_epi16(20);


	for( i = 0; i < (theight); i++){

		__declspec(align(16)) __m128i zero;
		zero = _mm_setzero_si128();

		src_ptr = src;
		dst_ptr = dst;

		for( j = 0; j < (twidth); j+=8)
		{
			__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5, temp;
			r0 = _mm_loadu_si128((__m128i*)(src_ptr));
			r1 = _mm_srli_si128(r0, 1);
			r2 = _mm_srli_si128(r0, 2);
			r3 = _mm_srli_si128(r0, 3);
			r4 = _mm_srli_si128(r0, 4);
			r5 = _mm_srli_si128(r0, 5);
			r0 = _mm_unpacklo_epi8(r0, zero);
			r5 = _mm_unpacklo_epi8(r5, zero);
			r1 = _mm_unpacklo_epi8(r1, zero);
			r4 = _mm_unpacklo_epi8(r4, zero);
			r2 = _mm_unpacklo_epi8(r2, zero);
			r3 = _mm_unpacklo_epi8(r3, zero);

			__declspec(align(16)) __m128i r1H, r2H;
			r1H = _mm_unpackhi_epi16(r1, r4);
			r2H = _mm_unpackhi_epi16(r2, r3);
			r1 = _mm_unpacklo_epi16(r1, r4);
			r2 = _mm_unpacklo_epi16(r2, r3);

			r1 = _mm_madd_epi16( r1, mulm5 );
			r1H = _mm_madd_epi16( r1H, mulm5 );
			r2 = _mm_madd_epi16( r2, mulm20 );
			r2H = _mm_madd_epi16( r2H, mulm20 );

			r1 = _mm_packs_epi32(r1, r1H);
			r2 = _mm_packs_epi32(r2, r2H);

			r0 = _mm_adds_epi16(r0, r5);
			r1 = _mm_adds_epi16(r1, r2);
			r0 = _mm_adds_epi16(r0, r1);

			_mm_storeu_si128((__m128i*)dst_ptr, r0);

			src_ptr += 8;
			dst_ptr += 8;

		}
		src += stride1;
		dst += dst_stride;
	}
} 

void read_luma_inter_pred_MxN_half_full_tmp_Encoder_intr5( unsigned char *m_pred_src, int m_src_stride, short *dst, int m_dst_stride, int m_height, int m_width)
{
	short *dst_backup = dst; 

	int j, i, k;

	const unsigned char* src = m_pred_src-(m_src_stride<<1)-2;
	const unsigned char* src_ptr;
	short* dst_ptr;
	int src_stride = m_src_stride;
	int dst_stride = m_dst_stride;
	int theight = m_height;
	int twidth = m_width;
	int stride1, stride2, stride3;
	stride1 = src_stride;

	__declspec(align(16)) __m128i mulm5;
	mulm5 = _mm_set1_epi16(5);

	__declspec(align(16)) __m128i mulm20;
	mulm20 = _mm_set1_epi16(20);

	__declspec(align(16)) __m128i zero;
	zero = _mm_setzero_si128();

	for( i = 0; i < (theight); i++)
	{
		src_ptr = src;
		dst_ptr = dst;
		for( j = 0; j < (twidth); j+=8)
		{
			__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5, temp;
			r0 = _mm_loadu_si128((__m128i*)(src_ptr));
			r1 = _mm_srli_si128(r0, 1);
			r2 = _mm_srli_si128(r0, 2);
			r3 = _mm_srli_si128(r0, 3);
			r4 = _mm_srli_si128(r0, 4);
			r5 = _mm_srli_si128(r0, 5);
			r0 = _mm_unpacklo_epi8(r0, zero);
			r1 = _mm_unpacklo_epi8(r1, zero);
			r2 = _mm_unpacklo_epi8(r2, zero);
			r3 = _mm_unpacklo_epi8(r3, zero);
			r4 = _mm_unpacklo_epi8(r4, zero);
			r5 = _mm_unpacklo_epi8(r5, zero);

			r0 = _mm_adds_epi16(r0, r5);
			r1 = _mm_adds_epi16(r1, r4);
			r2 = _mm_adds_epi16(r2, r3);

			r1 =  _mm_mullo_epi16( r1, mulm5 );
			r2 =  _mm_mullo_epi16( r2, mulm20 );

			r0 = _mm_subs_epi16(r0, r1);
			r0 = _mm_adds_epi16(r0, r2);

			_mm_storeu_si128((__m128i*)dst_ptr, r0);

			src_ptr += 8;
			dst_ptr += 8;

		}
		src += stride1;
		dst += dst_stride;
	}
} 

void read_luma_inter_pred_MxN_half_full_tmp_Encoder_intr6( unsigned char *m_pred_src, int m_src_stride, short *dst, int m_dst_stride, int m_height, int m_width)
{
	short *dst_backup = dst; 

	int j, i, k;

	const unsigned char* src = m_pred_src-(m_src_stride<<1)-2;
	const unsigned char* src_ptr;
	short* dst_ptr;
	int src_stride = m_src_stride;
	int dst_stride = m_dst_stride;
	int theight = m_height;
	int twidth = m_width;
	int stride1, stride2, stride3;
	stride1 = src_stride;

	__declspec(align(16)) __m128i mulm5;
	mulm5 = _mm_set1_epi16(5);

	__declspec(align(16)) __m128i mulm20;
	mulm20 = _mm_set1_epi16(20);

	__declspec(align(16)) __m128i zero;
	zero = _mm_setzero_si128();

	for( i = 0; i < (theight); i++)
	{
		src_ptr = src;
		dst_ptr = dst;
		for( j = 0; j < (twidth); j+=16)
		{
			__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5;
			r0 = _mm_loadu_si128((__m128i*)(src_ptr+0));
			r1 = _mm_loadu_si128((__m128i*)(src_ptr+1));
			r2 = _mm_loadu_si128((__m128i*)(src_ptr+2));
			r3 = _mm_loadu_si128((__m128i*)(src_ptr+3));
			r4 = _mm_loadu_si128((__m128i*)(src_ptr+4));
			r5 = _mm_loadu_si128((__m128i*)(src_ptr+5));
			__declspec(align(16)) __m128i r0h, r1h, r2h, r3h, r4h, r5h;
			r0h = _mm_unpackhi_epi8(r0, zero);
			r1h = _mm_unpackhi_epi8(r1, zero);
			r2h = _mm_unpackhi_epi8(r2, zero);
			r3h = _mm_unpackhi_epi8(r3, zero);
			r4h = _mm_unpackhi_epi8(r4, zero);
			r5h = _mm_unpackhi_epi8(r5, zero);

			r0 = _mm_unpacklo_epi8(r0, zero);
			r1 = _mm_unpacklo_epi8(r1, zero);
			r2 = _mm_unpacklo_epi8(r2, zero);
			r3 = _mm_unpacklo_epi8(r3, zero);
			r4 = _mm_unpacklo_epi8(r4, zero);
			r5 = _mm_unpacklo_epi8(r5, zero);

			r0 = _mm_adds_epi16(r0, r5);
			r1 = _mm_adds_epi16(r1, r4);
			r2 = _mm_adds_epi16(r2, r3);

			r1 =  _mm_mullo_epi16( r1, mulm5 );
			r2 =  _mm_mullo_epi16( r2, mulm20 );

			r0 = _mm_subs_epi16(r0, r1);
			r0 = _mm_adds_epi16(r0, r2);

			_mm_storeu_si128((__m128i*)dst_ptr, r0);

			r0 = _mm_adds_epi16(r0h, r5h);
			r1 = _mm_adds_epi16(r1h, r4h);
			r2 = _mm_adds_epi16(r2h, r3h);

			r1 =  _mm_mullo_epi16( r1, mulm5 );
			r2 =  _mm_mullo_epi16( r2, mulm20 );

			r0 = _mm_subs_epi16(r0, r1);
			r0 = _mm_adds_epi16(r0, r2);

			_mm_storeu_si128((__m128i*)(dst_ptr+8), r0);
			src_ptr += 16;
			dst_ptr += 16;
		}
		src += stride1;
		dst += dst_stride;
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
//					read_luma_inter_pred_MxN_half_full_tmp_Encoder( (short*)refa, stride1, (unsigned char*)cura, stride2, 16, REF_STRIDE );
					read_luma_inter_pred_MxN_half_full_tmp_Encoder_intr( refa, stride1, (short*)cura, stride2, 16, REF_STRIDE );
//printf("t");
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
//					read_luma_inter_pred_MxN_half_full_tmp_Encoder_intr5( refa, stride1, (short*)cura, stride2, 16, REF_STRIDE );
					read_luma_inter_pred_MxN_half_full_tmp_Encoder_intr6( refa, stride1, (short*)cura, stride2, 16, REF_STRIDE );
//printf("t");
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