#pragma once
class Player
{
public:
	uint64 SessionId() const
	{
		return _sessionId;
	}

	uint64 PlayerId() const
	{
		return _playerId;
	}

	uint64 CharacterId() const
	{
		return _characterId;
	}

	const wstring& Nickname() const
	{
		return _nickname;
	}

	uint64 FieldId() const
	{
		return _fieldId;
	}

public:
	Player() = default;

	void Init(uint64 sessionId)
	{
		_sessionId = sessionId;
		_fieldId = -1;
	}

	void SetPlayerId(uint64 id)
	{
		_playerId = id;
	}

	void SetCharacter(uint64 id, const wstring& nickname, uint64 fieldId)
	{
		_characterId = id;
		_nickname = nickname;
		_fieldId = fieldId;
	}

public:
	Lock    lock;

private:
	uint64	_sessionId;
	uint64	_playerId;
	uint64	_characterId;
	wstring _nickname;
	int32	_fieldId = -1;

};

