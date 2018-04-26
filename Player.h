#pragma once

#include "Definitions.h"

#include "MyEntity.h"
#include "MyEntityManager.h"

namespace Player {
	void CreatePlayer();

	void SetBoosting(bool isBoosting);

	bool GetBoosting();

	void SetSpeed(float newSpeed);

	void SetPosition(Simplex::vector3 pos);

	float GetSpeed();

	float GetHealth();

	Simplex::vector3 GetPosition();

	void SetVelocity(bool collided, float distance);

	void SetHealth(float h);

	void Display();
}