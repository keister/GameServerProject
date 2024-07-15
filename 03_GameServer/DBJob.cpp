#include "stdafx.h"
#include "DBJob.h"

#include "SqlSession.h"

void DBJob::CreateCharacter::Execute()
{
	mysqlx::Schema sch = GetSqlSession().getSchema("game");

	try
	{
		sch.getTable("character")
			.insert("id", "player_id", "idx", "nickname", "model_id", "weapon_id")
			.values(_id, _playerId, _idx, _nickname, _modelId, _weaponId)
			.execute();
	}
	catch (const mysqlx::Error& e)
	{
		cout << e << endl;
	}

}
