#include "UIBar.h"
#include "Player.h"

Simplex::MyEntityManager* m_pEntityMngr;
Simplex::CameraManager* m_pCameraMngr;

namespace UIBar {

	///Initialize all of the needed variables for the player
	void Init() {
		m_pEntityMngr = Simplex::MyEntityManager::GetInstance();
		m_pCameraMngr = Simplex::CameraManager::GetInstance();
	}

	///Create the player
	void CreateBars() {

		//Initialize the variables if they haven't been yet
		if (m_pEntityMngr == nullptr) Init();

		//Create the bars
		m_pEntityMngr->AddEntity("models\\model_ui_mdBarBack.obj", "uiBarBack");

		m_pEntityMngr->AddEntity("models\\model_ui_mdBarFront.obj", "uiBarFront");

		Simplex::matrix4 lastMatrix = m_pEntityMngr->GetModelMatrix("uiBarBack");
		lastMatrix *= glm::rotate(Simplex::IDENTITY_M4, 90.0f, Simplex::vector3(1.0f, 0.0f, 0.0f)) * 
			glm::rotate(Simplex::IDENTITY_M4, -90.0f, Simplex::vector3(0.0f, 1.0f, 0.0f));
		m_pEntityMngr->SetModelMatrix(lastMatrix, "uiBarBack");
		m_pEntityMngr->SetModelMatrix(lastMatrix, "uiBarFront");
	}

	///Return the position of the bars
	Simplex::vector3 GetPosition() {
		Simplex::vector4 vec4Pos = m_pEntityMngr->GetModelMatrix() * Simplex::vector4(0, 0, 0, 1);

		return Simplex::vector3(vec4Pos.x, vec4Pos.y, vec4Pos.z);
	}

	///Update the position and dislay of the ui bars
	void Display() {
		// Follow the camera
	}
}
