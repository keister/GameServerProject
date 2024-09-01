#pragma once
namespace login
{
	class Player
	{
	public:
		uint64	sessionId;
		DWORD	lastRecvTime;
		bool	isLogin;
	};
}

