#pragma once

#include <vector>

#include "Simplex/Simplex.h"
#include "MyEntity.h"
#include "MyEntityManager.h"

using namespace std;
using namespace Simplex;

class Segment
{
public:
	Segment();
	~Segment();

	std::vector<MyEntity*> m_vObstacles;

	void GenerateContents();
};

