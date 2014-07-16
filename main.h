#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <assert.h>
#if _MSC_VER > 1400
	#include <smmintrin.h>	//only available with VS2008
#else
	#include <emmintrin.h>
#endif
#include <conio.h>

#define MB_X_REF 68
#define MB_Y_REF 45
#define MB_SIZE 16
#define REF_STRIDE (MB_X_REF*MB_SIZE)
#define LOOP_TEST_COUNT	30	//to have a larger runtime

/////////////////////////////////////////////
// SAD related tests
/////////////////////////////////////////////

extern "C" int SAD16x16_ASM_SSE2(const unsigned char *, const int, const unsigned char *, const int);
extern "C" int SAD16x8_ASM_SSE2(const unsigned char *, const int, const unsigned char *, const int);

extern "C" int SAD16x16_ASM_SSE2_CACHED(const unsigned char *, const unsigned char *);
extern "C" int SAD16x8_ASM_SSE2_CACHED(const unsigned char *, const unsigned char *);

UINT SAD_16x16( const BYTE * address1, const INT stride1, const BYTE * address2, const INT stride2 );
UINT SAD_16x8( const BYTE * address1, const INT stride1, const BYTE * address2, const INT stride2 );

void DoSADTest(BYTE* address1,int stride1, BYTE* address2,int stride2,UINT* orio,UINT* newo);
void DoCachedSADTest(BYTE* address1,int stride1, BYTE* address2,int stride2,UINT* orio,UINT* newo);

/////////////////////////////////////////////
// SATD related tests
/////////////////////////////////////////////

extern "C" int SATD4x4_ASM_SSE2(const unsigned char *, const int, const unsigned char *, const int);
extern "C" int SATD4x4_ASM_SSE2_CACHED(const unsigned char *, const unsigned char *);
extern "C" int SATD4x4_ASM_MMX(const unsigned char *, const int, const unsigned char *, const int);

UINT SATD_4x4( BYTE *address1, INT stride1, BYTE *address2, INT stride2 );
UINT SATD4x4_INTEL_ASM( const unsigned char *pSrc1, unsigned int src1Step, const unsigned char *pSrc2, unsigned int src2Step );
UINT SATD4x4_INTEL( const unsigned char *pSrc1, unsigned int src1Step, const unsigned char *pSrc2, unsigned int src2Step );
UINT SATD_4x4_SSE3( BYTE *address1, INT stride1, BYTE *address2, INT stride2 );

extern "C" int satd_4x4_sse3_YASM( BYTE *address1, INT stride1, BYTE *address2, INT stride2 );
extern "C" int satd_4x4_sse4_YASM( BYTE *address1, INT stride1, BYTE *address2, INT stride2 );
extern "C" int sad_16x16_sse4_YASM( BYTE *address1, INT stride1, BYTE *address2, INT stride2 );
extern "C" int sad_16x16_sse4_YASM_jozsa( BYTE *address1, INT stride1, BYTE *address2, INT stride2 );
extern "C" int sad_16x16_sse2_YASM( BYTE *address1, INT stride1, BYTE *address2, INT stride2 );
extern "C" int sad_16x16_sse4_YASM_V2( BYTE *address1, INT stride1, BYTE *address2, INT stride2 );
extern "C" int sad_16x16_sse4_YASM_V3( BYTE *address1, INT stride1, BYTE *address2, INT stride2 );
extern "C" int sad_16x8_sse4_YASM( BYTE *address1, INT stride1, BYTE *address2, INT stride2 );
extern "C" int sad_8x16_sse4_YASM( BYTE *address1, INT stride1, BYTE *address2, INT stride2 );
extern "C" int ssd_16x16_sse4_YASM( BYTE *address1, INT stride1, BYTE *address2, INT stride2 );
extern "C" int test_BYTE( BYTE *address1, INT stride1, BYTE *address2, INT stride2 );
extern "C" unsigned test_YASM( BYTE *address1, INT stride1, BYTE *address2, INT stride2);
extern "C" int read_luma_inter_pred_avg_16x16_SSE4( BYTE *address1, BYTE *address2, INT stride_src, BYTE *dst, INT stride_dst );
extern "C" int read_luma_inter_pred_avg_16x8_SSE4( BYTE *address1, BYTE *address2, INT stride_src, BYTE *dst, INT stride_dst );
extern "C" void Separate16x16_SSE4_YASM(BYTE * src, INT src_stride, BYTE * pred, INT pred_stride, SHORT * texture, INT texture_stride);
extern "C" void Copy128bits_SSE4( BYTE *address1, INT stride1, BYTE *address2, INT stride2 );
extern "C" void Copy128bits_SSE4_V2( BYTE *address1, INT stride1, BYTE *address2, INT stride2 );







void DoSATDTest(BYTE* address1,int stride1, BYTE* address2,int stride2,UINT* orio,UINT* newo);