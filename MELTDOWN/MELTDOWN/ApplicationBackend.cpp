#include "Application.h"

#include "SFML\Graphics.hpp"

ImGuiObject Application::gui;

sf::Image LoadImageFromResource(const std::string& name) {
    HRSRC rsrcData = FindResource(NULL, name.c_str( ), RT_RCDATA);
    if (!rsrcData)
        throw std::runtime_error("Failed to find resource.");

    DWORD rsrcDataSize = SizeofResource(NULL, rsrcData);
    if (rsrcDataSize <= 0)
        throw std::runtime_error("Size of resource is 0.");

    HGLOBAL grsrcData = LoadResource(NULL, rsrcData);
    if (!grsrcData)
        throw std::runtime_error("Failed to load resource.");

    LPVOID firstByte = LockResource(grsrcData);
    if (!firstByte)
        throw std::runtime_error("Failed to lock resource.");

    sf::Image image;
    if (!image.loadFromMemory(firstByte, rsrcDataSize))
        throw std::runtime_error("Failed to load image from memory.");

    return image;
}

float Application::CurrentTime(void) {
    return _current_time;
}

float Application::LastTime(void) {
    return _last_time;
}

void Application::InitWindow(Simplex::String windowName) {
    Simplex::uint uStyle = sf::Style::Default;

    if (_system->IsWindowBorderless( ))
        uStyle = sf::Style::None;

    if (_system->IsWindowFullscreen( ))
        uStyle = sf::Style::Fullscreen;

    //If OpenGL 4.5 is not supported in the system glfw will warn you and determine the highest possible version
    _window = new sf::Window(sf::VideoMode(_system->GetWindowWidth( ), _system->GetWindowHeight( ), 32), //Window size
        windowName, //window name
        uStyle, //window style
        sf::ContextSettings(24, //depth buffer
            0,	//stencil
            0,	//Antialiasing
            4,	//OpenGL Major
            5,	//OpenGL Minor
            0,	//Attributes
            false //sRGB
        )); //context settings
    _window->setVerticalSyncEnabled(true);
    _window->setFramerateLimit(_system->GetMaxFrameRate( ));
    sf::Image icon = LoadImageFromResource("iconPNG");
    _window->setIcon(icon.getSize( ).x, icon.getSize( ).y, icon.getPixelsPtr( ));

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit( ) != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);	// Enables Depth Testing
    glDepthFunc(GL_LEQUAL);		// The Type Of Depth Testing To Do

    glEnable(GL_CULL_FACE);
}

void Application::Reshape(void) {
    sf::Vector2i pos = _window->getPosition( );
    sf::Vector2u size = _window->getSize( );

    _system->SetWindowX(pos.x);
    _system->SetWindowY(pos.y);
    _system->SetWindowWidth(size.x);
    _system->SetWindowHeight(size.y);

    // adjust the viewport when the window is resized
    glViewport(0, 0, size.x, size.y);
    _mesh->SetRenderTarget(0, 0, 0, size.x, size.y);
}

void Application::InitImGui(void) {
    ImGuiIO& io = ImGui::GetIO( );

    // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array that we will update during the application lifetime.
    io.KeyMap[ImGuiKey_Tab] = sf::Keyboard::Tab;
    io.KeyMap[ImGuiKey_LeftArrow] = sf::Keyboard::Left;
    io.KeyMap[ImGuiKey_RightArrow] = sf::Keyboard::Right;
    io.KeyMap[ImGuiKey_UpArrow] = sf::Keyboard::Up;
    io.KeyMap[ImGuiKey_DownArrow] = sf::Keyboard::Down;
    io.KeyMap[ImGuiKey_PageUp] = sf::Keyboard::PageUp;
    io.KeyMap[ImGuiKey_PageDown] = sf::Keyboard::PageDown;
    io.KeyMap[ImGuiKey_Home] = sf::Keyboard::Home;
    io.KeyMap[ImGuiKey_End] = sf::Keyboard::End;
    io.KeyMap[ImGuiKey_Delete] = sf::Keyboard::Delete;
    io.KeyMap[ImGuiKey_Backspace] = sf::Keyboard::BackSpace;
    io.KeyMap[ImGuiKey_Enter] = sf::Keyboard::Return;
    io.KeyMap[ImGuiKey_Escape] = sf::Keyboard::Escape;
    io.KeyMap[ImGuiKey_A] = sf::Keyboard::A;
    io.KeyMap[ImGuiKey_C] = sf::Keyboard::C;
    io.KeyMap[ImGuiKey_V] = sf::Keyboard::V;
    io.KeyMap[ImGuiKey_X] = sf::Keyboard::X;
    io.KeyMap[ImGuiKey_Y] = sf::Keyboard::Y;
    io.KeyMap[ImGuiKey_Z] = sf::Keyboard::Z;

    // We are using the alternative; set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.
    io.RenderDrawListsFn = NULL; // = RenderDrawListsFunction;
    io.SetClipboardTextFn = NULL;
    io.GetClipboardTextFn = NULL;
    //io.ClipboardUserData = NULL;
    io.ClipboardUserData = _window;
    io.ImeWindowHandle = _window->getSystemHandle( );

    //Setup clock
    gui.m_nClock = _system->GenClock( );
}

void Application::ReadConfig(void) {
    Simplex::String data_root = _system->m_pFolder->GetFolderRoot( );
    Simplex::String app_name = _system->GetAppName( );
    app_name = data_root + app_name + ".ini";

    Simplex::FileReader reader;
    //If the file doesn't exist, exit this method
    if (reader.ReadFile(app_name.c_str( )) == Simplex::OUT_ERR_FILE_MISSING) {
        return;
    }

    //read the file for startup values
    reader.Rewind( );
    while (reader.ReadNextLine( ) == Simplex::OUT_RUNNING) {
        Simplex::String sWord = reader.GetFirstWord( );

        int nLenght = reader.m_sLine.length( );
        char* zsTemp = new char[nLenght];

        if (sWord == "Fullscreen:") {
            int nValue;
            sscanf_s(reader.m_sLine.c_str( ), "Fullscreen: %d", &nValue);
            _system->SetWindowFullscreen(nValue != 0);
        } else if (sWord == "Borderless:") {
            int nValue;
            sscanf_s(reader.m_sLine.c_str( ), "Borderless: %d", &nValue);
            _system->SetWindowBorderless(nValue != 0);
        } else if (sWord == "FrameRate:") {
            int nValue;
            sscanf_s(reader.m_sLine.c_str( ), "FrameRate: %d", &nValue);
            _system->SetMaxFrameRate(static_cast<Simplex::uint>(nValue));
        } else if (sWord == "Resolution:") {
            int nValue1;
            int nValue2;
            sscanf_s(reader.m_sLine.c_str( ), "Resolution: [ %d x %d ]", &nValue1, &nValue2);
            _system->SetWindowWidth(nValue1);
            _system->SetWindowHeight(nValue2);
        } else if (sWord == "Ambient:") {
            float fValueX;
            float fValueY;
            float fValueZ;
            sscanf_s(reader.m_sLine.c_str( ), "Ambient: [%f,%f,%f]", &fValueX, &fValueY, &fValueZ);
            _lights->SetColor(Simplex::vector3(fValueX, fValueY, fValueZ), 0);
        } else if (sWord == "Background:") {
            float fValueX;
            float fValueY;
            float fValueZ;
            float fValueW;
            sscanf_s(reader.m_sLine.c_str( ), "Background: [%f,%f,%f,%f]", &fValueX, &fValueY, &fValueZ, &fValueW);
            _clear_color = Simplex::vector4(fValueX, fValueY, fValueZ, fValueW);
        } else if (sWord == "AmbientPower:") {
            float fValue;
            sscanf_s(reader.m_sLine.c_str( ), "AmbientPower: %f", &fValue);
            _lights->SetIntensity(fValue, 0);
        } else if (sWord == "Data:") {
            sscanf_s(reader.m_sLine.c_str( ), "Data: %s", zsTemp, nLenght);
            _system->m_pFolder->SetFolderData(zsTemp);
        } else if (sWord == "BTO:") {
            sscanf_s(reader.m_sLine.c_str( ), "BTO: %s", zsTemp, nLenght);
            _system->m_pFolder->SetFolderMBTO(zsTemp);
        } else if (sWord == "FBX:") {
            sscanf_s(reader.m_sLine.c_str( ), "FBX: %s", zsTemp, nLenght);
            _system->m_pFolder->SetFolderMFBX(zsTemp);
        } else if (sWord == "OBJ:") {
            sscanf_s(reader.m_sLine.c_str( ), "OBJ: %s", zsTemp, nLenght);
            _system->m_pFolder->SetFolderMOBJ(zsTemp);
        } else if (sWord == "Level:") {
            sscanf_s(reader.m_sLine.c_str( ), "Level: %s", zsTemp, nLenght);
            _system->m_pFolder->SetFolderLVL(zsTemp);
        } else if (sWord == "Textures:") {
            sscanf_s(reader.m_sLine.c_str( ), "Textures: %s", zsTemp, nLenght);
            _system->m_pFolder->SetFolderTextures(zsTemp);
        }

        delete[] zsTemp;
        zsTemp = nullptr;
    }
    reader.CloseFile( );
}

void Application::WriteConfig(void) {
    // Write the configuration for this application
    Simplex::String sRoot = _system->m_pFolder->GetFolderRoot( );
    Simplex::String App = _system->GetAppName( );
    App = sRoot + App + ".ini";

    FILE *pFile;
    fopen_s(&pFile, App.c_str( ), "w");
    if (!pFile)	//If we couldn't create the file we exit without changes.
        return;

    //Save DEFAULT_V3NEG values for a config file
    rewind(pFile);
    fprintf(pFile, "# Configuration file for the program: %s", _system->GetAppName( ).c_str( ));

    fprintf(pFile, "\n\nFullscreen: ");
    if (_system->IsWindowFullscreen( ))
        fprintf(pFile, "1");
    else
        fprintf(pFile, "0");

    fprintf(pFile, "\nBorderless: ");
    if (_system->IsWindowBorderless( ))
        fprintf(pFile, "1");
    else
        fprintf(pFile, "0");

    fprintf(pFile, "\n\nFrameRate: ");
    fprintf(pFile, "%d", _system->GetMaxFrameRate( ));

    fprintf(pFile, "\n\nResolution: [ %d x %d ]", _system->GetWindowWidth( ), _system->GetWindowHeight( ));
    fprintf(pFile, "\n# Resolution: [ 640 x 480 ]");
    fprintf(pFile, "\n# Resolution: [ 1280 x 720 ]");
    fprintf(pFile, "\n# Resolution: [ 1680 x 1050 ]");
    fprintf(pFile, "\n# Resolution: [ 1920 x 1080 ]");
    fprintf(pFile, "\n# Resolution: [ 2560 x 1080 ]");

    Simplex::vector3 light_color = _lights->GetColor(0);
    fprintf(pFile, "\n\nAmbient: [%.2f,%.2f,%.2f]",
        light_color.r, light_color.g, light_color.b);
    fprintf(pFile, "\nAmbientPower: %.2f", _lights->GetIntensity(0));

    fprintf(pFile, "\n\nBackground: [%.3f,%.3f,%.3f,%.3f]",
        _clear_color.r, _clear_color.g, _clear_color.b, _clear_color.a);

    fprintf(pFile, "\n\n# Folders:");
    fprintf(pFile, "\nData:		%s", _system->m_pFolder->GetFolderData( ).c_str( ));
    fprintf(pFile, "\nBTO:		%s", _system->m_pFolder->GetFolderMBTO( ).c_str( ));
    fprintf(pFile, "\nFBX:		%s", _system->m_pFolder->GetFolderMFBX( ).c_str( ));
    fprintf(pFile, "\nOBJ:		%s", _system->m_pFolder->GetFolderMOBJ( ).c_str( ));
    fprintf(pFile, "\nLevel:		%s", _system->m_pFolder->GetFolderLVL( ).c_str( ));
    fprintf(pFile, "\nTextures:	%s", _system->m_pFolder->GetFolderTextures( ).c_str( ));

    fclose(pFile);
}

Application::Application( ) { }

void Application::Init(Simplex::String appName, Simplex::BTO_RESOLUTIONS size, bool fullscreen, bool borderless) {
    if (_initialized) return;
    //Init System
    _system = Simplex::SystemSingleton::GetInstance( );
    _system->SetWindowResolution(size);
    Simplex::uint width = _system->GetWindowWidth( );
    Simplex::uint height = _system->GetWindowHeight( );

    _running = false;

    if (appName == "")
        _system->SetWindowName(_system->GetAppName( ));
    else
        _system->SetWindowName(appName);
    _system->SetWindowWidth(width);
    _system->SetWindowHeight(height);
    _system->SetWindowFullscreen(fullscreen);

    //Get light manager
    _lights = Simplex::LightManager::GetInstance( );

    // Set the clear color based on Microsoft's CornflowerBlue (default in XNA)
    _clear_color = Simplex::vector4(0.4f, 0.6f, 0.9f, 1.0f);

    //Read configuration
    ReadConfig( );

    //Init GLFW Window
    InitWindow(_system->GetWindowName( ));

    //Init Camera
    _camera = Simplex::CameraManager::GetInstance( );
    _camera->SetPositionTargetAndUp(
        Simplex::vector3(0.0f, 0.0f, 0.0f),
        Simplex::vector3(0.0f, 0.0f, 1.0f),
        Simplex::vector3(0.0f, 1.0f, 0.0f)
    );

    //Init Mesh Manager
    _mesh = Simplex::MeshManager::GetInstance( );

    //Init sizes
    Reshape( );

    InitImGui( );
    GameInit( );

    _clock = _system->GenClock( );

    _initialized = true;
}

void Application::Run(void) {
    if (!_initialized) {
        return;
    }

    _running = true;

    sf::Vector2i pos = _window->getPosition( );
    while (_running) {
        sf::Event last_event;
        sf::Vector2u size;
        while (_window->pollEvent(last_event)) {
            switch (last_event.type) {
                case sf::Event::Closed:
                    // window closed
                    _running = false;
                    break;
                case sf::Event::Resized:
                    size = _window->getSize( );
                    _system->SetWindowWidth(size.x);
                    _system->SetWindowHeight(size.y);
                    Reshape( );
                    break;
                case sf::Event::MouseMoved:
                    OnMouseMoved(last_event);
                    if (pos != _window->getPosition( )) {
                        pos = _window->getPosition( );
                        size = _window->getSize( );
                        _system->SetWindowX(pos.x);
                        _system->SetWindowY(pos.y);
                        _system->SetWindowWidth(size.x);
                        _system->SetWindowHeight(size.y);
                    }
                    break;
                case sf::Event::MouseButtonPressed:
                    OnMousePressed(last_event);
                    break;
                case sf::Event::MouseButtonReleased:
                    OnMouseReleased(last_event);
                    break;
                case sf::Event::MouseWheelScrolled:
                    OnMouseScroll(last_event);
                    break;
                case sf::Event::KeyPressed:
                    OnKeyPressed(last_event);
                    break;
                case sf::Event::KeyReleased:
                    OnKeyReleased(last_event);
                    break;
                case sf::Event::TextEntered:
                    if (last_event.text.unicode > 0 && last_event.text.unicode < 0x10000)
                        ImGui::GetIO( ).AddInputCharacter(last_event.text.unicode);
                    break;
            } // switch (last_event.type)

            _system->Update( );
            _last_time = _current_time;
            _current_time = _system->GetDeltaTime(_clock);

            CheckKeyboard( );
            _running = Update( );


            BuildRenderList( );

            glClearColor(_clear_color.r, _clear_color.g, _clear_color.b, _clear_color.a);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            _mesh->Render( );
            _mesh->ClearRenderList( );
            // TODO GUI display
            _window->display( );
        }
    }
    WriteConfig( );
}

void Application::Release(void) {
    GameRelease( );
    Simplex::ReleaseAllSingletons( );
    delete _window;
    _window = nullptr;
}

Application::~Application( ) { Release( ); }
