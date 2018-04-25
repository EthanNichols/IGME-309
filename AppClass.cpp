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
	m_bDebug = false;
}
void Application::Update(void)
{
	//Update the system so it knows how much time has passed since the last call
	m_pSystem->Update();

	//Update Entity Manager
	m_pEntityMngr->Update();

	CameraRotation();

	// Set the ship's velocity (check for/resolve collisions)
	Player::SetVelocity();

	//Update the camera position
	if (!m_bDebug) {
		//Get the ship position and set the camera offset relative to the ship
		vector3 shipPos = Player::GetPosition();

		//The specfic camera offset from the model is (15, 0, 60);

		vector3 cameraPos = shipPos + vector3(0, 10, 25);
		m_pCameraMngr->SetPositionTargetAndUp(cameraPos, cameraPos - vector3(0, 0.15f, 0.7f), AXIS_Y);
	}

	//Add objects to render list
	if (!m_bDebug) {
		m_pEntityMngr->AddEntityToRenderList(-1, false);
	}
	else {
		m_pEntityMngr->AddEntityToRenderList(-1, true);
	}

	if (Player::GetHealth() <= 0.0f)
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


	//Update the display of the player and the map
	Player::Display();

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
	Player::SetHealth(1.0f);
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

	if (m_bDebug) {
		bool bMultiplier = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);

		float fMultiplier = 1.0f;

		if (bMultiplier)
			fMultiplier = 5.0f;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			m_pCameraMngr->MoveForward(m_fMovementSpeed * fMultiplier);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			m_pCameraMngr->MoveForward(-m_fMovementSpeed * fMultiplier);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			m_pCameraMngr->MoveSideways(-m_fMovementSpeed * fMultiplier);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			m_pCameraMngr->MoveSideways(m_fMovementSpeed * fMultiplier);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::PageUp))
			m_pCameraMngr->MoveVertical(-m_fMovementSpeed * fMultiplier);

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::PageDown))
			m_pCameraMngr->MoveVertical(m_fMovementSpeed * fMultiplier);
	}
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
	case sf::Keyboard::BackSlash:
		if (m_bDebug == true) { m_bDebug = false; }
		else if (m_bDebug == false) { m_bDebug = true; }
		break;
	case sf::Keyboard::N:
		if (m_bDebug) {
			vector3 shipPos = Player::GetPosition();
			vector3 cameraPos = shipPos + vector3(0, 10, 25);
			m_pCameraMngr->SetPositionTargetAndUp(cameraPos, cameraPos - vector3(0, 0.15f, 0.7f), AXIS_Y);
		}
		break;
	case sf::Keyboard::M:
		if (m_bDebug) {
			Player::SetSpeed(15.0f);
		}
		break;
	case sf::Keyboard::L:
		if (m_bDebug) {
			Generation::ChangeMaxPillars(40.0f);
		}
		break;
	case sf::Keyboard::K:
		if (m_bDebug) {
			Generation::ChangeMaxPillars(5.0f);
		}
		break;
	case sf::Keyboard::U:
		if (m_bDebug) {
			//Toggle S.O. on or off
		}
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
	if (m_bDebug) {
		switch (a_event.mouseButton.button)
		{
		default: break;
		case sf::Mouse::Button::Left:
			gui.m_bMousePressed[0] = true;
			break;
		case sf::Mouse::Button::Middle:
			gui.m_bMousePressed[1] = true;
			m_bArcBall = true;
			break;
		case sf::Mouse::Button::Right:
			gui.m_bMousePressed[2] = true;
			m_bFPC = true;
			break;
		}

		for (int i = 0; i < 3; i++)
			gui.io.MouseDown[i] = gui.m_bMousePressed[i];
	}
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
	if (m_bDebug) {
		switch (a_event.mouseButton.button)
		{
		default: break;
		case sf::Mouse::Button::Left:
			gui.m_bMousePressed[0] = false;
			break;
		case sf::Mouse::Button::Middle:
			gui.m_bMousePressed[1] = false;
			m_bArcBall = false;
			break;
		case sf::Mouse::Button::Right:
			gui.m_bMousePressed[2] = false;
			m_bFPC = false;
			break;
		}

		for (int i = 0; i < 3; i++)
			gui.io.MouseDown[i] = gui.m_bMousePressed[i];
	}
	switch (a_event.mouseButton.button)
	{
	default: break;
	case sf::Mouse::Button::Left:
		m_bRolling = false;
		break;
	}
}

#pragma endregion