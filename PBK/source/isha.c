/*
 * isha.c
 *
 * A completely insecure and bad hashing algorithm, based loosely on
 * SHA-1 (which is itself no longer considered a good hashing
 * algorithm)
 *
 * Based on code for sha1 processing from Paul E. Jones, available at
 * https://www.packetizer.com/security/sha1/
 */

#include "isha.h"
#include <string.h>
#include <stdio.h>
#include "ticktime.h"

#define A1 0xFFFFFFFF
#define A2 0xFF

/*
 * circular shift macro
 */
#define ISHACircularShift(bits,word) \
		((((word) << (bits)) & A1) | ((word) >> (32-(bits))))


/*  
 * Processes the next 512 bits of the message stored in the MBlock
 * array.
 *
 * Parameters:
 *   ctx         The ISHAContext (in/out)
 */

static void ISHAProcessMessageBlock(ISHAContext *ctx)
{

	uint32_t temp;
	register uint32_t B;
	uint32_t A, C, D, E;
	register int t = 0;

	A = ctx->MD[0];
	B = ctx->MD[1];
	C = ctx->MD[2];
	D = ctx->MD[3];
	E = ctx->MD[4];

	while (t<16)
	{

		temp = (ISHACircularShift(5,A) + ((B & C) | ((~B) & D)) + E + ((((uint32_t) ctx->MBlock[t * 4]) << 24) | (((uint32_t) ctx->MBlock[t * 4 + 1]) << 16) | (((uint32_t) ctx->MBlock[t * 4 + 2]) << 8) | (((uint32_t) ctx->MBlock[t * 4 + 3])))) & A1;
		E = D;
		D = C;
		C = ISHACircularShift(30,B);
		B = A;
		A = temp;
		t++;
	}

	ctx->MD[0] = (ctx->MD[0] + A) & A1;
	ctx->MD[1] = (ctx->MD[1] + B) & A1;
	ctx->MD[2] = (ctx->MD[2] + C) & A1;
	ctx->MD[3] = (ctx->MD[3] + D) & A1;
	ctx->MD[4] = (ctx->MD[4] + E) & A1;

	ctx->MB_Idx = 0;

}


/*  
 * The message must be padded to an even 512 bits.  The first padding
 * bit must be a '1'.  The last 64 bits represent the length of the
 * original message.  All bits in between should be 0. This function
 * will pad the message according to those rules by filling the MBlock
 * array accordingly. It will also call ISHAProcessMessageBlock()
 * appropriately. When it returns, it can be assumed that the message
 * digest has been computed.
 *
 * Parameters:
 *   ctx         The ISHAContext (in/out)
 */
static void ISHAPadMessage(ISHAContext *ctx)
{
	/*
	 *  Check to see if the current message block is too small to hold
	 *  the initial padding bits and length.  If so, we will pad the
	 *  block, process it, and then continue padding into a second
	 *  block.
	 */
	if (ctx->MB_Idx > 55)
	{
		ctx->MBlock[ctx->MB_Idx++] = 0x80;
		memset(ctx->MBlock + ctx->MB_Idx, 0, 64 - ctx ->MB_Idx);
		ISHAProcessMessageBlock(ctx);
		memset(ctx->MBlock ,0, 64-6);
	}

	else
	{
		ctx->MBlock[ctx->MB_Idx++] = 0x80;
		memset(ctx->MBlock + ctx-> MB_Idx, 0, 60-ctx->MB_Idx);
	}

	/*
	 *  Store the message length as the last 8 octets
	 */
	ctx->MBlock[60] = (ctx->Len>> 24) & A2;
	ctx->MBlock[61] = (ctx->Len>> 16) & A2;
	ctx->MBlock[62] = (ctx->Len>> 8) & A2;
	ctx->MBlock[63] = (ctx->Len) & A2;

	ISHAProcessMessageBlock(ctx);
}


void ISHAReset(ISHAContext *ctx)
{
	ctx->Len  = 0;
	ctx->MB_Idx  = 0;

	ctx->MD[0]       = 0x67452301;
	ctx->MD[1]       = 0xEFCDAB89;
	ctx->MD[2]       = 0x98BADCFE;
	ctx->MD[3]       = 0x10325476;
	ctx->MD[4]       = 0xC3D2E1F0;

	ctx->Computed    = 0;
	ctx->Corrupted   = 0;
}


void ISHAResult(ISHAContext *ctx, uint8_t *digest_out)
{
	if (ctx->Corrupted)
	{
		return;
	}

	if (!ctx->Computed)
	{
		ISHAPadMessage(ctx);
		ctx->Computed = 1;
	}

	//the ctx->MD bytes are swapped with little endian an 8 bit pointer pointing at MD was created.
	ctx->MD[0] = __builtin_bswap32(ctx->MD[0]);
	ctx->MD[1] = __builtin_bswap32(ctx->MD[1]);
	ctx->MD[2] = __builtin_bswap32(ctx->MD[2]);
	ctx->MD[3] = __builtin_bswap32(ctx->MD[3]);
	ctx->MD[4] = __builtin_bswap32(ctx->MD[4]);
	uint8_t *ptr_MD = (uint8_t *) &ctx->MD;

	//unrolled the loop
	digest_out[0] = *(ptr_MD);
	digest_out[1] = *(ptr_MD + 1);
	digest_out[2] = *(ptr_MD + 2);
	digest_out[3] = *(ptr_MD + 3);

	digest_out[4] = *(ptr_MD + 4);
	digest_out[5] = *(ptr_MD + 5);
	digest_out[6] = *(ptr_MD + 6);
	digest_out[7] = *(ptr_MD + 7);

	digest_out[8]  = *(ptr_MD + 8);
	digest_out[9]  = *(ptr_MD + 9);
	digest_out[10] = *(ptr_MD + 10);
	digest_out[11] = *(ptr_MD + 11);

	digest_out[12] = *(ptr_MD + 12);
	digest_out[13] = *(ptr_MD + 13);
	digest_out[14] = *(ptr_MD + 14);
	digest_out[15] = *(ptr_MD + 15);

	digest_out[16] = *(ptr_MD + 16);
	digest_out[17] = *(ptr_MD + 17);
	digest_out[18] = *(ptr_MD + 18);
	digest_out[19] = *(ptr_MD + 19);


	return;
}


void ISHAInput(ISHAContext *ctx, const uint8_t *message_array, size_t length)
{
	//Length_Low and Length_High were replaced by a single message Length calculation and Corrupted check was removed
	ctx->Len += (8*length);
	int count = 0;

	while(length)
	{
		count = length;
		if ((length + ctx->MB_Idx) > ISHA_BLOCKLEN) {
			count = ISHA_BLOCKLEN - ctx->MB_Idx;
		}

		memcpy(ctx->MBlock + ctx->MB_Idx, message_array, count);
		ctx->MB_Idx += count;
		message_array += count;
		length -= count;


		if (ctx->MB_Idx == ISHA_BLOCKLEN)
		{
			ISHAProcessMessageBlock(ctx);
		}
	}
}

