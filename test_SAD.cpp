#include "main.h"


void DoSADTest(BYTE* address1,int stride1, BYTE* address2,int stride2,UINT* orio,UINT* newo)
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
				for( int mvy=-16;mvy<=16;mvy++)
					for( int mvx=-16;mvx<=16;mvx++)
						if( mby * MB_SIZE + mvy >= 0 && (MB_Y_REF-1) * MB_SIZE > mvy + mby * MB_SIZE
							&& mbx * MB_SIZE + mvx >= 0 && (MB_X_REF-1) * MB_SIZE > mvx + mbx * MB_SIZE )
						{
							BYTE* refa = address1 + ( mby * MB_SIZE + mvy ) * REF_STRIDE + mvx + mbx * MB_SIZE;
							BYTE* cura = address2 + ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE;
							newo[ ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE ] = SAD16x16_ASM_SSE2( refa, stride1, cura, stride2 );
						}
			}
		en = GetTickCount();
		diffn += en - sn;

		so = GetTickCount();
		//do a full search motion estimation. This does not require to be correct. It is a speed test
		for( int mby = 0; mby < MB_Y_REF; mby++ )
			for( int mbx = 0; mbx < MB_X_REF; mbx++ )
			{
				for( int mvy=-16;mvy<=16;mvy++)
					for( int mvx=-16;mvx<=16;mvx++)
						if( mby * MB_SIZE + mvy >= 0 && (MB_Y_REF-1) * MB_SIZE > mvy + mby * MB_SIZE
							&& mbx * MB_SIZE + mvx >= 0 && (MB_X_REF-1) * MB_SIZE > mvx + mbx * MB_SIZE )
						{
							BYTE* refa = address1 + ( mby * MB_SIZE + mvy ) * REF_STRIDE + mvx + mbx * MB_SIZE;
							BYTE* cura = address2 + ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE;
							orio[ ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE ] = SAD_16x16( refa, stride1, cura, stride2 );
						}
			}
		eo = GetTickCount();
		diffo += eo - so;
	}
	printf("SAD 16x16: MS for old algorithm : %u\n",diffo);
	printf("SAD 16x16: MS for new algorithm : %u\n",diffn);
	printf("speed gain PCT : %u\n",(diffo)*100/(diffn)-100);
	printf("SAD 16x16: Results match : %d \n", memcmp( newo,orio, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( UINT ) ) == 0 );

	diffo = diffn = 0;
	for( int loop=0;loop<LOOP_TEST_COUNT;loop++)
	{
		so = GetTickCount();
		//do a full search motion estimation. This does not require to be correct. It is a speed test
		for( int mby = 0; mby < MB_Y_REF; mby++ )
			for( int mbx = 0; mbx < MB_X_REF; mbx++ )
			{
				for( int mvy=-16;mvy<=16;mvy++)
					for( int mvx=-16;mvx<=16;mvx++)
						if( mby * MB_SIZE + mvy >= 0 && (MB_Y_REF-1) * MB_SIZE > mvy + mby * MB_SIZE
							&& mbx * MB_SIZE + mvx >= 0 && (MB_X_REF-1) * MB_SIZE > mvx + mbx * MB_SIZE )
						{
							BYTE* refa = address1 + ( mby * MB_SIZE + mvy ) * REF_STRIDE + mvx + mbx * MB_SIZE;
							BYTE* cura = address2 + ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE;
							orio[ ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE ] = SAD_16x8( refa, stride1, cura, stride2 );
						}
			}
		eo = GetTickCount();
		diffo += eo - so;

		sn = GetTickCount();
		for( int mby = 0; mby < MB_Y_REF; mby++ )
			for( int mbx = 0; mbx < MB_X_REF; mbx++ )
			{
				for( int mvy=-16;mvy<=16;mvy++)
					for( int mvx=-16;mvx<=16;mvx++)
						if( mby * MB_SIZE + mvy >= 0 && (MB_Y_REF-1) * MB_SIZE > mvy + mby * MB_SIZE
							&& mbx * MB_SIZE + mvx >= 0 && (MB_X_REF-1) * MB_SIZE > mvx + mbx * MB_SIZE )
						{
							BYTE* refa = address1 + ( mby * MB_SIZE + mvy ) * REF_STRIDE + mvx + mbx * MB_SIZE;
							BYTE* cura = address2 + ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE;
							newo[ ( mby * MB_SIZE ) * REF_STRIDE + mbx * MB_SIZE ] = SAD16x8_ASM_SSE2( refa, stride1, cura, stride2 );
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