// ConsoleApplication1.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include<vector>
#include<string>

#include "iconv/iconv.h"


/////////////////////////////////////////////////////////////////////////////
//===========================================================================
bool convert_charset
(
	const char* icharset, std::uint8_t* istring, std::uint32_t isize, std::uint32_t* irsize,
	const char* ocharset, std::uint8_t* ostring, std::uint32_t osize, std::uint32_t* owsize
)
{
	//--------------------------------------------------------------------------
	bool result;


	result = true;


	//--------------------------------------------------------------------------
	iconv_t descriptor;


		char* ipointer;
	::size_t ileft;

	char* opointer;
	::size_t oleft;

	::size_t converted;


	int rc;
	int error;


	ipointer = reinterpret_cast<char*>(istring);
	opointer = reinterpret_cast<char*>(ostring);
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
			*irsize = static_cast<std::uint32_t>(isize - ileft);
		}

		if (nullptr != owsize)
		{
			*owsize = static_cast<std::uint32_t>(osize - oleft);
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

std::string utf8_string_to_string(std::string input, const char* cs)
{
	std::string s;

	const char* icharset = "UTF-8";
	const char* ocharset = cs;

	std::uint8_t* istring;
	std::uint32_t  isize;
	std::uint32_t  irsize;
	std::uint8_t* ostring;
	std::uint32_t  osize;
	std::uint32_t  owsize;

	bool result;


	istring = reinterpret_cast<std::uint8_t*>(const_cast<char*>(input.c_str()));
	isize = static_cast<std::uint32_t>(input.size() + 1);
	irsize = 0u;


	std::vector<char> buffer(isize * 4u);

	ostring = reinterpret_cast<std::uint8_t*>(&buffer[0]);
	osize = static_cast<std::uint32_t>(buffer.size() + 1);
	owsize = 0u;


	result = convert_charset(icharset, istring, isize, &irsize, ocharset, ostring, osize, &owsize);


	if (true == result)
	{
		s = &buffer[0];
	}

	return s;
}

std::string string_to_utf8_string(std::string input, const char* cs)
{
	std::string s;

	const char* icharset = cs;
	const char* ocharset = "UTF-8";

	std::uint8_t* istring;
	std::uint32_t  isize;
	std::uint32_t  irsize;
	std::uint8_t* ostring;
	std::uint32_t  osize;
	std::uint32_t  owsize;

	bool result;


	istring = reinterpret_cast<std::uint8_t*>(const_cast<char*>(input.c_str()));
	isize = static_cast<std::uint32_t>(input.size() + 1);
	irsize = 0u;


	std::vector<char> buffer(isize * 4u);

	ostring = reinterpret_cast<std::uint8_t*>(&buffer[0]);
	osize = static_cast<std::uint32_t>(buffer.size() + 1);
	owsize = 0u;


	result = convert_charset(icharset, istring, isize, &irsize, ocharset, ostring, osize, &owsize);


	if (true == result)
	{
		s = &buffer[0];
	}

	return s;
}


void print_dst_string(const std::string& dst)
{
	std::size_t i;
	std::size_t count;
	std::uint8_t ch;

	count = dst.size();
	for (i = 0U; i < count; i++)
	{
		ch = dst.at(i);
		printf("%02x ", ch);
	}
	printf("\r\n");
}

/*
config.h
iconv.c
iconv.h
libcharset.h
localcharset.c
localcharset.h
relocatable.c
*/

int main()
{
/*
가
완성형 : 0xb0a1
조합형 : 0x8861
utf16 : 0xac00
utf8 : 234(0xEA), 176(0xB0), 128(0x80)
*/

	std::string src;
	std::string dst;


	//---------------------------------------------------------
	//완성형(euc-kr) -> utf8
	src = "가";
	dst = string_to_utf8_string(src, "EUC-KR");
	print_dst_string(dst);


	//---------------------------------------------------------
	//utf8 -> 완성형(euc-kr)
	src.clear();
	src.push_back(0xEA);
	src.push_back(0xB0);
	src.push_back(0x80);
	dst = utf8_string_to_string(src, "EUC-KR");
	print_dst_string(dst);


	//---------------------------------------------------------
	//utf8 - > 조합형
	dst = utf8_string_to_string(src, "JOHAB");
	print_dst_string(dst);


	//---------------------------------------------------------
	//조합형 -> utf8
	src.clear();
	src.push_back(0x88);
	src.push_back(0x61);
	dst = string_to_utf8_string(src, "JOHAB");
	print_dst_string(dst);

	return 0;
}
