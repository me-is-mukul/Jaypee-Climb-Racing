#include "raylib.h"
#include <vector>
#include <ctime>
#include <cmath>

float Clamp(float value, float min, float max)
{
    return fminf(fmaxf(value, min), max);
}

struct GameObject
{
    Rectangle rect;
    Texture2D texture;
    bool active = true;
};

int main()
{
    InitWindow(1, 1, "Loading...");

    Texture2D roadTex = LoadTexture("assets/road.png");
    int screenWidth = roadTex.width;
    int screenHeight = roadTex.height;
    SetWindowSize(screenWidth, screenHeight);
    SetWindowTitle("Car Dodge Game");

    SetTargetFPS(60);
    srand(time(0));

    Texture2D playerTex = LoadTexture("assets/player.png");
    Texture2D enemyTex1 = LoadTexture("assets/enemy1.png");
    Texture2D enemyTex2 = LoadTexture("assets/enemy2.png");
    Texture2D coinTex = LoadTexture("assets/coin.png");

    float leftLimit = screenWidth / 6.0f;
    float rightLimit = screenWidth * 5.0f / 6.0f - 80.0f;

    GameObject player = {
        {screenWidth / 2.0f - 40.0f, screenHeight - 150.0f, 80.0f, 150.0f},
        playerTex};

    std::vector<GameObject> enemies;
    std::vector<GameObject> coins;

    float spawnTimer = 0;
    float coinTimer = 0;
    float enemySpacingY = 0;

    float velocity = 200.0f;
    float baseVelocity = 200.0f;
    float maxVelocity = 600.0f;
    float accelerationTime = 10.0f;

    float roadScrollY = 0;

    int score = 0;
    int coinCount = 0;
    float timeElapsed = 0;
    bool gameOver = false;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        if (!gameOver)
        {
            timeElapsed += dt;
            velocity = baseVelocity + (maxVelocity - baseVelocity) * (1 - expf(-timeElapsed / accelerationTime));

            float scrollSpeed = velocity * 0.75f;
            roadScrollY += scrollSpeed * dt;
            if (roadScrollY >= screenHeight)
                roadScrollY -= screenHeight;

            // Player movement
            if (IsKeyDown(KEY_LEFT))
                player.rect.x -= 400 * dt;
            if (IsKeyDown(KEY_RIGHT))
                player.rect.x += 400 * dt;
            player.rect.x = Clamp(player.rect.x, leftLimit, rightLimit);

            // Enemy spawn
            spawnTimer += dt;
            enemySpacingY -= velocity * dt;
            if (spawnTimer > 1.2f && enemySpacingY <= 0)
            {
                Texture2D chosenTex = (rand() % 2 == 0) ? enemyTex1 : enemyTex2;
                float x = leftLimit + (float)(rand() % (int)(rightLimit - leftLimit + 1));
                GameObject e = {{x, -160.0f, 80.0f, 150.0f}, chosenTex};
                enemies.push_back(e);
                spawnTimer = 0;
                enemySpacingY = 200;
            }

            // Coin spawn
            coinTimer += dt;
            if (coinTimer > 2.5f)
            {
                float x = leftLimit + (float)(rand() % (int)(rightLimit - leftLimit + 1));
                GameObject c = {{x, -50.0f, 40.0f, 40.0f}, coinTex};

                bool overlaps = false;
                for (auto &existing : coins)
                {
                    if (existing.active && CheckCollisionRecs(c.rect, existing.rect))
                    {
                        overlaps = true;
                        break;
                    }
                }

                if (!overlaps)
                    coins.push_back(c);
                coinTimer = 0;
            }

            // Move enemies
            for (auto &e : enemies)
            {
                if (e.active)
                {
                    e.rect.y += velocity * dt;
                    if (CheckCollisionRecs(player.rect, e.rect))
                        gameOver = true;
                    if (e.rect.y > screenHeight)
                    {
                        e.active = false;
                        score++;
                    }
                }
            }

            // Move coins
            for (auto &c : coins)
            {
                if (c.active)
                {
                    c.rect.y += velocity * dt;
                    if (CheckCollisionRecs(player.rect, c.rect))
                    {
                        c.active = false;
                        coinCount++;
                        score += 5;
                    }
                }
            }
        }

        // Drawing
        BeginDrawing();
        ClearBackground(BLACK);

        // ✅ Fixed scrolling logic: no black gaps
        int scrollY = (int)roadScrollY % screenHeight;
        if (scrollY < 0)
            scrollY += screenHeight;

        DrawTexture(roadTex, 0, scrollY - screenHeight, WHITE);
        DrawTexture(roadTex, 0, scrollY, WHITE);

        DrawTexturePro(player.texture, {0, 0, (float)player.texture.width, (float)player.texture.height},
                       player.rect, {0, 0}, 0, WHITE);

        for (auto &e : enemies)
            if (e.active)
                DrawTexturePro(e.texture, {0, 0, (float)e.texture.width, (float)e.texture.height},
                               e.rect, {0, 0}, 0, WHITE);

        for (auto &c : coins)
            if (c.active)
                DrawTexturePro(c.texture, {0, 0, (float)coinTex.width, (float)coinTex.height},
                               c.rect, {0, 0}, 0, YELLOW);

        DrawText(TextFormat("Score: %d", score), 20, 20, 30, WHITE);
        DrawText(TextFormat("Time: %.1fs", timeElapsed), 20, 60, 30, LIGHTGRAY);

        DrawTexturePro(coinTex, {0, 0, (float)coinTex.width, (float)coinTex.height},
                       {(float)(screenWidth - 100), 20, 40, 40}, {0, 0}, 0, YELLOW);
        DrawText(TextFormat("x %d", coinCount), screenWidth - 50, 30, 30, WHITE);

        if (gameOver)
            DrawText("GAME OVER", screenWidth / 2 - 120, screenHeight / 2, 40, RED);

        EndDrawing();
    }

    // Cleanup
    UnloadTexture(playerTex);
    UnloadTexture(enemyTex1);
    UnloadTexture(enemyTex2);
    UnloadTexture(coinTex);
    UnloadTexture(roadTex);
    CloseWindow();
    return 0;
}
