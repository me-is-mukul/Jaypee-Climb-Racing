// Car Dodge Game with Enhanced Features (Raylib)
// Author: [Your Name] with AI Assistance
// Project Version: Ultimate (Enhanced)

#include "raylib.h"
#include <vector>
#include <string>
#include <cmath>
#include <ctime>
#include <memory>
#include <map>
#include <algorithm>
#include <fstream>

// Utilities
float Clamp(float value, float min, float max)
{
    return fminf(fmaxf(value, min), max);
}

bool CheckOverlap(Rectangle a, Rectangle b)
{
    return CheckCollisionRecs(a, b);
}

// Particle System
struct Particle
{
    Vector2 pos;
    Vector2 vel;
    Color color;
    float size;
    float life;
};

class ParticleSystem
{
public:
    std::vector<Particle> particles;

    void AddExplosion(Vector2 pos, int count, Color baseColor)
    {
        for (int i = 0; i < count; i++)
        {
            float angle = (float)(rand() % 360) * DEG2RAD;
            float speed = (float)(rand() % 100 + 50);
            Particle p = {pos, {cosf(angle) * speed, sinf(angle) * speed}, baseColor, (float)(rand() % 20 + 10), 1.0f};
            particles.push_back(p);
        }
    }

    void Update(float dt)
    {
        for (auto it = particles.begin(); it != particles.end();)
        {
            it->pos.x += it->vel.x * dt;
            it->pos.y += it->vel.y * dt;
            it->life -= dt * 2.0f;
            if (it->life <= 0)
                it = particles.erase(it);
            else
                ++it;
        }
    }

    void Draw()
    {
        for (auto &p : particles)
        {
            float alpha = p.life;
            Color c = {p.color.r, p.color.g, p.color.b, (unsigned char)(255 * alpha)};
            DrawCircleV(p.pos, p.size * p.life, c);
        }
    }
};

// Base GameObject
struct GameObject
{
    Rectangle rect;
    Texture2D texture;
    bool active = true;
};

// Sound System
class SoundManager
{
public:
    SoundManager()
    {
        InitAudioDevice();
        sounds["coin"] = LoadSound("assets/coin.wav");
        sounds["crash"] = LoadSound("assets/crash.wav");
        sounds["engine"] = LoadSound("assets/engine.wav");
        sounds["powerup"] = LoadSound("assets/powerup.wav");
        bgMusic = LoadMusicStream("assets/background.mp3");
        SetSoundVolume(sounds["engine"], 0.3f);
        SetMusicVolume(bgMusic, 0.5f);
        PlaySound(sounds["engine"]);
        PlayMusicStream(bgMusic);
    }

    ~SoundManager()
    {
        for (auto &[k, s] : sounds)
            UnloadSound(s);
        UnloadMusicStream(bgMusic);
        CloseAudioDevice();
    }

    void Play(const std::string &name)
    {
        if (sounds.count(name))
            PlaySound(sounds[name]);
    }

    void UpdateMusic()
    {
        UpdateMusicStream(bgMusic);
    }

    void SetVolume(float volume)
    {
        SetMusicVolume(bgMusic, volume);
        for (auto &[k, s] : sounds)
            SetSoundVolume(s, volume);
    }

private:
    std::map<std::string, Sound> sounds;
    Music bgMusic;
};

// Player Class
class Player
{
public:
    GameObject car;
    std::vector<Texture2D> skins;
    int currentSkin = 0;
    float speed = 0.0f;
    float maxSpeed = 600.0f;
    float acceleration = 350.0f;
    float deceleration = 250.0f;
    float xSpeed = 450.0f;
    float leftLimit, rightLimit;
    bool shield = false;
    float shieldTime = 0;
    bool magnet = false;
    float magnetTime = 0;
    float nitroTime = 0;

    Player(std::vector<Texture2D> texs, int screenWidth, int screenHeight)
    {
        skins = texs;
        car = {{(float)(screenWidth / 2 - 40), (float)(screenHeight - 160), 80, 150}, skins[currentSkin]};
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

        float effectiveMax = maxSpeed * (nitroTime > 0 ? 1.5f : 1.0f);
        if (IsKeyDown(KEY_UP))
        {
            speed += acceleration * dt;
            if (speed > effectiveMax)
                speed = effectiveMax;
        }
        else
        {
            speed -= deceleration * dt;
            if (speed < 0)
                speed = 0;
        }

        if (shield)
        {
            shieldTime -= dt;
            if (shieldTime <= 0)
                shield = false;
        }
        if (magnet)
        {
            magnetTime -= dt;
            if (magnetTime <= 0)
                magnet = false;
        }
        if (nitroTime > 0)
        {
            nitroTime -= dt;
            if (nitroTime <= 0)
                speed = maxSpeed;
        }
    }

    void Draw()
    {
        DrawTexturePro(car.texture, {0, 0, (float)car.texture.width, (float)car.texture.height}, car.rect, {0, 0}, 0, WHITE);
        if (shield)
            DrawRectangleLinesEx(car.rect, 3, Fade(BLUE, 0.5f));
        if (magnet)
            DrawCircle((int)car.rect.x + 40, (int)car.rect.y + 75, 100, Fade(PURPLE, 0.2f));
    }

    void ActivatePowerUp(const std::string &type)
    {
        if (type == "shield")
        {
            shield = true;
            shieldTime = 5.0f;
        }
        else if (type == "magnet")
        {
            magnet = true;
            magnetTime = 7.0f;
        }
        else if (type == "nitro")
        {
            nitroTime = 3.0f;
        }
    }

    void ChangeSkin(int direction)
    {
        currentSkin = (currentSkin + direction + skins.size()) % skins.size();
        car.texture = skins[currentSkin];
    }
};

// Enemy Manager
// Enemy Manager
class EnemyManager
{
public:
    std::vector<GameObject> enemies;
    std::vector<Texture2D> textures;
    float spawnTimer = 0;
    float spacing = 300.0f;
    float leftLimit, rightLimit;
    int screenHeight;

    EnemyManager(std::vector<Texture2D> texs, float left, float right, int height)
        : textures(texs), leftLimit(left), rightLimit(right), screenHeight(height) {}

    void Update(float dt, float baseSpeed)
    {
        spawnTimer += dt;
        if (spawnTimer >= 1.0f)
        {
            float x = leftLimit + rand() % (int)(rightLimit - leftLimit);
            Texture2D chosen = textures[rand() % textures.size()]; // Fixed line
            GameObject e = {{x, -160.0f, 80, 150}, chosen};
            enemies.push_back(e);
            spawnTimer = 0;
        }

        for (auto &e : enemies)
        {
            if (!e.active)
                continue;
            e.rect.y += baseSpeed * dt;
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

// Power-Up Manager
class PowerUpManager
{
public:
    std::vector<GameObject> powerUps;
    std::map<std::string, Texture2D> textures;
    float spawnTimer = 0;
    float leftLimit, rightLimit;
    int screenHeight;

    PowerUpManager(std::map<std::string, Texture2D> texs, float left, float right, int height)
        : textures(texs), leftLimit(left), rightLimit(right), screenHeight(height) {}

    void Update(float dt, float relSpeed, std::vector<GameObject> &enemies)
    {
        spawnTimer += dt;
        if (spawnTimer > 5.0f)
        {
            float x = leftLimit + rand() % (int)(rightLimit - leftLimit);
            std::string type = (rand() % 3 == 0) ? "shield" : (rand() % 2 ? "magnet" : "nitro");
            GameObject p = {{x, -50, 40, 40}, textures[type]};
            bool overlap = false;
            for (auto &e : enemies)
            {
                if (e.active && CheckOverlap(e.rect, p.rect))
                {
                    overlap = true;
                    break;
                }
            }
            if (!overlap)
                powerUps.push_back(p);
            spawnTimer = 0;
        }

        for (auto &p : powerUps)
        {
            if (!p.active)
                continue;
            p.rect.y += relSpeed * dt;
            if (p.rect.y > screenHeight)
                p.active = false;
        }
    }

    void Draw()
    {
        for (auto &p : powerUps)
            if (p.active)
                DrawTexturePro(p.texture, {0, 0, (float)p.texture.width, (float)p.texture.height}, p.rect, {0, 0}, 0, WHITE);
    }

    std::string GetType(GameObject &p)
    {
        for (auto &[type, tex] : textures)
            if (p.texture.id == tex.id)
                return type;
        return "";
    }
};

// Coin Manager
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

    void Update(float dt, float roadSpeed, std::vector<GameObject> &enemies)
    {
        spawnTimer += dt;
        if (spawnTimer > 1.5f)
        {
            float x = leftLimit + rand() % (int)(rightLimit - leftLimit);
            GameObject c = {{x, -50, 40, 40}, texture};
            bool overlap = false;
            for (auto &e : enemies)
            {
                if (e.active && CheckOverlap(e.rect, c.rect))
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
            c.rect.y += roadSpeed * dt;
            if (c.rect.y > screenHeight)
                c.active = false;
        }
    }

    void Draw(ParticleSystem &ps)
    {
        for (auto &c : coins)
            if (c.active)
            {
                DrawTexturePro(texture, {0, 0, (float)texture.width, (float)texture.height}, c.rect, {0, 0}, 0, YELLOW);
                ps.AddExplosion({c.rect.x + 20, c.rect.y + 20}, 2, YELLOW);
            }
    }
};

// HUD Manager
class HUD
{
public:
    Texture2D coinTex;
    int *score, *coins;
    float *speed;
    float gameTime;
    int screenWidth;

    HUD(Texture2D c, int *s, int *coin, float *sp, int width)
        : coinTex(c), score(s), coins(coin), speed(sp), gameTime(0), screenWidth(width) {}

    void Update(float dt, bool active)
    {
        if (active)
            gameTime += dt;
    }

    void Draw()
    {
        DrawText(TextFormat("Score: %d", *score), 20, 20, 30, WHITE);
        DrawText(TextFormat("Speed: %.0f", *speed), screenWidth / 2 - 50, 20, 30, ORANGE);
        DrawText(TextFormat("Time: %.1f", gameTime), 20, 60, 30, GREEN);
        DrawTexturePro(coinTex, {0, 0, (float)coinTex.width, (float)coinTex.height}, {(float)(screenWidth - 100), 20, 40, 40}, {0, 0}, 0, YELLOW);
        DrawText(TextFormat("x %d", *coins), screenWidth - 50, 30, 30, WHITE);
    }
};

// Leaderboard
class Leaderboard
{
public:
    std::vector<std::pair<std::string, int>> scores;

    void Load()
    {
        std::ifstream file("leaderboard.txt");
        if (file.is_open())
        {
            std::string name;
            int score;
            while (file >> name >> score)
                scores.push_back({name, score});
            file.close();
        }
    }

    void Save()
    {
        std::ofstream file("leaderboard.txt");
        for (auto &[name, score] : scores)
            file << name << " " << score << "\n";
        file.close();
    }

    void AddScore(const std::string &name, int score)
    {
        scores.push_back({name, score});
        std::sort(scores.begin(), scores.end(), [](auto &a, auto &b) { return a.second > b.second; });
        if (scores.size() > 5)
            scores.resize(5);
        Save();
    }

    void Draw(int screenWidth, int screenHeight)
    {
        DrawText("Leaderboard", screenWidth / 2 - 100, screenHeight / 2 - 100, 40, YELLOW);
        for (size_t i = 0; i < scores.size(); i++)
            DrawText(TextFormat("%d. %s - %d", i + 1, scores[i].first.c_str(), scores[i].second), screenWidth / 2 - 100, screenHeight / 2 - 50 + i * 40, 30, WHITE);
    }
};

// Settings Menu
class Settings
{
public:
    float volume = 0.5f;

    void Update(SoundManager &sound, Player &player)
    {
        if (IsKeyPressed(KEY_KP_ADD) || IsKeyPressed(KEY_EQUAL))
            volume = Clamp(volume + 0.1f, 0.0f, 1.0f);
        if (IsKeyPressed(KEY_KP_SUBTRACT) || IsKeyPressed(KEY_MINUS))
            volume = Clamp(volume - 0.1f, 0.0f, 1.0f);
        if (IsKeyPressed(KEY_LEFT))
            player.ChangeSkin(-1);
        if (IsKeyPressed(KEY_RIGHT))
            player.ChangeSkin(1);
        sound.SetVolume(volume);
    }

    void Draw(int screenWidth, int screenHeight, Player &player)
    {
        DrawText("Settings", screenWidth / 2 - 80, screenHeight / 2 - 100, 40, YELLOW);
        DrawText(TextFormat("Volume: %.2f", volume), screenWidth / 2 - 100, screenHeight / 2 - 50, 30, WHITE);
        DrawText("Press +/- to adjust volume", screenWidth / 2 - 150, screenHeight / 2 - 20, 20, GRAY);
        DrawText("Car Skin: Use LEFT/RIGHT", screenWidth / 2 - 150, screenHeight / 2 + 20, 30, WHITE);
        player.car.rect.x = screenWidth / 2 - 40;
        player.car.rect.y = screenHeight / 2 + 60;
        player.Draw();
        DrawText("Press ESC to return", screenWidth / 2 - 120, screenHeight / 2 + 220, 20, GRAY);
    }
};

// Main Function
int main()
{
    InitWindow(1, 1, "Loading...");

    Texture2D roadTex = LoadTexture("assets/road.png");
    int screenWidth = roadTex.width;
    int screenHeight = roadTex.height;
    SetWindowSize(screenWidth, screenHeight);
    SetWindowPosition(100,100); // Center the window
    SetWindowTitle("team - mukul, ananya, aastha");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL); // Disable default ESC to close window
    srand(time(0));

    // Load assets
    SoundManager soundManager;
    std::vector<Texture2D> playerSkins = {LoadTexture("assets/player1.png"), LoadTexture("assets/player2.png"), LoadTexture("assets/player3.png")};
    std::vector<Texture2D> enemySkins = {LoadTexture("assets/enemy1.png"), LoadTexture("assets/enemy2.png"), LoadTexture("assets/enemy3.png")};
    Texture2D coinTex = LoadTexture("assets/coin.png");
    std::map<std::string, Texture2D> powerUpTex = {
        {"shield", LoadTexture("assets/shield.png")},
        {"magnet", LoadTexture("assets/magnet.png")},
        {"nitro", LoadTexture("assets/nitro.png")}};

    Player player(playerSkins, screenWidth, screenHeight);
    EnemyManager enemyManager(enemySkins, player.leftLimit, player.rightLimit, screenHeight);
    CoinManager coinManager(coinTex, player.leftLimit, player.rightLimit, screenHeight);
    PowerUpManager powerUpManager(powerUpTex, player.leftLimit, player.rightLimit, screenHeight);
    ParticleSystem particles;

    int score = 0, coinCount = 0;
    HUD hud(coinTex, &score, &coinCount, &player.speed, screenWidth);
    Leaderboard leaderboard;
    leaderboard.Load();
    Settings settings;

    float roadScrollY = 0;
    bool gameOver = false;
    bool pause = false;
    bool startMenu = true;
    bool settingsMenu = false;
    bool showLeaderboard = false;
    bool enterName = false;
    std::string playerName = "";

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        soundManager.UpdateMusic();

        if (startMenu)
        {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("CAR DODGE GAME", screenWidth / 2 - 150, screenHeight / 2 - 100, 40, WHITE);
            DrawText("Press ENTER to Start", screenWidth / 2 - 140, screenHeight / 2 - 20, 30, YELLOW);
            DrawText("Press S for Settings", screenWidth / 2 - 140, screenHeight / 2 + 20, 30, GRAY);
            DrawText("Press L for Leaderboard", screenWidth / 2 - 140, screenHeight / 2 + 60, 30, GRAY);
            DrawText("Press Q to Quit", screenWidth / 2 - 100, screenHeight / 2 + 100, 20, GRAY);
            EndDrawing();

            if (IsKeyPressed(KEY_ENTER))
                startMenu = false;
            if (IsKeyPressed(KEY_S))
            {
                settingsMenu = true;
                startMenu = false;
            }
            if (IsKeyPressed(KEY_L))
            {
                showLeaderboard = true;
                startMenu = false;
            }
            if (IsKeyPressed(KEY_Q)) // Explicit quit option
                break;
            continue;
        }

        if (settingsMenu)
        {
            settings.Update(soundManager, player);
            BeginDrawing();
            ClearBackground(BLACK);
            settings.Draw(screenWidth, screenHeight, player);
            EndDrawing();
            if (IsKeyPressed(KEY_ESCAPE))
            {
                settingsMenu = false;
                startMenu = true; // Return to start menu
            }
            continue;
        }

        if (showLeaderboard)
        {
            BeginDrawing();
            ClearBackground(BLACK);
            leaderboard.Draw(screenWidth, screenHeight);
            DrawText("Press ESC to return", screenWidth / 2 - 120, screenHeight / 2 + 150, 20, GRAY);
            EndDrawing();
            if (IsKeyPressed(KEY_ESCAPE))
            {
                showLeaderboard = false;
                startMenu = true;
            }
            continue;
        }

        if (enterName)
        {
            BeginDrawing();
            ClearBackground(BLACK);
            DrawText("GAME OVER", screenWidth / 2 - 120, screenHeight / 2 - 100, 40, RED);
            DrawText(TextFormat("Final Score: %d", score), screenWidth / 2 - 100, screenHeight / 2 - 50, 30, WHITE);
            DrawText("Enter your name:", screenWidth / 2 - 100, screenHeight / 2, 30, YELLOW);
            DrawText(playerName.c_str(), screenWidth / 2 - 100, screenHeight / 2 + 40, 30, WHITE);
            DrawText("Press ENTER to submit", screenWidth / 2 - 120, screenHeight / 2 + 100, 20, GRAY);
            EndDrawing();

            int key = GetKeyPressed();
            if (key >= 32 && key <= 126 && playerName.length() < 10)
                playerName += (char)key;
            if (IsKeyPressed(KEY_BACKSPACE) && !playerName.empty())
                playerName.pop_back();
            if (IsKeyPressed(KEY_ENTER) && !playerName.empty())
            {
                leaderboard.AddScore(playerName, score);
                score = 0;
                coinCount = 0;
                hud.gameTime = 0;
                player = Player(playerSkins, screenWidth, screenHeight);
                enemyManager.enemies.clear();
                coinManager.coins.clear();
                powerUpManager.powerUps.clear();
                particles.particles.clear();
                gameOver = false;
                enterName = false;
                playerName = "";
                startMenu = true;
            }
            continue;
        }

        if (IsKeyPressed(KEY_P))
            pause = !pause;

        if (!pause && !gameOver)
        {
            player.Update(dt);
            float baseSpeed = 150.0f;
            float relSpeed = baseSpeed + player.speed;
            float roadSpeed = player.speed * 0.75f;

            if (player.speed > 10)
            {
                roadScrollY += roadSpeed * dt;
                if (roadScrollY >= screenHeight)
                    roadScrollY -= screenHeight;
            }

            enemyManager.Update(dt, relSpeed);
            coinManager.Update(dt, roadSpeed, enemyManager.enemies);
            powerUpManager.Update(dt, relSpeed, enemyManager.enemies);
            hud.Update(dt, true);
            particles.Update(dt);

            for (auto &e : enemyManager.enemies)
            {
                if (e.active && CheckOverlap(e.rect, player.car.rect))
                {
                    if (!player.shield)
                    {
                        soundManager.Play("crash");
                        particles.AddExplosion({player.car.rect.x + 40, player.car.rect.y + 75}, 20, GRAY);
                        gameOver = true;
                    }
                    else
                        e.active = false;
                }
                if (!e.active)
                    score++;
            }

            for (auto &c : coinManager.coins)
            {
                if (!c.active)
                    continue;
                Vector2 playerCenter = {player.car.rect.x + 40, player.car.rect.y + 75};
                if (player.magnet && CheckCollisionCircleRec(playerCenter, 100, c.rect))
                {
                    Vector2 coinCenter = {c.rect.x + 20, c.rect.y + 20};
                    Vector2 dir = {playerCenter.x - coinCenter.x, playerCenter.y - coinCenter.y};
                    float dist = sqrtf(dir.x * dir.x + dir.y * dir.y);
                    if (dist > 0)
                    {
                        dir.x /= dist;
                        dir.y /= dist;
                    }
                    c.rect.x += dir.x * 300 * dt;
                    c.rect.y += dir.y * 300 * dt;
                }
                if (CheckOverlap(c.rect, player.car.rect))
                {
                    c.active = false;
                    coinCount++;
                    score += 5;
                    soundManager.Play("coin");
                    particles.AddExplosion({c.rect.x + 20, c.rect.y + 20}, 5, YELLOW);
                }
            }

            for (auto &p : powerUpManager.powerUps)
            {
                if (p.active && CheckOverlap(p.rect, player.car.rect))
                {
                    p.active = false;
                    player.ActivatePowerUp(powerUpManager.GetType(p));
                    soundManager.Play("powerup");
                    particles.AddExplosion({p.rect.x + 20, p.rect.y + 20}, 10, GREEN);
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
        coinManager.Draw(particles);
        powerUpManager.Draw();
        particles.Draw();
        hud.Draw();

        if (pause)
            DrawText("PAUSED", screenWidth / 2 - 70, screenHeight / 2, 40, YELLOW);
        else if (gameOver && !enterName)
        {
            DrawText("GAME OVER", screenWidth / 2 - 120, screenHeight / 2, 40, RED);
            DrawText(TextFormat("Final Score: %d", score), screenWidth / 2 - 100, screenHeight / 2 + 50, 30, WHITE);
            DrawText("Press R to Restart", screenWidth / 2 - 100, screenHeight / 2 + 100, 20, GRAY);
            if (IsKeyPressed(KEY_R))
            {
                enterName = true;
            }
        }

        EndDrawing();
    }

    for (auto &tex : playerSkins)
        UnloadTexture(tex);
    for (auto &tex : enemySkins)
        UnloadTexture(tex);
    UnloadTexture(coinTex);
    for (auto &[k, tex] : powerUpTex)
        UnloadTexture(tex);
    UnloadTexture(roadTex);
    CloseWindow();
    return 0;
}