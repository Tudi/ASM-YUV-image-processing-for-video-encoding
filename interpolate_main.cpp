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
#define LOOP_TEST_COUNT	30	//to have a larger runtime
#define LOOP_TEST_COUNT_INTRA	10	//to have a larger runtime
#define LOOP_TEST_COUNT_INTERP	1	//to have a larger runtime

void read_luma_inter_pred_MxN_half_half_Encoder( short *src, int src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
{
	int i,j;
#define CLIP3(a,b,c)	((c) < (a) ? (a) : ((c) > (b) ? (b) : (c))) // MIN, MAX, val
#define CLIP1(x)		(CLIP3( 0, 255, (x) ))
	short *s1=src+2*m_dst_stride;
	for( i = 0; i < m_height; i++ )
	{
		for( j = 0; j < m_width; j++ )
		{
			int v = (int)s1[j-2*m_dst_stride] + (int)s1[j+3*m_dst_stride] - 5*( (int)s1[j-1*m_dst_stride] + (int)s1[j+2*m_dst_stride] ) + 20*( (int)s1[j] + (int)s1[j+1*m_dst_stride] );			
			v=CLIP1( (v +512)>>10 );	
			dst[j] = v;
		}
		s1 += m_dst_stride;
		dst += m_dst_stride;
	}

}

//this is wrong in many ways, do not use it !
void read_luma_inter_pred_MxN_half_half_Encoder2( short *src, int src_stride, unsigned char *dst, int dst_stride, int m_height, int m_width)
{
	int i,j;
	for( i = 0; i < m_height; i++ )
	{
		for( j = 0; j < m_width; j++ )
		{
			//min value here is -5*(65535*2)=-10*65535 => 655350 = 0x9FFF6, closest mask is 0xFFFFF and use a shift of 24
			//max value is 20*2*65535=40*65535+2*65535=0x29FFD6 - 24 bits
			int v = (int)src[j] + (int)src[j+5*dst_stride] - 5*( (int)src[j+1*dst_stride] + (int)src[j+4*dst_stride] ) + 20*( (int)src[j+2*dst_stride] + (int)src[j+3*dst_stride] );
//			v = (( v << 20 ) + ( 512 << 20 )) >> 30;
//			v = (( v << 15 ) + ( 512 << 15 )) >> 25;
//			v = (( v << 10 ) + ( 512 << 10 )) >> 20;
//			v = (v + ( 512 << 3 )) >> 13;
//			v = v & 0xFF;
			v = (v+512)>>10;
			v = (v + 65535) & 0xFF;
			dst[j]=v;					
		}

		src += src_stride;
		dst += dst_stride;
	}
} 

void read_luma_inter_pred_MxN_half_half_Encoder_intr( short *src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
{

	unsigned char *dst_backup = dst; 
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

	for( i = 0; i < (twidth); i+=8)
	{
		__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5, r0_b, r1_b, r2_b, r3_b, r4_b, r5_b, 
			zero, offset512, temp, rResult1;
		zero = _mm_setzero_si128();
		//zero = _mm_xor_si128(zero, zero);
		offset512 = _mm_set1_epi32(512);
		src_ptr = src;
		dst_ptr = dst;
		r0_b = _mm_loadu_si128((__m128i*)(src_ptr));
		r1_b = _mm_loadu_si128((__m128i*)(src_ptr+stride1));
		r2_b = _mm_loadu_si128((__m128i*)(src_ptr+stride2));
		r3_b = _mm_loadu_si128((__m128i*)(src_ptr+stride3));
		r4_b = _mm_loadu_si128((__m128i*)(src_ptr+(stride2<<1)));
		for( j = 0; j < theight; j++)
		{
			r5_b = _mm_loadu_si128((__m128i*)(src_ptr+stride3+stride2));
			//first 64 bits
			r0 = _mm_unpacklo_epi16(r0_b, zero);
			r1 = _mm_unpacklo_epi16(r1_b, zero);
			r2 = _mm_unpacklo_epi16(r2_b, zero);
			r3 = _mm_unpacklo_epi16(r3_b, zero);
			r4 = _mm_unpacklo_epi16(r4_b, zero);
			r5 = _mm_unpacklo_epi16(r5_b, zero);
			r0 = _mm_slli_epi32(r0, 16);
			r0 = _mm_srai_epi32(r0, 16);
			r1 = _mm_slli_epi32(r1, 16);
			r1 = _mm_srai_epi32(r1, 16);
			r2 = _mm_slli_epi32(r2, 16);
			r2 = _mm_srai_epi32(r2, 16);
			r3 = _mm_slli_epi32(r3, 16);
			r3 = _mm_srai_epi32(r3, 16);
			r4 = _mm_slli_epi32(r4, 16);
			r4 = _mm_srai_epi32(r4, 16);
			r5 = _mm_slli_epi32(r5, 16);
			r5 = _mm_srai_epi32(r5, 16);

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
			r0 = _mm_slli_epi32(r0, 16);
			r0 = _mm_srai_epi32(r0, 16);
			r1 = _mm_slli_epi32(r1, 16);
			r1 = _mm_srai_epi32(r1, 16);
			r2 = _mm_slli_epi32(r2, 16);
			r2 = _mm_srai_epi32(r2, 16);
			r3 = _mm_slli_epi32(r3, 16);
			r3 = _mm_srai_epi32(r3, 16);
			r4 = _mm_slli_epi32(r4, 16);
			r4 = _mm_srai_epi32(r4, 16);
			r5 = _mm_slli_epi32(r5, 16);
			r5 = _mm_srai_epi32(r5, 16);

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

void read_luma_inter_pred_MxN_half_half_Encoder_asm( short *src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
{
	_asm {
;  push        ebp  
;  mov         ebp,esp 
;  and         esp,0FFFFFFF0h 
;  sub         esp,58h 
;  mov         eax,dword ptr [ebp+8] 
;  push        ebx  
;  push        edi  
;  lea         ecx,[esi+esi*2] 
;  mov         edi,200h 
;  lea         ecx,[ecx+esi*2] 
;  movd        xmm0,edi 
;  add         ecx,ecx 
;  pshufd      xmm0,xmm0,0 

  mov         dword ptr [esp+10h],edx 
  pxor        xmm1,xmm1 
  movdqa      xmmword ptr [esp+40h],xmm0 
  mov         dword ptr [esp+1Ch],ecx 
  lea         edi,[esi+esi] 
  mov         dword ptr [esp+18h],2 
LOOP_LOC_0:
  movdqu      xmm7,xmmword ptr [eax] 
  movdqu      xmm5,xmmword ptr [edi+eax] 
  lea         ebx,[esi+esi] 
  movdqu      xmm6,xmmword ptr [eax+ebx*2] 
  lea         ebx,[esi+esi*2] 
  movdqu      xmm3,xmmword ptr [eax+ebx*2] 
  lea         ebx,[esi+esi] 
  movdqu      xmm2,xmmword ptr [eax+ebx*4] 
  mov         ecx,eax 
  movdqa      xmmword ptr [esp+30h],xmm3 
  movdqa      xmmword ptr [esp+20h],xmm2 
  mov         dword ptr [esp+14h],10h 
  jmp         LOOP_LOC_1
  lea         ebx,[ebx] 
LOOP_LOC_1:
  mov         ebx,dword ptr [esp+1Ch] 
  movdqu      xmm4,xmmword ptr [ebx+ecx] 
  punpcklwd   xmm2,xmm1 
  pslld       xmm2,10h 
  psrad       xmm2,10h 
  punpcklwd   xmm3,xmm1 
  pslld       xmm3,10h 
  psrad       xmm3,10h 
  movdqa      xmm0,xmm5 
  punpcklwd   xmm0,xmm1 
  pslld       xmm0,10h 
  psrad       xmm0,10h 
  paddd       xmm0,xmm2 
  movdqa      xmmword ptr [esp+50h],xmm4 
  punpcklwd   xmm4,xmm1 
  movdqa      xmm2,xmm6 
  punpcklwd   xmm2,xmm1 
  pslld       xmm2,10h 
  psrad       xmm2,10h 
  paddd       xmm2,xmm3 
  movdqa      xmm3,xmm0 
  pslld       xmm3,2 
  paddd       xmm3,xmm0 
  movdqa      xmm0,xmm7 
  pslld       xmm4,10h 
  punpcklwd   xmm0,xmm1 
  psrad       xmm4,10h 
  pslld       xmm0,10h 
  psrad       xmm0,10h 
  paddd       xmm0,xmm4 
  movdqa      xmm4,xmm2 
  pslld       xmm2,2 
  pslld       xmm4,4 
  paddd       xmm4,xmm2 
  movdqa      xmm2,xmmword ptr [esp+20h] 
  paddd       xmm0,xmm4 
  movdqa      xmm4,xmmword ptr [esp+30h] 
  psubd       xmm0,xmm3 
  paddd       xmm0,xmmword ptr [esp+40h] 
  punpckhwd   xmm2,xmm1 
  pslld       xmm2,10h 
  psrad       xmm2,10h 
  movdqa      xmm3,xmm5 
  punpckhwd   xmm3,xmm1 
  pslld       xmm3,10h 
  psrad       xmm3,10h 
  paddd       xmm3,xmm2 
  punpckhwd   xmm4,xmm1 
  movdqa      xmm2,xmm6 
  punpckhwd   xmm2,xmm1 
  pslld       xmm4,10h 
  pslld       xmm2,10h 
  psrad       xmm4,10h 
  psrad       xmm2,10h 
  paddd       xmm2,xmm4 
  movdqa      xmm4,xmm3 
  punpckhwd   xmm7,xmm1 
  pslld       xmm7,10h 
  pslld       xmm4,2 
  paddd       xmm4,xmm3 
  movdqa      xmm3,xmmword ptr [esp+50h] 
  punpckhwd   xmm3,xmm1 
  psrad       xmm7,10h 
  pslld       xmm3,10h 
  psrad       xmm3,10h 
  paddd       xmm7,xmm3 
  movdqa      xmm3,xmm2 
  pslld       xmm3,4 
  pslld       xmm2,2 
  paddd       xmm3,xmm2 
  paddd       xmm7,xmm3 
  psubd       xmm7,xmm4 
  paddd       xmm7,xmmword ptr [esp+40h] 
  psrad       xmm0,0Ah 
  psrad       xmm7,0Ah 
  packssdw    xmm0,xmm7 
  movdqa      xmm7,xmm5 
  packuswb    xmm0,xmm1 
  movdqa      xmm5,xmm6 
  movdqa      xmm6,xmmword ptr [esp+30h] 
  movq        mmword ptr [edx],xmm0 
  movdqa      xmm3,xmmword ptr [esp+20h] 
  movdqa      xmm2,xmmword ptr [esp+50h] 
  add         ecx,edi 
  add         edx,esi 
  sub         dword ptr [esp+14h],1 
  movdqa      xmmword ptr [esp+30h],xmm3 
  movdqa      xmmword ptr [esp+20h],xmm2 
  jne         LOOP_LOC_1 
  mov         edx,dword ptr [esp+10h] 
  add         edx,8 
  add         eax,10h 
  sub         dword ptr [esp+18h],1 
  mov         dword ptr [esp+10h],edx 
  jne         LOOP_LOC_0
;  pop         edi  
;  pop         ebx  
;  mov         esp,ebp 
;  pop         ebp  
;  ret        
  };
}

static inline int imin(int a, int b)
{
  return ((a) < (b)) ? (a) : (b);
}

static inline int imax(int a, int b)
{
  return ((a) > (b)) ? (a) : (b);
}

static inline int iClip1(int high, int x)
{
  x = imax(x, 0);
  x = imin(x, high);

  return x;
}
static const int COEF[6] = { 1, -5, 20, 20, -5, 1 };
typedef byte imgpel;
static int max_imgpel_value = 255;
void read_luma_inter_pred_MxN_half_half_Encoder_ref( short *src, int src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
{
//	imgpel *p0, *p1, *p2, *p3, *p4, *p5, *orig_line;
	short *p0, *p1, *p2, *p3, *p4, *p5;
	unsigned char *orig_line;
	for( int i = 0; i < m_height; i++ )
	{
		p0 = src-2*src_stride;
        p1 = p0 + src_stride;          
        p2 = p1 + src_stride;
        p3 = p2 + src_stride;
        p4 = p3 + src_stride;
        p5 = p4 + src_stride;
		orig_line = dst;
		for(int j = 0; j < m_width; j++ )
		{
            int result  = (*(p0++) + *(p5++)) * COEF[0]
            + (*(p1++) + *(p4++)) * COEF[1]
            + (*(p2++) + *(p3++)) * COEF[2];

            *orig_line++ = (imgpel) iClip1(max_imgpel_value, ((result + 512)>>10));
		}
		src += m_dst_stride;
		dst += m_dst_stride;
	}
}

void read_luma_inter_pred_MxN_half_half_Encoder_intr2( short *src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
{
	unsigned char *dst_backup = dst; 
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

//at step 1 we are using int8 as input and output int16
//max value will be : 255+255+20*(255+255) = 512+20*512 = 21 * 512 ~=~ 2^5*2^9 = 2^14
//max value at step 2 will be : 2^15+20*2^15=21*2^15=2^5*2^15=2^20
void read_luma_inter_pred_MxN_half_half_Encoder_short( short *src, int src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
{
	int i,j;
#define CLIP3(a,b,c)	((c) < (a) ? (a) : ((c) > (b) ? (b) : (c))) // MIN, MAX, val
#define CLIP1(x)		(CLIP3( 0, 255, (x) ))
	short *s1=src+2*m_dst_stride;
	for( i = 0; i < m_height; i++ )
	{
		for( j = 0; j < m_width; j++ )
		{
			int r_0 = s1[j-2*m_dst_stride];
			int r_1 = s1[j-1*m_dst_stride];
			int r_2 = s1[j-0*m_dst_stride];
			int r_3 = s1[j+1*m_dst_stride];
			int r_4 = s1[j+2*m_dst_stride];
			int r_5 = s1[j+3*m_dst_stride];
//			r_0 = r_0 >> 2; r_1 = r_1 >> 2; r_3 = r_3 >> 2; r_4 = r_4 >> 2; r_5 = r_5 >> 2;
//			short v = (r_0 + r_5 + 512)/5 - (r_1 + r_4 ) + 4*( r_2 + r_3 );			
//			v = (v + 512 ) >> 10;
//			v = (v + ( 512 >> 2 ) ) >> ( 10 - 1 );
//			v = (v + ( 512 / 5 ) ) / ( 1024 / 5 );
//			v = v / 205;
//			int v = (r_0 + r_5 + 512)/8 - ((r_1 + r_4 )/8)*5 + (( r_2 + r_3 )/8)*20;			
//			v = v / ( 1024 / 8 );
			short v = (r_0 + r_5 + 512)/16 - ((r_1 + r_4 )/16)*5 + (( r_2 + r_3 )/16)*20;			
			v = v / ( 1024 / 16 );
			v=CLIP1( v );	
			dst[j] = v;
		}
		s1 += m_dst_stride;
		dst += m_dst_stride;
	}
}

void read_luma_inter_pred_MxN_half_half_Encoder_intr3( short *src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
{
	int j, i;
	const short* src_ptr;
	unsigned char* dst_ptr;
	int src_stride = m_dst_stride;
	int dst_stride = m_dst_stride;
	int theight = m_height;
	int twidth = m_width;
	int stride1, stride3;
	stride1 = src_stride;
	stride3 = (src_stride<<1)+src_stride;
	__declspec(align(16)) __m128i zero, offset512;
	zero = _mm_setzero_si128();
	offset512 = _mm_set1_epi32(512);

	for( j = 0; j < theight; j++ )
	{
		__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5, r0_b, r1_b, r2_b, r3_b, r4_b, r5_b;
		src_ptr = src;
		dst_ptr = dst;
		for( i = 0; i < (twidth); i+=8)
		{
			__declspec(align(16)) __m128i temp, rResult1;
			r0_b = _mm_loadu_si128((__m128i*)(src_ptr));
			r1_b = _mm_loadu_si128((__m128i*)(src_ptr+stride1));
			r2_b = _mm_loadu_si128((__m128i*)(src_ptr+(stride1<<1)));
			r3_b = _mm_loadu_si128((__m128i*)(src_ptr+stride3));
			r4_b = _mm_loadu_si128((__m128i*)(src_ptr+(stride1<<2)));
			r5_b = _mm_loadu_si128((__m128i*)(src_ptr+stride3+(stride1<<1)));

			//low 64 bits
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

			//high 64 bits
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
			src_ptr+=8;
			dst_ptr+=8;
		}
		src += stride1;
		dst += dst_stride;
	}
}

void read_luma_inter_pred_MxN_half_half_Encoder_intr4( short *src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
{

	int j, i;
	const short* src_ptr;
	unsigned char* dst_ptr;
	int src_stride = m_dst_stride;
	int dst_stride = m_dst_stride;
	int theight = m_height;
	int twidth = m_width;
	int stride1, stride3;
	stride1 = src_stride;
	stride3 = (src_stride<<1)+src_stride;
	__declspec(align(16)) __m128i zero, offset512;
	zero = _mm_setzero_si128();
	offset512 = _mm_set1_epi32(512);

	for( j = 0; j < theight; j++ )
	{
		__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5;
		src_ptr = src;
		dst_ptr = dst;
		for( i = 0; i < (twidth); i+=4)
		{
			__declspec(align(16)) __m128i temp;
			r0 = _mm_loadl_epi64((__m128i*)(src_ptr));
			r1 = _mm_loadl_epi64((__m128i*)(src_ptr+stride1));
			r2 = _mm_loadl_epi64((__m128i*)(src_ptr+(stride1<<1)));
			r3 = _mm_loadl_epi64((__m128i*)(src_ptr+stride3));
			r4 = _mm_loadl_epi64((__m128i*)(src_ptr+(stride1<<2)));
			r5 = _mm_loadl_epi64((__m128i*)(src_ptr+stride3+(stride1<<1)));

			//low 64 bits
			r0 = _mm_unpacklo_epi16(r0, zero);
			r1 = _mm_unpacklo_epi16(r1, zero);
			r2 = _mm_unpacklo_epi16(r2, zero);
			r3 = _mm_unpacklo_epi16(r3, zero);
			r4 = _mm_unpacklo_epi16(r4, zero);
			r5 = _mm_unpacklo_epi16(r5, zero);
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
			r0 = _mm_packus_epi16(r0, zero);
			r0 = _mm_packus_epi16(r0, zero);
			_mm_storel_epi64((__m128i*)dst_ptr, r0);
			src_ptr+=4;
			dst_ptr+=4;
		}
		src += stride1;
		dst += dst_stride;
	}
}

void read_luma_inter_pred_MxN_half_half_Encoder_intr5( short *src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
{

	unsigned char *dst_backup = dst; 
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

	for( i = 0; i < (twidth); i+=16)
	{
		__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5, r0_b, r1_b, r2_b, r3_b, r4_b, r5_b;
		__declspec(align(16)) __m128i r0_c, r1_c, r2_c, r3_c, r4_c, r5_c;
		src_ptr = src;
		dst_ptr = dst;
		r0_b = _mm_loadu_si128((__m128i*)(src_ptr));
		r1_b = _mm_loadu_si128((__m128i*)(src_ptr+stride1));
		r2_b = _mm_loadu_si128((__m128i*)(src_ptr+stride2));
		r3_b = _mm_loadu_si128((__m128i*)(src_ptr+stride3));
		r4_b = _mm_loadu_si128((__m128i*)(src_ptr+(stride2<<1)));
		r0_c = _mm_loadu_si128((__m128i*)(src_ptr+8));
		r1_c = _mm_loadu_si128((__m128i*)(src_ptr+8+stride1));
		r2_c = _mm_loadu_si128((__m128i*)(src_ptr+8+stride2));
		r3_c = _mm_loadu_si128((__m128i*)(src_ptr+8+stride3));
		r4_c = _mm_loadu_si128((__m128i*)(src_ptr+8+(stride2<<1)));
		for( j = 0; j < theight; j++)
		{
			__declspec(align(16)) __m128i temp, rResult1;
			r5_b = _mm_loadu_si128((__m128i*)(src_ptr+stride3+stride2));
			r5_c = _mm_loadu_si128((__m128i*)(src_ptr+8+stride3+stride2));
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

			//first 64 bits
			r0 = _mm_unpacklo_epi16(r0_c, zero);
			r1 = _mm_unpacklo_epi16(r1_c, zero);
			r2 = _mm_unpacklo_epi16(r2_c, zero);
			r3 = _mm_unpacklo_epi16(r3_c, zero);
			r4 = _mm_unpacklo_epi16(r4_c, zero);
			r5 = _mm_unpacklo_epi16(r5_c, zero);

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
			r0 = _mm_unpackhi_epi16(r0_c, zero);
			r1 = _mm_unpackhi_epi16(r1_c, zero);
			r2 = _mm_unpackhi_epi16(r2_c, zero);
			r3 = _mm_unpackhi_epi16(r3_c, zero);
			r4 = _mm_unpackhi_epi16(r4_c, zero);
			r5 = _mm_unpackhi_epi16(r5_c, zero);

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

			_mm_storel_epi64((__m128i*)dst_ptr+8, rResult1);
			r0_c = r1_c;
			r1_c = r2_c;
			r2_c = r3_c;
			r3_c = r4_c;
			r4_c = r5_c;

			src_ptr += stride1;
			dst_ptr += dst_stride;
		}
		src+=16;
		dst+=16;
	}
}

void read_luma_inter_pred_MxN_half_half_Encoder_intr6( short *src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
{

	unsigned char *dst_backup = dst; 
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

	for( i = 0; i < (twidth); i+=8)
	{
		__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5, r0_b, r1_b, r2_b, r3_b, r4_b, r5_b, 
			offset512, temp, rResult1;
//		zero = _mm_setzero_si128();
		//zero = _mm_xor_si128(zero, zero);
		offset512 = _mm_set1_epi32(512);
		src_ptr = src;
		dst_ptr = dst;
		r0_b = _mm_loadu_si128((__m128i*)(src_ptr));
		r1_b = _mm_loadu_si128((__m128i*)(src_ptr+stride1));
		r2_b = _mm_loadu_si128((__m128i*)(src_ptr+stride2));
		r3_b = _mm_loadu_si128((__m128i*)(src_ptr+stride3));
		r4_b = _mm_loadu_si128((__m128i*)(src_ptr+(stride2<<1)));
		for( j = 0; j < theight; j++)
		{
			r5_b = _mm_loadu_si128((__m128i*)(src_ptr+stride3+stride2));
			//first 64 bits
			r0 = _mm_unpacklo_epi16(r0_b, r0_b);
			r1 = _mm_unpacklo_epi16(r1_b, r1_b);
			r2 = _mm_unpacklo_epi16(r2_b, r2_b);
			r3 = _mm_unpacklo_epi16(r3_b, r3_b);
			r4 = _mm_unpacklo_epi16(r4_b, r4_b);
			r5 = _mm_unpacklo_epi16(r5_b, r5_b);
			r0 = _mm_srai_epi32(r0, 16);
			r1 = _mm_srai_epi32(r1, 16);
			r2 = _mm_srai_epi32(r2, 16);
			r3 = _mm_srai_epi32(r3, 16);
			r4 = _mm_srai_epi32(r4, 16);
			r5 = _mm_srai_epi32(r5, 16);

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
			r0 = _mm_unpackhi_epi16(r0_b, r0_b);
			r1 = _mm_unpackhi_epi16(r1_b, r1_b);
			r2 = _mm_unpackhi_epi16(r2_b, r2_b);
			r3 = _mm_unpackhi_epi16(r3_b, r3_b);
			r4 = _mm_unpackhi_epi16(r4_b, r4_b);
			r5 = _mm_unpackhi_epi16(r5_b, r5_b);
			r0 = _mm_srai_epi32(r0, 16);
			r1 = _mm_srai_epi32(r1, 16);
			r2 = _mm_srai_epi32(r2, 16);
			r3 = _mm_srai_epi32(r3, 16);
			r4 = _mm_srai_epi32(r4, 16);
			r5 = _mm_srai_epi32(r5, 16);

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
//			rResult1 = _mm_packus_epi16(r0, zero);
			rResult1 = _mm_packus_epi16(r0, r0);
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

void read_luma_inter_pred_MxN_half_half_Encoder_asm2( short *src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
{
	src = src - 2*m_src_stride;
	__declspec(align(16)) int _x264_pw_16[8]={16,16,16,16,16,16,16,16};
	__declspec(align(16)) char filt_mul20[16]={20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20};
	__declspec(align(16)) char filt_mul15[16]={1,-5,1,-5,1,-5,1,-5,1,-5,1,-5,1,-5,1,-5};
	for(int i=0;i<m_height;i++)
	{
	__asm{
;_x264_pw_16:    times 8 dw 16
;filt_mul20:		times 16 db 20
;filt_mul15:		times 8 db 1, -5

		push ebx
		push esi
		push edi
;		mov eax, [esp + 12 + 4 + 0*4]	; dst pointer
;		mov ecx, [esp + 12 + 4 + 1*4]	; src pointer
;	mov edx, [esp + 12 + 4 + 2*4]	; buff pointer
;		mov ebx, [esp + 12 + 4 + 3*4]	; src stride
;		mov esi, [esp + 12 + 4 + 4*4]	; width

		mov eax, [dst]
		mov ecx, [src]
		mov esi, m_width
		mov ebx, m_src_stride

		lea edi, [ecx+ebx]				; src + 1 row
		sub ecx, ebx					; src - 1 row
		sub ecx, ebx					; src - 1 row ( total -2 )
		add eax, esi					;we will process buffer from bottom to top, keep substracting a width until we get to 0
;		lea edx, [edx+esi*2]
		neg esi							; width = -width
		movdqa xmm0, [filt_mul15]		; filter for alpha blending
loop_block:
		movdqa xmm1, [ecx]			; load row 0
		movdqa xmm4, [ecx+ebx]		; load row 1
		movdqa xmm2, [edi+ebx*2]	; load row 5
		movdqa xmm5, [edi+ebx]		; load row 4
		movdqa xmm3, [ecx+ebx*2]	; load row 2
		movdqa xmm6, [edi]			; load row 3
		movdqa xmm7, xmm1
		punpcklbw xmm1, xmm4		; unpack row 0 and row 1 into xmm1 low
		punpckhbw xmm7, xmm4		; unpack row 0 and row 1 into xmm7 high
		movdqa xmm4, xmm2			
		punpcklbw xmm2, xmm5		; unpack row 3 and row 4 into xmm2 low 
		punpckhbw xmm4, xmm5		; unpack row 3 and row 4 into xmm4 high
		movdqa xmm5, xmm3
		punpcklbw xmm3, xmm6		; unpack row 2 and row 1 into xmm3 low 
		punpckhbw xmm5, xmm6		; unpack row 2 and row 1 into xmm5 high 
		pmaddubsw xmm1, xmm0			; alpha blend rows
		pmaddubsw xmm7, xmm0			; alpha blend rows
		pmaddubsw xmm2, xmm0			; alpha blend rows
		pmaddubsw xmm4, xmm0			; alpha blend rows
		pmaddubsw xmm3, [filt_mul20]	; alpha blend rows
		pmaddubsw xmm5, [filt_mul20]	; alpha blend rows
		paddw xmm1, xmm2				; sum avg values
		paddw xmm7, xmm4				; sum avg values
		paddw xmm1, xmm3				; sum avg values
		paddw xmm7, xmm5				; sum avg values
		movdqa xmm6, [_x264_pw_16]		
;		movdqa [edx+esi*2], xmm1
;		movdqa [edx+esi*2+16], xmm7
		paddw xmm1, xmm6
		paddw xmm7, xmm6
		psraw xmm1, 5					; divide to get real avg
		psraw xmm7, 5					; divide to get real avg
		packuswb xmm1, xmm7				; pack 32 bit into 16 bit
		movntdq [eax+esi], xmm1			; store result
		add ecx, 16						; process next 16 bytes of input
		add edi, 16						; process next 16 bytes of input
		add esi, 16						; store detination here next time
		jl loop_block
		pop edi
		pop esi
		pop ebx
	};
	src += m_src_stride;
	dst += m_dst_stride;
	}
}

void read_luma_inter_pred_MxN_half_half_Encoder_intr7( short *src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
{
	int j, i;
	const short* src_ptr;
	unsigned char* dst_ptr;
	int src_stride = m_dst_stride;
	int dst_stride = m_dst_stride;
	int stride1, stride2, stride3;
	stride1 = src_stride;
	stride2 = src_stride<<1;
	stride3 = (src_stride<<1)+src_stride;

	__declspec(align(16)) __m128i offset32;
	offset32 = _mm_set1_epi16(32);

	for( i = 0; i < m_width; i+=8)
	{
		__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5, r0_b, r1_b, r2_b, r3_b, r4_b, r5_b;
		src_ptr = src;
		dst_ptr = dst;
		r0_b = _mm_loadu_si128((__m128i*)(src_ptr));
		r1_b = _mm_loadu_si128((__m128i*)(src_ptr+stride1));
		r2_b = _mm_loadu_si128((__m128i*)(src_ptr+stride2));
		r3_b = _mm_loadu_si128((__m128i*)(src_ptr+stride3));
		r4_b = _mm_loadu_si128((__m128i*)(src_ptr+(stride2<<1)));
		for( j = 0; j < m_height; j++)
		{
			__declspec(align(16)) __m128i temp, rResult1;
			r5_b = _mm_loadu_si128((__m128i*)(src_ptr+stride3+stride2));

			r0 = r0_b;
			r1 = r1_b;
			r2 = r2_b;
			r3 = r3_b;
			r4 = r4_b;
			r5 = r5_b;

			r0 = _mm_add_epi16(r0, r5);
			r1 = _mm_add_epi16(r1, r4);
			r2 = _mm_add_epi16(r2, r3);

			r0 = _mm_srai_epi16(r0, 4);
			r0 = _mm_add_epi16(r0, offset32);

			r1 = _mm_srai_epi16(r1, 4);				// divide
			temp = r1;
			r1 = _mm_slli_epi16(r1, 2);
			r1 = _mm_add_epi16(r1, temp);			// mult by 5

			r2 = _mm_srai_epi16(r2, 4);				// divide
			temp = _mm_slli_epi16(r2, 2);		
			r2 = _mm_slli_epi16(r2, 4);
			r2 = _mm_add_epi16(r2, temp);			// mult by 20

			r0 = _mm_add_epi16(r0, r2);
			r0 = _mm_sub_epi16(r0, r1);

			r0 = _mm_srai_epi16(r0, 6);				// avg
			rResult1 = _mm_packus_epi16(r0, r0);	// short to byte and do the saturation

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

void read_luma_inter_pred_MxN_half_half_Encoder_intr8( short *src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
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
	__declspec(align(16)) __m128i offset32;
	offset32 = _mm_set1_epi16(32);

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

			r0 = r0_b;
			r1 = r1_b;
			r2 = r2_b;
			r3 = r3_b;
			r4 = r4_b;
			r5 = r5_b;

			r0 = _mm_add_epi16(r0, r5);
			r1 = _mm_add_epi16(r1, r4);
			r2 = _mm_add_epi16(r2, r3);

			r0 = _mm_sub_epi16(r0, r1);
			r0 = _mm_srai_epi16(r0, 4);
			r0 = _mm_add_epi16(r0, offset32);

			r1 = _mm_sub_epi16(r2, r1);
			r1 = _mm_srai_epi16(r1, 2);

			r0 = _mm_add_epi16(r0, r2);
			r0 = _mm_add_epi16(r0, r1);

			r0 = _mm_srai_epi16(r0, 6);
			rResult1 = _mm_packus_epi16(r0, r0);

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

void read_luma_inter_pred_MxN_half_half_Encoder_intr9( short *src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
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
	__declspec(align(16)) __m128i offset32;
	offset32 = _mm_set1_epi16(32);
	for( i = 0; i < theight; i++)
	{
		__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5;
		src_ptr = src;
		dst_ptr = dst;
		for( j = 0; j < twidth; j+=8)
		{
			__declspec(align(16)) __m128i temp, rResult1;
			r0 = _mm_loadu_si128((__m128i*)(src_ptr));
			r1 = _mm_loadu_si128((__m128i*)(src_ptr+stride1));
			r2 = _mm_loadu_si128((__m128i*)(src_ptr+stride2));
			r3 = _mm_loadu_si128((__m128i*)(src_ptr+stride3));
			r4 = _mm_loadu_si128((__m128i*)(src_ptr+(stride2<<1)));
			r5 = _mm_loadu_si128((__m128i*)(src_ptr+stride3+stride2));

			r0 = _mm_add_epi16(r0, r5);
			r1 = _mm_add_epi16(r1, r4);
			r2 = _mm_add_epi16(r2, r3);

			r0 = _mm_srai_epi16(r0, 4);
			r0 = _mm_add_epi16(r0, offset32);

			r1 = _mm_srai_epi16(r1, 4);			//divide
			temp = r1;
			r1 = _mm_slli_epi16(r1, 2);
			r1 = _mm_add_epi16(r1, temp);		//mult by 5

			r2 = _mm_srai_epi16(r2, 4);			//divide
			temp = _mm_slli_epi16(r2, 2);		
			r2 = _mm_slli_epi16(r2, 4);
			r2 = _mm_add_epi16(r2, temp);		//mult by 20

			r0 = _mm_add_epi16(r0, r2);
			r0 = _mm_sub_epi16(r0, r1);

			r0 = _mm_srai_epi16(r0, 6);
			rResult1 = _mm_packus_epi16(r0, r0);

			_mm_storel_epi64((__m128i*)dst_ptr, rResult1);
			src_ptr+=8;
			dst_ptr+=8;
		}
		src += stride1;
		dst += dst_stride;
	}
}

void read_luma_inter_pred_MxN_half_half_Encoder_intr10( short *src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
{
	int j, i;
	const short* src_ptr;
	unsigned char* dst_ptr;
	int src_stride = m_dst_stride;
	int dst_stride = m_dst_stride;
	int stride1, stride2, stride3;
	stride1 = src_stride;
	stride2 = src_stride<<1;
	stride3 = (src_stride<<1)+src_stride;

	__declspec(align(16)) __m128i offset32;
	offset32 = _mm_set1_epi16(64);

	for( i = 0; i < m_width; i+=8)
	{
		__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5, r0_b, r1_b, r2_b, r3_b, r4_b, r5_b;
		src_ptr = src;
		dst_ptr = dst;
		r0_b = _mm_loadu_si128((__m128i*)(src_ptr));
		r1_b = _mm_loadu_si128((__m128i*)(src_ptr+stride1));
		r2_b = _mm_loadu_si128((__m128i*)(src_ptr+stride2));
		r3_b = _mm_loadu_si128((__m128i*)(src_ptr+stride3));
		r4_b = _mm_loadu_si128((__m128i*)(src_ptr+(stride2<<1)));
		for( j = 0; j < m_height; j++)
		{
			__declspec(align(16)) __m128i temp, rResult1;
			r5_b = _mm_loadu_si128((__m128i*)(src_ptr+stride3+stride2));

			r0 = r0_b;
			r1 = r1_b;
			r2 = r2_b;
			r3 = r3_b;
			r4 = r4_b;
			r5 = r5_b;

			r0 = _mm_add_epi16(r0, r5);
			r1 = _mm_add_epi16(r1, r4);
			r2 = _mm_add_epi16(r2, r3);

			r0 = _mm_srai_epi16(r0, 4);
			r0 = _mm_add_epi16(r0, offset32);

			r1 = _mm_srai_epi16(r1, 4);				// divide
			temp = r1;
			r1 = _mm_slli_epi16(r1, 2);
			r1 = _mm_add_epi16(r1, temp);			// mult by 5

			r2 = _mm_srai_epi16(r2, 4);				// divide
			temp = _mm_slli_epi16(r2, 2);		
			r2 = _mm_slli_epi16(r2, 4);
			r2 = _mm_add_epi16(r2, temp);			// mult by 20

			r0 = _mm_add_epi16(r0, r2);
			r0 = _mm_sub_epi16(r0, r1);

			r0 = _mm_srai_epi16(r0, 6);				// avg
			rResult1 = _mm_packus_epi16(r0, r0);	// short to byte and do the saturation

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

void read_luma_inter_pred_MxN_half_half_Encoder_intr11( short *src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
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
	__declspec(align(16)) __m128i offset16;
	offset16 = _mm_set1_epi16(16);

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

			r0 = r0_b;
			r1 = r1_b;
			r2 = r2_b;
			r3 = r3_b;
			r4 = r4_b;
			r5 = r5_b;

			r0 = _mm_add_epi16(r0, r5);
			r1 = _mm_add_epi16(r1, r4);
			r2 = _mm_add_epi16(r2, r3);

			r0 = _mm_srai_epi16(r0, 1);
			r1 = _mm_srai_epi16(r1, 1);
			r2 = _mm_srai_epi16(r2, 1);

			r0 = _mm_sub_epi16(r0, r1);
			r0 = _mm_srai_epi16(r0, 4);
			r0 = _mm_add_epi16(r0, offset16);

			r1 = _mm_sub_epi16(r2, r1);
			r1 = _mm_srai_epi16(r1, 2);

			r0 = _mm_add_epi16(r0, r2);
			r0 = _mm_add_epi16(r0, r1);

			r0 = _mm_srai_epi16(r0, 5);
			rResult1 = _mm_packus_epi16(r0, r0);

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

void read_luma_inter_pred_MxN_half_half_Encoder_intr12( short *src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
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
	__declspec(align(16)) __m128i offset8;
	offset8 = _mm_set1_epi16(8);

	__declspec(align(16)) __m128i offset3;
	offset3 = _mm_set1_epi16(3);

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

			r0 = r0_b;
			r1 = r1_b;
			r2 = r2_b;
			r3 = r3_b;
			r4 = r4_b;
			r5 = r5_b;

			r0 = _mm_add_epi16(r0, r5);
			r1 = _mm_add_epi16(r1, r4);
			r2 = _mm_add_epi16(r2, r3);

			r0 = _mm_add_epi16(r0, offset3);
			r1 = _mm_add_epi16(r1, offset3);
			r2 = _mm_add_epi16(r2, offset3);

			r0 = _mm_srai_epi16(r0, 2);
			r1 = _mm_srai_epi16(r1, 2);
			r2 = _mm_srai_epi16(r2, 2);

			r0 = _mm_sub_epi16(r0, r1);
			r0 = _mm_srai_epi16(r0, 4);
			r0 = _mm_add_epi16(r0, offset8);

			r1 = _mm_sub_epi16(r2, r1);
			r1 = _mm_srai_epi16(r1, 2);

			r0 = _mm_add_epi16(r0, r2);
			r0 = _mm_add_epi16(r0, r1);

			r0 = _mm_srai_epi16(r0, 4);
			rResult1 = _mm_packus_epi16(r0, r0);

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

void read_luma_inter_pred_MxN_half_half_Encoder_intr13( short *src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
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
	__declspec(align(16)) __m128i offset4;
	offset4 = _mm_set1_epi16(4);

	__declspec(align(16)) __m128i offset7;
	offset7 = _mm_set1_epi16(7);

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

			r0 = r0_b;
			r1 = r1_b;
			r2 = r2_b;
			r3 = r3_b;
			r4 = r4_b;
			r5 = r5_b;

			r0 = _mm_add_epi16(r0, offset7);
			r1 = _mm_add_epi16(r1, offset7);
			r2 = _mm_add_epi16(r2, offset7);
			r3 = _mm_add_epi16(r3, offset7);
			r4 = _mm_add_epi16(r4, offset7);
			r5 = _mm_add_epi16(r5, offset7);

			r0 = _mm_srai_epi16(r0, 3);
			r1 = _mm_srai_epi16(r1, 3);
			r2 = _mm_srai_epi16(r2, 3);
			r3 = _mm_srai_epi16(r3, 3);
			r4 = _mm_srai_epi16(r4, 3);
			r5 = _mm_srai_epi16(r5, 3);

			r0 = _mm_add_epi16(r0, r5);
			r1 = _mm_add_epi16(r1, r4);
			r2 = _mm_add_epi16(r2, r3);

			r0 = _mm_sub_epi16(r0, r1);
			r0 = _mm_srai_epi16(r0, 4);
			r0 = _mm_add_epi16(r0, offset4);

			r1 = _mm_sub_epi16(r2, r1);
			r1 = _mm_srai_epi16(r1, 2);

			r0 = _mm_add_epi16(r0, r2);
			r0 = _mm_add_epi16(r0, r1);

			r0 = _mm_srai_epi16(r0, 3);
			rResult1 = _mm_packus_epi16(r0, r0);

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

void read_luma_inter_pred_MxN_half_half_Encoder_intr14( short *src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
{
	unsigned char *dst_backup = dst; 
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

	__declspec(align(16)) __m128i offset512;
	offset512 = _mm_set1_epi32(512);

	for( i = 0; i < (twidth); i+=8)
	{
		__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5, r0_b, r1_b, r2_b, r3_b, r4_b, r5_b, r0a, r1a, r2a;
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

			r0a = _mm_add_epi32(r0_b, r5_b);
			r1a = _mm_add_epi32(r1_b, r4_b);
			r2a = _mm_add_epi32(r2_b, r3_b);

			//first 64 bits
			r0 = _mm_unpacklo_epi16(r0a, r0a);
			r1 = _mm_unpacklo_epi16(r1a, r1a);
			r2 = _mm_unpacklo_epi16(r2a, r2a);

			r0 = _mm_srai_epi32(r0, 16);
			r1 = _mm_srai_epi32(r1, 16);
			r2 = _mm_srai_epi32(r2, 16);

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
			r0 = _mm_unpackhi_epi16(r0a, r0a);
			r1 = _mm_unpackhi_epi16(r1a, r1a);
			r2 = _mm_unpackhi_epi16(r2a, r2a);

			r0 = _mm_srai_epi32(r0, 16);
			r1 = _mm_srai_epi32(r1, 16);
			r2 = _mm_srai_epi32(r2, 16);

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

			rResult1 = _mm_packus_epi16(r0, r0);
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

void read_luma_inter_pred_MxN_half_half_Encoder_intr15( short *src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
{
	unsigned char *dst_backup = dst; 
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

	__declspec(align(16)) __m128i offset512;
	offset512 = _mm_set1_epi32(512);

	__declspec(align(16)) __m128i mulm5;
	mulm5 = _mm_set1_epi16(-5);

	__declspec(align(16)) __m128i mulm20;
	mulm20 = _mm_set1_epi16(20);

	for( i = 0; i < (twidth); i+=8)
	{
		__declspec(align(16)) __m128i r0_b, r1_b, r2_b, r3_b, r4_b, r5_b;
		src_ptr = src;
		dst_ptr = dst;
		r0_b = _mm_loadu_si128((__m128i*)(src_ptr));
		r1_b = _mm_loadu_si128((__m128i*)(src_ptr+stride1));
		r2_b = _mm_loadu_si128((__m128i*)(src_ptr+stride2));
		r3_b = _mm_loadu_si128((__m128i*)(src_ptr+stride3));
		r4_b = _mm_loadu_si128((__m128i*)(src_ptr+(stride2<<1)));
		for( j = 0; j < theight; j++)
		{
			__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5, temp, rResult1, r0a, r1a, r2a;
			r5_b = _mm_loadu_si128((__m128i*)(src_ptr+stride3+stride2));

			r0a = _mm_add_epi32(r0_b, r5_b);
			r1a = _mm_add_epi32(r1_b, r4_b);
			r2a = _mm_add_epi32(r2_b, r3_b);

			//first 64 bits
			r0 = _mm_unpacklo_epi16(r0a, r0a);
			r1 = _mm_unpacklo_epi16(r1a, r1a);
			r2 = _mm_unpacklo_epi16(r2a, r2a);

			r0 = _mm_srai_epi32(r0, 16);
			r1 = _mm_srai_epi32(r1, 16);
			r2 = _mm_srai_epi32(r2, 16);

			r1 = _mm_madd_epi16( r1, mulm5 );
			r2 = _mm_madd_epi16( r2, mulm20 );

			r0 = _mm_add_epi32(r0, r2);
			r1 = _mm_add_epi32(r1, offset512);
			r0 = _mm_add_epi32(r0, r1);

			rResult1 = _mm_srai_epi32(r0, 10);
			//end

			//first 64 bits
			r0 = _mm_unpackhi_epi16(r0a, r0a);
			r1 = _mm_unpackhi_epi16(r1a, r1a);
			r2 = _mm_unpackhi_epi16(r2a, r2a);

			r0 = _mm_srai_epi32(r0, 16);
			r1 = _mm_srai_epi32(r1, 16);
			r2 = _mm_srai_epi32(r2, 16);

			r1 = _mm_madd_epi16( r1, mulm5 );
			r2 = _mm_madd_epi16( r2, mulm20 );

			r0 = _mm_add_epi32(r0, r2);
			r1 = _mm_add_epi32(r1, offset512);
			r0 = _mm_add_epi32(r0, r1);

			r0 = _mm_srai_epi32(r0, 10);

			r0 = _mm_packs_epi32(rResult1, r0);
			rResult1 = _mm_packus_epi16(r0, r0);
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

void read_luma_inter_pred_MxN_half_half_Encoder_intr16( short *src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
{
	const short* src_ptr;
	unsigned char* dst_ptr;
	int src_stride = m_dst_stride;
	int dst_stride = m_dst_stride;
	int stride1, stride2, stride3;
	stride1 = src_stride;
	stride2 = src_stride<<1;
	stride3 = (src_stride<<1)+src_stride;

	__declspec(align(16)) __m128i offset512;
	offset512 = _mm_set1_epi32(512);

	__declspec(align(16)) __m128i mulm5;
	mulm5 = _mm_set1_epi32(5);

	__declspec(align(16)) __m128i mulm20;
	mulm20 = _mm_set1_epi32(20);

	for( int i = 0; i < m_width; i+=8)
	{
		__declspec(align(16)) __m128i r0_b, r1_b, r2_b, r3_b, r4_b, r5_b;
		src_ptr = src;
		dst_ptr = dst;
		r0_b = _mm_loadu_si128((__m128i*)(src_ptr));
		r1_b = _mm_loadu_si128((__m128i*)(src_ptr+stride1));
		r2_b = _mm_loadu_si128((__m128i*)(src_ptr+stride2));
		r3_b = _mm_loadu_si128((__m128i*)(src_ptr+stride3));
		r4_b = _mm_loadu_si128((__m128i*)(src_ptr+(stride2<<1)));
		for( int j = 0; j < m_height; j++)
		{
			__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5, temp, rResult1, r0a, r1a, r2a;
			r5_b = _mm_loadu_si128((__m128i*)(src_ptr+stride3+stride2));

			r0a = _mm_add_epi16(r0_b, r5_b);
			r1a = _mm_add_epi16(r1_b, r4_b);
			r2a = _mm_add_epi16(r2_b, r3_b);

			//first 64 bits
			r0 = _mm_unpacklo_epi16(r0a, r0a);
			r1 = _mm_unpacklo_epi16(r1a, r1a);
			r2 = _mm_unpacklo_epi16(r2a, r2a);

			r0 = _mm_srai_epi32( r0, 16 );
			r1 = _mm_madd_epi16( r1, mulm5 );
			r2 = _mm_madd_epi16( r2, mulm20 );

			r0 = _mm_sub_epi32(r0, r1);
			r2 = _mm_add_epi32(r2, offset512);
			r0 = _mm_add_epi32(r0, r2);

			rResult1 = _mm_srai_epi32(r0, 10);
			//end

			//first 64 bits
			r0 = _mm_unpackhi_epi16(r0a, r0a);
			r1 = _mm_unpackhi_epi16(r1a, r1a);
			r2 = _mm_unpackhi_epi16(r2a, r2a);

			r0 = _mm_srai_epi32( r0, 16 );
			r1 = _mm_madd_epi16( r1, mulm5 );
			r2 = _mm_madd_epi16( r2, mulm20 );

			r0 = _mm_sub_epi32(r0, r1);
			r2 = _mm_add_epi32(r2, offset512);
			r0 = _mm_add_epi32(r0, r2);

			r0 = _mm_srai_epi32(r0, 10);

			r0 = _mm_packs_epi32(rResult1, r0);
			rResult1 = _mm_packus_epi16(r0, r0);
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

void read_luma_inter_pred_MxN_half_half_Encoder_asm2_i( short *src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
{
	src = src - 2*m_src_stride;
	__declspec(align(16)) int _x264_pw_16[8]={16,16,16,16,16,16,16,16};
	__declspec(align(16)) char filt_mul20[16]={20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20};
	__declspec(align(16)) char filt_mul15[16]={1,-5,1,-5,1,-5,1,-5,1,-5,1,-5,1,-5,1,-5};
	for(int i=0;i<m_height;i++)
	{
		__declspec(align(16)) __m128i r0, r1, r2, r3, r4, r5, r0_b, r1_b, r2_b, r3_b, r4_b, r5_b;
		short *src_ptr = src;
		unsigned char *dst_ptr = dst;
		r0_b = _mm_loadu_si128((__m128i*)(src_ptr));
		r1_b = _mm_loadu_si128((__m128i*)(src_ptr+m_src_stride));
		r2_b = _mm_loadu_si128((__m128i*)(src_ptr+m_src_stride*2));
		r3_b = _mm_loadu_si128((__m128i*)(src_ptr+m_src_stride*3));
		r4_b = _mm_loadu_si128((__m128i*)(src_ptr+m_src_stride*4));
		r5_b = _mm_loadu_si128((__m128i*)(src_ptr+m_src_stride*5));

		r0 = _mm_unpacklo_epi16(r0_b, r1_b);
		r1 = _mm_unpackhi_epi16(r0_b, r1_b);

		r2 = _mm_unpacklo_epi16(r2_b, r3_b);
		r3 = _mm_unpackhi_epi16(r2_b, r3_b);

		r4 = _mm_unpacklo_epi16(r4_b, r5_b);
		r5 = _mm_unpackhi_epi16(r4_b, r5_b);

//		r0 = _mm_maddubs_epi16( r0, filt_mul15 );

		__asm{
		mov eax, [dst]
		mov ecx, [src]
		mov esi, m_width
		mov ebx, m_src_stride

		lea edi, [ecx+ebx]				; src + 1 row
		sub ecx, ebx					; src - 1 row
		sub ecx, ebx					; src - 1 row ( total -2 )
		add eax, esi					;we will process buffer from bottom to top, keep substracting a width until we get to 0
;		lea edx, [edx+esi*2]
		neg esi							; width = -width
		movdqa xmm0, [filt_mul15]		; filter for alpha blending
loop_block:
		movdqa xmm1, [ecx]			; load row 0
		movdqa xmm4, [ecx+ebx]		; load row 1
		movdqa xmm2, [edi+ebx*2]	; load row 5
		movdqa xmm5, [edi+ebx]		; load row 4
		movdqa xmm3, [ecx+ebx*2]	; load row 2
		movdqa xmm6, [edi]			; load row 3
		movdqa xmm7, xmm1
		punpcklbw xmm1, xmm4		; unpack row 0 and row 1 into xmm1 low
		punpckhbw xmm7, xmm4		; unpack row 0 and row 1 into xmm7 high
		movdqa xmm4, xmm2			
		punpcklbw xmm2, xmm5		; unpack row 3 and row 4 into xmm2 low 
		punpckhbw xmm4, xmm5		; unpack row 3 and row 4 into xmm4 high
		movdqa xmm5, xmm3
		punpcklbw xmm3, xmm6		; unpack row 2 and row 1 into xmm3 low 
		punpckhbw xmm5, xmm6		; unpack row 2 and row 1 into xmm5 high 
		pmaddubsw xmm1, xmm0			; alpha blend rows
		pmaddubsw xmm7, xmm0			; alpha blend rows
		pmaddubsw xmm2, xmm0			; alpha blend rows
		pmaddubsw xmm4, xmm0			; alpha blend rows
		pmaddubsw xmm3, [filt_mul20]	; alpha blend rows
		pmaddubsw xmm5, [filt_mul20]	; alpha blend rows
		paddw xmm1, xmm2				; sum avg values
		paddw xmm7, xmm4				; sum avg values
		paddw xmm1, xmm3				; sum avg values
		paddw xmm7, xmm5				; sum avg values
		movdqa xmm6, [_x264_pw_16]		
;		movdqa [edx+esi*2], xmm1
;		movdqa [edx+esi*2+16], xmm7
		paddw xmm1, xmm6
		paddw xmm7, xmm6
		psraw xmm1, 5					; divide to get real avg
		psraw xmm7, 5					; divide to get real avg
		packuswb xmm1, xmm7				; pack 32 bit into 16 bit
		movntdq [eax+esi], xmm1			; store result
		add ecx, 16						; process next 16 bytes of input
		add edi, 16						; process next 16 bytes of input
		add esi, 16						; store detination here next time
		jl loop_block
		pop edi
		pop esi
		pop ebx
	};
	src += m_src_stride;
	dst += m_dst_stride;
	}
}

void read_luma_inter_pred_MxN_half_half_Encoder_asm3( short *src, int m_src_stride, unsigned char *dst, int m_dst_stride, int m_height, int m_width)
{
	src = src - 2*m_src_stride;
	__declspec(align(16)) int _x264_pw_16[8]={16,16,16,16,16,16,16,16};
	__declspec(align(16)) char filt_mul20[16]={20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20};
	__declspec(align(16)) char filt_mul15[16]={1,-5,1,-5,1,-5,1,-5,1,-5,1,-5,1,-5,1,-5};
	for(int i=0;i<m_height;i++)
	{
		short *tsrc = src;
		unsigned char *tdst = dst;
		for( int j=0;j<m_width;j+=16)
		{
			tdst += 16;
			tsrc += 16;
		__asm{
			push ebx
			push edi

			mov eax, [tdst]
			mov ecx, [tsrc]
			mov ebx, m_src_stride

			lea edi, [ecx+ebx]				; src + 1 row
			sub ecx, ebx					; src - 1 row
			sub ecx, ebx					; src - 1 row ( total -2 )

			movdqa xmm1, [ecx]				; load row 0
			movdqa xmm4, [ecx+ebx]			; load row 1
			movdqa xmm2, [edi+ebx*2]		; load row 5
			movdqa xmm5, [edi+ebx]			; load row 4
			movdqa xmm3, [ecx+ebx*2]		; load row 2
			movdqa xmm6, [edi]				; load row 3

			movdqa xmm7, xmm1
			punpcklbw xmm1, xmm4			; unpack row 0 and row 1 into xmm1 low
			punpckhbw xmm7, xmm4			; unpack row 0 and row 1 into xmm7 high
			movdqa xmm4, xmm2			
			punpcklbw xmm2, xmm5			; unpack row 3 and row 4 into xmm2 low 
			punpckhbw xmm4, xmm5			; unpack row 3 and row 4 into xmm4 high
			movdqa xmm5, xmm3
			punpcklbw xmm3, xmm6			; unpack row 2 and row 1 into xmm3 low 
			punpckhbw xmm5, xmm6			; unpack row 2 and row 1 into xmm5 high 

			pmaddubsw xmm1, [filt_mul15]	; multiply + add values then saturate : xmm1[0] = xmm1[0] * xmm0[0] + xmm1[1] * xmm0[1]
			pmaddubsw xmm7, [filt_mul15]	; multiply + add values then saturate
			pmaddubsw xmm2, [filt_mul15]	; multiply + add values then saturate
			pmaddubsw xmm4, [filt_mul15]	; multiply + add values then saturate
			pmaddubsw xmm3, [filt_mul20]	; multiply + add values then saturate
			pmaddubsw xmm5, [filt_mul20]	; multiply + add values then saturate

			paddw xmm1, xmm2				; sum avg values
			paddw xmm7, xmm4				; sum avg values
			paddw xmm1, xmm3				; sum avg values
			paddw xmm7, xmm5				; sum avg values

			movdqa xmm6, [_x264_pw_16]		; to roundup the avg values
			paddw xmm1, xmm6				; to roundup the avg values
			paddw xmm7, xmm6				; to roundup the avg values

			psraw xmm1, 5					; divide to get real avg
			psraw xmm7, 5					; divide to get real avg

			packuswb xmm1, xmm7				; pack 32 bit into 16 bit
			movntdq [eax], xmm1				; store result

			pop edi
			pop ebx
		};
	}
	src += m_src_stride;
	dst += m_dst_stride;
	}
}

void DoInterpolateTest(unsigned char* address1,int stride1, unsigned char* address2,int stride2,unsigned int* orio,unsigned int* newo)
{
	UINT so,eo,sn,en;
	UINT diffo,diffn,diffnc;
	diffo = 0;
	diffn = 0;
	diffnc = 0;
	memset( address1, 0, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( char ) );
	memset( address2, 0, MB_X_REF * MB_SIZE * MB_Y_REF * MB_SIZE * sizeof( char ) );
	for(int y=0;y<MB_Y_REF * MB_SIZE;y++)
		for(int i=0;i<MB_X_REF * MB_SIZE;i+=2)
		{
//			*(signed short*)&address1[y*REF_STRIDE+i] = ( 21 * 512 / 2) - (( y + i ) * 20) % ( 21 * 512);
			*(signed short*)&address1[y*REF_STRIDE+i] = ( 21 * 512 / 2) - (( y + i ) * 256) % ( 21 * 512);
			*(signed short*)&address2[y*REF_STRIDE+i] = ( i ) % ( 21 * 512 );
		}

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
//					read_luma_inter_pred_MxN_half_half_Encoder( (short*)refa, stride1, (unsigned char*)cura, stride2, 16, REF_STRIDE );
					read_luma_inter_pred_MxN_half_half_Encoder_intr( (short*)refa, stride1, (unsigned char*)cura, stride2, 16, REF_STRIDE );
//					read_luma_inter_pred_MxN_half_half_Encoder_intr6( (short*)refa, stride1, (unsigned char*)cura, stride2, 16, REF_STRIDE );
//					read_luma_inter_pred_MxN_half_half_Encoder_intr14( (short*)refa, stride1, (unsigned char*)cura, stride2, 16, REF_STRIDE );
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
//					read_luma_inter_pred_MxN_half_half_Encoder2( (short*)refa, stride1, (unsigned char*)cura, stride2, 16, REF_STRIDE );
//					read_luma_inter_pred_MxN_half_half_Encoder_intr( (short*)refa, stride1, (unsigned char*)cura, stride2, 16, REF_STRIDE );
//					read_luma_inter_pred_MxN_half_half_Encoder_ref( (short*)refa, stride1, (unsigned char*)cura, stride2, 16, REF_STRIDE );
//					read_luma_inter_pred_MxN_half_half_Encoder_intr2( (short*)refa, stride1, (unsigned char*)cura, stride2, 16, REF_STRIDE );
//					read_luma_inter_pred_MxN_half_half_Encoder_asm( (short*)refa, stride1, (unsigned char*)cura, stride2, 16, REF_STRIDE );
//					read_luma_inter_pred_MxN_half_half_Encoder_short( (short*)refa, stride1, (unsigned char*)cura, stride2, 16, REF_STRIDE );
//					read_luma_inter_pred_MxN_half_half_Encoder_intr3( (short*)refa, stride1, (unsigned char*)cura, stride2, 16, REF_STRIDE );
//					read_luma_inter_pred_MxN_half_half_Encoder_intr5( (short*)refa, stride1, (unsigned char*)cura, stride2, 16, REF_STRIDE );
//					read_luma_inter_pred_MxN_half_half_Encoder_intr6( (short*)refa, stride1, (unsigned char*)cura, stride2, 16, REF_STRIDE );
//					read_luma_inter_pred_MxN_half_half_Encoder_asm2( (short*)refa, stride1, (unsigned char*)cura, stride2, 16, REF_STRIDE );
//					read_luma_inter_pred_MxN_half_half_Encoder_intr7( (short*)refa, stride1, (unsigned char*)cura, stride2, 16, REF_STRIDE );
//					read_luma_inter_pred_MxN_half_half_Encoder_intr8( (short*)refa, stride1, (unsigned char*)cura, stride2, 16, REF_STRIDE );
//					read_luma_inter_pred_MxN_half_half_Encoder_intr9( (short*)refa, stride1, (unsigned char*)cura, stride2, 16, REF_STRIDE );
//					read_luma_inter_pred_MxN_half_half_Encoder_intr10( (short*)refa, stride1, (unsigned char*)cura, stride2, 16, REF_STRIDE );
//					read_luma_inter_pred_MxN_half_half_Encoder_intr11( (short*)refa, stride1, (unsigned char*)cura, stride2, 16, REF_STRIDE );
//					read_luma_inter_pred_MxN_half_half_Encoder_intr12( (short*)refa, stride1, (unsigned char*)cura, stride2, 16, REF_STRIDE );
//					read_luma_inter_pred_MxN_half_half_Encoder_intr13( (short*)refa, stride1, (unsigned char*)cura, stride2, 16, REF_STRIDE );
//					read_luma_inter_pred_MxN_half_half_Encoder_asm3( (short*)refa, stride1, (unsigned char*)cura, stride2, 16, REF_STRIDE );
//					read_luma_inter_pred_MxN_half_half_Encoder_intr14( (short*)refa, stride1, (unsigned char*)cura, stride2, 16, REF_STRIDE );
//					read_luma_inter_pred_MxN_half_half_Encoder_intr15( (short*)refa, stride1, (unsigned char*)cura, stride2, 16, REF_STRIDE );
					read_luma_inter_pred_MxN_half_half_Encoder_intr16( (short*)refa, stride1, (unsigned char*)cura, stride2, 16, REF_STRIDE );
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
					fprintf(f,"%03d ",((unsigned char*)orio)[y*REF_STRIDE+x]);
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
					fprintf(f,"%03d ",((unsigned char*)newo)[y*REF_STRIDE+x]);
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