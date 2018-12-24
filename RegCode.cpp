#include "stdafx.h"
#include "RegCode.h"


#define VERBOSE

#define SUCCESS 0
#define FAILURE -1
#define ONT_WRAP

#define HASH_SIZE 5     /* Produces n*8-bit digest of the message */

// Base64 Encoding associated Constants
#define END_OF_BASE64_ENCODED_DATA ('=')
#define BASE64_END_OF_BUFFER       (0xFD)
#define BASE64_IGNORABLE_CHARACTER (0xFE)
#define BASE64_UNKNOWN_VALUE       (0xFF)
#define BASE64_CARRIAGE_RETURN 13
#define BASE64_LINE_FEED       10


/*
#ifdef LITTLE_ENDIAN    // Imported from Peter Gutmann's implementation

// When run on a little-endian CPU we need to perform byte reversal on an
   //array of longwords.  It is possible to make the code endianness-
   //independant by fiddling around with data at the byte level, but this
   //makes for very slow code, so we rely on the user to sort out endianness
   //at compile time

static void byteReverse( unsigned long *buffer, int byteCount )
    {
    unsigned long value;
    int count;

    byteCount /= sizeof( unsigned long );
    for( count = 0; count < byteCount; count++ )
	{
	value = ( buffer[ count ] << 16 ) | ( buffer[ count ] >> 16 );
	buffer[ count ] = ( ( value & 0xFF00FF00L ) >> 8 ) | ( ( value & 0x00FF00FFL ) << 8 );
	}
    }
#endif 
*/

union longbyte
{
    unsigned long W[80];        /* Process 16 32-bit words at a time */
    char B[320];                /* But read them as bytes for counting */
};

#define f0(x,y,z) (z ^ (x & (y ^ z)))           /* Magic functions */
#define f1(x,y,z) (x ^ y ^ z)
#define f2(x,y,z) ((x & y) | (z & (x | y)))
#define f3(x,y,z) (x ^ y ^ z)

#define K0 0x5a827999                           /* Magic constants */
#define K1 0x6ed9eba1
#define K2 0x8f1bbcdc
#define K3 0xca62c1d6

#define S(n, X) ((X << n) | (X >> (32 - n)))    /* Barrel roll */

#define r0(f, K) \
    temp = S(5, A) + f(B, C, D) + E + *p0++ + K; \
    E = D;  \
    D = C;  \
    C = S(30, B); \
    B = A;  \
    A = temp

#ifdef VERSION_0
#define r1(f, K) \
    temp = S(5, A) + f(B, C, D) + E + \
	   (*p0++ = *p1++ ^ *p2++ ^ *p3++ ^ *p4++) + K; \
    E = D;  \
    D = C;  \
    C = S(30, B); \
    B = A;  \
    A = temp
#else                   /* Version 1: Summer '94 update */
#define r1(f, K) \
    temp = *p1++ ^ *p2++ ^ *p3++ ^ *p4++; \
    temp = S(5, A) + f(B, C, D) + E + (*p0++ = S(1,temp)) + K; \
    E = D;  \
    D = C;  \
    C = S(30, B); \
    B = A;  \
    A = temp
#endif

void CRegCode::SHA(char* mem, unsigned long length, unsigned long* buf)
{
    int i, nread;
	unsigned int nbits;
    union longbyte d;
    unsigned long hi_length, lo_length;
    int padded;
    char *s;

    register unsigned long *p0, *p1, *p2, *p3, *p4;
    unsigned long A, B, C, D, E, temp;

    unsigned long h0, h1, h2, h3, h4;

    h0 = m_L0;                            /* Accumulators */
    h1 = m_L1;
    h2 = m_L2;
    h3 = m_L3;
    h4 = m_L4;

    padded = FALSE;
    s = mem;
    for (hi_length = lo_length = 0; ;)  /* Process 16 longs at a time */
    {
		if (length < 64) 
			nread = length;
		else
			nread = 64;

		length -= nread;
		memcpy(d.B, s, nread);
		s += nread;

		if (nread < 64)   /* Partial block? */
		{
			nbits = nread << 3;               /* Length: bits */
			if ((lo_length += nbits) < nbits)
				hi_length++;              /* 64-bit integer */

			if (nread < 64 && ! padded)  /* Append a single bit */
			{
				d.B[nread++] = 0x80; /* Using up next byte */
				padded = TRUE;       /* Single bit once */
			}
		
			for (i = nread; i < 64; i++) /* Pad with nulls */
				d.B[i] = 0;
			
			if (nread <= 56)   /* Room for length in this block */
			{
				d.W[14] = hi_length;
				d.W[15] = lo_length;
#ifdef LITTLE_ENDIAN
				byteReverse(d.W, 56 );
#endif /* LITTLE_ENDIAN */
			}
#ifdef LITTLE_ENDIAN
		   else 
			   byteReverse(d.W, 64 );
#endif /* LITTLE_ENDIAN */
		}
		else    /* Full block -- get efficient */
		{
			if ((lo_length += 512) < 512)
				hi_length++;    /* 64-bit integer */
#ifdef LITTLE_ENDIAN
		   byteReverse(d.W, 64 );
#endif /* LITTLE_ENDIAN */
		}

		p0 = d.W;
		A = h0; B = h1; C = h2; D = h3; E = h4;

		r0(f0,K0); r0(f0,K0); r0(f0,K0); r0(f0,K0); r0(f0,K0);
		r0(f0,K0); r0(f0,K0); r0(f0,K0); r0(f0,K0); r0(f0,K0);
		r0(f0,K0); r0(f0,K0); r0(f0,K0); r0(f0,K0); r0(f0,K0);
		r0(f0,K0);

		p1 = &d.W[13]; p2 = &d.W[8]; p3 = &d.W[2]; p4 = &d.W[0];

			   r1(f0,K0); r1(f0,K0); r1(f0,K0); r1(f0,K0);
		r1(f1,K1); r1(f1,K1); r1(f1,K1); r1(f1,K1); r1(f1,K1);
		r1(f1,K1); r1(f1,K1); r1(f1,K1); r1(f1,K1); r1(f1,K1);
		r1(f1,K1); r1(f1,K1); r1(f1,K1); r1(f1,K1); r1(f1,K1);
		r1(f1,K1); r1(f1,K1); r1(f1,K1); r1(f1,K1); r1(f1,K1);
		r1(f2,K2); r1(f2,K2); r1(f2,K2); r1(f2,K2); r1(f2,K2);
		r1(f2,K2); r1(f2,K2); r1(f2,K2); r1(f2,K2); r1(f2,K2);
		r1(f2,K2); r1(f2,K2); r1(f2,K2); r1(f2,K2); r1(f2,K2);
		r1(f2,K2); r1(f2,K2); r1(f2,K2); r1(f2,K2); r1(f2,K2);
		r1(f3,K3); r1(f3,K3); r1(f3,K3); r1(f3,K3); r1(f3,K3);
		r1(f3,K3); r1(f3,K3); r1(f3,K3); r1(f3,K3); r1(f3,K3);
		r1(f3,K3); r1(f3,K3); r1(f3,K3); r1(f3,K3); r1(f3,K3);
		r1(f3,K3); r1(f3,K3); r1(f3,K3); r1(f3,K3); r1(f3,K3);

		h0 += A; h1 += B; h2 += C; h3 += D; h4 += E;

		if (nread <= 56) /* If it's greater, length in next block */
			break; 
    }
    buf[0] = h0; buf[1] = h1; buf[2] = h2; buf[3] = h3; buf[4] = h4;
}


BOOL CRegCode::Base64Encode(BYTE* pSource, int iSourceLen, CString& destination)
{
   // Always start with an empty destination
   destination.Empty();

   const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

   int index                     = 0;
   
   DWORD number_of_bytes_encoded = 0;
   
   BYTE byte_to_add = 0;
   BYTE byte_1      = 0;
   BYTE byte_2      = 0;
   BYTE byte_3      = 0;

   const BYTE* input_buffer = pSource;

   while(index < iSourceLen)
   {
      // Output the first byte

      byte_1 = input_buffer[ index ];
      byte_to_add = alphabet[ ( byte_1 >> 2 ) ];

      destination += static_cast< TCHAR >( byte_to_add );
      number_of_bytes_encoded++;

      index++;

      if (index >= iSourceLen)
      {
         // We're at the end of the data to encode

         byte_2 = 0;
         byte_to_add = alphabet[ ( ( ( byte_1 & 0x03 ) << 4 ) | ( ( byte_2 & 0xF0 ) >> 4 ) ) ];

         destination += static_cast< TCHAR >( byte_to_add );
         //destination += static_cast< TCHAR >( END_OF_BASE64_ENCODED_DATA );
         //destination += static_cast< TCHAR >( END_OF_BASE64_ENCODED_DATA );

         return( TRUE );
      }
      else
      {
         byte_2 = input_buffer[ index ];
      }

      byte_to_add = alphabet[ ( ( ( byte_1 & 0x03 ) << 4 ) | ( ( byte_2 & 0xF0 ) >> 4 ) ) ];

      destination += static_cast< TCHAR >( byte_to_add );
      number_of_bytes_encoded++;

      index++;

      if (index >= iSourceLen)
      {
         // We ran out of bytes, we need to add the last half of byte_2 and pad
         byte_3 = 0;

         byte_to_add = alphabet[ ( ( ( byte_2 & 0x0F ) << 2 ) | ( ( byte_3 & 0xC0 ) >> 6 ) ) ];

         destination += static_cast< TCHAR >( byte_to_add );
         //destination += static_cast< TCHAR >( END_OF_BASE64_ENCODED_DATA );

         return( TRUE );
      }
      else
      {
         byte_3 = input_buffer[ index ];
      }

      index++;

      byte_to_add = alphabet[ ( ( ( byte_2 & 0x0F ) << 2 ) | ( ( byte_3 & 0xC0 ) >> 6 ) ) ];

      destination += static_cast< TCHAR >( byte_to_add );
      number_of_bytes_encoded++;

      byte_to_add = alphabet[ ( byte_3 & 0x3F ) ];

      destination += static_cast< TCHAR >( byte_to_add );
      number_of_bytes_encoded++;

      if ( ( number_of_bytes_encoded % 72 ) == 0 )
      {
         destination += static_cast< TCHAR >( BASE64_CARRIAGE_RETURN );
         destination += static_cast< TCHAR >( BASE64_LINE_FEED );
      }
   }

   //destination += static_cast< TCHAR >( END_OF_BASE64_ENCODED_DATA );

   return( TRUE );
}


BOOL CRegCode::GenerateRegCode(CString strInput, CString& strOutput)
{
	BOOL bResult(FALSE);
	int iAlphaCount = 0;
	int i(0);
	int iSize(0);

	// Trim off the input
	strInput.TrimLeft();
	strInput.TrimRight();

	// Clear the output
	strOutput = _T("");

	// Ensure the user entered at least 5 characters
	for (i=0; i < strInput.GetLength(); i++)
	{
		if ((strInput.GetAt(i) >= _T('A') && strInput.GetAt(i) <= _T('Z')) ||
			(strInput.GetAt(i) >= _T('a') && strInput.GetAt(i) <= _T('z')))
		{
			iAlphaCount++;
		}
	}
	if (iAlphaCount < 5)
		return FALSE;

	unsigned long buf[5];

	iSize = strInput.GetLength();
	SHA(strInput.GetBuffer(0), iSize, buf);
	strInput.ReleaseBuffer();

	iSize = sizeof(unsigned long) * 5;
	bResult = Base64Encode((unsigned char*)buf, iSize, strOutput);	

	return bResult;
}