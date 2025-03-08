#pragma once
/// <summary>
/// Md5校验封装
/// </summary>
/// 

#include <string>
#include <fstream>

#include "Platform/FileSystem/FileSystem.h"
namespace photon 
{
	class Md5
	{
	public:
		static std::string GetMd5(const std::string& str);
		static std::string GetMd5FromFile(const std::filesystem::path& path);

	private:
		struct Md5Context
		{
			unsigned int count[2];
			unsigned int state[4];
			unsigned char buffer[64];
		};

		static unsigned int F(unsigned int x, unsigned int y, unsigned int z);
		static unsigned int G(unsigned int x, unsigned int y, unsigned int z);
		static unsigned int H(unsigned int x, unsigned int y, unsigned int z);
		static unsigned int I(unsigned int x, unsigned int y, unsigned int z);
		static unsigned int RotateLeft(unsigned int x, unsigned int n);

		static unsigned int FF(unsigned int a, unsigned int b, unsigned int c,
			unsigned int d, unsigned int x, unsigned int s, unsigned int ac);
		static unsigned int GG(unsigned int a, unsigned int b, unsigned int c,
			unsigned int d, unsigned int x, unsigned int s, unsigned int ac);
		static unsigned int HH(unsigned int a, unsigned int b, unsigned int c,
			unsigned int d, unsigned int x, unsigned int s, unsigned int ac);
		static unsigned int II(unsigned int a, unsigned int b, unsigned int c,
			unsigned int d, unsigned int x, unsigned int s, unsigned int ac);


		static void MD5Init(Md5Context* context);
		static void MD5Update(Md5Context* context, unsigned char* input, unsigned int inputlen);
		static void MD5Final(Md5Context* context, unsigned char digest[16]);
		static void MD5Transform(unsigned int state[4], unsigned char block[64]);
		static void MD5Encode(unsigned char* output, unsigned int* input, unsigned int len);
		static void MD5Decode(unsigned int* output, unsigned char* input, unsigned int len);
	};
}