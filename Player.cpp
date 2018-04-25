#include "Player.h"

static float health = 1.0f;

static Simplex::vector3 velocity;

float baseSpeed = 0.2125f;
float boostingMultiplier = 2.5f;

float hullDurability = 0.1f;

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
		Simplex::vector4 vec4Pos = p_entityManager->GetModelMatrix("ship") * Simplex::vector4(0, 0, 0, 1);

		return Simplex::vector3(vec4Pos.x, vec4Pos.y, vec4Pos.z);
	}

	void SetVelocity() {

		// Check for a collision

		//If there is a collision...
		    // Give the ship an x/z velocity so it moves away from the collision center
		    // Alter the ship's health accordingly
				// health -= (GetSpeed() * hullDurability);

		// Otherwise...
		velocity = Simplex::vector3(0.0f, 0.0f, GetSpeed());
	}

	void SetHealth(float h) {
		health = h;
	}

	void SetBoosting(bool isBoosting) {
		boosting = isBoosting;
	}

	void SetSpeed(float newSpeed) { speed = newSpeed; }

	float GetSpeed() { return baseSpeed * speed * (boosting ? boostingMultiplier : 1.0f); }

	float GetHealth() { return health; }

	bool GetBoosting() { return boosting; }

	///Update the position and dislay of the player
	void Display() {

		//Move the last entity added slowly to the right
		Simplex::matrix4 lastMatrix = p_entityManager->GetModelMatrix("ship");// get the model matrix of the last added
		lastMatrix *= glm::translate(Simplex::IDENTITY_M4, velocity); //translate it
		p_entityManager->SetModelMatrix(lastMatrix, "ship"); //return it to its owner
	}
}