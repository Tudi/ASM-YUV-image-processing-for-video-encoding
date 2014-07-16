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
	#define LOOP_TEST_COUNT	20	//to have a larger runtime
	#define LOOP_TEST_COUNT_INTERP	600	//to have a larger runtime
#endif

#define CLIP3(a,b,c)	((c) < (a) ? (a) : ((c) > (b) ? (b) : (c))) // MIN, MAX, val
#define CLIP1(x)		(CLIP3( 0, 255, (x) ))

#define P_A		*m_UpperPels
#define P_B		*(m_UpperPels + 1)
#define P_C		*(m_UpperPels + 2)
#define P_D		*(m_UpperPels + 3)
#define P_E		*(m_UpperPels + 4)
#define P_F		*(m_UpperPels + 5)
#define P_G		*(m_UpperPels + 6)
#define P_H		*(m_UpperPels + 7)
#define P_I		*m_LeftPels
#define P_J		*(m_LeftPels + 1)
#define P_K		*(m_LeftPels + 2)
#define P_L		*(m_LeftPels + 3)
//Hoi Ming High Profile
#define P_M		*(m_LeftPels + 4)
#define P_N		*(m_LeftPels + 5)
#define P_O		*(m_LeftPels + 6)
#define P_P		*(m_LeftPels + 7)

void read_vertical_right_prediction_4x4( BYTE *dst, BYTE *m_UpperPels, BYTE *m_LeftPels, INT m_dst_stride, INT P_X )
{
	INT ia[10];
	INT i,j;
	ia[0] = (P_X + P_A + 1) >> 1;			// a, j
	ia[1] = (P_A + P_B + 1) >> 1;			// b, k
	ia[2] = (P_B + P_C + 1) >> 1;			// c, l
	ia[3] = (P_I + 2*P_X + P_A + 2) >> 2;	// e, n
	ia[4] = (P_X + 2*P_A + P_B + 2) >> 2;	// f, o
	ia[5] = (P_A + 2*P_B + P_C + 2) >> 2;	// g, p
	ia[6] = (P_C + P_D + 1) >> 1;			// d
	ia[7] = (P_B + 2*P_C + P_D + 2) >> 2;	// h
	ia[8] = (P_X + 2*P_I + P_J + 2) >> 2;	// i
	ia[9] = (P_I + 2*P_J + P_K + 2) >> 2;	// m

	INT sum_ind = 0;
	for( i = 0; i < 2; i++ )
	{
		for( j = 0; j < 3; j++ )
		{
			dst[j] = dst[2 * m_dst_stride  + (j + 1)] = ia[i*3+j];
//			printf("dst[%u] = dst[2 * m_dst_stride  + (%u + 1)] = ia[%u*3+%u];\n",j,j,i,j);
		}

		dst[3] = ia[i+6];
//		printf("dst[3] = ia[%d+6];\n",i);
		dst[2 * m_dst_stride] = ia[i+8];
//		printf("dst[2 * m_dst_stride] = ia[%u+8];\n",i);
		dst += m_dst_stride;
	}
//	getch();
}

void read_vertical_right_prediction_4x4_2( BYTE *dst, BYTE *m_UpperPels, BYTE *m_LeftPels, INT m_dst_stride, INT P_X )
{
	INT ia[10];
	INT i,j;
	ia[0] = (P_X + P_A + 1) >> 1;			// a, j
	ia[1] = (P_A + P_B + 1) >> 1;			// b, k
	ia[2] = (P_B + P_C + 1) >> 1;			// c, l
	ia[3] = (P_I + 2*P_X + P_A + 2) >> 2;	// e, n
	ia[4] = (P_X + 2*P_A + P_B + 2) >> 2;	// f, o
	ia[5] = (P_A + 2*P_B + P_C + 2) >> 2;	// g, p
	ia[6] = (P_C + P_D + 1) >> 1;			// d
	ia[7] = (P_B + 2*P_C + P_D + 2) >> 2;	// h
	ia[8] = (P_X + 2*P_I + P_J + 2) >> 2;	// i
	ia[9] = (P_I + 2*P_J + P_K + 2) >> 2;	// m
	dst[0] = dst[2 * m_dst_stride  + (0 + 1)] = ia[0*3+0];
	dst[1] = dst[2 * m_dst_stride  + (1 + 1)] = ia[0*3+1];
	dst[2] = dst[2 * m_dst_stride  + (2 + 1)] = ia[0*3+2];
	dst[3] = ia[0+6];
	dst[2 * m_dst_stride] = ia[0+8];
	dst += m_dst_stride;
	dst[0] = dst[2 * m_dst_stride  + (0 + 1)] = ia[1*3+0];
	dst[1] = dst[2 * m_dst_stride  + (1 + 1)] = ia[1*3+1];
	dst[2] = dst[2 * m_dst_stride  + (2 + 1)] = ia[1*3+2];
	dst[3] = ia[1+6];
	dst[2 * m_dst_stride] = ia[1+8];
}

void read_vertical_right_prediction_4x4_3( BYTE *dst, BYTE *m_UpperPels, BYTE *m_LeftPels, INT m_dst_stride, INT P_X )
{
	INT ia[10];
	INT i,j;
	ia[0] = (P_X + P_A + 1) >> 1;			// a, j
	ia[1] = (P_A + P_B + 1) >> 1;			// b, k
	ia[2] = (P_B + P_C + 1) >> 1;			// c, l
	ia[3] = (P_I + 2*P_X + P_A + 2) >> 2;	// e, n
	ia[4] = (P_X + 2*P_A + P_B + 2) >> 2;	// f, o
	ia[5] = (P_A + 2*P_B + P_C + 2) >> 2;	// g, p
	ia[6] = (P_C + P_D + 1) >> 1;			// d
	ia[7] = (P_B + 2*P_C + P_D + 2) >> 2;	// h
	ia[8] = (P_X + 2*P_I + P_J + 2) >> 2;	// i
	ia[9] = (P_I + 2*P_J + P_K + 2) >> 2;	// m
	dst[0] = ia[0];
	dst[1] = ia[1];
	dst[2] = ia[2];
	dst[3] = ia[6];
	dst += m_dst_stride;
	dst[0] = ia[3];
	dst[1] = ia[4];
	dst[2] = ia[5];
	dst[3] = ia[7];
	dst += m_dst_stride;
	dst[0] = ia[8];
	dst[1] = ia[0];
	dst[2] = ia[1];
	dst[3] = ia[2];
	dst += m_dst_stride;
	dst[0] = ia[9];
	dst[1] = ia[3];
	dst[2] = ia[4];
	dst[3] = ia[5];
}

void read_vertical_right_prediction_4x4_4( BYTE *dst, BYTE *m_UpperPels, BYTE *m_LeftPels, INT m_dst_stride, INT P_X )
{
	BYTE ia[10];
	INT i,j;
	ia[0] = (P_X + 2*P_I + P_J + 2) >> 2;

	ia[1] = (P_X + P_A + 1) >> 1;			
	ia[2] = (P_A + P_B + 1) >> 1;			
	ia[3] = (P_B + P_C + 1) >> 1;			
	ia[4] = (P_C + P_D + 1) >> 1;			

	ia[5] = (P_I + 2*P_J + P_K + 2) >> 2;

	ia[6] = (P_I + 2*P_X + P_A + 2) >> 2;
	ia[7] = (P_X + 2*P_A + P_B + 2) >> 2;
	ia[8] = (P_A + 2*P_B + P_C + 2) >> 2;
	ia[9] = (P_B + 2*P_C + P_D + 2) >> 2;	

	*(INT*)dst = *(INT*)&ia[1];
	dst += m_dst_stride;
	*(INT*)dst = *(INT*)&ia[6];
	dst += m_dst_stride;
	*(INT*)dst = *(INT*)&ia[0];
	dst += m_dst_stride;
	*(INT*)dst = *(INT*)&ia[5];
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
					read_vertical_right_prediction_4x4( refa, cura, cura, stride1, 1 );
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
					read_vertical_right_prediction_4x4_4( refa, cura, cura, stride1, 1 );
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