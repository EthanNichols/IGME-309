#pragma once

#include <vector>

#include "World.h"

class Segment
{
	World* m_pWorld;
	std::vector<MyEntity*> m_vObstacles;

public:
	Segment();
	~Segment();

	void GenerateContents();
};

