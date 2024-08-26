#pragma once
#include "GameObject.h"

class PathFindingTestObject : GameObject
{

public:
	PathFindingTestObject() {}
protected:
	void OnUpdate()
	{
		float32 randX = (float32)(rand() % 30);
		float32 randY = (float32)(rand() % 30);
		Position dest = { randX, randY };
		GetPathReciever().RequestPathFinding(dest);
	}
};

