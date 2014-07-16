#include "main.h"

void DoCachedSADTest(BYTE* address1,int stride1, BYTE* address2,int stride2,UINT* orio,UINT* newo)
{
#ifndef _WIN64
	UINT so,eo,sn,en;
	UINT diffo,diffn;
	diffo = 0;
	diffn = 0;

	for( int loop=0;loop<LOOP_TEST_COUNT;loop++)
	{
		sn = GetTickCount();
		for( int mby = 0; mby < MB_Y_REF; mby++ )
			for( int mbx = 0; mbx < MB_X_REF; mbx++ )
			{
				__declspec(align(16)) BYTE cur_MB_cache[MB_SIZE * MB_SIZE];
				__declspec(align(16)) BYTE ref_MB_cache[MB_SIZE * MB_SIZE * 3 * 3];	//-16+16
				//fill cur_MB cache
				for( int i=0;i<MB_SIZE;i++)
				{
					BYTE* cura = address2 + (( mby * MB_SIZE )+i) * stride2 + mbx * MB_SIZE;
					memcpy( &cur_MB_cache[i*MB_SIZE], cura, MB_SIZE );
				}
				//fill ref_cache
				for( int mvy=-MB_SIZE;mvy<2*MB_SIZE;mvy++)
					if( mby * MB_SIZE + mvy >= 0 && (MB_Y_REF-1) * MB_SIZE > mvy + mby * MB_SIZE )
					{
						BYTE* refa = address1 + (( mby * MB_SIZE )+mvy) * stride1 + mbx * MB_SIZE;
						memcpy( &ref_MB_cache[mvy*MB_SIZE], refa, MB_SIZE * 3 );
					}
				for( int mvy=0;mvy<=2*16;mvy++)
					for( int mvx=0;mvx<=2*16;mvx++)
						{
							BYTE* refa = ref_MB_cache + mvy * 3 * MB_SIZE + mvx;
							newo[ ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE ] = 
								SAD16x16_ASM_SSE2_CACHED( cur_MB_cache, refa );
						}
			}
		en = GetTickCount();
		diffn += en - sn;

		so = GetTickCount();
		//do a full search motion estimation. This does not require to be correct. It is a speed test
		for( int mby = 0; mby < MB_Y_REF; mby++ )
			for( int mbx = 0; mbx < MB_X_REF; mbx++ )
			{
				__declspec(align(16)) BYTE cur_MB_cache[MB_SIZE * MB_SIZE];
				__declspec(align(16)) BYTE ref_MB_cache[MB_SIZE * MB_SIZE * 3 * 3];	//-16+16
				//fill cur_MB cache
				for( int i=0;i<MB_SIZE;i++)
				{
					BYTE* cura = address2 + (( mby * MB_SIZE )+i) * REF_STRIDE + mbx * MB_SIZE;
					memcpy( &cur_MB_cache[i*MB_SIZE], cura, MB_SIZE );
				}
				//fill ref_cache
				for( int mvy=-MB_SIZE;mvy<2*MB_SIZE;mvy++)
					if( mby * MB_SIZE + mvy >= 0 && (MB_Y_REF-1) * MB_SIZE > mvy + mby * MB_SIZE )
					{
						BYTE* refa = address1 + (( mby * MB_SIZE )+mvy) * REF_STRIDE + mbx * MB_SIZE;
						memcpy( &ref_MB_cache[mvy*MB_SIZE], refa, MB_SIZE * 3 );
					}
				for( int mvy=0;mvy<=2*16;mvy++)
					for( int mvx=0;mvx<=2*16;mvx++)
						{
							BYTE* refa = ref_MB_cache + mvy * 3 * MB_SIZE + mvx;
							orio[ ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE ] = 
								SAD_16x16( cur_MB_cache, MB_SIZE, refa, 3 * MB_SIZE );
						}
			}
		eo = GetTickCount();
		diffo += eo - so;
	}
	printf("SAD CACHED 16x16: MS for old algorithm : %u\n",diffo);
	printf("SAD CACHED 16x16: MS for new algorithm : %u\n",diffn);
	printf("speed gain PCT : %u\n",(diffo)*100/(diffn)-100);
	printf("SAD CACHED 16x16: Results match : %d \n", memcmp( newo,orio, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) ) == 0 );

	diffo = diffn = 0;
	for( int loop=0;loop<LOOP_TEST_COUNT;loop++)
	{
		so = GetTickCount();
		//do a full search motion estimation. This does not require to be correct. It is a speed test
		for( int mby = 0; mby < MB_Y_REF; mby++ )
			for( int mbx = 0; mbx < MB_X_REF; mbx++ )
			{
				__declspec(align(16)) BYTE cur_MB_cache[MB_SIZE * MB_SIZE];
				__declspec(align(16)) BYTE ref_MB_cache[MB_SIZE * MB_SIZE * 3 * 3];	//-16+16
				//fill cur_MB cache
				for( int i=0;i<MB_SIZE;i++)
				{
					BYTE* cura = address2 + (( mby * MB_SIZE )+i) * REF_STRIDE + mbx * MB_SIZE;
					memcpy( &cur_MB_cache[i*MB_SIZE], cura, MB_SIZE );
				}
				//fill ref_cache
				for( int mvy=-MB_SIZE;mvy<2*MB_SIZE;mvy++)
					if( mby * MB_SIZE + mvy >= 0 && (MB_Y_REF-1) * MB_SIZE > mvy + mby * MB_SIZE )
					{
						BYTE* refa = address1 + (( mby * MB_SIZE )+mvy) * REF_STRIDE + mbx * MB_SIZE;
						memcpy( &ref_MB_cache[mvy*MB_SIZE], refa, MB_SIZE * 3 );
					}
				for( int mvy=0;mvy<=2*16;mvy++)
					for( int mvx=0;mvx<=2*16;mvx++)
						{
							BYTE* refa = ref_MB_cache + mvy * 3 * MB_SIZE + mvx;
							orio[ ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE ] = 
								SAD_16x8( cur_MB_cache, MB_SIZE, refa, MB_SIZE * 3 );
						}
			}
		eo = GetTickCount();
		diffo += eo - so;

		sn = GetTickCount();
		for( int mby = 0; mby < MB_Y_REF; mby++ )
			for( int mbx = 0; mbx < MB_X_REF; mbx++ )
			{
				__declspec(align(16)) BYTE cur_MB_cache[MB_SIZE * MB_SIZE];
				__declspec(align(16)) BYTE ref_MB_cache[MB_SIZE * MB_SIZE * 3 * 3];	//-16+16
				//fill cur_MB cache
				for( int i=0;i<MB_SIZE;i++)
				{
					BYTE* cura = address2 + (( mby * MB_SIZE )+i) * REF_STRIDE + mbx * MB_SIZE;
					memcpy( &cur_MB_cache[i*MB_SIZE], cura, MB_SIZE );
				}
				//fill ref_cache
				for( int mvy=-MB_SIZE;mvy<2*MB_SIZE;mvy++)
					if( mby * MB_SIZE + mvy >= 0 && (MB_Y_REF-1) * MB_SIZE > mvy + mby * MB_SIZE )
					{
						BYTE* refa = address1 + (( mby * MB_SIZE )+mvy) * REF_STRIDE + mbx * MB_SIZE;
						memcpy( &ref_MB_cache[mvy*MB_SIZE], refa, MB_SIZE * 3 );
					}
				for( int mvy=0;mvy<=2*16;mvy++)
					for( int mvx=0;mvx<=2*16;mvx++)
						{
							BYTE* refa = ref_MB_cache + mvy * 3 * MB_SIZE + mvx;
							newo[ ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE ] = 
								SAD16x8_ASM_SSE2_CACHED( cur_MB_cache, refa );
						}
			}
		en = GetTickCount();
		diffn += en - sn;
	}

	printf("SAD 16x8: MS for old algorithm : %u\n",diffo);
	printf("SAD 16x8: MS for new algorithm : %u\n",diffn);
	printf("speed gain PCT : %u\n",(diffo)*100/(diffn)-100);
	printf("SAD 16x8: Results match : %d \n", memcmp( newo,orio, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) ) == 0 );
#endif
}