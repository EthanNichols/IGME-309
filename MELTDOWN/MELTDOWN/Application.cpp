#include "Application.h"

// Called BEFORE the clock starts!
void Application::GameInit(void) {
    _mesh->GenerateSkybox( );
    _ship = new Simplex::Model( );
    _ship->LoadOBJ("Ship.obj");
    _terrain = new Simplex::Model( );
    _terrain->LoadOBJ("Terrain.obj");

    _lights->SetColor(glm::vec3(1.0f), 0);
    _lights->AddLight(glm::vec3(0, 5.0f, 0), glm::vec3(0.5f, 0.5f, 1.0f), 1.0f);

    _camera_pos = glm::vec3(0, 5.0f, 0);
    _camera->SetPositionTargetAndUp(
        _camera_pos,
        _camera_pos + Simplex::vector3(0, 0, -1.0f),
        Simplex::vector3(0.0f, 1.0f, 0.0f)
    );
}

void Application::GameRelease(void) {
    delete _ship;
    _ship = nullptr;
    delete _terrain;
    _terrain = nullptr;
}

bool Application::Update(void) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
        _camera_pos.y -= 0.1f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
        _camera_pos.y += 0.1f;
    }

    _camera->SetPositionTargetAndUp(
        _camera_pos,
        _camera_pos + Simplex::vector3(0, 0, 0.1f),
        Simplex::vector3(0.0f, 1.0f, 0.0f)
    );

    return true;
}

void Application::BuildRenderList(void) {
    _mesh->AddSkyboxToRenderList( );
    for (int i = 0; i < 5; ++i) {
        _terrain->SetModelMatrix(
            glm::translate(
                0.0f,
                0.0f,
                (4-i) * (std::fmodf(_current_time, 10))
            )
        );
        _terrain->AddToRenderList( );
    }
}

void Application::OnKeyPressed(sf::Event keypress) {
    if (keypress.key.code == sf::Keyboard::Escape) {
        _running = false;
    }
}
