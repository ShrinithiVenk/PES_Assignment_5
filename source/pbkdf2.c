/*
 * pbkdf2.c
 *
 * A perfectly legitimate implementation of HMAC and PBKDF2, but based
 * on the "ISHA" insecure and bad hashing algorithm.
 * 
 * Author: Howdy Pierce, howdy.pierce@colorado.edu
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "pbkdf2.h"


/*
 * See function description in pbkdf2.h
 */
void hmac_isha(const uint8_t *key, size_t key_len,
		const uint8_t *msg, size_t msg_len,
		uint8_t *digest)
{
	uint8_t ipad[ISHA_BLOCKLEN];
	uint8_t opad[ISHA_BLOCKLEN];
	uint8_t keypad[ISHA_BLOCKLEN];
	uint8_t inner_digest[ISHA_DIGESTLEN];
	register size_t i;
	ISHAContext ctx;

	if (key_len > ISHA_BLOCKLEN) {
		    ISHAReset(&ctx);
		    ISHAInput(&ctx, key, key_len);
		    ISHAResult(&ctx, keypad);

	} else {
		// key_len <= ISHA_BLOCKLEN; copy key into keypad, zero pad the result
		i = ISHA_BLOCKLEN;
		while (i--)
		{
			keypad[i] = i<key_len ? key[i] : 0x00;
			ipad[i] = keypad[i] ^ 0x36;
			opad[i] = keypad[i] ^ 0x5c;
		}
	}

	// Perform inner ISHA
	ISHAReset(&ctx);
	ISHAInput(&ctx, ipad, ISHA_BLOCKLEN);
	ISHAInput(&ctx, msg, msg_len);
	ISHAResult(&ctx, inner_digest);

	// perform outer ISHA
	ISHAReset(&ctx);
	ISHAInput(&ctx, opad, ISHA_BLOCKLEN);
	ISHAInput(&ctx, inner_digest, ISHA_DIGESTLEN);
	ISHAResult(&ctx, digest);
}


/*
 * Implements the F function as defined in RFC 8018 section 5.2
 *
 * Parameters:
 *   pass      The password
 *   pass_len  length of pass
 *   salt      The salt
 *   salt_len  length of salt
 *   iter      The iteration count ("c" in RFC 8018)
 *   blkidx    the block index ("i" in RFC 8018)
 *   result    The result, which is ISHA_DIGESTLEN bytes long
 * 
 * Returns:
 *   The result of computing the F function, in result
 */
static void F(const uint8_t *pass, size_t pass_len,
		const uint8_t *salt, size_t salt_len,
		int iter, unsigned int blkidx, uint8_t *result)
{
	uint8_t temp[ISHA_DIGESTLEN];
	static uint8_t saltplus[2048];
	size_t i = 0;
	assert(salt_len + 4 <= sizeof(saltplus));

	while(i<salt_len)
	{
		saltplus[i] = salt[i];
		i++;
	}

	//the blkidx bytes are swapped to make it little endian and an 8 bit pointer pointing at blkidx was created
	blkidx = __builtin_bswap32(blkidx);
	uint8_t *ptr_blkidx = (uint8_t *) &blkidx;

	saltplus[i] = *ptr_blkidx;
	saltplus[i+1] = *(ptr_blkidx + 1);
	saltplus[i+2] = *(ptr_blkidx + 2);
	saltplus[i+3] = *(ptr_blkidx + 3);

	hmac_isha(pass, pass_len, saltplus, salt_len+4, temp);
    //unrolled the loop
	result[0] = temp[0];
	result[1] = temp[1];
	result[2] = temp[2];
	result[3] = temp[3];
	result[4] = temp[4];
	result[5] = temp[5];
	result[6] = temp[6];
	result[7] = temp[7];
	result[8] = temp[8];
	result[9] = temp[9];
	result[10] = temp[10];
	result[11] = temp[11];
	result[12] = temp[12];
	result[13] = temp[13];
	result[14] = temp[14];
	result[15] = temp[15];
	result[16] = temp[16];
	result[17] = temp[17];
	result[18] = temp[18];
	result[19] = temp[19];


	register int j=iter;
	while (--j){
		hmac_isha(pass, pass_len, temp, ISHA_DIGESTLEN, temp);
		//unrolled the loop
		result[0] ^= temp[0];
		result[1] ^= temp[1];
		result[2] ^= temp[2];
		result[3] ^= temp[3];
		result[4] ^= temp[4];
		result[5] ^= temp[5];
		result[6] ^= temp[6];
		result[7] ^= temp[7];
		result[8] ^= temp[8];
		result[9] ^= temp[9];
		result[10] ^= temp[10];
		result[11] ^= temp[11];
		result[12] ^= temp[12];
		result[13] ^= temp[13];
		result[14] ^= temp[14];
		result[15] ^= temp[15];
		result[16] ^= temp[16];
		result[17] ^= temp[17];
		result[18] ^= temp[18];
		result[19] ^= temp[19];
	}
}
/*
 * See function description in pbkdf2.h
 */
void pbkdf2_hmac_isha(const uint8_t *pass, size_t pass_len,
		const uint8_t *salt, size_t salt_len, int iter, size_t dkLen, uint8_t *DK)
{
	size_t i = 0;
	static uint8_t accumulator[2560];
	assert(dkLen < sizeof(accumulator));

	i = (dkLen / ISHA_DIGESTLEN + 1);
	while(i--)
	{
		F(pass, pass_len, salt, salt_len, iter, i+1, accumulator + i*ISHA_DIGESTLEN);
	}
	i = dkLen;
	while(i--)
	{
		DK[i] = accumulator[i];
	}
}




