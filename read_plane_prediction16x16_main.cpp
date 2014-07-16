#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <assert.h>
#include <conio.h>
#include <emmintrin.h>
//#include <tmmintrin.h>
//#include "tmmintrin.h"

#define MB_X_REF 68
#define MB_Y_REF 45
#define MB_SIZE 16
#define REF_STRIDE (MB_X_REF*MB_SIZE)
#ifdef _DEBUG
	#define LOOP_TEST_COUNT	1	//to have a larger runtime
	#define LOOP_TEST_COUNT_INTERP	1	//to have a larger runtime
#else
	#define LOOP_TEST_COUNT	10	//to have a larger runtime
	#define LOOP_TEST_COUNT_INTERP	600	//to have a larger runtime
#endif

#define CLIP3(a,b,c)	((c) < (a) ? (a) : ((c) > (b) ? (b) : (c))) // MIN, MAX, val
#define CLIP1(x)		(CLIP3( 0, 255, (x) ))

void read_plane_prediction16x16( BYTE *dst, BYTE *m_UpperPels, BYTE *m_LeftPels, INT m_dst_stride, SHORT P_X )
{
	INT i, j;
	INT a, b, c;
	INT H = 0, V = 0;

	a = 16*(m_UpperPels[15] + m_LeftPels[15]);

	for( i = 1; i < 8; i++ )
	{
		H += i * (m_UpperPels[7+i] - m_UpperPels[7-i]);
		V += i * (m_LeftPels[7+i] - m_LeftPels[7-i]);
	}

	H += 8*(m_UpperPels[15] - P_X);
	V += 8*(m_LeftPels[15] - P_X);

	b = (5*H + 32) >> 6;
	c = (5*V + 32) >> 6;

	for( i = 0; i < 16; i++ )
	{
		for( j = 0; j < 16; j++ )
		{
			dst[j] = CLIP1( ((a + b*(j-7) + c*(i-7) + 16) >> 5) );
		}

		dst += m_dst_stride;
	}
}

void read_plane_prediction16x16_intr1( BYTE *dst, BYTE *m_UpperPels, BYTE *m_LeftPels, INT m_dst_stride, SHORT P_X )
{
	INT i, j;
	INT a, b, c;
//	INT H = 0, V = 0;
	__declspec(align(16)) INT H[4], V[4];

	__declspec(align(16)) __m128i zero;
	zero = _mm_setzero_si128();

	//load values and convert to uint16
	__declspec(align(16)) __m128i upH,upLr,lpH,lpLr;
	upH = _mm_loadl_epi64((__m128i*)( &m_UpperPels[8] ));	//load 8 bytes
	upH = _mm_unpacklo_epi8(upH, zero);
	upLr =  _mm_setr_epi8( m_UpperPels[6],0,m_UpperPels[5],0,m_UpperPels[4],0,m_UpperPels[3],0,m_UpperPels[2],0,m_UpperPels[1],0,m_UpperPels[0],0,P_X,0);
	lpH = _mm_loadl_epi64((__m128i*)( &m_LeftPels[8] ));	//load 8 bytes
	lpH = _mm_unpacklo_epi8(lpH, zero);
	lpLr =  _mm_setr_epi8( m_LeftPels[6],0,m_LeftPels[5],0,m_LeftPels[4],0,m_LeftPels[3],0,m_LeftPels[2],0,m_LeftPels[1],0,m_LeftPels[0],0,P_X,0);
	
	//substract
	__declspec(align(16)) __m128i upHsL,lpHsL;
	upHsL = _mm_sub_epi16( upH, upLr );
	lpHsL = _mm_sub_epi16( lpH, lpLr );

	{
		const __declspec(align(16)) short c_mult[8] = { 1,2,3,4,5,6,7,8 };
		__declspec(align(16)) __m128i mult;
		mult = _mm_load_si128((__m128i*)( c_mult ));
	
		//multiply and add half of the values
		__declspec(align(16)) __m128i hsH,hsV;
		hsH = _mm_madd_epi16( upHsL, mult );
		hsV = _mm_madd_epi16( lpHsL, mult );

		//shift and add the second half of the values
		__declspec(align(16)) __m128i hsHs,hsVs;
		hsHs = _mm_srli_si128( hsH, 8 );
		hsVs = _mm_srli_si128( hsV, 8 );

		//sum lower and higher parts
		hsH = _mm_add_epi32(hsH, hsHs);
		hsV = _mm_add_epi32(hsV, hsVs);
		//shift and sum again
		hsHs = _mm_srli_si128( hsH, 4 );
		hsVs = _mm_srli_si128( hsV, 4 );
		__declspec(align(16)) __m128i Hi,Vi;
		Hi = _mm_add_epi32(hsH, hsHs);
		Vi = _mm_add_epi32(hsV, hsVs);
		//store result
		_mm_storel_epi64((__m128i*)H,Hi);
		_mm_storel_epi64((__m128i*)V,Vi);
	}/**/

/*	{
		H += 1 * (m_UpperPels[8] - m_UpperPels[6]);
		H += 2 * (m_UpperPels[9] - m_UpperPels[5]);
		H += 3 * (m_UpperPels[10] - m_UpperPels[4]);
		H += 4 * (m_UpperPels[11] - m_UpperPels[3]);
		H += 5 * (m_UpperPels[12] - m_UpperPels[2]);
		H += 6 * (m_UpperPels[13] - m_UpperPels[1]);
		H += 7 * (m_UpperPels[14] - m_UpperPels[0]);
		H += 8 * (m_UpperPels[15] - P_X);
		V += 1 * (m_LeftPels[8] - m_LeftPels[6]);
		V += 2 * (m_LeftPels[9] - m_LeftPels[5]);
		V += 3 * (m_LeftPels[10] - m_LeftPels[4]);
		V += 4 * (m_LeftPels[11] - m_LeftPels[3]);
		V += 5 * (m_LeftPels[12] - m_LeftPels[2]);
		V += 6 * (m_LeftPels[13] - m_LeftPels[1]);
		V += 7 * (m_LeftPels[14] - m_LeftPels[0]);
		V += 8 * (m_LeftPels[15] - P_X);
	}/**/

	a = 16*(m_UpperPels[15] + m_LeftPels[15]);

	b = (5*H[0] + 32) >> 6;
	c = (5*V[0] + 32) >> 6;

	__declspec(align(16)) __m128i sixteen;
	sixteen = _mm_set1_epi16(16);

	__declspec(align(16)) __m128i a_vect;
	a_vect = _mm_set1_epi16(a);
	a_vect = _mm_add_epi16(a_vect, sixteen);

	__declspec(align(16)) short bm1[8];
	__declspec(align(16)) short bm2[8];
	bm1[0] = b*(-7);	bm1[1] = b*(-6);	bm1[2] = b*(-5);	bm1[3] = b*(-4);
	bm1[4] = b*(-3);	bm1[5] = b*(-2);	bm1[6] = b*(-1);	bm1[7] = 0;
	bm2[0] = b*1;		bm2[1] = b*2;		bm2[2] = b*3;		bm2[3] = b*4;
	bm2[4] = b*5;		bm2[5] = b*6;		bm2[6] = b*7;		bm2[7] = b*8;

	__declspec(align(16)) __m128i b_vect1,b_vect2;
	b_vect1 = _mm_load_si128((__m128i*)( bm1 ));
	b_vect2 = _mm_load_si128((__m128i*)( bm2 ));

	b_vect1 = _mm_add_epi16(b_vect1, a_vect);
	b_vect2 = _mm_add_epi16(b_vect2, a_vect);

	for( i = 0; i < 16; i++ )
	{

//		for( j = 0; j < 16; j++ )
		{
			INT lc = c*(i-7);
			__declspec(align(16)) __m128i c_vect;
			c_vect = _mm_set1_epi16(lc);

			__declspec(align(16)) __m128i r1,r2;
			r1 = _mm_add_epi16(b_vect1, c_vect);
			r2 = _mm_add_epi16(b_vect2, c_vect);
			r1 = _mm_srai_epi16( r1, 5 );
			r2 = _mm_srai_epi16( r2, 5 );
			r1 = _mm_max_epi16( r1 , zero );		//clip1
			r2 = _mm_max_epi16( r2 , zero );		//clip1
			r1 = _mm_packus_epi16( r1, r2 );		//clip1
			_mm_store_si128((__m128i*)dst,r1);

/*			dst[0] = CLIP1( ((a + b*(-7) + 16 + c*(i-7)) >> 5) );
			dst[1] = CLIP1( ((a + b*(-6) + 16 + c*(i-7)) >> 5) );
			dst[2] = CLIP1( ((a + b*(-5) + 16 + c*(i-7)) >> 5) );
			dst[3] = CLIP1( ((a + b*(-4) + 16 + c*(i-7)) >> 5) );
			dst[4] = CLIP1( ((a + b*(-3) + 16 + c*(i-7)) >> 5) );
			dst[5] = CLIP1( ((a + b*(-2) + 16 + c*(i-7)) >> 5) );
			dst[6] = CLIP1( ((a + b*(-1) + 16 + c*(i-7)) >> 5) );
			dst[7] = CLIP1( ((a + b*(0) + 16 + c*(i-7) ) >> 5) );
			dst[8] = CLIP1( ((a + b*(1) + 16 + c*(i-7) ) >> 5) );
			dst[9] = CLIP1( ((a + b*(2) + 16 + c*(i-7) ) >> 5) );
			dst[10] = CLIP1( ((a + b*(3) + 16 + c*(i-7)) >> 5) );
			dst[11] = CLIP1( ((a + b*(4) + 16 + c*(i-7)) >> 5) );
			dst[12] = CLIP1( ((a + b*(5) + 16 + c*(i-7)) >> 5) );
			dst[13] = CLIP1( ((a + b*(6) + 16 + c*(i-7)) >> 5) );
			dst[14] = CLIP1( ((a + b*(7) + 16 + c*(i-7)) >> 5) );
			dst[15] = CLIP1( ((a + b*(8) + 16 + c*(i-7)) >> 5) );/**/
//			dst[j] = CLIP1( ((a + b*(j-7) + c*(i-7) + 16) >> 5) );
		}
		dst += m_dst_stride;
	}
}

void read_plane_prediction16x16_intr2( BYTE *dst, BYTE *m_UpperPels, BYTE *m_LeftPels, INT m_dst_stride, SHORT P_X )
{
	INT i, j;
	INT a, b, c;

	__declspec(align(16)) __m128i zero;
	zero = _mm_setzero_si128();

	//load values and convert to uint16
	__declspec(align(16)) __m128i upH,upLr,lpH,lpLr;
	upH = _mm_loadl_epi64((__m128i*)( &m_UpperPels[8] ));	//load 8 bytes
	upH = _mm_unpacklo_epi8(upH, zero);
	upLr =  _mm_setr_epi8( m_UpperPels[6],0,m_UpperPels[5],0,m_UpperPels[4],0,m_UpperPels[3],0,m_UpperPels[2],0,m_UpperPels[1],0,m_UpperPels[0],0,P_X,0);
	lpH = _mm_loadl_epi64((__m128i*)( &m_LeftPels[8] ));	//load 8 bytes
	lpH = _mm_unpacklo_epi8(lpH, zero);
	lpLr =  _mm_setr_epi8( m_LeftPels[6],0,m_LeftPels[5],0,m_LeftPels[4],0,m_LeftPels[3],0,m_LeftPels[2],0,m_LeftPels[1],0,m_LeftPels[0],0,P_X,0);
	
	//substract
	__declspec(align(16)) __m128i upHsL,lpHsL;
	upHsL = _mm_sub_epi16( upH, upLr );
	lpHsL = _mm_sub_epi16( lpH, lpLr );

	const __declspec(align(16)) short c_mult[8] = { 1,2,3,4,5,6,7,8 };
	__declspec(align(16)) __m128i mult;
	mult = _mm_load_si128((__m128i*)( c_mult ));

	//multiply and add half of the values
	__declspec(align(16)) __m128i hsH,hsV;
	hsH = _mm_madd_epi16( upHsL, mult );
	hsV = _mm_madd_epi16( lpHsL, mult );

	__declspec(align(16)) INT H[4], V[4];
	_mm_store_si128((__m128i*)H,hsH);
	_mm_store_si128((__m128i*)V,hsV);

	a = 16*(m_UpperPels[15] + m_LeftPels[15]);

	b = (5*(H[0]+H[1]+H[2]+H[3]) + 32) >> 6;
	c = (5*(V[0]+V[1]+V[2]+V[3]) + 32) >> 6;

	__declspec(align(16)) __m128i sixteen;
	sixteen = _mm_set1_epi16(16);

	__declspec(align(16)) __m128i a_vect;
	a_vect = _mm_set1_epi16(a);
	a_vect = _mm_add_epi16(a_vect, sixteen);

	__declspec(align(16)) short bm1[8];
	__declspec(align(16)) short bm2[8];
	bm1[0] = b*(-7);	bm1[1] = b*(-6);	bm1[2] = b*(-5);	bm1[3] = b*(-4);
	bm1[4] = b*(-3);	bm1[5] = b*(-2);	bm1[6] = b*(-1);	bm1[7] = 0;
	bm2[0] = b*1;		bm2[1] = b*2;		bm2[2] = b*3;		bm2[3] = b*4;
	bm2[4] = b*5;		bm2[5] = b*6;		bm2[6] = b*7;		bm2[7] = b*8;

	__declspec(align(16)) __m128i b_vect1,b_vect2;
	b_vect1 = _mm_load_si128((__m128i*)( bm1 ));
	b_vect2 = _mm_load_si128((__m128i*)( bm2 ));

	b_vect1 = _mm_add_epi16(b_vect1, a_vect);
	b_vect2 = _mm_add_epi16(b_vect2, a_vect);

	__declspec(align(16)) __m128i c_vect,c_vect_add;
	c_vect = _mm_set1_epi16(c*(-7));
	c_vect_add = _mm_set1_epi16(c);
	for( i = 0; i < 16; i++ )
	{
		__declspec(align(16)) __m128i r1,r2;
		r1 = _mm_add_epi16(b_vect1, c_vect);
		r2 = _mm_add_epi16(b_vect2, c_vect);
		r1 = _mm_srai_epi16( r1, 5 );
		r2 = _mm_srai_epi16( r2, 5 );
		r1 = _mm_max_epi16( r1 , zero );		//clip1
		r2 = _mm_max_epi16( r2 , zero );		//clip1
		r1 = _mm_packus_epi16( r1, r2 );		//clip1
		_mm_store_si128((__m128i*)dst,r1);
		dst += m_dst_stride;
		c_vect = _mm_add_epi16(c_vect,c_vect_add);
	}
}

void DoInterpolateTest(unsigned char* address1,int stride1, unsigned char* address2,int stride2,unsigned char* orio,unsigned char* newo)
{
	UINT so,eo,sn,en;
	UINT diffo,diffn,diffnc;
	diffo = 0;
	diffn = 0;
	diffnc = 0;

	//just for debugging 
#define bugstart_y 0
#define bugstart_x 0
	address1 = address1 + REF_STRIDE * bugstart_y + bugstart_x;
	address2 = address2 + REF_STRIDE * bugstart_y + bugstart_x;
	orio = orio + REF_STRIDE * bugstart_y + bugstart_x;
	newo = newo + REF_STRIDE * bugstart_y + bugstart_x;
	/**/

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
					read_plane_prediction16x16( refa, cura, cura, stride1, 1 );
//					read_plane_prediction16x16_intr1( refa, cura, cura, stride1, 1 );
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
					BYTE* refa = address2 + ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE;
					BYTE* cura = (BYTE*)newo + ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE;
					read_plane_prediction16x16_intr2( refa, cura, cura, stride1, 1 );
				}
		}
		en = GetTickCount();
		diffn += en - sn;
/**/
	}

	printf("Old algorithm will perform statistically 25% better then here\n");
	printf("Interpolate NxN: MS for old algorithm : %u\n",diffo);
	printf("Interpolate NxN: MS for new algorithm : %u\n",diffn);
//	printf("speed diff PCT : %u\n",(diffn)*100/(diffo));
	printf("Interpolate: Results match : %s \n", memcmp( newo,orio, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) ) == 0 ? "true":"false");
	printf("Interpolate: Results match 2 : %s \n", memcmp( address1,address2, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) ) == 0 ? "true":"false");
	//dump results in text file for investigation
	{
		FILE *f=fopen("old_p.txt","wt");
		if( f )
		{
			for(int y=MB_SIZE;y<MB_Y_REF * MB_SIZE;y++)
			{
				for(int x=MB_SIZE;x<MB_Y_REF * MB_SIZE;x++)
					fprintf(f,"%03d ",((unsigned char*)orio)[y*REF_STRIDE+x]);
				fprintf(f,"\n");
			}
			fclose(f);
		}
		f=fopen("new_p.txt","wt");
		if( f )
		{
			for(int y=MB_SIZE;y<MB_Y_REF * MB_SIZE;y++)
			{
				for(int x=MB_SIZE;x<MB_Y_REF * MB_SIZE;x++)
					fprintf(f,"%03d ",((unsigned char*)newo)[y*REF_STRIDE+x]);
				fprintf(f,"\n");
			}
			fclose(f);
		}
		f=fopen("old_q.txt","wt");
		if( f )
		{
			for(int y=MB_SIZE;y<MB_Y_REF * MB_SIZE;y++)
			{
				for(int x=MB_SIZE;x<MB_Y_REF * MB_SIZE;x++)
					fprintf(f,"%03d ",((unsigned char*)address1)[y*REF_STRIDE+x]);
				fprintf(f,"\n");
			}
			fclose(f);
		}
		f=fopen("new_q.txt","wt");
		if( f )
		{
			for(int y=MB_SIZE;y<MB_Y_REF * MB_SIZE;y++)
			{
				for(int x=MB_SIZE;x<MB_Y_REF * MB_SIZE;x++)
					fprintf(f,"%03d ",((unsigned char*)address2)[y*REF_STRIDE+x]);
				fprintf(f,"\n");
			}
			fclose(f);
		}
	}
}

int main()
{
	volatile int stride = MB_X_REF * MB_SIZE;
	BYTE* address1 = (BYTE*)malloc( 2*MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) );
	BYTE* address2 = (BYTE*)malloc( 2*MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) );
	BYTE* orio = (BYTE*)malloc( 2*MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) );
	BYTE* newo = (BYTE*)malloc( 2*MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) );
	memset( orio, 0, 2*MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) );
	memset( newo, 0, 2*MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) );
	for(int y=0;y<2*MB_Y_REF * MB_SIZE;y++)
	{
		for(int x=0;x<MB_X_REF * MB_SIZE;x++)
		{
			int i = y * REF_STRIDE + x;
			address1[i] = x+y*2;
			orio[i] = x+y*2+5;
		}
	}

	memcpy( address2, address1, 2*MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) );
	memcpy( newo, orio, 2*MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( BYTE ) );

	DoInterpolateTest( address1, stride, address2, stride, orio, newo );

	getch();
	free( address1 );
	free( address2 );
	free( orio );
	free( newo );
	return 0;

}