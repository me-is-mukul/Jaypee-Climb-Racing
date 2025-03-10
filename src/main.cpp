#include "raylib.h"

int main() {
    // Initialize the window
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "3D Environment with 2D Movement");

    // Create a 3D camera
    Camera3D camera = { 0 };
    camera.position = { 0.0f, 10.0f, 10.0f }; // Camera position
    camera.target = { 0.0f, 0.0f, 0.0f };   // Camera looks at
    camera.up = { 0.0f, 1.0f, 0.0f };       // Up vector
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Player (2D movement in a 3D world)
    Vector3 playerPos = { 0.0f, 0.0f, 0.0f };

    SetTargetFPS(60);

    while (!WindowShouldClose()) { // Main game loop
        // Player Movement (only in X and Z, not Y)
        if (IsKeyDown(KEY_W)) playerPos.z -= 0.1f; // Move forward
        if (IsKeyDown(KEY_S)) playerPos.z += 0.1f; // Move backward
        if (IsKeyDown(KEY_A)) playerPos.x -= 0.1f; // Move left
        if (IsKeyDown(KEY_D)) playerPos.x += 0.1f; // Move right

        // Camera follows the player
        camera.target = playerPos;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera); // Start 3D rendering

        DrawGrid(20, 1.0f); // Draw a ground grid
        DrawCube(playerPos, 1.0f, 1.0f, 1.0f, RED); // Player as a red cube

        EndMode3D();

        DrawText("Move with WASD in a 3D world!", 10, 10, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

