#include "Generation.h"

#include "MyCollider.h"

const int CHUNK_AMOUNT = 5;
const char* CHUNK_NAME = "ground_";

const int PILLAR_AMOUNT = 6;
char* pillarFiles[PILLAR_AMOUNT] = {
    "models\\model_pillar_0.obj"
    "models\\model_pillar_1.obj"
    "models\\model_pillar_2.obj"
    "models\\model_pillar_3.obj"
    "models\\model_pillar_4.obj"
    "models\\model_pillar_5.obj"
};
static Simplex::Collider pillarColliders[PILLAR_AMOUNT] = {
    Simplex::Collider({
        glm::vec2(+1.0f, +1.0f),
        glm::vec2(+1.0f, -1.0f),
        glm::vec2(-1.0f, -1.0f),
        glm::vec2(-1.0f, +1.0f),
    }, 3.0f),
    Simplex::Collider({
        glm::vec2(+1.0f, +1.0f),
        glm::vec2(+1.0f, -1.0f),
        glm::vec2(-1.0f, -1.0f),
        glm::vec2(-1.0f, +1.0f),
    }, 3.0f),
    Simplex::Collider({
        glm::vec2(+1.0f, +1.0f),
        glm::vec2(+1.0f, -1.0f),
        glm::vec2(-1.0f, -1.0f),
        glm::vec2(-1.0f, +1.0f),
    }, 3.0f),
    Simplex::Collider({
        glm::vec2(+1.0f, +1.0f),
        glm::vec2(+1.0f, -1.0f),
        glm::vec2(-1.0f, -1.0f),
        glm::vec2(-1.0f, +1.0f),
    }, 3.0f),
    Simplex::Collider({
        glm::vec2(+1.0f, +1.0f),
        glm::vec2(+1.0f, -1.0f),
        glm::vec2(-1.0f, -1.0f),
        glm::vec2(-1.0f, +1.0f),
    }, 3.0f),
    Simplex::Collider({
        glm::vec2(+1.0f, +1.0f),
        glm::vec2(+1.0f, -1.0f),
        glm::vec2(-1.0f, -1.0f),
        glm::vec2(-1.0f, +1.0f),
    }, 3.0f)
};
static Simplex::Collider groundCollider({
    glm::vec2(+5.0f, +2.0f),
    glm::vec2(+5.0f, -2.0f),
    glm::vec2(-5.0f, -2.0f),
    glm::vec2(-5.0f, +2.0f),
}, 3.0f);
int pillarCount = 0;

Simplex::vector3 chunkSize;
float zOffset = .7;

Simplex::MyEntityManager* g_entityManager;

namespace Generation {

    /// Parse a mat4 and return it's position as a vector3
    Simplex::vector3 Mat4Position(Simplex::matrix4 mat4) {
        Simplex::vector3 pos;
        pos.x = mat4[3][0];
        pos.y = mat4[3][1];
        pos.z = mat4[3][2];
        return pos;
    }

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
            g_entityManager->AddEntity("models\\model_ground.obj", groundCollider, CHUNK_NAME + i);

            //Set the half size of the chunk
            chunkSize = g_entityManager->GetHalfWidth();

            //Center the chunk at the start
            Simplex::matrix4 groundMatrix = Simplex::IDENTITY_M4;

            //Calculate and set the offset of the pillar
            groundMatrix = glm::translate(groundMatrix, Simplex::vector3(-20, -30, -(chunkSize.z * 2 - zOffset) * i));
            g_entityManager->SetModelMatrix(groundMatrix);

            //Spawn pillars at the new chunk position
            //THIS WILL BE REMOVED, FOR TESTING ONLY
            SpawnPillars(Mat4Position(groundMatrix));
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
            if (Mat4Position(groundMatrix).z - (chunkSize.z * 2) > Player::GetPosition().z) {

                //Calulate the new position of the chunk relative to its current position
                Simplex::vector3 newPos = Mat4Position(groundMatrix);
                newPos.z -= (chunkSize.z * 2 - zOffset) * CHUNK_AMOUNT;
                groundMatrix = glm::translate(Simplex::IDENTITY_M4, newPos);

                //Set the new position for the chunk
                g_entityManager->SetModelMatrix(groundMatrix, name);

                //Spawn pillars at the new chunk position
                SpawnPillars(Mat4Position(groundMatrix));
            }
        }
    }

    void DestroyPillars() {
        //TODO:
        //Delete all of the pillars that were apart of a chunk that despawned
    }

    /// Spawn pillars relative to the position
    void SpawnPillars(Simplex::vector3 centerPos) {
        int pillarAmount = rand() % 5 + 1;

        //Loop through the random amount of pillars to create
        for (int i = 0; i < pillarAmount; i++) {

            auto pillaridx = rand() % PILLAR_AMOUNT;

            //Create a new pillar
            g_entityManager->AddEntity(pillarFiles[pillaridx], pillarColliders[pillaridx], "Pillar_" + pillarCount);

            //Randomly set an offset for the pillar
            Simplex::vector3 offset = Simplex::vector3((rand() % (int)(chunkSize.x * 2)) - chunkSize.x, 7, (rand() % (int)(chunkSize.z * 2)) - chunkSize.z);

            //Calculate the position of the pillar relative to the chunk position
            Simplex::matrix4 pos = glm::translate(Simplex::IDENTITY_M4, centerPos + offset);

            //Set the offsetted local position to the chunk
            g_entityManager->SetModelMatrix(pos);

            //Increase the amount of pillars spawned
            pillarCount++;
        }
    }
}