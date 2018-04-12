#pragma once

#include <Simplex\Simplex.h>
#include <SFML\Window.hpp>
#include <SFML\OpenGL.hpp>

#include <imgui\ImGuiObject.h>

class Application {
    // ==== System data ====
    // Double-check the references of variables you modify,
    // but feel free to mess with them if you need to.
    static ImGuiObject gui;
    bool _initialized;
    bool _running;
    Simplex::vector4 _clear_color;
    sf::Window* _window;
    Simplex::SystemSingleton* _system;
    Simplex::LightManager* _lights;
    Simplex::CameraManager* _camera;
    Simplex::MeshManager* _mesh;
    Simplex::uint _clock;
    float _current_time;
    float _last_time;
    void InitWindow(Simplex::String);
    void Reshape(void);
    void InitImGui(void);
    void ReadConfig(void);
    void WriteConfig(void);

    // ==== System methods ====
    float CurrentTime(void);
    float LastTime(void);

    // ==== Game-specific functions ====

    // Override these to do setup/teardown
    void GameInit(void);
    void GameRelease(void);

    // Override these to do things every frame
    // (separated mostly for code cleanliness)
    void CheckKeyboard(void) { }
    // return `true` to keep going, `false` to quit (after some cleanup, etc.)
    bool Update(void);
    // glClear called between Update and this
    // (that way you don't render in Update by accident)
    void BuildRenderList(void);

    // Override these to react to mouse events
    void OnMouseMoved(sf::Event) { }
    void OnMousePressed(sf::Event) { }
    void OnMouseReleased(sf::Event) { }
    void OnMouseScroll(sf::Event) { }
    void OnKeyPressed(sf::Event);
    void OnKeyReleased(sf::Event) { }

    // ==== Game-specific variables ====
    Simplex::Model *_ship;
    Simplex::Model *_terrain;
    Simplex::vector3 _camera_pos;
public:
    Application( );

    void Init(Simplex::String appName, Simplex::BTO_RESOLUTIONS size, bool fullscreen, bool borderless);
    void Run(void);
    void Release(void);

    ~Application( );
};

