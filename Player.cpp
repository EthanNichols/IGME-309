#include "Player.h"

float baseSpeed = 0.1f;
float boostingMultiplier = 2.5f;

float speed = 1.0f;

bool boosting = false;

Simplex::MyEntityManager* p_entityManager;

namespace Player {

	///Initialize all of the needed variables for the player
	void Init() {
		p_entityManager = Simplex::MyEntityManager::GetInstance();
	}

	///Create the player
	void CreatePlayer() {

		//Initialize the variables if they haven't been yet
		if (p_entityManager == nullptr) Init();

		//Create the player
		p_entityManager->AddEntity("models\\model_ship.obj", "ship");
		p_entityManager->SetModelMatrix(glm::rotate(Simplex::IDENTITY_M4, 180.0f, 0.0f, 1.0f, 0.0f));
	}

	///Return the position of the player
	Simplex::vector3 GetPosition() {
		Simplex::vector4 vec4Pos = p_entityManager->GetModelMatrix() * Simplex::vector4(0, 0, 0, 1);

		return Simplex::vector3(vec4Pos.x, vec4Pos.y, vec4Pos.z);
	}

	void SetBoosting(bool isBoosting) {
		boosting = isBoosting;
	}

	void SetSpeed(float newSpeed) { speed = newSpeed; }

	float GetSpeed() { return speed * (boosting ? boostingMultiplier : 1.0f); }

	bool GetBoosting() { return boosting; }

	///Update the position and dislay of the player
	void Display() {

		//Move the last entity added slowly to the right
		Simplex::matrix4 lastMatrix = p_entityManager->GetModelMatrix("ship");// get the model matrix of the last added
		lastMatrix *= glm::translate(Simplex::IDENTITY_M4, Simplex::vector3(0.0f, 0.0f, baseSpeed * speed * (boosting ? boostingMultiplier : 1.0f))); //translate it
		p_entityManager->SetModelMatrix(lastMatrix, "ship"); //return it to its owner
	}
}