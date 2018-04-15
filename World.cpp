#include "World.h"

World* World::m_pInstance = nullptr;

World::World()
{

}

World::~World()
{

}

// Initially generate the chunks and their contents
void World::DoInitialGeneration()
{
	for (int i = 0; i < m_iGenerationDistance; ++i) {
		// Add a segment to the entity manager
		// Add the segment to the vector of chunks
		// Generate the segment's contents
	}
}

// Method to get the singleton instance of the game world
World* World::GetInstance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = new World();
	}
	return m_pInstance;
}

// Releases the singleton instance of the game world
void World::ReleaseInstance()
{
	if (m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

void World::CycleSegments()
{
}
