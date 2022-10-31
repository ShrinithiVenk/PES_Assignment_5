# PES_ASSIGNMENT OPTIMIZATION:


### Runtime
8744ms (Previous)
2763ms (Post Optimization)
### Code Size
21056 bytes (Previous)
21096 bytes (Post Optimization)

## Code Optimization - Following Techniques were used:

###Loop Unrolling:

In IshaResult function, loop unrolling was done to replace the for statement.
Initial:
  for (int i=0; i<20; i+=4) {
    digest_out[i]   = (ctx->MD[i/4] & 0xff000000) >> 24;
    digest_out[i+1] = (ctx->MD[i/4] & 0x00ff0000) >> 16;
    digest_out[i+2] = (ctx->MD[i/4] & 0x0000ff00) >> 8;
    digest_out[i+3] = (ctx->MD[i/4] & 0x000000ff);
  }

Post Optimization:
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

             ###Loop Jamming:
             ###Loop Inversion
             ###Defining Macros
             ###Using register variables
             ###Replacing for with while loops
             ###Using string library functions
             ###Little endianess
             ###




1) ISHAProcessMessageBlock( ):
W(t) loop was combined into a single loop and was added directly into the temp variable.
            For loop was supplanted with the while loop and 

Originally:
for(t = 0; t < 16; t++) { W[t] = ((uint32_t) ctx->MBlock[t * 4]) << 24; W[t] |= ((uint32_t) ctx->MBlock[t * 4 + 1]) << 16; W[t] |= ((uint32_t) ctx->MBlock[t * 4 + 2]) << 8; W[t] |= ((uint32_t) ctx->MBlock[t * 4 + 3]); }

for(t = 0; t < 16; t++) { temp = ISHACircularShift(5,A) + ((B & C) | ((~B) & D)) + E + W[t]; temp &= 0xFFFFFFFF; E = D; D = C; C = ISHACircularShift(30,B); B = A; A = temp; }

Updated:
while(t<16) { temp = (ISHACircularShift(5,A) + ((B & C) | ((~B) & D)) + E + ( (((uint32_t) ctx->MBlock[t4]) << 24) | (((uint32_t) ctx->MBlock[t4+1]) << 16) | (((uint32_t) ctx->MBlock[t4+2]) << 8) | ( ((uint32_t) ctx->MBlock[t4+3]))) ) & 0xFFFFFFFF ; E = D; D = C; C = ISHACircularShift(30,B); B = A; A = temp; t++; }

2) ISHAPadMessage():
• Padding logic was changed to incorporate processing data from a single length file • memset replaced setting to '0' logic • Padding of length had to be recalculated in terms of bytes and bits

Originally:
if (ctx->MB_Idx > 55) { ctx->MBlock[ctx->MB_Idx++] = 0x80; while(ctx->MB_Idx < 64) { ctx->MBlock[ctx->MB_Idx++] = 0; } ISHAProcessMessageBlock(ctx); while(ctx->MB_Idx < 56) { ctx->MBlock[ctx->MB_Idx++] = 0; } } else { ctx->MBlock[ctx->MB_Idx++] = 0x80; while(ctx->MB_Idx < 56) { ctx->MBlock[ctx->MB_Idx++] = 0; } } ctx->MBlock[56] = (ctx->Length_High >> 24) & 0xFF; ctx->MBlock[57] = (ctx->Length_High >> 16) & 0xFF; ctx->MBlock[58] = (ctx->Length_High >> 8) & 0xFF; ctx->MBlock[59] = (ctx->Length_High) & 0xFF; ctx->MBlock[60] = (ctx->Length_Low >> 24) & 0xFF; ctx->MBlock[61] = (ctx->Length_Low >> 16) & 0xFF; ctx->MBlock[62] = (ctx->Length_Low >> 8) & 0xFF; ctx->MBlock[63] = (ctx->Length_Low) & 0xFF;

Updated:
/Change: While loops replaced by memset for setting to '0' padding/ if (ctx->MB_Idx > 55) { ctx->MBlock[ctx->MB_Idx++] = 0x80;

    memset(ctx->MBlock + ctx->MB_Idx, 0, ISHA_BLOCKLEN - ctx->MB_Idx);
    ISHAProcessMessageBlock(ctx);
    memset(ctx->MBlock, 0, ISHA_BLOCKLEN - 6);
  }
  else
  {
	ctx->MBlock[ctx->MB_Idx++] = 0x80;
	memset(ctx->MBlock + ctx->MB_Idx,0, 59 - ctx->MB_Idx); 
  }
/*

Store the message length as the last 8 octets */
/* Change: Store the message length as last 5 bytes

Everything is shifted over by 2 bits therefore using 29, 21,13, 5, 3

Since it accounts for the 2 bit shift when converting from

bytes to bits.

Big endian / /

Padding logic was changed to incorporate processing data from a single length file

Padding of length had to be recalculated in terms of bytes and bits */

ctx->MBlock[59] = (ctx->len_byte >> constant1) & 0xFF; ctx->MBlock[60] = (ctx->len_byte >> constant2) & 0xFF; ctx->MBlock[61] = (ctx->len_byte >> constant3) & 0xFF; ctx->MBlock[62] = (ctx->len_byte >> constant4) & 0xFF; ctx->MBlock[63] = (ctx->len_byte << constant5) & 0xFF;

3) ISHAResult( ):
• All the big-endian calculations were replaced with __builtin_bswap32.

Originally:
for (int i=0; i<20; i+=4) { digest_out[i] = (ctx->MD[i/4] & 0xff000000) >> 24; digest_out[i+1] = (ctx->MD[i/4] & 0x00ff0000) >> 16; digest_out[i+2] = (ctx->MD[i/4] & 0x0000ff00) >> 8; digest_out[i+3] = (ctx->MD[i/4] & 0x000000ff); }

Updated:
*((uint32_t *)(digest_out))=__builtin_bswap32(ctx->MD[0]); *((uint32_t *)(digest_out + 4))=__builtin_bswap32(ctx->MD[1]); *((uint32_t *)(digest_out + 8))=__builtin_bswap32(ctx->MD[2]); *((uint32_t *)(digest_out + 12))=__builtin_bswap32(ctx->MD[3]); *((uint32_t *)(digest_out + 16))=__builtin_bswap32(ctx->MD[4]);

4) ISHAInput( ):
• Length_Low and Length_High was replaced by a single Length byte calculation and Corrupted check was removed

Originally:
if (ctx->Computed || ctx->Corrupted) { ctx->Corrupted = 1; return; } while(length-- && !ctx->Corrupted) { ctx->MBlock[ctx->MB_Idx++] = (*message_array & 0xFF); ctx->Length_Low += 8; ctx->Length_Low &= 0xFFFFFFFF; if (ctx->Length_Low == 0) { ctx->Length_High++; ctx->Length_High &= 0xFFFFFFFF; if (ctx->Length_High == 0) { ctx->Corrupted = 1; } }

Updated:
int temp_length = 0;
/* Corrupted check was removed */

ctx->len_byte += length; /* Length_Low and Length_High were replaced by a single len_byte calculation */

while(length) { /* * According to the capacity if the message is too big then store * as much as possible or store the entire message * */ temp_length = length; if ( (ISHA_BLOCKLEN - ctx->MB_Idx) < length) { temp_length = ISHA_BLOCKLEN - ctx->MB_Idx; }

/*
 * Memcpy is used to Store message at location ctx->MBlock[ctx->MB_Idx]
 */
memcpy(ctx->MBlock + ctx->MB_Idx, message_array, temp_length);

/*Change */
length -= temp_length;          // Updating the length which is to store
ctx->MB_Idx += temp_length;     // Updating index by temp length
message_array += temp_length;   // Updating ptr to msg

if (ctx->MB_Idx == ISHA_BLOCKLEN)
  {
   ISHAProcessMessageBlock(ctx);
}
}
}

5) F( ):
Originally:
for (i=0; i<salt_len; i++) saltplus[i] = salt[i];

// append blkidx in 4 bytes big endian saltplus[i] = (blkidx & 0xff000000) >> 24; saltplus[i+1] = (blkidx & 0x00ff0000) >> 16; saltplus[i+2] = (blkidx & 0x0000ff00) >> 8; saltplus[i+3] = (blkidx & 0x000000ff);

hmac_isha(pass, pass_len, saltplus, salt_len+4, temp); for (int i=0; i<ISHA_DIGESTLEN; i++) result[i] = temp[i];

for (int j=1; j<iter; j++) { hmac_isha(pass, pass_len, temp, ISHA_DIGESTLEN, temp); for (int i=0; i<ISHA_DIGESTLEN; i++) result[i] ^= temp[i];

Updated:
/* Replaced for loop by using memcpy */ memcpy(saltplus, salt, salt_len); i = salt_len; // i value is changed to salt len

// append blkidx in 4 bytes big endian saltplus[i] = (blkidx & 0xff000000) >> 24; saltplus[i+1] = (blkidx & 0x00ff0000) >> 16; saltplus[i+2] = (blkidx & 0x0000ff00) >> 8; saltplus[i+3] = (blkidx & 0x000000ff);

hmac_isha(pass, pass_len, saltplus, salt_len+4, temp);
/* Replaced for loop by using memcpy */ memcpy(result, temp, ISHA_DIGESTLEN);

register int j=1;
while(j<iter) {
    hmac_isha(pass, pass_len, temp, ISHA_DIGESTLEN, temp);
    // Unrolling the XOR loop
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
  j++;
6) hmac_isha( ):
• Replaced while loop with memcpy

Originally:
if (key_len > ISHA_BLOCKLEN) { // If key_len > ISHA_BLOCKLEN reset it to key=ISHA(key) ISHAReset(&ctx); ISHAInput(&ctx, key, key_len); ISHAResult(&ctx, keypad);

} else { // key_len <= ISHA_BLOCKLEN; copy key into keypad, zero pad the result for (i=0; i<key_len; i++) keypad[i] = key[i]; for(i=key_len; i<ISHA_BLOCKLEN; i++) keypad[i] = 0x00; }

Updated:
/* Change: Replaced while loop with memcpy */ memcpy(keypad,key,key_len); memset(keypad+key_len,0x00,ISHA_BLOCKLEN);








while (t<16)
  //for(t = 0; t < 16; t++)
      {

		   temp = (ISHACircularShift(5,A) + ((B & C) | ((~B) & D)) + E + ((((uint32_t) ctx->MBlock[t * 4]) << 24) | (((uint32_t) ctx->MBlock[t * 4 + 1]) << 16) | (((uint32_t) ctx->MBlock[t * 4 + 2]) << 8) | (((uint32_t) ctx->MBlock[t * 4 + 3])))) & A1;
		   E = D;
		   D = C;
		   C = ISHACircularShift(30,B);
		   B = A;
		   A = temp;
		   t++;
	  }

2.. 	if (ctx->MB_Idx > 55)
	  {
	    ctx->MBlock[ctx->MB_Idx++] = 0x80;
	    memset(ctx->MBlock + ctx->MB_Idx, 0, 64 - ctx ->MB_Idx);
	    ISHAProcessMessageBlock(ctx);
	    memset(ctx->MBlock ,0, 64-6);
	  }
//	    while(ctx->MB_Idx < 64)
//	    {
//	      ctx->MBlock[ctx->MB_Idx++] = 0;
//	    }
//
//	    ISHAProcessMessageBlock(ctx);
//	  }
	  else
	  {
	    ctx->MBlock[ctx->MB_Idx++] = 0x80;
	    memset(ctx->MBlock + ctx-> MB_Idx, 0, 56-ctx->MB_Idx);
	  }
//	  while(ctx->MB_Idx < 56)
//	    {
//		  ctx->MBlock[ctx->MB_Idx++] = 0;
//	    }

3.    *  Store the message length as the last 8 octets
   */
  ctx->MBlock[56] = (ctx->Len>> 56) & A2;
  ctx->MBlock[57] = (ctx->Len>> 48) & A2;
  ctx->MBlock[58] = (ctx->Len>> 40) & A2;
  ctx->MBlock[59] = (ctx->Len>> 32) & A2;
  ctx->MBlock[60] = (ctx->Len>> 24) & A2;
  ctx->MBlock[61] = (ctx->Len>> 16) & A2;
  ctx->MBlock[62] = (ctx->Len>> 8) & A2;
  ctx->MBlock[63] = (ctx->Len) & A2;

4.   ctx->MD[0] = __builtin_bswap32(ctx->MD[0]);
  ctx->MD[1] = __builtin_bswap32(ctx->MD[1]);
  ctx->MD[2] = __builtin_bswap32(ctx->MD[2]);
  ctx->MD[3] = __builtin_bswap32(ctx->MD[3]);
  ctx->MD[4] = __builtin_bswap32(ctx->MD[4]);
  uint8_t *ptr_MD = (uint8_t *) &ctx->MD;

//  for (int i=0; i<20; i+=4) {
//    digest_out[i]   = (ctx->MD[i/4] & 0xff000000) >> 24;
//    digest_out[i+1] = (ctx->MD[i/4] & 0x00ff0000) >> 16;
//    digest_out[i+2] = (ctx->MD[i/4] & 0x0000ff00) >> 8;
//    digest_out[i+3] = (ctx->MD[i/4] & 0x000000ff);
//  }

//  *((uint32_t *)(digest_out)   = __builtin_bswap32(ctx->MD[0]);
//  *((uint32_t *)(digest_out+ 4) = __builtin_bswap32(ctx->MD[1]);
//  *((uint32_t *)(digest_out+8) = __builtin_bswap32(ctx->MD[2]);
//  *((uint32_t *)(digest_out+16) = __builtin_bswap32(ctx->MD[3]);
//  *((uint32_t *)(digest_out+32) = __builtin_bswap32(ctx->MD[4]);

 // digest_out[0]   = (ctx->MD[0/4] & 0xff000000) >> 24;

  digest_out[0]   = *(ptr_MD);
  digest_out[1] = *(ptr_MD + 1);
  digest_out[2] = *(ptr_MD + 2);
  digest_out[3] = *(ptr_MD + 3);

  digest_out[4]   = *(ptr_MD + 4);
  digest_out[5] = *(ptr_MD + 5);
  digest_out[6] = *(ptr_MD + 6);
  digest_out[7] = *(ptr_MD + 7);

  digest_out[8]   = *(ptr_MD + 8);
  digest_out[9] = *(ptr_MD + 9);
  digest_out[10] = *(ptr_MD + 10);
  digest_out[11] = *(ptr_MD + 11);

  digest_out[12]   = *(ptr_MD + 12);
  digest_out[13] = *(ptr_MD + 13);
  digest_out[14] = *(ptr_MD + 14);
  digest_out[15] = *(ptr_MD + 15);

  digest_out[16]   = *(ptr_MD + 16);
  digest_out[17] = *(ptr_MD + 17);
  digest_out[18] = *(ptr_MD + 18);
  digest_out[19] = *(ptr_MD + 19);


  return;

5.  ctx->Len += (8*length);
  int count = 0;

  while(length)
  {
	  count = length;
	if (length + ctx->MB_Idx > 64) {
		count = ISHA_BLOCKLEN - ctx->MB_Idx;
	}

	memcpy(ctx->MBlock + ctx->MB_Idx, message_array, count);
	ctx->MB_Idx += count;
	message_array += count;
	length -= count;


    if (ctx->MB_Idx == 64)
    {
      ISHAProcessMessageBlock(ctx);
    }
  }

6, {
   uint8_t ipad[ISHA_BLOCKLEN];
   uint8_t opad[ISHA_BLOCKLEN];
   uint8_t keypad[ISHA_BLOCKLEN];
   uint8_t inner_digest[ISHA_DIGESTLEN];
   register size_t i;
   ISHAContext ctx;
   memcpy(keypad, key, key_len);

  if (key_len > ISHA_BLOCKLEN) {
    // If key_len > ISHA_BLOCKLEN reset it to key=ISHA(key)
//    ISHAReset(&ctx);
//    ISHAInput(&ctx, key, key_len);
    ISHAResult(&ctx, keypad);

  } else {
    // key_len <= ISHA_BLOCKLEN; copy key into keypad, zero pad the result
	  i = ISHA_BLOCKLEN;
	  while (i--)
	 {
	     keypad[i] = i<key_len ? key[i] : 0x00;
//	 }
//  }
//
//  i = ISHA_BLOCKLEN;
//  while (i--)
//  {
	    ipad[i] = keypad[i] ^ 0x36;
	    opad[i] = keypad[i] ^ 0x5c;
  }
  }
  // XOR key into ipad and opad
//  for (i=0; i<ISHA_BLOCKLEN; i++) {
//    ipad[i] = keypad[i] ^ 0x36;
//    opad[i] = keypad[i] ^ 0x5c;
//  }

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





7. while(i<salt_len)
  {
  //for (i=0; i<salt_len; i++)
  saltplus[i] = salt[i];
  i++;
  }

8.  hmac_isha(pass, pass_len, saltplus, salt_len+4, temp);

  i = ISHA_DIGESTLEN;
  while(i--)
  {
  //for (int i=0; i<ISHA_DIGESTLEN; i++)
    result[i] = temp[i];
  }

  int j=iter;
  while (--j)
  //for (int j=1; j<iter; j++)
	  {
    hmac_isha(pass, pass_len, temp, ISHA_DIGESTLEN, temp);
    int i = ISHA_DIGESTLEN;
    while(i--)
    //for (int i=0; i<ISHA_DIGESTLEN; i++)
    {
      result[i] ^= temp[i];
    }
  }
}

9.  i = (dkLen / ISHA_DIGESTLEN + 1);
 while(i--)
 {
    F(pass, pass_len, salt, salt_len, iter, i+1, accumulator + i*ISHA_DIGESTLEN);
 }

 i = dkLen;
 while(i--)
  //for (size_t i=0; i<dkLen; i++)
  {
    DK[i] = accumulator[i];
  }


