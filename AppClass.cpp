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
	m_pCameraMngr->SetFOV(80.0f);

	m_pLightMngr->SetPosition(vector3(0.0f, 3.0f, 13.0f), 1); //set the position of first light (0 is reserved for ambient light)

	//Entity Manager
	m_pEntityMngr = MyEntityManager::GetInstance();

	//Create the player and chunks for the game
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

	if (health <= 0.0f)
		ResetGame();
}
void Application::Display(void)
{
	// Clear the screen
	ClearScreen();

	// draw a skybox
	//m_pMeshMngr->AddSkyboxToRenderList();

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

#pragma region Meltdown / Scoring

	// Meltdown logic
	if (meltdownMeter < 1.0f && !Player::GetBoosting()) {
		meltdownMeter += meltdownMeterChargeRate;

		if (meltdownMeter > 1.0f)
			meltdownMeter = 1.0f;
	} else if (Player::GetBoosting()) {
		meltdownMeter -= boostDepleteRate;

		if (meltdownMeter <= 0.0f) {
			//Perform a MELTDOWN
			meltdownMeter = 1.0f;
			meltdownMultiplier += 1;
			Player::SetSpeed(meltdownMultiplier);
			m_pCameraMngr->SetFOV(80.0f + meltdownMultiplier);
		}
	}

	thisRunScore += Player::GetSpeed() * meltdownMultiplier;

	if (thisRunScore > bestRunScore)
		bestRunScore = thisRunScore;

#pragma endregion

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

void Application::ResetGame() {
	health = 1.0f;
	meltdownMultiplier = 1;

	if (thisRunScore < bestRunScore)
		bestRunScore = thisRunScore;

	lastRunScore = thisRunScore;
	thisRunScore = 0;
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
		lastMatrix = glm::translate(Simplex::IDENTITY_M4, Simplex::vector3(-(strafeSpeed + (meltdownMultiplier * strafeModifier)), 0.0f, 0.0f)) * lastMatrix; //translate it
		shipDistFromCenter -= (strafeSpeed + (meltdownMultiplier * strafeModifier));
	}

	// Strafe right
	if (abs(shipDistFromCenter + 5.0f) < maxDistFromCenter && sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		lastMatrix = glm::translate(Simplex::IDENTITY_M4, Simplex::vector3(strafeSpeed + (meltdownMultiplier * strafeModifier), 0.0f, 0.0f)) * lastMatrix; //translate it
		shipDistFromCenter += strafeSpeed + (meltdownMultiplier * strafeModifier);
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
	//case sf::Keyboard::W:
	case sf::Keyboard::Space:
		// We're Boosting
		if (meltdownMeter > 0.9f) {
			Player::SetBoosting(true);
			m_pCameraMngr->SetFOV(80.0f + meltdownMultiplier);
		}
		break;

	//Increase/Decrease the amount of chunks visible
	case sf::Keyboard::I:
		Generation::ChangeChunkAmount(1);
		break;
	case sf::Keyboard::O:
		Generation::ChangeChunkAmount(-1);
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
	//case sf::Keyboard::W:
	case sf::Keyboard::Space:
		// No longer boosting
		Player::SetBoosting(false);
		m_pCameraMngr->SetFOV(80.0f);
		meltdownMultiplier = 1;
		Player::SetSpeed(1);
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