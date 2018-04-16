#include "AppClass.h"

using namespace Simplex;
void Application::InitVariables(void)
{
	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUp(
		vector3(0.0f, 0.0f, 0.0f), //Position
		vector3(0.0f, 0.0f, 1.0f),	//Target
		AXIS_Y);					//Up

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

	//Entity Manager
	m_pEntityMngr = MyEntityManager::GetInstance();

	for (int i = 0; i < 5; i++) {
		m_pEntityMngr->AddEntity("models\\model_ground.obj", "ground_" + i);
		matrix4 groundMatrix = IDENTITY_M4;
		groundMatrix[3][2] = -(m_pEntityMngr->GetRigidBody()->GetHalfWidth().z * 2) * i;
		m_pEntityMngr->SetModelMatrix(groundMatrix);
	}

	m_pEntityMngr->AddEntity("models\\model_ship.obj", "ship");
	m_pEntityMngr->SetModelMatrix(IDENTITY_M4);
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Is the ArcBall active?
	ArcBall();

	//Is the first person camera active?
	CameraRotation();

	//Update Entity Manager
	m_pEntityMngr->Update();

	//Add objects to render list
	m_pEntityMngr->AddEntityToRenderList(-1, true);
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	// draw a skybox
	m_pMeshMngr->AddSkyboxToRenderList();

	//render list call
	m_uRenderCallCount = m_pMeshMngr->Render();

	static float speed = 0.1f;
	static int offset = 5;

	//Move the last entity added slowly to the right
	matrix4 lastMatrix = m_pEntityMngr->GetModelMatrix("ship");// get the model matrix of the last added
	lastMatrix *= glm::translate(IDENTITY_M4, vector3(0.0f, 0.0f, -speed)); //translate it
	m_pEntityMngr->SetModelMatrix(lastMatrix, "ship"); //return it to its owner

	//Get the ship position and set the camera offset relative to the ship
	vector3 shipPos = vector3(lastMatrix[3][0], lastMatrix[3][1], lastMatrix[3][2]);

	//The specfic camera offset from the model is (15, 0, 60);
	vector3 cameraPos = shipPos + vector3(15, 75, 120);

	speed += 0.01f;

	//Update the camera position
	m_pCameraMngr->SetPositionTargetAndUp(cameraPos, cameraPos - vector3(0, 0.5f, 1.0f), AXIS_Y);

	for (int i = 0; i < 5; i++) {
		String name = "ground_" + i;
		matrix4 groundMatrix = m_pEntityMngr->GetModelMatrix(name);
		if (groundMatrix[3][2] - (m_pEntityMngr->GetRigidBody(name)->GetHalfWidth().z * 2) > shipPos.z) {
			groundMatrix[3][2] = -(m_pEntityMngr->GetRigidBody(name)->GetHalfWidth().z * 2) * offset;
			m_pEntityMngr->SetModelMatrix(groundMatrix, name);
			offset++;
		}
	}

	//clear the render list
	m_pMeshMngr->ClearRenderList();

	//draw gui
	DrawGUI();

	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();
}
void Application::Release(void)
{
	//release the entity manager
	m_pEntityMngr->ReleaseInstance();

	//release GUI
	ShutdownGUI();
}