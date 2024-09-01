#pragma once
#include "ObjectPool.h"
#include "ObjectPoolTls.h"


namespace netlib
{
	class RawPacket;
	class RingBuffer;


#pragma pack(push, 1)
	struct NetPacketHeader
	{
		BYTE	code;
		uint16	size;
		BYTE	randKey;
		BYTE	checkSum;
	};
#pragma pack(pop)

	struct LanPacketHeader
	{
		uint16	size;
	};


	/// @details 개선사항. 프리미티브 타입들의 경우 memcpy보다 캐스팅을 통한 복사가 더빨랐다.
	///	해당 작업을 통해 프리미티브 타입의 버퍼 복사 속도가 빨라짐
	class RawPacket
	{
		enum
		{
			BUFFER_DEFAULT_SIZE = 500,
		};
	public:
		friend ObjectPoolTls<RawPacket, false>;

		void	Clear();

		int32	GetBufferSize() { return _bufferSize; };
		int32	GetDataSize() { return _writePos - _readPos; };

		BYTE*	GetBufferPtr() { return &_buffer[_readPos]; }

		int32	MoveWritePos(int32 size);
		int32	MoveReadPos(int32 size);

		int32	GetData(BYTE* dest, int32 size);
		int32	PutData(const BYTE* src, int32 size);

		void	InsertNetHeader(BYTE packetCode);
		void	InsertLanHeader();

		void	Encode(uint8 key);
		bool	Decode(uint8 key);

		//-------------------------------------------------------------------------------
		// Operator Overloading
		//-------------------------------------------------------------------------------

		RawPacket& operator=(RawPacket& src);

		RawPacket& operator<<(bool value);
		RawPacket& operator<<(BYTE value);
		RawPacket& operator<<(char value);
		RawPacket& operator<<(int16 value);
		RawPacket& operator<<(uint16 value);
		RawPacket& operator<<(int32 value);
		RawPacket& operator<<(uint32 value);
		RawPacket& operator<<(int64 value);
		RawPacket& operator<<(uint64 value);
		RawPacket& operator<<(float value);
		RawPacket& operator<<(double value);
		RawPacket& operator<<(const wstring& value);

		RawPacket& operator>>(bool& value);
		RawPacket& operator>>(BYTE& value);
		RawPacket& operator>>(char& value);
		RawPacket& operator>>(int16& value);
		RawPacket& operator>>(uint16& value);
		RawPacket& operator>>(int32& value);
		RawPacket& operator>>(uint32& value);
		RawPacket& operator>>(int64& value);
		RawPacket& operator>>(uint64& value);
		RawPacket& operator>>(float& value);

		RawPacket& operator>>(double& value);

		RawPacket& operator>>(wstring& value);

		template <typename Container>
			requires (requires (Container c) {
				{ std::begin(c) };
				{ std::end(c) };
				typename Container::value_type;
		})
			&& (!requires (Container c)
		{
			typename Container::key_type;
			typename Container::mapped_type;
		})
			RawPacket& operator<<(const Container& value);

		template <typename Container>
			requires (requires (Container c) {
				{ std::begin(c) };
				{ std::end(c) };
				{ std::back_inserter(c) };
				{ std::inserter(c, std::begin(c)) };
				typename Container::value_type;
		})
			&& (!requires (Container c)
		{
			typename Container::key_type;
			typename Container::mapped_type;
		})
			RawPacket& operator>>(Container& value);

		template <typename Container>
			requires requires (Container c) {
				{ std::begin(c) };
				{ std::end(c) };
				typename Container::value_type;
				typename Container::key_type;
				typename Container::mapped_type;
		}
		RawPacket& operator<<(const Container& value);


		template <typename Container>
			requires requires (Container c) {
				{ std::begin(c) };
				{ std::end(c) };
				{ std::inserter(c, std::begin(c)) };
				typename Container::value_type;
				typename Container::key_type;
				typename Container::mapped_type;
		}
		RawPacket& operator>>(Container& value);


		void IncRef()
		{
			InterlockedIncrement((LONG*)&_refCount);
		}
		void DecRef()
		{
			//int32 ret = InterlockedDecrement((LONG*)&_refCount);

			//CRASH_ASSERT(ret >= 0);

			if (InterlockedDecrement((LONG*)&_refCount) == 0)
			{
				Free(*this);
			}
		}

		static RawPacket& Alloc()
		{
			RawPacket& ret = *_packetPool.Alloc();

			ret.Clear();

			return ret;
		}

		static void		Free(RawPacket& pkt) { _packetPool.Free(&pkt); }
		static int32	GetUseCount() { return _packetPool.GetUseCount(); }
		static int32    GetCapacity() { return _packetPool.GetCapacity(); }


	private:
		RawPacket();
		RawPacket(int32 bufferSize);

		~RawPacket();

	private:
		BYTE* _buffer;
		int32 _bufferSize;
		int32 _readPos;
		int32 _writePos;
		int32 _refCount;
		bool  _encoded = false;
		bool  _headerHas = false;


		inline static ObjectPoolTls<RawPacket, false> _packetPool;
	};
}



/// @brief STL 콘테이너들을 직렬화버퍼에 넣는 오버레이터
/// @tparam Container STL 콘테이너
template <typename Container>
	requires (requires (Container c) {
		{ std::begin(c) };
		{ std::end(c) };
		typename Container::value_type;
})
&& (!requires (Container c)
{
	typename Container::key_type;
	typename Container::mapped_type;
})
netlib::RawPacket& netlib::RawPacket::operator<<(const Container& value)
{
	uint16 size = value.size();

	*this << size;

	for (const typename Container::value_type& item : value)
	{
		*this << item;
	}

	return *this;
}

/// @brief key-value 타입의 STL 콘테이너들을 직렬화버퍼에 넣는 오버레이터
/// @tparam Container key-value 타입의 STL 콘테이너
template <typename Container>
	requires requires (Container c) {
		{ std::begin(c) };
		{ std::end(c) };
		typename Container::value_type;
		typename Container::key_type;
		typename Container::mapped_type;
}
netlib::RawPacket& netlib::RawPacket::operator<<(const Container& value)
{

	uint16 size = value.size();

	*this << size;

	for (const auto& [key, value] : value)
	{
		*this << key << value;
	}

	return *this;


}

/// @brief 직렬화버퍼에서 STL 콘테이너로 데이터를 빼오는 오퍼레이터
/// @tparam Container STL 콘테이너
template <typename Container>
	requires (requires (Container c) {
		{ std::begin(c) };
		{ std::end(c) };
		{ std::back_inserter(c) };
		{ std::inserter(c, std::begin(c)) };
		typename Container::value_type;
})
&& (!requires (Container c)
{
	typename Container::key_type;
	typename Container::mapped_type;
})
netlib::RawPacket& netlib::RawPacket::operator>>(Container& value)
{
	uint16 size;
	*this >> size;

	insert_iterator<Container> insert = inserter(value, value.begin());

	for (int32 i = 0; i < size; i++)
	{
		typename Container::value_type item;
		*this >> item;

		*insert++ = item;
	}

	return *this;

}


/// @brief 직렬화버퍼에서 key-value 타입의 STL 콘테이너로 데이터를 빼오는 오퍼레이터
/// @tparam Container key-value 타입의 STL 콘테이너
template <typename Container>
	requires requires (Container c) {
		{ std::begin(c) };
		{ std::end(c) };
		{ std::inserter(c, std::begin(c)) };
		typename Container::value_type;
		typename Container::key_type;
		typename Container::mapped_type;
}
netlib::RawPacket& netlib::RawPacket::operator>>(Container& value)
{
	uint16 size;
	*this >> size;

	insert_iterator<Container> insert = inserter(value, value.begin());

	for (int32 i = 0; i < size; i++)
	{
		typename Container::key_type key;
		typename Container::mapped_type value;
		*this >> key >> value;

		*insert++ = make_pair(key, value);
	}

	return *this;

}

inline void netlib::RawPacket::InsertNetHeader(BYTE packetCode)
{
	if (_headerHas == true)
	{
		return;
	}

	_headerHas = true;

	_readPos -= sizeof(NetPacketHeader);
	NetPacketHeader* header = reinterpret_cast<NetPacketHeader*>(GetBufferPtr());
	header->size = GetDataSize() - sizeof(NetPacketHeader);
	header->code = packetCode;
	header->randKey = rand() % UINT8_MAX;
	uint8 checkSum = 0;

	for (int32 i = sizeof(NetPacketHeader); i < GetDataSize(); i++)
	{
		checkSum += _buffer[i];
	}

	header->checkSum = checkSum;
}

inline netlib::RawPacket& netlib::RawPacket::operator<<(bool value)
{
	//PutData(reinterpret_cast<BYTE*>(&value), sizeof(bool));

	*(bool*)(_buffer + _writePos) = value;

	_writePos += sizeof(bool);

	return *this;
}

inline netlib::RawPacket& netlib::RawPacket::operator<<(BYTE value)
{
	//PutData(&value, sizeof(BYTE));

	*(_buffer + _writePos) = value;

	_writePos += sizeof(BYTE);

	return *this;
}

inline netlib::RawPacket& netlib::RawPacket::operator<<(char value)
{
	//PutData(reinterpret_cast<BYTE*>(&value), sizeof(char));

	*(char*)(_buffer + _writePos) = value;

	_writePos += sizeof(char);

	return *this;
}

inline netlib::RawPacket& netlib::RawPacket::operator<<(int16 value)
{
	//PutData(reinterpret_cast<BYTE*>(&value), sizeof(int16));

	*(int16*)(_buffer + _writePos) = value;

	_writePos += sizeof(int16);

	return *this;
}

inline netlib::RawPacket& netlib::RawPacket::operator<<(uint16 value)
{
	//PutData(reinterpret_cast<BYTE*>(&value), sizeof(uint16));

	*(uint16*)(_buffer + _writePos) = value;

	_writePos += sizeof(uint16);

	return *this;
}

inline netlib::RawPacket& netlib::RawPacket::operator<<(int32 value)
{
	//PutData(reinterpret_cast<BYTE*>(&value), sizeof(int32));

	*(int32*)(_buffer + _writePos) = value;

	_writePos += sizeof(int32);

	return *this;
}

inline netlib::RawPacket& netlib::RawPacket::operator<<(uint32 value)
{
	//PutData(reinterpret_cast<BYTE*>(&value), sizeof(uint32));


	*(uint32*)(_buffer + _writePos) = value;

	_writePos += sizeof(uint32);

	return *this;
}

inline netlib::RawPacket& netlib::RawPacket::operator<<(int64 value)
{
	//PutData(reinterpret_cast<BYTE*>(&value), sizeof(int64));

	*(int64*)(_buffer + _writePos) = value;

	_writePos += sizeof(int64);

	return *this;
}


inline netlib::RawPacket& netlib::RawPacket::operator<<(uint64 value)
{
	//PutData(reinterpret_cast<BYTE*>(&value), sizeof(uint64));

	*(uint64*)(_buffer + _writePos) = value;

	_writePos += sizeof(uint64);

	return *this;
}

inline netlib::RawPacket& netlib::RawPacket::operator<<(float value)
{
	//PutData(reinterpret_cast<BYTE*>(&value), sizeof(float));

	*(float*)(_buffer + _writePos) = value;

	_writePos += sizeof(float);

	return *this;
}

inline netlib::RawPacket& netlib::RawPacket::operator<<(double value)
{
	//PutData(reinterpret_cast<BYTE*>(&value), sizeof(double));

	*(double*)(_buffer + _writePos) = value;

	_writePos += sizeof(double);


	return *this;
}

inline netlib::RawPacket& netlib::RawPacket::operator<<(const wstring& value)
{
	uint16 size = value.size() * sizeof(WCHAR);
	PutData(reinterpret_cast<BYTE*>(&size), sizeof(uint16));
	PutData(reinterpret_cast<BYTE*>(const_cast<WCHAR*>(value.data())), size);

	return *this;
}

inline netlib::RawPacket& netlib::RawPacket::operator>>(bool& value)
{
	//GetData(reinterpret_cast<BYTE*>(&value), sizeof(bool));

	value = *(bool*)(_buffer + _readPos);
	_readPos += sizeof(bool);

	return *this;
}

inline netlib::RawPacket& netlib::RawPacket::operator>>(BYTE& value)
{
	//GetData(&value, sizeof(BYTE));

	value = *(BYTE*)(_buffer + _readPos);
	_readPos += sizeof(BYTE);

	return *this;
}

inline netlib::RawPacket& netlib::RawPacket::operator>>(char& value)
{
	//GetData(reinterpret_cast<BYTE*>(&value), sizeof(BYTE));

	value = *(char*)(_buffer + _readPos);
	_readPos += sizeof(char);

	return *this;
}

inline netlib::RawPacket& netlib::RawPacket::operator>>(int16& value)
{
	//GetData(reinterpret_cast<BYTE*>(&value), sizeof(int16));

	value = *(int16*)(_buffer + _readPos);
	_readPos += sizeof(int16);

	return *this;
}

inline netlib::RawPacket& netlib::RawPacket::operator>>(uint16& value)
{
	//GetData(reinterpret_cast<BYTE*>(&value), sizeof(uint16));

	value = *(uint16*)(_buffer + _readPos);
	_readPos += sizeof(uint16);

	return *this;
}

inline netlib::RawPacket& netlib::RawPacket::operator>>(int32& value)
{
	//GetData(reinterpret_cast<BYTE*>(&value), sizeof(int32));

	value = *(int32*)(_buffer + _readPos);
	_readPos += sizeof(int32);

	return *this;
}

inline netlib::RawPacket& netlib::RawPacket::operator>>(uint32& value)
{
	//GetData(reinterpret_cast<BYTE*>(&value), sizeof(uint32));

	value = *(uint32*)(_buffer + _readPos);
	_readPos += sizeof(uint32);

	return *this;
}

inline netlib::RawPacket& netlib::RawPacket::operator>>(int64& value)
{
	//GetData(reinterpret_cast<BYTE*>(&value), sizeof(int64));

	value = *(int64*)(_buffer + _readPos);
	_readPos += sizeof(int64);

	return *this;
}

inline netlib::RawPacket& netlib::RawPacket::operator>>(uint64& value)
{
	//GetData(reinterpret_cast<BYTE*>(&value), sizeof(uint64));

	value = *(uint64*)(_buffer + _readPos);
	_readPos += sizeof(uint64);

	return *this;
}

inline netlib::RawPacket& netlib::RawPacket::operator>>(float& value)
{
	//GetData(reinterpret_cast<BYTE*>(&value), sizeof(float));

	value = *(float*)(_buffer + _readPos);
	_readPos += sizeof(float);

	return *this;
}

inline netlib::RawPacket& netlib::RawPacket::operator>>(double& value)
{
	//GetData(reinterpret_cast<BYTE*>(&value), sizeof(double));

	value = *(double*)(_buffer + _readPos);
	_readPos += sizeof(double);

	return *this;
}

inline netlib::RawPacket& netlib::RawPacket::operator>>(wstring& value)
{
	uint16 size = 0;
	*this >> size;
	value.resize(size / 2);

	GetData(reinterpret_cast<BYTE*>(value.data()), size);

	return *this;
}


