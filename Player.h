#pragma once

#include "Definitions.h"

#include "MyEntity.h"
#include "MyEntityManager.h"

namespace Player {

	void CreatePlayer();

	Simplex::vector3 GetPosition();

	void Display();
}