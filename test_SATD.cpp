#include "main.h"

#include <time.h>

#include <windows.h>

#include <Winbase.h>

//#include "SAD_SSE2.h"



UINT SAD_4x4( BYTE *address1, INT stride1, BYTE *address2, INT stride2 )

{

	/*INT i;

	INT v1;

	INT v2;

	INT v3;

	INT v4;

	UINT sad = 0;

	for( i = 0; i < 4; i++ )

	{

			v1 = abs((INT)( *(address1 + 0) - *(address2 + 0) ));

			v2 = abs((INT)( *(address1 + 1) - *(address2 + 1) ));

			v3 = abs((INT)( *(address1 + 2) - *(address2 + 2) ));

			v4=  abs((INT)( *(address1 + 3) - *(address2 + 3) ));

			sad += v1+v2+v3+v4;

	

			address1 += stride1;

			address2 += stride2;

	}



	return sad;*/



	#ifdef X64_COMPATIBLE

	unsigned short sad_array[8];

	__m128i l0a, l1a, line_sad, acc_sad;

	__m128i l0b, l1b;

	__m128i l0, l1;



	acc_sad = _mm_setzero_si128();

	//acc_sad = _mm_xor_si128(acc_sad, acc_sad);





	l0a = _mm_cvtsi32_si128(*(int*)(address1));

	l1a = _mm_cvtsi32_si128(*(int*)(address2));

	l0b = _mm_cvtsi32_si128(*(int*)(address1+stride1));

	l1b = _mm_cvtsi32_si128(*(int*)(address2+stride2));

	l0 = _mm_unpacklo_epi8(l0a,l0b);

	l1 = _mm_unpacklo_epi8(l1a,l1b);



	address1 += (stride1<<1);

	address2 += (stride2<<1);



	l0a = _mm_cvtsi32_si128(*(int*)(address1));

	l1a = _mm_cvtsi32_si128(*(int*)(address2));

	l0b = _mm_cvtsi32_si128(*(int*)(address1+stride1));

	l1b = _mm_cvtsi32_si128(*(int*)(address2+stride2));

	l0a = _mm_unpacklo_epi8(l0a,l0b);

	l1a = _mm_unpacklo_epi8(l1a,l1b);



	/*l0a = _mm_loadl_epi64((__m128i*)address1);

	l1a = _mm_loadl_epi64((__m128i*)address2);

	l0b = _mm_loadl_epi64((__m128i*)(address1+stride1));

	l1b = _mm_loadl_epi64((__m128i*)(address2+stride2));

	l0 = _mm_unpacklo_epi8(l0a,l0b);

	l1 = _mm_unpacklo_epi8(l1a,l1b);



	l0a = _mm_loadl_epi64((__m128i*)(address1+stride1+stride1));

	l1a = _mm_loadl_epi64((__m128i*)(address2+stride2+stride2));

	l0b = _mm_loadl_epi64((__m128i*)(address1+stride1+stride1+stride1));

	l1b = _mm_loadl_epi64((__m128i*)(address2+stride2+stride2+stride2));

	l0a = _mm_unpacklo_epi8(l0a,l0b);

	l1a = _mm_unpacklo_epi8(l1a,l1b);*/





	l0 = _mm_unpacklo_epi8(l0a,l0);

	l1 = _mm_unpacklo_epi8(l1a,l1);



	line_sad = _mm_sad_epu8(l0, l1);

	

	_mm_storeu_si128((__m128i*)(&sad_array[0]), line_sad);



	return (sad_array[0]+sad_array[4]);

#endif



	//Hoi Ming x64

#if 0//!defined( WIN64 ) && !defined( _WIN64 )

	int sad;

	__asm

	{

		mov eax, stride1

		mov ebx, stride2

		mov esi, address1

		mov edi, address2

		

		lea ecx, [2*eax+eax]

		lea edx, [2*ebx+ebx]

		

		movd mm0, [esi]

		movd mm4, [edi]

		

		movd mm1, [esi+eax]

		movd mm5, [edi+ebx]

		psadbw mm0,mm4

		

		movd mm2, [esi+2*eax]

		psadbw mm1,mm5

		

		movd mm6, [edi+2*ebx]

		movd mm3, [esi+ecx]

		movd mm7, [edi+edx]

		

		psadbw mm2,mm6

		psadbw mm3,mm7

		

		paddw mm0,mm1

		paddw mm2,mm3

		

		paddw mm0,mm2

		movd sad, mm0

		emms

	}

	return sad;

#else

	return 0;

#endif

}



void DoSATDTest(BYTE* address1,int stride1, BYTE* address2,int stride2,UINT* orio,UINT* newo)

{

	__int64 so,eo,sn,en,snc,enc;

	__int64 diffo,diffn,diffnc;

	diffo = 0;

	diffn = 0;

	diffnc = 0;



	LARGE_INTEGER li;

	double PCFreq = 0.0;







	srand ( time(NULL) );

	

	short Dst[256];

	short DCT_coeff[64];



	for(int i = 0;i<8;i++){

		for(int j = 0;j<8;j++){

			DCT_coeff[i*8+j] = i*8+j+1;

		}

	}

	int count = 0;

	unsigned sum = 0;

	//QueryPerformanceCounter myTimer = new QueryPerformanceCounter();

	QueryPerformanceFrequency(&li);

	PCFreq = double(li.QuadPart)/1000.0;

	int * m_mb_pscore = (int*)malloc(MB_Y_REF*MB_X_REF*sizeof(int));



    //QueryPerformanceCounter(&li);

   // CounterStart = li.QuadPart;







	while(1)

	{

	



		for(int i = 0;i<MB_Y_REF*16;i++){

			for(int j = 0;j<MB_X_REF*16;j++){

				address1[i*stride1+j] = rand() % 255;//i*16+j+1;

				address2[i*stride2+j] = rand() % 255;

			}

		}



		/*UINT result[4] = {0,0,0,0};

		UINT sad = 0;

		for(int i = 0;i<16;i++)

		{

			for(int j = 0;j<8;j++){

				//int temp = abs(address1[i*stride1+j] - address2[i*stride2+j]);

				//sad += (temp*temp);

				sad += abs(address1[i*stride1+j] - address2[i*stride2+j]);

			}

		}*/



		//UINT t1 = 

		//int a = 16;

		//read_luma_inter_pred_avg_16x16_SSE4( address1, address2, stride1, Dst, a);

		

		//so = GetTickCount();

		int result;

		QueryPerformanceCounter(&li);

		so = li.QuadPart;



		

		for( int y = 0; y < MB_Y_REF*16; y+=16 ){

			for( int x = 0; x < MB_X_REF*16; x+=16 ){

				//printf("!!");

				result = SAD_16x16(address1+y*stride1+x,stride1,address2+y*stride2+x,stride2);

			}

		}



		

		QueryPerformanceCounter(&li);

		eo = li.QuadPart;

		diffo += eo - so;



		

		QueryPerformanceCounter(&li);

		sn = li.QuadPart;

		

		for( int y = 0; y < MB_Y_REF*16; y+=16 ){

			for( int x = 0; x < MB_X_REF*16; x+=16 ){

				//printf("!!");

				result = sad_16x16_sse4_YASM(address1+y*stride1+x,stride1,address2+y*stride2+x,stride2);

			}

		}

		QueryPerformanceCounter(&li);

		en = li.QuadPart;

		diffn += en - sn;





		

		/*for(int i = 0;i<MB_Y_REF*16;i++){

			for(int j = 0;j<MB_X_REF*16;j++){

				if(address1[i*stride1+j] != address2[i*stride2+j])

					printf("!!");

			}

		}*/



		





		/*

		for(int i = 0;i<4;i++)

		//int i = 0;

		{

			for(int j = 0;j<16;j++){

				//int temp = abs(address1[i*stride1+j] - address2[i*stride2+j]);

				//sad += (temp*temp);

				int avg = (address1[i*stride1+j] - address2[i*stride2+j]);

				if(avg != Dst[i*16+j])

					printf("!!");

			}

		}

		*/



		count++;



		if(count > 1000)

			break;



	}



	//printf("SATD 4x4: MS for old algorithm(new) : %f\n",(double)sum/PCFreq);

	printf("SATD 4x4: MS for old algorithm : %f\n",(double)diffo/PCFreq);

	printf("SATD 4x4: MS for new algorithm : %f\n",(double)diffn/PCFreq);

//	printf("SATD 4x4: MS for new algorithm(cached) : %u\n",diffnc);

//	printf("speed diff PCT : %u\n",(diffn)*100/(diffo));

	printf("SATD: Results match : %s \n", memcmp( newo,orio, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) ) == 0 ? "true":"false");

	





	return;



	



	

	UINT t = test_BYTE( address1, stride1, address2, stride2 );

	



	for( int loop=0;loop<LOOP_TEST_COUNT;loop++)

	{

		so = GetTickCount();

		//do a full search motion estimation. This does not require to be correct. It is a speed test

		for( int mby = 0; mby < MB_Y_REF; mby++ )

			for( int mbx = 0; mbx < MB_X_REF; mbx++ )

			{

				for( int mvy=0;mvy<16;mvy+=4)

					for( int mvx=0;mvx<16;mvx+=4)

						if( mby * MB_SIZE + mvy >= 0 && (MB_Y_REF-1)  * MB_SIZE > mvy + mby * MB_SIZE

							&& mbx * MB_SIZE + mvx >= 0 && (MB_X_REF-1) * MB_SIZE > mvx + mbx * MB_SIZE )

						{

							BYTE* refa = address1 + ( mby * MB_SIZE + mvy ) * REF_STRIDE + mvx + mbx * MB_SIZE;

							BYTE* cura = address2 + ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE;

							/*for( int loop=0;loop<LOOP_TEST_COUNT;loop++)

							{

								unsigned int tres = SATD_4x4( refa, stride1, cura, stride2 );

								if( mvy >=0 && mvy < MB_SIZE && mvx >=0 && mvx < MB_SIZE )

									orio[ ( mby * MB_SIZE + mvy ) * REF_STRIDE + mbx * MB_SIZE + mvx ] = tres;

							}*/

						}

			}

		eo = GetTickCount();

		diffo += eo - so;



		sn = GetTickCount();

		for( int mby = 0; mby < MB_Y_REF; mby++ )

			for( int mbx = 0; mbx < MB_X_REF; mbx++ )

			{

				for( int mvy=0;mvy<16;mvy+=4)

					for( int mvx=0;mvx<16;mvx+=4)

						if( mby * MB_SIZE + mvy >= 0 && (MB_Y_REF-1) * MB_SIZE > mvy + mby * MB_SIZE

							&& mbx * MB_SIZE + mvx >= 0 && (MB_X_REF-1) * MB_SIZE > mvx + mbx * MB_SIZE )

						{

							BYTE* refa = address1 + ( mby * MB_SIZE + mvy ) * REF_STRIDE + mvx + mbx * MB_SIZE;

							BYTE* cura = address2 + ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE;

							for( int loop=0;loop<LOOP_TEST_COUNT;loop++)

							{

//								unsigned int tres = SATD4x4_INTEL_ASM( refa, stride1, cura, stride2 );

//								unsigned int tres = SATD4x4_ASM_SSE2( refa, stride1, cura, stride2 );

//								unsigned int tres = SATD4x4_INTEL( refa, stride1, cura, stride2 );							

//								unsigned int tres = SATD4x4_ASM_MMX( refa, stride1, cura, stride2 );

								unsigned int tres = satd_4x4_sse3_YASM( refa, stride1, cura, stride2 );

								if( mvy >=0 && mvy < MB_SIZE && mvx >=0 && mvx < MB_SIZE )

								{

/*

if( orio[ ( mby * MB_SIZE + mvy ) * REF_STRIDE + mbx * MB_SIZE + mvx ] != tres )

{

	unsigned int tresold = SATD4x4_INTEL( refa, stride1, cura, stride2 );

printf("here is a mismatch");

}*/

									newo[ ( mby * MB_SIZE + mvy ) * REF_STRIDE + mbx * MB_SIZE + mvx ] = tres;

								}

							}

						}

			}

		en = GetTickCount();

		diffn += en - sn;

/**/

/*		snc = GetTickCount();

		for( int mby = 0; mby < MB_Y_REF; mby++ )

			for( int mbx = 0; mbx < MB_X_REF; mbx++ )

			{

				__declspec(align(16)) BYTE cur_MB_cache[MB_SIZE * MB_SIZE];

				__declspec(align(16)) BYTE ref_MB_cache[MB_SIZE * MB_SIZE];

				//fill cur_MB cache

				for( int i=0;i<MB_SIZE;i++)

				{

					BYTE* cura = address2 + (( mby * MB_SIZE )+i) * stride2 + mbx * MB_SIZE;

					memcpy( &cur_MB_cache[i*MB_SIZE], cura, MB_SIZE );

				}

				//fill ref_cache

				for( int i=0;i<MB_SIZE;i++)

				{

					BYTE* refa = address1 + (( mby * MB_SIZE )+i) * stride1 + mbx * MB_SIZE;

					memcpy( &ref_MB_cache[i*MB_SIZE], refa, MB_SIZE );

				}

				for( int mvy=0;mvy<16;mvy+=4)

					for( int mvx=0;mvx<16;mvx+=4)

						if( mby * MB_SIZE + mvy >= 0 && (MB_Y_REF-1) * MB_SIZE > mvy + mby * MB_SIZE

							&& mbx * MB_SIZE + mvx >= 0 && (MB_X_REF-1) * MB_SIZE > mvx + mbx * MB_SIZE )

						{

							BYTE* refa = ref_MB_cache + mvy * MB_SIZE + mvx;

							for( int loop=0;loop<LOOP_TEST_COUNT;loop++)

							{

								unsigned int tres = SATD4x4_ASM_SSE2_CACHED( cur_MB_cache, refa );

								if( mvy >=0 && mvy < MB_SIZE && mvx >=0 && mvx < MB_SIZE )

									newo[ ( mby * MB_SIZE + mvy ) * REF_STRIDE + mbx * MB_SIZE + mvx ] = tres;

							}

						}

			}

		enc = GetTickCount();

		diffnc += enc - snc;

		/**/

	}



	printf("SATD 4x4: MS for old algorithm : %u\n",diffo);

	printf("SATD 4x4: MS for new algorithm : %u\n",diffn);

//	printf("SATD 4x4: MS for new algorithm(cached) : %u\n",diffnc);

//	printf("speed diff PCT : %u\n",(diffn)*100/(diffo));

	printf("SATD: Results match : %s \n", memcmp( newo,orio, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) ) == 0 ? "true":"false");

}