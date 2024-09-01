#pragma once


namespace game
{
	enum class ObjectType : uint8
	{
		PLAYER,
		MONSTER,
	};


	inline RawPacket& operator<< (RawPacket& pkt, ObjectType type)
	{
		pkt << static_cast<uint8>(type);

		return pkt;
	}

	inline RawPacket& operator>> (RawPacket& pkt, ObjectType& type)
	{
		pkt >> *reinterpret_cast<uint8*>(&type);

		return pkt;
	}
}


