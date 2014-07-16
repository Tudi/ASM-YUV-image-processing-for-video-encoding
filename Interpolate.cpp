{\
    int realign = (intptr_t)src & (align-1);\
    src -= realign;\
    dstv -= realign;\
    dstc -= realign;\
    dsth -= realign;\
    width += realign;\
    while( height-- )\
    {\
        x264_hpel_filter_v_##cpuv( dstv, src, buf+8, stride, width );\
        x264_hpel_filter_c_##cpuc( dstc, buf+8, width );\
        x264_hpel_filter_h_##cpuh( dsth, src, width );\
        dsth += stride;\
        dstv += stride;\
        dstc += stride;\
        src  += stride;\
    }\
    x264_sfence();\
}

[global _x264_hpel_filter_v_ssse3]
[align 16]
 _x264_hpel_filter_v_ssse3:
 push ebx
 push esi
 push edi
 mov eax, [esp + 12 + 4 + 0*4]
 mov ecx, [esp + 12 + 4 + 1*4]
 mov edx, [esp + 12 + 4 + 2*4]
 mov ebx, [esp + 12 + 4 + 3*4]
 mov esi, [esp + 12 + 4 + 4*4]

 lea edi, [ecx+ebx]
 sub ecx, ebx
 sub ecx, ebx
 add eax, esi
 lea edx, [edx+esi*2]
 neg esi
 movdqa xmm0, [filt_mul15]
.loop:
 movdqa xmm1, [ecx]
 movdqa xmm4, [ecx+ebx]
 movdqa xmm2, [edi+ebx*2]
 movdqa xmm5, [edi+ebx]
 movdqa xmm3, [ecx+ebx*2]
 movdqa xmm6, [edi]
 movdqa xmm7, xmm1
 punpcklbw xmm1, xmm4
 punpckhbw xmm7, xmm4
 movdqa xmm4, xmm2
 punpcklbw xmm2, xmm5
 punpckhbw xmm4, xmm5
 movdqa xmm5, xmm3
 punpcklbw xmm3, xmm6
 punpckhbw xmm5, xmm6
 pmaddubsw xmm1, xmm0
 pmaddubsw xmm7, xmm0
 pmaddubsw xmm2, xmm0
 pmaddubsw xmm4, xmm0
 pmaddubsw xmm3, [filt_mul20]
 pmaddubsw xmm5, [filt_mul20]
 paddw xmm1, xmm2
 paddw xmm7, xmm4
 paddw xmm1, xmm3
 paddw xmm7, xmm5
 movdqa xmm6, [_x264_pw_16]
 movdqa [edx+esi*2], xmm1
 movdqa [edx+esi*2+16], xmm7
 paddw xmm1, xmm6
 paddw xmm7, xmm6
 psraw xmm1, 5
 psraw xmm7, 5
 packuswb xmm1, xmm7
 movntdq [eax+esi], xmm1
 add ecx, 16
 add edi, 16
 add esi, 16
 jl .loop
 pop edi
 pop esi
 pop ebx
 ret
 
 [global _x264_hpel_filter_c_ssse3]
[align 16]
 _x264_hpel_filter_c_ssse3:
 mov eax, [esp + 0 + 4 + 0*4]
 mov ecx, [esp + 0 + 4 + 1*4]
 mov edx, [esp + 0 + 4 + 2*4]

 add eax, edx
 lea ecx, [ecx+edx*2]
 neg edx
 movdqa xmm7, [_x264_pw_32]
 movdqa xmm0, [ecx+edx*2-16]
 movdqa xmm1, [ecx+edx*2]
.loop:
 movdqa xmm2, [ecx+edx*2+16]
 movdqa xmm4, xmm1
 palignr xmm4, xmm0, 12
 movdqa xmm5, xmm1
 palignr xmm5, xmm0, 14
 movdqa xmm0, xmm2
 palignr xmm0, xmm1, 6
 paddw xmm4, xmm0
 movdqa xmm0, xmm2
 palignr xmm0, xmm1, 4
 paddw xmm5, xmm0
 movdqa xmm6, xmm2
 palignr xmm6, xmm1, 2
 paddw xmm6, xmm1
 psubw xmm4, xmm5
 psraw xmm4, 2
 psubw xmm4, xmm5
 paddw xmm4, xmm6
 psraw xmm4, 2
 paddw xmm4, xmm6

 movdqa xmm0, xmm2
 movdqa xmm5, xmm2
 palignr xmm2, xmm1, 12
 palignr xmm5, xmm1, 14
 movdqa xmm1, [ecx+edx*2+32]
 movdqa xmm3, xmm1
 palignr xmm3, xmm0, 6
 paddw xmm3, xmm2
 movdqa xmm6, xmm1
 palignr xmm6, xmm0, 4
 paddw xmm5, xmm6
 movdqa xmm6, xmm1
 palignr xmm6, xmm0, 2
 paddw xmm6, xmm0
 psubw xmm3, xmm5
 psraw xmm3, 2
 psubw xmm3, xmm5
 paddw xmm3, xmm6
 psraw xmm3, 2
 paddw xmm3, xmm6
 paddw xmm4, xmm7
 paddw xmm3, xmm7
 psraw xmm4, 6
 psraw xmm3, 6
 packuswb xmm4, xmm3
 movntps [eax+edx], xmm4
 add edx, 16
 jl .loop
 rep ret
 
 [global _x264_hpel_filter_h_ssse3]
%line 578+0 mc-a2.asm
[align 16]
 _x264_hpel_filter_h_ssse3:
 mov eax, [esp + 0 + 4 + 0*4]
 mov ecx, [esp + 0 + 4 + 1*4]
 mov edx, [esp + 0 + 4 + 2*4]
 add eax, edx
 add ecx, edx
 neg edx
 movdqa xmm0, [ecx+edx-16]
 movdqa xmm1, [ecx+edx]
 movdqa xmm7, [_x264_pw_16]
.loop:
 movdqa xmm2, [ecx+edx+16]
 movdqa xmm3, xmm1
 palignr xmm3, xmm0, 14
 movdqa xmm4, xmm1
 palignr xmm4, xmm0, 15
 movdqa xmm0, xmm2
 palignr xmm0, xmm1, 2
 pmaddubsw xmm3, [filt_mul15]
 pmaddubsw xmm4, [filt_mul15]
 pmaddubsw xmm0, [filt_mul51]
 movdqa xmm5, xmm2
 palignr xmm5, xmm1, 1
 movdqa xmm6, xmm2
 palignr xmm6, xmm1, 3
 paddw xmm3, xmm0
 movdqa xmm0, xmm1
 pmaddubsw xmm1, [filt_mul20]
 pmaddubsw xmm5, [filt_mul20]
 pmaddubsw xmm6, [filt_mul51]
 paddw xmm3, xmm1
 paddw xmm4, xmm5
 paddw xmm4, xmm6
 paddw xmm3, xmm7
 paddw xmm4, xmm7
 psraw xmm3, 5
 psraw xmm4, 5
 packuswb xmm3, xmm4
 pshufb xmm3, [hpel_shuf]
 movdqa xmm1, xmm2
 movntps [eax+edx], xmm3
 add edx, 16
 jl .loop
 rep ret
 
void FASTCALL CPredictionReader::read_luma_inter_pred_MxN_half_half_Encoder( SHORT *src, BYTE*dst)
{

	BYTE *dst_backup = dst; 
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
		r5_b = _mm_loadu_si128((__m128i*)(src_ptr+stride3+stride2));
		for( j = 0; j < theight; j++)
		{
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
