#include "Md5.h"
#include "Macro.h"

#include <sstream>
#include <string>
namespace photon 
{
	static unsigned char PADDING[] = {
			0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	std::string Md5::GetMd5(const std::string& str)
	{
		std::string decrypt;
		decrypt.resize(16);
		Md5Context md5ctx;
		MD5Init(&md5ctx);
		MD5Update(&md5ctx, (unsigned char*)str.data(), str.size());
		MD5Final(&md5ctx, (unsigned char*)decrypt.data());
		return decrypt;
	}

	std::string Md5::GetMd5FromFile(const std::filesystem::path& path)
	{
		std::ifstream infile(path);
		if(!infile.is_open())
		{
			LOG_FATAL("Can't open file: {}", path.generic_string());
			return "";
		}

		std::ostringstream oss;
		std::string line;
		while(std::getline(infile, line))
		{
			if(!line.empty())
			{
				oss << line << "\n";
			}
		}
		infile.close();

		return GetMd5(oss.str());
	}

	unsigned int Md5::F(unsigned int x, unsigned int y, unsigned int z)
	{
		return ((x & y) | (~x & z));
	}

	unsigned int Md5::G(unsigned int x, unsigned int y, unsigned int z)
	{
		return ((x & z) | (y & ~z));
	}

	unsigned int Md5::H(unsigned int x, unsigned int y, unsigned int z)
	{
		return (x ^ y ^ z);
	}

	unsigned int Md5::I(unsigned int x, unsigned int y, unsigned int z)
	{
		return (y ^ (x | ~z));
	}

	unsigned int Md5::RotateLeft(unsigned int x, unsigned int n)
	{
		return ((x << n) | (x >> (32 - n)));
	}

	unsigned int Md5::FF(unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned int x, unsigned int s, unsigned int ac)
	{
		a += F(b, c, d) + x + ac; 
		a = RotateLeft(a, s); 
		a += b;
		return a;
	}

	unsigned int Md5::GG(unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned int x, unsigned int s, unsigned int ac)
	{
		a += G(b, c, d) + x + ac;
		a = RotateLeft(a, s);
		a += b;
		return a;
	}

	unsigned int Md5::HH(unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned int x, unsigned int s, unsigned int ac)
	{
		a += H(b, c, d) + x + ac;
		a = RotateLeft(a, s);
		a += b;
		return a;
	}

	unsigned int Md5::II(unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned int x, unsigned int s, unsigned int ac)
	{
		a += I(b, c, d) + x + ac;
		a = RotateLeft(a, s);
		a += b;
		return a;
	}

	void Md5::MD5Init(Md5Context* context)
	{
		context->count[0] = 0;
		context->count[1] = 0;
		context->state[0] = 0x67452301;
		context->state[1] = 0xEFCDAB89;
		context->state[2] = 0x98BADCFE;
		context->state[3] = 0x10325476;
	}

	void Md5::MD5Update(Md5Context* context, unsigned char* input, unsigned int inputlen)
	{
		unsigned int i = 0, index = 0, partlen = 0;
		index = (context->count[0] >> 3) & 0x3F;
		partlen = 64 - index;
		context->count[0] += inputlen << 3;
		if (context->count[0] < (inputlen << 3))
			context->count[1]++;
		context->count[1] += inputlen >> 29;

		if (inputlen >= partlen)
		{
			memcpy(&context->buffer[index], input, partlen);
			MD5Transform(context->state, context->buffer);
			for (i = partlen; i + 64 <= inputlen; i += 64)
				MD5Transform(context->state, &input[i]);
			index = 0;
		}
		else
		{
			i = 0;
		}
		memcpy(&context->buffer[index], &input[i], inputlen - i);
	}

	void Md5::MD5Final(Md5Context* context, unsigned char digest[16])
	{
		unsigned int index = 0, padlen = 0;
		unsigned char bits[8];
		index = (context->count[0] >> 3) & 0x3F;
		padlen = (index < 56) ? (56 - index) : (120 - index);
		MD5Encode(bits, context->count, 8);
		MD5Update(context, PADDING, padlen);
		MD5Update(context, bits, 8);
		MD5Encode(digest, context->state, 16);
	}

	void Md5::MD5Transform(unsigned int state[4], unsigned char block[64])
	{
		unsigned int a = state[0];
		unsigned int b = state[1];
		unsigned int c = state[2];
		unsigned int d = state[3];
		unsigned int x[64];

		MD5Decode(x, block, 64);
		a = FF(a, b, c, d, x[0], 7, 0xd76aa478);
		d = FF(d, a, b, c, x[1], 12, 0xe8c7b756);
		c = FF(c, d, a, b, x[2], 17, 0x242070db);
		b = FF(b, c, d, a, x[3], 22, 0xc1bdceee);
		a = FF(a, b, c, d, x[4], 7, 0xf57c0faf);
		d = FF(d, a, b, c, x[5], 12, 0x4787c62a);
		c = FF(c, d, a, b, x[6], 17, 0xa8304613);
		b = FF(b, c, d, a, x[7], 22, 0xfd469501);
		a = FF(a, b, c, d, x[8], 7, 0x698098d8);
		d = FF(d, a, b, c, x[9], 12, 0x8b44f7af);
		c = FF(c, d, a, b, x[10], 17, 0xffff5bb1);
		b = FF(b, c, d, a, x[11], 22, 0x895cd7be);
		a = FF(a, b, c, d, x[12], 7, 0x6b901122);
		d = FF(d, a, b, c, x[13], 12, 0xfd987193);
		c = FF(c, d, a, b, x[14], 17, 0xa679438e);
		b = FF(b, c, d, a, x[15], 22, 0x49b40821);


		a = GG(a, b, c, d, x[1], 5, 0xf61e2562);
		d = GG(d, a, b, c, x[6], 9, 0xc040b340);
		c = GG(c, d, a, b, x[11], 14, 0x265e5a51);
		b = GG(b, c, d, a, x[0], 20, 0xe9b6c7aa);
		a = GG(a, b, c, d, x[5], 5, 0xd62f105d);
		d = GG(d, a, b, c, x[10], 9, 0x2441453);
		c = GG(c, d, a, b, x[15], 14, 0xd8a1e681);
		b = GG(b, c, d, a, x[4], 20, 0xe7d3fbc8);
		a = GG(a, b, c, d, x[9], 5, 0x21e1cde6);
		d = GG(d, a, b, c, x[14], 9, 0xc33707d6);
		c = GG(c, d, a, b, x[3], 14, 0xf4d50d87);
		b = GG(b, c, d, a, x[8], 20, 0x455a14ed);
		a = GG(a, b, c, d, x[13], 5, 0xa9e3e905);
		d = GG(d, a, b, c, x[2], 9, 0xfcefa3f8);
		c = GG(c, d, a, b, x[7], 14, 0x676f02d9);
		b = GG(b, c, d, a, x[12], 20, 0x8d2a4c8a);


		a = HH(a, b, c, d, x[5], 4, 0xfffa3942);
		d = HH(d, a, b, c, x[8], 11, 0x8771f681);
		c = HH(c, d, a, b, x[11], 16, 0x6d9d6122);
		b = HH(b, c, d, a, x[14], 23, 0xfde5380c);
		a = HH(a, b, c, d, x[1], 4, 0xa4beea44);
		d = HH(d, a, b, c, x[4], 11, 0x4bdecfa9);
		c = HH(c, d, a, b, x[7], 16, 0xf6bb4b60);
		b = HH(b, c, d, a, x[10], 23, 0xbebfbc70);
		a = HH(a, b, c, d, x[13], 4, 0x289b7ec6);
		d = HH(d, a, b, c, x[0], 11, 0xeaa127fa);
		c = HH(c, d, a, b, x[3], 16, 0xd4ef3085);
		b = HH(b, c, d, a, x[6], 23, 0x4881d05);
		a = HH(a, b, c, d, x[9], 4, 0xd9d4d039);
		d = HH(d, a, b, c, x[12], 11, 0xe6db99e5);
		c = HH(c, d, a, b, x[15], 16, 0x1fa27cf8);
		b = HH(b, c, d, a, x[2], 23, 0xc4ac5665);


		a = II(a, b, c, d, x[0], 6, 0xf4292244);
		d = II(d, a, b, c, x[7], 10, 0x432aff97);
		c = II(c, d, a, b, x[14], 15, 0xab9423a7);
		b = II(b, c, d, a, x[5], 21, 0xfc93a039);
		a = II(a, b, c, d, x[12], 6, 0x655b59c3);
		d = II(d, a, b, c, x[3], 10, 0x8f0ccc92);
		c = II(c, d, a, b, x[10], 15, 0xffeff47d);
		b = II(b, c, d, a, x[1], 21, 0x85845dd1);
		a = II(a, b, c, d, x[8], 6, 0x6fa87e4f);
		d = II(d, a, b, c, x[15], 10, 0xfe2ce6e0);
		c = II(c, d, a, b, x[6], 15, 0xa3014314);
		b = II(b, c, d, a, x[13], 21, 0x4e0811a1);
		a = II(a, b, c, d, x[4], 6, 0xf7537e82);
		d = II(d, a, b, c, x[11], 10, 0xbd3af235);
		c = II(c, d, a, b, x[2], 15, 0x2ad7d2bb);
		b = II(b, c, d, a, x[9], 21, 0xeb86d391);

		state[0] += a;
		state[1] += b;
		state[2] += c;
		state[3] += d;
	}

	void Md5::MD5Encode(unsigned char* output, unsigned int* input, unsigned int len)
	{
		unsigned int i = 0, j = 0;
		while (j < len)
		{
			output[j] = input[i] & 0xFF;
			output[j + 1] = (input[i] >> 8) & 0xFF;
			output[j + 2] = (input[i] >> 16) & 0xFF;
			output[j + 3] = (input[i] >> 24) & 0xFF;
			i++;
			j += 4;
		}
	}

	void Md5::MD5Decode(unsigned int* output, unsigned char* input, unsigned int len)
	{
		unsigned int i = 0, j = 0;
		while (j < len)
		{
			output[i] = (input[j]) |
				(input[j + 1] << 8) |
				(input[j + 2] << 16) |
				(input[j + 3] << 24);
			i++;
			j += 4;
		}
	}

}