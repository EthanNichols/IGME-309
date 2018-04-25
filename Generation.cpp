#include "Generation.h"

#include <cstdlib>

#include "MyCollider.h"

//The max chunk amount and the current chunk count
int CHUNK_AMOUNT = 10;
int chunks = 0;

//The chunk and pillar name to access the entity
const char* CHUNK_NAME = "ground_";
const char* PILLAR_NAME = "pillar_";

//The random amount of pillars that can spawn and the clamped max
float PILLAR_AMOUNT = 1;
int MAX_PILLARS = 5;

//Paths to the siz different pillar models
char* pillarFiles[6] = {
    "models\\model_pillar_0.obj",
    "models\\model_pillar_1.obj",
    "models\\model_pillar_2.obj",
    "models\\model_pillar_3.obj",
    "models\\model_pillar_4.obj",
    "models\\model_pillar_5.obj",
};

Simplex::Collider pillarColliders[6] = {
    Simplex::Collider({
        glm::vec2(1.0f, 1.0f),
        glm::vec2(1.0f, -1.0f),
        glm::vec2(-1.0f, -1.0f),
        glm::vec2(-1.0f, 1.0f)
    }, 3.0f),
    Simplex::Collider({
        glm::vec2(1.0f, 1.0f),
        glm::vec2(1.0f, -1.0f),
        glm::vec2(-1.0f, -1.0f),
        glm::vec2(-1.0f, 1.0f)
    }, 3.0f),
    Simplex::Collider({
        glm::vec2(1.0f, 1.0f),
        glm::vec2(1.0f, -1.0f),
        glm::vec2(-1.0f, -1.0f),
        glm::vec2(-1.0f, 1.0f)
    }, 3.0f),
    Simplex::Collider({
        glm::vec2(1.0f, 1.0f),
        glm::vec2(1.0f, -1.0f),
        glm::vec2(-1.0f, -1.0f),
        glm::vec2(-1.0f, 1.0f)
    }, 3.0f),
    Simplex::Collider({
        glm::vec2(1.0f, 1.0f),
        glm::vec2(1.0f, -1.0f),
        glm::vec2(-1.0f, -1.0f),
        glm::vec2(-1.0f, 1.0f)
    }, 3.0f),
    Simplex::Collider({
        glm::vec2(1.0f, 1.0f),
        glm::vec2(1.0f, -1.0f),
        glm::vec2(-1.0f, -1.0f),
        glm::vec2(-1.0f, 1.0f)
    }, 3.0f)
};

Simplex::Collider groundCollider({
    glm::vec2(-10.0f, 3.0f),
    glm::vec2(10.0f, 3.0f),
    glm::vec2(10.0f, -3.0f),
    glm::vec2(-10.0f, -3.0f)
}, 10.0f);

//The amount of pillars created and deleted
int pillarCount = 0;
int pillarsDeleted = 0;

//Random range for the pillar's scale
float scaleRange = 2.0f;

//The size of the chunk and the offsets
Simplex::vector3 chunkSize;
float zOffset = 0.7f;
int chunkZOffset = 0;

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
    for (int i = 0; i < CHUNK_AMOUNT; i++) {

        //Set the name of the chunk to access
        Simplex::String name = CHUNK_NAME;
        name += std::to_string(i);

        //Create the chunk and give it a name
        g_entityManager->AddEntity("models\\model_ground.obj", groundCollider, name);

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

        //Increase the chance for pillars to spawn and increase the chunkamount
        PILLAR_AMOUNT += (std::rand() % 100) / 200.0f;
        chunks++;
    }
}

void ChangeChunkAmount(int amount) {

    //Set the previous and new chunk amount
    int prevAmount = CHUNK_AMOUNT;
    CHUNK_AMOUNT += amount;

    //Limit the min chunk amount to 2
    if (CHUNK_AMOUNT <= 2) {
        CHUNK_AMOUNT = 2;
    }

    if (CHUNK_AMOUNT == prevAmount) { return; }

    //Loop until the chunk count matches the chunk amount
    while (CHUNK_AMOUNT < chunks) {

        //Get the name of the chunk
        Simplex::String name = CHUNK_NAME;
        name += std::to_string(chunks - 1);

        //Delete the chunk and decrease the chunk count
        g_entityManager->RemoveEntity(name);
        chunks--;
    }

    //Loop until all the new chunks are created
    while (CHUNK_AMOUNT > chunks) {

        //Set the name of the chunk to access
        Simplex::String name = CHUNK_NAME;
        name += std::to_string(chunks);

        //Create a new chunk with the name
        g_entityManager->AddEntity("models\\model_ground.obj", groundCollider, name);

        //Center the chunk at the start
        Simplex::matrix4 groundMatrix = Simplex::IDENTITY_M4;

        //Calculate and set the offset of the pillar
        groundMatrix = Simplex::IDENTITY_M4;
        g_entityManager->SetModelMatrix(groundMatrix, name);

        //Increase the amount of chunks that have been created
        chunks++;
    }

    //Reset the position of all the chunks
    for (int i = 0; i < chunks; i++) {

        //Get the name of the chunk
        Simplex::String name = CHUNK_NAME;
        name += std::to_string(i);

        //Start with a identity matrix
        Simplex::matrix4 groundMatrix = Simplex::IDENTITY_M4;

        //Set the new position of the chunk
        groundMatrix = glm::translate(groundMatrix, Simplex::vector3(-20, -30, -(chunkSize.z * 2 - zOffset) * (i + chunkZOffset)));
        g_entityManager->SetModelMatrix(groundMatrix, name);

        //Only spawn pillars on newly created chunks
        if (i >= prevAmount) {
            SpawnPillars(Mat4Position(groundMatrix));
        }
    }

    //Destroy pillars that are to far away
    //These are pillars that were on chunks that were destroyed
    DestroyPillars();
}

void ChangeMaxPillars(int amount) {
    MAX_PILLARS += amount;

    if (MAX_PILLARS <= 1) {
        MAX_PILLARS = 1;
    }
}

///Update the position and dislay of the chunks
void Display() {

    //Loop through all of the chunks
    for (int i = 0; i < CHUNK_AMOUNT; i++) {

        //Get the entitie's matrix
        Simplex::String name = CHUNK_NAME;
        name += std::to_string(i);

        Simplex::matrix4 groundMatrix = g_entityManager->GetModelMatrix(name);

        //Set the new position of the chunk if the player passed it
        if (Mat4Position(groundMatrix).z - (chunkSize.z * 2) > Player::GetPosition().z) {

            //Calulate the new position of the chunk relative to its current position
            Simplex::vector3 newPos = Mat4Position(groundMatrix);
            newPos.z -= (chunkSize.z * 2 - zOffset) * chunks;
            groundMatrix = glm::translate(Simplex::IDENTITY_M4, newPos);

            //Set the new position for the chunk
            g_entityManager->SetModelMatrix(groundMatrix, name);

            DestroyPillars();

            //Spawn pillars at the new chunk position
            SpawnPillars(Mat4Position(groundMatrix));

            //Increase the chances of a pillar spawning
            PILLAR_AMOUNT += (std::rand() % 100) / 200.0f;

            chunkZOffset++;
        }
    }
}

///Destroy pillars that are to far away or behind the player
void DestroyPillars() {

    //Loop through all of the possible pillars
    for (int i = 0; i < pillarCount; i++) {

        //Get the name of the pillar
        Simplex::String name = PILLAR_NAME;
        name += std::to_string(i);

        //Get the matrix of the pillar
        Simplex::matrix4 pillarMatrix = g_entityManager->GetModelMatrix(name);

        //Skip matrixes that aren't associated with a pillar
        if (pillarMatrix == Simplex::IDENTITY_M4) { continue; }

        //Test if the pillar is behind the player and destroy it
        if (Mat4Position(pillarMatrix).z > Player::GetPosition().z) {
            g_entityManager->RemoveEntity(name);
            pillarsDeleted++;
        }

        //Test if the pillar is to far away from the player and destroy it
        if (glm::distance(Mat4Position(pillarMatrix).z, Player::GetPosition().z) > (chunks - 1) * chunkSize.z * 2) {
            g_entityManager->RemoveEntity(name);
            pillarsDeleted++;
        }
    }
}

/// Spawn pillars relative to the position
void SpawnPillars(Simplex::vector3 centerPos) {

    //Get a random amount of pillars to spawn in a chunk
    int pillarAmount = glm::clamp(std::rand() % (int)PILLAR_AMOUNT, 1, MAX_PILLARS);

    //Loop through the random amount of pillars to create
    for (int i = 0; i < pillarAmount; i++) {

        //Set the name of the pillar so it can be destroyed
        Simplex::String name = PILLAR_NAME;
        name += std::to_string(pillarCount);

        //Create a new pillar
        int ridx = std::rand() % 6;
        g_entityManager->AddEntity(pillarFiles[ridx], pillarColliders[ridx], name);

        //Randomly set an offset for the pillar
        Simplex::vector3 offset = Simplex::vector3((std::rand() % (int)(chunkSize.x * 1.8f)) - chunkSize.x, 5, (rand() % (int)(chunkSize.z * 2)) - chunkSize.z);

        //Randomly scale the pillars
        float thisScale = 2 + rand() % (int)scaleRange;

        //Calculate the position of the pillar relative to the chunk position
        Simplex::matrix4 pos = glm::translate(Simplex::IDENTITY_M4, centerPos + offset) * glm::scale(Simplex::IDENTITY_M4, glm::vec3(thisScale));

        //Set the offsetted local position to the chunk
        g_entityManager->SetModelMatrix(pos);

        //Increase the amount of pillars spawned
        pillarCount++;
    }
}
}