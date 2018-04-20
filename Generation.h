#pragma once

#include "Definitions.h"

#include "MyEntity.h"
#include "MyEntityManager.h"

#include "Player.h"

namespace Generation {

	void GenerateChunks();
	void Display();

	void SpawnPillars(Simplex::vector3 centerPos);
	void DestroyPillars();
}