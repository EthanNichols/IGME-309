#pragma once

#include <vector>

#include "Simplex/Simplex.h"
#include "MyEntity.h"
#include "MyEntityManager.h"
#include "Segment.h"

using namespace std;
using namespace Simplex;

class World
{

	static World* m_pInstance; // Singleton pointer

public:
	World();
	~World();

	unsigned int m_iGenerationDistance = 3;
	float m_fGroundLength = 10.0f;
	float m_fDifficulty = 1.0f;

	MyEntityManager* m_pEntityMngr = nullptr;

	std::vector<Segment*> m_vSegments;

	void DoInitialGeneration();

	static World* GetInstance();

	void ReleaseInstance();

	void GenNewSegment();

	void MoveSegments();

	void CycleSegments();
};