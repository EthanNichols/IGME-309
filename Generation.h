#pragma once

#include <vector>

#include "Definitions.h"

#include "MyEntity.h"
#include "MyEntityManager.h"

#include "Player.h"

namespace Generation {

	void GenerateChunks();
	void Display();

	void ChangeChunkAmount(int amount);
	void ChangeMaxPillars(int amount);

	void SpawnPillars(Simplex::vector3 centerPos);
	void DestroyPillars();
}