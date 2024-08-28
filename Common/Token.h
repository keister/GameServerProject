#pragma once

#include "Packet.h"
#include <random>

enum
{
	TOKEN_SIZE = 256
};

struct Token
{
	string token;

	static void Generate(Token* token)
	{
		static minstd_rand randomGen{ random_device{}() };
		static uniform_int_distribution distribution(0x21, 0x7E);
		// 0x21 ~ 0x7E

		token->token.resize(256);
		for (int32 i = 0; i < TOKEN_SIZE; i++)
		{
			token->token[i] = distribution(randomGen);
		}
	}


	void operator= (const Token& rhs)
	{
		//memcpy(token, rhs.token, TOKEN_SIZE);
	}

	bool operator== (const Token& rhs) const
	{
		return token == rhs.token;
	}
	//
	// bool operator== (const string& rhs) const
	// {
	// 	if (memcmp(token, rhs.c_str(), TOKEN_SIZE) == 0)
	// 		return true;
	// 	else
	// 		return false;
	// }
};

inline RawPacket& operator>> (RawPacket& pkt, Token& token)
{
	uint16 size = 0;
	pkt >> size;
	token.token.resize(size);

	pkt.GetData(reinterpret_cast<BYTE*>(token.token.data()), size);
	return pkt;
}

inline RawPacket& operator<< (RawPacket& pkt, const Token& token)
{

	uint16 size = token.token.size();
	pkt << size;
	pkt.PutData(reinterpret_cast<BYTE*>(const_cast<char*>(token.token.data())), size);

	return pkt;
}

