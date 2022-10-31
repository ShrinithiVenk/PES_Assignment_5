# PES_ASSIGNMENT OPTIMIZATION:


### Runtime
8744ms (Previous)<br /> 
2764ms (Post Optimization)
### Code Size
21056 bytes (Previous)<br /> 
21008 bytes (Post Optimization)

#### Code Optimization - Following Techniques were used:

## Loop Unrolling:

1. In IshaResult function, loop unrolling was done to replace the for statement.<br /> 
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
	
2. In function F, loop unrolling was done to replace for loops:
Initial: 
             
	   hmac_isha(pass, pass_len, saltplus, salt_len+4, temp);
           for (int i=0; i<ISHA_DIGESTLEN; i++)
           result[i] = temp[i];

           for (int j=1; j<iter; j++) {
           hmac_isha(pass, pass_len, temp, ISHA_DIGESTLEN, temp);
           for (int i=0; i<ISHA_DIGESTLEN; i++)
           result[i] ^= temp[i];
	   
Post Optimization:

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
	     
## Loop Jamming:
3. In IshaProcessMessageBlock function, two for loops with same conditions were jammed togther to make as a one loop and was then chnaged to while loop for optimization.
Initial:

           for(t = 0; t < 16; t++)
           {
           W[t] = ((uint32_t) ctx->MBlock[t * 4]) << 24;
           W[t] |= ((uint32_t) ctx->MBlock[t * 4 + 1]) << 16;
           W[t] |= ((uint32_t) ctx->MBlock[t * 4 + 2]) << 8;
           W[t] |= ((uint32_t) ctx->MBlock[t * 4 + 3]);
           }

          for(t = 0; t < 16; t++)
          {
           temp = ISHACircularShift(5,A) + ((B & C) | ((~B) & D)) + E + W[t];
           temp &= 0xFFFFFFFF;
            E = D;
            D = C;
            C = ISHACircularShift(30,B);
            B = A;
            A = temp;
            }
	   
	   
Post Optimization:

	while (t<16)
	{

		temp = (ISHACircularShift(5,A) + ((B & C) | ((~B) & D)) + E + ((((uint32_t) ctx->MBlock[t * 4]) << 24) | (((uint32_t) ctx->MBlock[t * 4 + 1]) << 16) |                 (((uint32_t) ctx->MBlock[t * 4 + 2]) << 8) | (((uint32_t) ctx->MBlock[t * 4 + 3])))) & A1;
		E = D;
		D = C;
		C = ISHACircularShift(30,B);
		B = A;
		A = temp;
		t++;
	}
2. In hmac_isha , the three for loops were concatenated to make into one and was converted to while for optimization.
Initial:
            
	    for (i=0; i<key_len; i++)
            keypad[i] = key[i];
            for(i=key_len; i<ISHA_BLOCKLEN; i++)
            keypad[i] = 0x00;
            }

          // XOR key into ipad and opad
          for (i=0; i<ISHA_BLOCKLEN; i++) {
          ipad[i] = keypad[i] ^ 0x36;
          opad[i] = keypad[i] ^ 0x5c;
	  }
Post Optimization:
     
         	i = ISHA_BLOCKLEN;
		while (i--)
		{
			keypad[i] = i<key_len ? key[i] : 0x00;
			ipad[i] = keypad[i] ^ 0x36;
			opad[i] = keypad[i] ^ 0x5c;
		}

## Loop Inversion:
1. The two changes from Loop jamming above also shows the loop inversions. Apart, in function pbkf2_hmac_isha, the for loops were inverted to while loops.
Initial:
            
	       int l = dkLen / ISHA_DIGESTLEN + 1;
               for (int i=0; i<l; i++) {
               F(pass, pass_len, salt, salt_len, iter, i+1, accumulator + i*ISHA_DIGESTLEN);
            }
                for (size_t i=0; i<dkLen; i++) {
                DK[i] = accumulator[i];
            }
	    
Post Optimization:

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
	
## Defining Macros
1. The values  0xFFFFFFFF and 0xFF were defined as macros for effcient reusability:

          #define A1 0xFFFFFFFF
          #define A2 0xFF
	  
## Using register variables
1. In isha.c file, the variable B and t were declared as register variables directly to access the direct memory location and hence optimize the code.
2. In pbkf2.c file, the variables i and j were declared as register variables directly to access the direct memory location and hence optimize the code.

             register uint32_t B;
	     register int t = 0;
	     register size_t i;
	     register int j=iter;
	     
## Using string library functions
1. memcpy and memset string functions were used to improve the code reusability and lessen the space and hence time:
Initial:

               if (ctx->MB_Idx > 55)
              {
              ctx->MBlock[ctx->MB_Idx++] = 0x80;
              while(ctx->MB_Idx < 64)
               {
              ctx->MBlock[ctx->MB_Idx++] = 0;
               }

              ISHAProcessMessageBlock(ctx);

              while(ctx->MB_Idx < 56)
              {
             ctx->MBlock[ctx->MB_Idx++] = 0;
              }
             }
            else
            {
            ctx->MBlock[ctx->MB_Idx++] = 0x80;
            while(ctx->MB_Idx < 56)
            {
            ctx->MBlock[ctx->MB_Idx++] = 0;
             }
             }

Post Optimization:

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
	
	
## Little endianess
1. Big endianess was converted to small endianess and pointers were used to access the variables. Changes were made in ishaResult and F functions respectively.

Initial:

    for (int i=0; i<20; i+=4) {
    digest_out[i]   = (ctx->MD[i/4] & 0xff000000) >> 24;
    digest_out[i+1] = (ctx->MD[i/4] & 0x00ff0000) >> 16;
    digest_out[i+2] = (ctx->MD[i/4] & 0x0000ff00) >> 8;
    digest_out[i+3] = (ctx->MD[i/4] & 0x000000ff);
    }
    ///////////////////////////////////////////
     saltplus[i] = (blkidx & 0xff000000) >> 24;
     saltplus[i+1] = (blkidx & 0x00ff0000) >> 16;
     saltplus[i+2] = (blkidx & 0x0000ff00) >> 8;
     saltplus[i+3] = (blkidx & 0x000000ff);

Post Optimization:

        ctx->MD[0] = __builtin_bswap32(ctx->MD[0]);
	ctx->MD[1] = __builtin_bswap32(ctx->MD[1]);
	ctx->MD[2] = __builtin_bswap32(ctx->MD[2]);
	ctx->MD[3] = __builtin_bswap32(ctx->MD[3]);
	ctx->MD[4] = __builtin_bswap32(ctx->MD[4]);
	uint8_t *ptr_MD = (uint8_t *) &ctx->MD;
	///////////////////////////////////////////
	blkidx = __builtin_bswap32(blkidx);
	uint8_t *ptr_blkidx = (uint8_t *) &blkidx;
	
	saltplus[i] = *ptr_blkidx;
	saltplus[i+1] = *(ptr_blkidx + 1);
	saltplus[i+2] = *(ptr_blkidx + 2);
	saltplus[i+3] = *(ptr_blkidx + 3);
	
	
	
## Code shrinking:
1. Initially, in ishaInput, there were some redundant statements that were removed and also Length was concatenated to being used as a single varaible.
          
	  
	   {
         if (!length)
         {
         return;
         }

         if (ctx->Computed || ctx->Corrupted)
         {
        ctx->Corrupted = 1;
        return;
         }

        while(length-- && !ctx->Corrupted)
         {
        
	   ctx->MBlock[ctx->MB_Idx++] = (*message_array & 0xFF);

        ctx->Length_Low += 8;
        / * Force it to 32 bits */
        ctx->Length_Low &= 0xFFFFFFFF;
        if (ctx->Length_Low == 0)
       {
        ctx->Length_High++;
        /* Force it to 32 bits */
        ctx->Length_High &= 0xFFFFFFFF;
        if (ctx->Length_High == 0)
       {
        /* Message is too long */
        ctx->Corrupted = 1;
       }
       }

       if (ctx->MB_Idx == 64)
       {
       ISHAProcessMessageBlock(ctx);
       }

       message_array++;
       }
       } 

Post Optimiaztion:

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
       
2. Length_Low and Length_high were replaced with one variable.
	
	   	ctx->MBlock[60] = (ctx->Len>> 24) & A2;
	        ctx->MBlock[61] = (ctx->Len>> 16) & A2;
	        ctx->MBlock[62] = (ctx->Len>> 8) & A2;
	        ctx->MBlock[63] = (ctx->Len) & A2;


## Static declarations:
1. Static declarations were introduced in places of local variables used in functions.

       static uint8_t saltplus[2048];
       static uint8_t accumulator[2560];
