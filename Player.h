#pragma once

#include "Definitions.h"

#include "MyEntity.h"
#include "MyEntityManager.h"

namespace Player {
	void CreatePlayer();

	void SetBoosting(bool isBoosting);

	Simplex::vector3 GetPosition();

	void Display();
}