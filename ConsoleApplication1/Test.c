/////////////////////////////////////////////////////////////////////////////
//===========================================================================
#include <stdio.h>
#include <string.h>




/////////////////////////////////////////////////////////////////////////////
//===========================================================================
#include "iconv/iconv.h"





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
typedef unsigned char bool;
#define true 1
#define false 0
#define nullptr 0





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
bool convert_charset
(
	const char* icharset, unsigned char* istring, unsigned int isize, unsigned int* irsize,
	const char* ocharset, unsigned char* ostring, unsigned int osize, unsigned int* owsize
)
{
	//--------------------------------------------------------------------------
	bool result;


	result = true;


	//--------------------------------------------------------------------------
	iconv_t descriptor;


	char* ipointer;
	unsigned int ileft;

	char* opointer;
	unsigned int oleft;

	unsigned int converted;


	int rc;
	int error;


	ipointer = (char*)(istring);
	opointer = (char*)(ostring);
	ileft = isize;
	oleft = osize;

	descriptor = iconv_open(ocharset, icharset);
	if (((iconv_t)-1) == descriptor)
	{
		if (nullptr != irsize)
		{
			*irsize = 0u;
		}

		if (nullptr != owsize)
		{
			*owsize = 0u;
		}

		return false;
	}

	// iconv() 널문자 자동으로 안붙임 입력에 널문자 포함되어야 변환함
	converted = iconv(descriptor, &ipointer, &ileft, &opointer, &oleft);
	if (converted < 0)
	{
		error = errno;

		result = false;
	}

	rc = iconv_close(descriptor);
	if (rc < 0)
	{
		error = errno;

		result = false;
	}

	//--------------------------------------------------------------------------
	if (true == result)
	{
		if (nullptr != irsize)
		{
			*irsize = (unsigned int)(isize - ileft);
		}

		if (nullptr != owsize)
		{
			*owsize = (unsigned int)(osize - oleft);
		}
	}
	else
	{
		if (nullptr != irsize)
		{
			*irsize = 0u;
		}

		if (nullptr != owsize)
		{
			*owsize = 0u;
		}
	}

	return result;
}





/////////////////////////////////////////////////////////////////////////////
//===========================================================================
/*
# iconv 1.17 소스 포팅
config.h
iconv.c
iconv.h
libcharset.h
localcharset.c
localcharset.h
relocatable.c
*/

//===========================================================================
void print_hex_string(const char* dst)
{
	unsigned int i;
	unsigned int count;
	unsigned char ch;


	count = strlen(dst);
	for (i = 0U; i < count; i++)
	{
		ch = *(dst + i);
		printf("%02x ", ch);
	}
	printf("\r\n");
}

//===========================================================================
int main()
{
	//---------------------------------------------------------
	/*
	가
	완성형 : 0xb0a1
	조합형 : 0x8861
	utf16  : 0xac00
	utf8   : 234(0xEA), 176(0xB0), 128(0x80)
	*/


	//---------------------------------------------------------
	unsigned char ibuffer[8] = { 0xb0, 0xa1, 0x00 };
	unsigned char obuffer[8];
	bool result;



	//---------------------------------------------------------
	const char* icharset = "EUC-KR";
	const char* ocharset = "UTF-8";

	unsigned char* istring = ibuffer;
	unsigned int  isize = strlen(ibuffer) + 1U;
	unsigned int  irsize = 0;
	unsigned char* ostring = obuffer;
	unsigned int  osize = sizeof(obuffer);
	unsigned int  owsize = 0;


	result = convert_charset(icharset, istring, isize, &irsize, ocharset, ostring, osize, &owsize);
	print_hex_string(istring);
	print_hex_string(ostring);

	/*
	b0 a1
	ea b0 80
	*/

	return 0;
}
