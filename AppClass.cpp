#include "AppClass.h"

using namespace Simplex;

void Application::InitVariables(void)
{
	//Set the position and target of the camera
	m_pCameraMngr->SetPositionTargetAndUp(
		vector3(0.0f, 0.0f, 0.0f), //Position
		vector3(0.0f, 0.0f, 1.0f),	//Target
		AXIS_Y);					//Up

	m_pCameraMngr->SetNearFarPlanes(0.1f, 2000.0f);

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

	//Entity Manager
	m_pEntityMngr = MyEntityManager::GetInstance();

	//Create the player and chunks for the game
	//UIBar::CreateBars();
	Generation::GenerateChunks();
	Player::CreatePlayer();
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

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

	//Get the ship position and set the camera offset relative to the ship
	vector3 shipPos = Player::GetPosition();

	//The specfic camera offset from the model is (15, 0, 60);
	vector3 cameraPos = shipPos + vector3(0, 10, 25);

	//Update the display of the player and the map
	Player::Display();

	//Update the camera position
	m_pCameraMngr->SetPositionTargetAndUp(cameraPos, cameraPos - vector3(0, 0.15f, 0.7f), AXIS_Y);
	Generation::Display();

	//clear the render list
	m_pMeshMngr->ClearRenderList();
	//draw gui
	DrawGUI();

	//UIBar::Display();

	//end the current frame (internally swaps the front and back buffers)
	m_pWindow->display();

	m_v3LastMouse = m_v3Mouse;
}
void Application::Release(void)
{
	//release the entity manager
	m_pEntityMngr->ReleaseInstance();

	//release GUI
	ShutdownGUI();
}

#pragma region Ship Controls

void Application::ProcessKeyboard(void)
{
	if (!m_bFocused)
		return;

	static float rotation = 0.0f;
	static float shipDistFromCenter = 0.0f;
	float maxDistFromCenter = 50.0f;

	float strafeSpeed = 0.8f;
	float rollSpeed = 0.1f;

	Simplex::matrix4 lastMatrix = m_pEntityMngr->GetModelMatrix("ship");

	// Strafe left
	if (abs(shipDistFromCenter - 5.0f) < maxDistFromCenter && sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		lastMatrix = glm::translate(Simplex::IDENTITY_M4, Simplex::vector3(-strafeSpeed, 0.0f, 0.0f)) * lastMatrix; //translate it
		shipDistFromCenter -= strafeSpeed;
	}

	// Strafe right
	if (abs(shipDistFromCenter + 5.0f) < maxDistFromCenter && sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		lastMatrix = glm::translate(Simplex::IDENTITY_M4, Simplex::vector3(strafeSpeed, 0.0f, 0.0f)) * lastMatrix; //translate it
		shipDistFromCenter += strafeSpeed;
	}

	// Roll
	float deltaMouseX = m_v3LastMouse.x - m_v3Mouse.x;
	
	if (m_bRolling) {
		rotation -= deltaMouseX * rollSpeed;
		lastMatrix *= glm::rotate(Simplex::IDENTITY_M4, -deltaMouseX * rollSpeed, 0.0f, 0.0f, 1.0f); //rotate it
	}
	
	m_pEntityMngr->SetModelMatrix(lastMatrix, "ship"); //return it to its owner
}
void Application::ProcessKeyPressed(sf::Event a_event)
{
	switch (a_event.key.code)
	{
	default: break;
	case sf::Keyboard::Space:
		// We're Boosting
		Player::SetBoosting(true);
		m_pCameraMngr->SetFOV(55.0f);
		break;
	}
}
void Application::ProcessKeyReleased(sf::Event a_event)
{
	switch (a_event.key.code)
	{
	default: break;
	case sf::Keyboard::Escape:
		m_bRunning = false;
		break;
	case sf::Keyboard::Space:
		// No longer boosting
		Player::SetBoosting(false);
		m_pCameraMngr->SetFOV(50.0f);
		break;
	}
}
void Application::ProcessMousePressed(sf::Event a_event)
{
	switch (a_event.mouseButton.button)
	{
	default: break;
	case sf::Mouse::Button::Left:
		m_bRolling = true;
		break;
	}
}
void Application::ProcessMouseReleased(sf::Event a_event)
{
	switch (a_event.mouseButton.button)
	{
	default: break;
	case sf::Mouse::Button::Left:
		m_bRolling = false;
		break;
	}
}

#pragma endregion