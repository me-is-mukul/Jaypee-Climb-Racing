// Car Dodge Game with Enhanced Features (Raylib)
// Author: [Your Name]
// Project Version: Final

#include "raylib.h"
#include <vector>
#include <string>
#include <cmath>
#include <ctime>
#include <memory>
#include <map>
#include <algorithm>

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

class SoundManager
{
public:
    SoundManager()
    {
        InitAudioDevice();
        sounds["coin"] = LoadSound("assets/coin.wav");
        sounds["crash"] = LoadSound("assets/crash.wav");
        sounds["engine"] = LoadSound("assets/engine.wav");
        SetSoundVolume(sounds["engine"], 0.3f);
        PlaySound(sounds["engine"]);
    }

    ~SoundManager()
    {
        for (auto &[k, s] : sounds)
            UnloadSound(s);
        CloseAudioDevice();
    }

    void Play(const std::string &name)
    {
        if (sounds.count(name))
            PlaySound(sounds[name]);
    }

private:
    std::map<std::string, Sound> sounds;
};

class Player
{
public:
    GameObject car;
    float speed = 0.0f;
    float maxSpeed = 500.0f;
    float acceleration = 300.0f;
    float deceleration = 200.0f;
    float xSpeed = 400.0f;
    float leftLimit, rightLimit;

    Player(Texture2D tex, int screenWidth, int screenHeight)
    {
        car = {{screenWidth / 2.0f - 40.0f, screenHeight - 160.0f, 80.0f, 150.0f}, tex};
        leftLimit = screenWidth / 6.0f;
        rightLimit = screenWidth * 5.0f / 6.0f - car.rect.width;
    }

    void Update(float dt)
    {
        if (IsKeyDown(KEY_LEFT))
            car.rect.x -= xSpeed * dt;
        if (IsKeyDown(KEY_RIGHT))
            car.rect.x += xSpeed * dt;
        car.rect.x = Clamp(car.rect.x, leftLimit, rightLimit);

        if (IsKeyDown(KEY_UP))
        {
            speed += acceleration * dt;
            if (speed > maxSpeed)
                speed = maxSpeed;
        }
        else
        {
            speed -= deceleration * dt;
            if (speed < 0)
                speed = 0;
        }
    }

    void Draw()
    {
        DrawTexturePro(car.texture, {0, 0, (float)car.texture.width, (float)car.texture.height}, car.rect, {0, 0}, 0, WHITE);
    }
};

class EnemyManager
{
public:
    std::vector<GameObject> enemies;
    Texture2D tex1, tex2;
    float spawnTimer = 0;
    float spacing = 250.0f;
    float leftLimit, rightLimit;
    int screenHeight;

    EnemyManager(Texture2D t1, Texture2D t2, float left, float right, int height)
        : tex1(t1), tex2(t2), leftLimit(left), rightLimit(right), screenHeight(height) {}

    void Update(float dt, float relSpeed)
    {
        spawnTimer += dt;
        if (spawnTimer >= 1.3f)
        {
            float x = leftLimit + rand() % (int)(rightLimit - leftLimit);
            Texture2D chosen = rand() % 2 ? tex1 : tex2;
            GameObject e = {{x, -150.0f, 80, 150}, chosen};
            enemies.push_back(e);
            spawnTimer = 0;
        }

        for (auto &e : enemies)
        {
            if (!e.active)
                continue;
            e.rect.y += relSpeed * dt;
            if (e.rect.y > screenHeight)
                e.active = false;
        }
    }

    void Draw()
    {
        for (auto &e : enemies)
            if (e.active)
                DrawTexturePro(e.texture, {0, 0, (float)e.texture.width, (float)e.texture.height}, e.rect, {0, 0}, 0, WHITE);
    }
};

class CoinManager
{
public:
    std::vector<GameObject> coins;
    Texture2D texture;
    float spawnTimer = 0;
    float leftLimit, rightLimit;
    int screenHeight;

    CoinManager(Texture2D tex, float left, float right, int height)
        : texture(tex), leftLimit(left), rightLimit(right), screenHeight(height) {}

    void Update(float dt, float relSpeed, std::vector<GameObject> &enemies)
    {
        spawnTimer += dt;
        if (spawnTimer > 2.0f)
        {
            float x = leftLimit + rand() % (int)(rightLimit - leftLimit);
            GameObject c = {{x, -50, 40, 40}, texture};
            bool overlap = false;
            for (auto &e : enemies)
            {
                if (e.active && CheckCollisionRecs(e.rect, c.rect))
                {
                    overlap = true;
                    break;
                }
            }
            if (!overlap)
                coins.push_back(c);
            spawnTimer = 0;
        }

        for (auto &c : coins)
        {
            if (!c.active)
                continue;
            c.rect.y += relSpeed * dt;
            if (c.rect.y > screenHeight)
                c.active = false;
        }
    }

    void Draw()
    {
        for (auto &c : coins)
            if (c.active)
                DrawTexturePro(texture, {0, 0, (float)texture.width, (float)texture.height}, c.rect, {0, 0}, 0, YELLOW);
    }
};

class HUD
{
public:
    Texture2D coinTex;
    int *score, *coins;
    float *speed;
    int screenWidth;

    HUD(Texture2D c, int *s, int *coin, float *sp, int width)
        : coinTex(c), score(s), coins(coin), speed(sp), screenWidth(width) {}

    void Draw()
    {
        DrawText(TextFormat("Score: %d", *score), 20, 20, 30, WHITE);
        DrawText(TextFormat("Speed: %.0f", *speed), screenWidth / 2 - 50, 20, 30, ORANGE);

        DrawTexturePro(coinTex, {0, 0, (float)coinTex.width, (float)coinTex.height},
                       {(float)(screenWidth - 100), 20, 40, 40}, {0, 0}, 0, YELLOW);
        DrawText(TextFormat("x %d", *coins), screenWidth - 50, 30, 30, WHITE);
    }
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

    SoundManager soundManager;
    Texture2D playerTex = LoadTexture("assets/player.png");
    Texture2D enemyTex1 = LoadTexture("assets/enemy1.png");
    Texture2D enemyTex2 = LoadTexture("assets/enemy2.png");
    Texture2D coinTex = LoadTexture("assets/coin.png");

    Player player(playerTex, screenWidth, screenHeight);
    EnemyManager enemyManager(enemyTex1, enemyTex2, player.leftLimit, player.rightLimit, screenHeight);
    CoinManager coinManager(coinTex, player.leftLimit, player.rightLimit, screenHeight);

    int score = 0, coinCount = 0;
    HUD hud(coinTex, &score, &coinCount, &player.speed, screenWidth);

    float roadScrollY = 0;
    bool gameOver = false;
    bool pause = false;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        if (IsKeyPressed(KEY_P))
            pause = !pause;

        if (!pause && !gameOver)
        {
            player.Update(dt);
            float relSpeed = 150.0f + player.speed;

            roadScrollY += player.speed * dt * 0.75f;
            if (roadScrollY >= screenHeight)
                roadScrollY -= screenHeight;

            enemyManager.Update(dt, relSpeed);
            coinManager.Update(dt, relSpeed, enemyManager.enemies);

            for (auto &e : enemyManager.enemies)
            {
                if (e.active && CheckCollisionRecs(e.rect, player.car.rect))
                {
                    soundManager.Play("crash");
                    gameOver = true;
                }
                if (!e.active)
                    score++;
            }

            for (auto &c : coinManager.coins)
            {
                if (c.active && CheckCollisionRecs(c.rect, player.car.rect))
                {
                    c.active = false;
                    coinCount++;
                    score += 5;
                    soundManager.Play("coin");
                }
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        int scrollY = (int)roadScrollY % screenHeight;
        if (scrollY < 0)
            scrollY += screenHeight;

        DrawTexture(roadTex, 0, scrollY - screenHeight, WHITE);
        DrawTexture(roadTex, 0, scrollY, WHITE);

        player.Draw();
        enemyManager.Draw();
        coinManager.Draw();
        hud.Draw();

        if (pause)
            DrawText("PAUSED", screenWidth / 2 - 70, screenHeight / 2, 40, YELLOW);
        else if (gameOver)
            DrawText("GAME OVER", screenWidth / 2 - 120, screenHeight / 2, 40, RED);

        EndDrawing();
    }

    UnloadTexture(playerTex);
    UnloadTexture(enemyTex1);
    UnloadTexture(enemyTex2);
    UnloadTexture(coinTex);
    UnloadTexture(roadTex);
    CloseWindow();
    return 0;
}
