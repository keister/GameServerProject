#pragma once


namespace netlib
{
	class NetworkAddress;
	class RawPacket;

	class IServer
	{
		friend SessionManager;
	protected:
		virtual void OnStart() = 0;
		virtual void OnStop() = 0;
		virtual bool OnConnectionRequest(const NetworkAddress& netInfo) = 0;
		virtual void OnAccept(const NetworkAddress& netInfo, uint64 sessionId) = 0;
		virtual void OnDisconnect(uint64 sessionId) = 0;
	};
}

