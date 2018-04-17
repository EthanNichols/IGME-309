#include "Generation.h"

const int CHUNK_AMOUNT = 5;
const char CHUNK_NAME[] = "ground_";

Simplex::MyEntityManager* g_entityManager;

namespace Generation {

	///Initialize all of the needed variables for the map generation
	void Init() {
		g_entityManager = Simplex::MyEntityManager::GetInstance();
	}

	///Create the chunks that will be used
	void GenerateChunks() {

		//Initialize the variables if they haven't been yet
		if (g_entityManager == nullptr) Init();

		//Create all of the chunks needed for the terrain
		for (int i = 0; i < 5; i++) {

			//Create the chunk and give it a name
			g_entityManager->AddEntity("models\\model_ground.obj", CHUNK_NAME + i);

			//Center the chunk at the start
			Simplex::matrix4 groundMatrix = Simplex::IDENTITY_M4;
			groundMatrix[3][0] = -20;
			groundMatrix[3][1] = -30;
			groundMatrix[3][2] = -(g_entityManager->GetRigidBody()->GetHalfWidth().z * 2) * i;
			g_entityManager->SetModelMatrix(groundMatrix);
		}
	}

	///Update the position and dislay of the chunks
	void Display() {

		//Loop through all of the chunks
		for (int i = 0; i < 5; i++) {
			
			//Get the entitie's matrix
			Simplex::String name = CHUNK_NAME + i;
			Simplex::matrix4 groundMatrix = g_entityManager->GetModelMatrix(name);

			//Set the new position of the chunk if the player passed it
			if (groundMatrix[3][2] - (g_entityManager->GetRigidBody(name)->GetHalfWidth().z * 2) > Player::GetPosition().z) {
				groundMatrix[3][2] -= (g_entityManager->GetRigidBody(name)->GetHalfWidth().z * 2) * CHUNK_AMOUNT;
				g_entityManager->SetModelMatrix(groundMatrix, name);
			}
		}
	}
}