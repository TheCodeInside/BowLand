// ----------------------------------------------------------------------------
//  A few notes on project settings
//
//  - The project is set to use the UNICODE character set
//    - This was changed in Project Properties > Config Properties > General > Character Set
//    - This basically adds a "#define UNICODE" to the project
//
//  - The include directories were automagically correct, since the DirectX 
//    headers and libs are part of the windows SDK
//    - For instance, $(WindowsSDK_IncludePath) is set as a project include 
//      path by default.  That's where the DirectX headers are located.
//
//  - Two libraries had to be manually added to the Linker Input Dependencies
//    - d3d11.lib
//    - d3dcompiler.lib
//    - This was changed in Project Properties > Config Properties > Linker > Input > Additional Dependencies
//
//  - The Working Directory was changed to match the actual .exe's 
//    output directory, since we need to load the compiled shader files at run time
//    - This was changed in Project Properties > Config Properties > Debugging > Working Directory
//
// ----------------------------------------------------------------------------

#include "MyDemoGame.hpp"
#include "Input.hpp"
#include "Time.hpp"
#include "Vertex.hpp"
#include "MeshLoader.hpp"
#include "RenderManager.hpp"
#include "GameObject.hpp"
#include "Components.hpp"

#include <sstream>
#include <DirectXColors.h>

#include <iostream>
#include <fstream>
#include <time.h>

// Include run-time memory checking in debug builds, so 
// we can be notified of memory leaks
#if defined(DEBUG) || defined(_DEBUG)
#   define _CRTDBG_MAP_ALLOC
#   include <crtdbg.h>
#endif

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Win32 Entry Point - Where your program starts
// --------------------------------------------------------
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd )
{
    // Enable run-time memory check for debug builds.
    #if defined( DEBUG ) || defined( _DEBUG )
        _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
    #endif

    // Create the game object.
    MyDemoGame game( hInstance );

    // This is where we'll create the window, initialize DirectX, 
    // set up geometry and shaders, etc.
    if ( !game.Init() )
        return 0;

    // All set to run the game loop
    return game.Run();
}

// Creates a new game
MyDemoGame::MyDemoGame( HINSTANCE hInstance )
    : DirectXGameCore( hInstance )
{
    // Set up a custom caption for the game window.
    // - The "L" before the string signifies a "wide character" string
    // - "Wide" characters take up more space in memory (hence the name)
    // - This allows for an extended character set (more fancy letters/symbols)
    // - Lots of Windows functions want "wide characters", so we use the "L"
    windowCaption = L"Bow Land";

    // Custom window size - will be created by Init() later
    windowWidth = 1280;
    windowHeight = 720;

    // Re-create the camera's projection matrix
    
}

// Cleans up the game
MyDemoGame::~MyDemoGame()
{
    DirectXGameCore::~DirectXGameCore();
}

// Initialize the game
bool MyDemoGame::Init()
{
    // Call the base class's Init() method to create the window,
    // initialize DirectX, etc.
    if ( !DirectXGameCore::Init() )
        return false;

    // Tell the input assembler stage of the pipeline what kind of
    // geometric primitives we'll be using and how to interpret them
    deviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

    // Create and load our test scene
    _testScene = std::make_shared<Scene>( device, deviceContext );
    if ( !_testScene->LoadFromFile( "Scenes\\Test.scene" ) )
    {
        return false;
    }
    
    DirectionalLight dl;
    dl.DiffuseColor = XMFLOAT4( 0.960784376f, 0.870588303f, 0.701960802f, 1.0f );
    dl.Direction = XMFLOAT3( 0, -1, 0 );

    PointLight pl;
    pl.DiffuseColor = dl.DiffuseColor;
    pl.Position = XMFLOAT3( 0, -100, 0 );

    std::shared_ptr<Mesh> spMesh = MeshLoader::Load( "Models\\sphere.obj", device, deviceContext );
    std::shared_ptr<Texture2D> spTex = Texture2D::FromFile( device, deviceContext, "Textures\\Rocks2.jpg" );
    std::shared_ptr<Texture2D> spNorm = Texture2D::FromFile( device, deviceContext, "Textures\\Rocks2Normals.jpg" );

    // Helper function for getting a random value
    srand( static_cast<unsigned>( time( nullptr ) ) );
    auto random = []() -> float
    {
        return static_cast<float>( rand() ) / RAND_MAX;
    };

    // Add some test spheres
    const int SQ = 1;
    for ( int x = -SQ; x <= SQ; ++x )
    {
        for ( int z = -SQ; z <= SQ; ++z )
        {
            for ( int y = 8; y <= 9 + SQ; ++y )
            {
                std::string name = "Ball_" + std::to_string( x ) +
                                   "_" + std::to_string( y ) +
                                   "_" + std::to_string( z );
                GameObject* go = _testScene->AddGameObject( name );

                std::string msg = "Creating " + name + "\n";
                OutputDebugStringA( msg.c_str() );

                // Set the position to be slightly off kilter
                Transform* tr = go->GetTransform();
                tr->SetPosition( XMFLOAT3(
                    x * 1.1f + random() * 0.05f,
                    y * 1.1f + random() * 0.05f,
                    z * 1.1f + random() * 0.05f
                ) );
                
                // Add the sphere collider
                SphereCollider* sp = go->AddComponent<SphereCollider>();
                sp->SetRadius( 0.5f );

                // Add the rigidbody
                Rigidbody* rb = go->AddComponent<Rigidbody>();
                rb->SetMass( 1.0f );

                // Add the default material
                DefaultMaterial* dm = go->AddComponent<DefaultMaterial>();
                dm->SetDiffuseMap( spTex );
                dm->SetNormalMap( spNorm );
                dm->SetDirectionalLight( dl );
                dm->SetPointLight( pl );

                // Add the mesh renderer
                MeshRenderer* mr = go->AddComponent<MeshRenderer>();
                mr->SetMaterial( dm );
                mr->SetMesh( spMesh );
            }
        }
    }

    // Update the active camera's projection matrix
    Camera::GetActiveCamera()->UpdateProjectionMatrix(static_cast<float>(windowWidth) / windowHeight);

    // Successfully initialized
    return true;
}

// Handle the window resizing
void MyDemoGame::OnResize()
{
    // Handle base-level DX resize stuff
    DirectXGameCore::OnResize();
    
    std::cout << "test2" << std::endl;
    // Update the camera's projection
    // firstRun is to prevent the ProjectionMatrix attempting to update before the cameras are created from the scene
    if (firstRun){
        firstRun = false;
    } else {
        Camera::GetActiveCamera()->UpdateProjectionMatrix(static_cast<float>(windowWidth) / windowHeight);
    }
    
}

// Updates the scene
void MyDemoGame::UpdateScene()
{
    // Quit if the escape key is pressed
    if ( Input::IsKeyDown( Key::Escape ) )
    {
        Quit();
        return;
    }
    

    _testScene->Update();
    

    // Update the camera based on input
    float moveSpeed = Time::GetElapsedTime() * 4.00f;
    float rotSpeed  = Time::GetElapsedTime() * 0.25f;

    // Speed up when shift is pressed
    if ( Input::IsKeyDown( Key::LeftShift ) ) { moveSpeed *= 5; }

    // Movement
    if (Input::IsKeyDown(Key::W)) { Camera::GetActiveCamera()->MoveRelative(0, 0, moveSpeed); }
    if (Input::IsKeyDown(Key::S)) { Camera::GetActiveCamera()->MoveRelative(0, 0, -moveSpeed); }
    if (Input::IsKeyDown(Key::A)) { Camera::GetActiveCamera()->MoveRelative(-moveSpeed, 0, 0); }
    if (Input::IsKeyDown(Key::D)) { Camera::GetActiveCamera()->MoveRelative(moveSpeed, 0, 0); }
    if (Input::IsKeyDown(Key::Q)) { Camera::GetActiveCamera()->MoveAbsolute(0, -moveSpeed, 0); }
    if (Input::IsKeyDown(Key::E)) { Camera::GetActiveCamera()->MoveAbsolute(0, moveSpeed, 0); }
    
    if ( hasMouseFocus )
    {
        // Rotate the camera
        XMFLOAT2 deltaMouse = Input::GetDeltaMousePosition();
        Camera::GetActiveCamera()->Rotate(rotSpeed * deltaMouse.y,
                                          rotSpeed * deltaMouse.x );
    }
    Camera::GetActiveCamera()->UpdateViewMatrix();

    // After everything, we can get rid of mouse delta positions
    prevMousePos = currMousePos;
}

// Draws the scene
void MyDemoGame::DrawScene()
{
    // Background color (Cornflower Blue in this case) for clearing
    const float* color = Colors::CornflowerBlue;


    // Clear the render target and depth buffer (erases what's on the screen)
    //  - Do this ONCE PER FRAME
    //  - At the beginning of DrawScene (before drawing *anything*)
    deviceContext->ClearRenderTargetView( renderTargetView, color );
    deviceContext->ClearDepthStencilView( depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );



    // Draw the scene
    //_testScene->Draw();
    RenderManager::Draw();



    // Present the buffer
    //  - Puts the image we're drawing into the window so the user can see it
    //  - Do this exactly ONCE PER FRAME
    //  - Always at the very end of the frame
    HR( swapChain->Present( 1, 0 ) );
}

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
//
// Feel free to add code to this method
// --------------------------------------------------------
void MyDemoGame::OnMouseDown( WPARAM btnState, int x, int y )
{
    // Save the previous mouse position, so we have it for the future
    /* prevMousePos.x = x;
    prevMousePos.y = y; */
    OnMouseMove( btnState, x, y );

    // Capture the mouse so we keep getting mouse move
    // events even if the mouse leaves the window.  we'll be
    // releasing the capture once a mouse button is released
    SetCapture( hMainWnd );
    hasMouseFocus = true;
}

// --------------------------------------------------------
// Helper method for mouse release
//
// Feel free to add code to this method
// --------------------------------------------------------
void MyDemoGame::OnMouseUp( WPARAM btnState, int x, int y )
{
    // We don't care about the tracking the cursor outside
    // the window anymore (we're not dragging if the mouse is up)
    ReleaseCapture();
    hasMouseFocus = false;
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
//
// Feel free to add code to this method
// --------------------------------------------------------
void MyDemoGame::OnMouseMove( WPARAM btnState, int x, int y )
{
    // Save the previous mouse position, so we have it for the future
    prevMousePos.x = currMousePos.x;
    prevMousePos.y = currMousePos.y;

    currMousePos.x = x;
    currMousePos.y = y;
}
