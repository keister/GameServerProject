#pragma once
enum class ObjectType : uint8
{
	PLAYER,
	MONSTER,
};


inline Packet& operator<< (Packet& pkt, ObjectType type)
{
	pkt << static_cast<uint8>(type);

	return pkt;
}

inline Packet& operator>> (Packet& pkt, ObjectType& type)
{
	pkt >> *reinterpret_cast<uint8*>(&type);

	return pkt;
}

