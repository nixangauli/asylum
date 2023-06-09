#include <raylib.h>
#include <iostream>
#include<vector>
using namespace std;

#define GRAVITY 900
#define JUMP_HEIGHT 500
#define MOVE_SPEED 200
#define ENEMY_MOVE_SPEED 100

const int screen_Width = 1000;
const int screen_Height = 600;

bool toggle = true;
bool darkMode = false;

class Player {
public:
    Rectangle rect;
    Vector2 velocity;
    bool isJumping;
    Color color;


    Player(Rectangle r, Vector2 v, bool j, Color c) : rect(r), velocity(v), isJumping(j), color(c) {
    }

    void control(bool left, bool right) {
        if (left) {
            rect.x -= MOVE_SPEED * GetFrameTime();
        }
        else if (right) {
            rect.x += MOVE_SPEED * GetFrameTime();
        }

        if (IsKeyPressed(KEY_UP)) {
            isJumping = true;
            velocity.y = -JUMP_HEIGHT;
        }

        velocity.y += GRAVITY * GetFrameTime();
        rect.y += velocity.y * GetFrameTime();
    }

    void draw() {

        Texture2D playerTexture = LoadTexture("Images/Player.png");
        DrawRectangleRec(rect, BLANK);
        DrawTexture(playerTexture, rect.x, rect.y - 30, WHITE);
    }
};

class Enemy {
public:
    Rectangle rect;
    Vector2 velocity;
    bool isMovingRight;
    Color color;

    Enemy(Rectangle r, Vector2 v, bool m, Color c) : rect(r), velocity(v), isMovingRight(m), color(c) {}

    void update(float deltaTime, Rectangle platformRect) {
        if (isMovingRight) {
            rect.x += ENEMY_MOVE_SPEED * deltaTime;
            if (rect.x + rect.width > platformRect.x + platformRect.width) {
                isMovingRight = false;
            }
        }
        else {
            rect.x -= ENEMY_MOVE_SPEED * deltaTime;
            if (rect.x < platformRect.x) {
                isMovingRight = true;
            }
        }
    }

    void draw() {
        Texture2D playerTexture = LoadTexture("Images/enemycolor.png");
        DrawRectangleRec(rect, BLANK);
        DrawTexture(playerTexture, rect.x, rect.y , WHITE);
    }
};

class Bullet {
public:
    Rectangle rect;
    Vector2 velocity;
    bool isActive;
    Color color;

    Bullet(Rectangle r, Vector2 v, bool a, Color c) : rect(r), velocity(v), isActive(a), color(c) {
    }

    void update() {
        if (isActive) {
            rect.x += velocity.x * GetFrameTime();
            rect.y += velocity.y * GetFrameTime();
        }
    }

    void draw() {
        if (isActive) {
            DrawRectangleRec(rect, color);
        }
    }
};

class Platform {
public:
    Rectangle rect;
    Color color;

    Platform(Rectangle r, Color c) : rect(r), color(c) {}

    void draw() {
        Texture2D playerTexture = LoadTexture("Images/platform.png");
        DrawRectangleRec(rect, BLANK);
        DrawTexture(playerTexture, rect.x, rect.y -10, WHITE);

    }
};

class MainPlatform {
public:
    Rectangle rect;
    Color color;

    MainPlatform(Rectangle r, Color c) : rect(r), color(c) {}

    void draw() {
        Texture2D playerTexture = LoadTexture("Images/ground.png");
        DrawRectangleRec(rect, BLANK);
        DrawTexture(playerTexture, rect.x, rect.y - 10, WHITE);

    }
};

class Menu;

class Game {
private:

    int score;
    Bullet bullet;
    Player player;

    MainPlatform platform1;
    Platform platform2;
    Platform platform3;
    Platform platform4;
    Platform platform5;
    Platform platform6;
    vector<Enemy> enemies;

public:
    Game()
        : player(Rectangle{ 10, 0, 40, 40 }, Vector2{ 0, 0 }, false, BLUE),
        bullet(Rectangle{ 0, 0, 10, 10 }, Vector2{ 600, 20 }, false, RED),
        platform1(Rectangle{ 0, 575, 1000, 25 }, DARKGRAY),
        platform2(Rectangle{ 200, 300, 250, 25 }, DARKGRAY),
        platform3(Rectangle{ 400, 200, 250, 25 }, DARKGRAY),
        platform4(Rectangle{ 300, 475, 250, 25 }, DARKGRAY),
        platform5(Rectangle{ 50, 150, 250, 25 }, DARKGRAY),
        platform6(Rectangle{ 600, 100, 250, 25 }, DARKGRAY),
        score(0)
    {
        //Enemy Spawn
        enemies.push_back(Enemy(Rectangle{ 350, 80, 30, 30 }, Vector2{ ENEMY_MOVE_SPEED, 0 }, true, GREEN));
        enemies.push_back(Enemy(Rectangle{ 400, 350, 30, 30 }, Vector2{ ENEMY_MOVE_SPEED, 0 }, true, RED));
        enemies.push_back(Enemy(Rectangle{ 700, 250, 30, 30 }, Vector2{ ENEMY_MOVE_SPEED, 0 }, true, RED));
        enemies.push_back(Enemy(Rectangle{ 600, 300, 30, 30 }, Vector2{ ENEMY_MOVE_SPEED, 0 }, true, YELLOW));
        enemies.push_back(Enemy(Rectangle{ 800, 400, 30, 30 }, Vector2{ ENEMY_MOVE_SPEED, 0 }, true, ORANGE));
    }


    void init() {
        InitWindow(screen_Width, screen_Height, "Asylum");
        SetTargetFPS(60);
    }


    void update() {
        player.control(IsKeyDown(KEY_LEFT), IsKeyDown(KEY_RIGHT));

        if (player.rect.x < 0 || player.rect.y < 0 || player.rect.x > GetScreenWidth() || player.rect.y > GetScreenHeight()) {
            player.rect.x = 0;
            player.rect.y = GetScreenHeight() / 2;
        }

        if (IsKeyPressed(KEY_SPACE)) {
            bullet.isActive = true;
            bullet.rect.x = player.rect.x + player.rect.width / 2 - bullet.rect.width / 2;
            bullet.rect.y = player.rect.y - bullet.rect.height;
        }

        bullet.update();

        if (bullet.isActive) {
            if (CheckCollisionRecs(bullet.rect, platform1.rect) ||
                CheckCollisionRecs(bullet.rect, platform2.rect) ||
                CheckCollisionRecs(bullet.rect, platform3.rect) ||
                CheckCollisionRecs(bullet.rect, platform4.rect) ||
                CheckCollisionRecs(bullet.rect, platform5.rect) ||
                CheckCollisionRecs(bullet.rect, platform6.rect)) {
                bullet.isActive = false;
            }
        }

        if (CheckCollisionRecs(player.rect, platform1.rect) && player.velocity.y > 0) {
            player.rect.y = platform1.rect.y - player.rect.height;
            player.isJumping = false;
            player.velocity.y = 0;
        }
        else if (CheckCollisionRecs(player.rect, platform2.rect) && player.velocity.y > 0) {
            player.rect.y = platform2.rect.y - player.rect.height;
            player.isJumping = false;
            player.velocity.y = 0;
        }
        else if (CheckCollisionRecs(player.rect, platform3.rect) && player.velocity.y > 0) {
            player.rect.y = platform3.rect.y - player.rect.height;
            player.isJumping = false;
            player.velocity.y = 0;
        }
        else if (CheckCollisionRecs(player.rect, platform4.rect) && player.velocity.y > 0) {
            player.rect.y = platform4.rect.y - player.rect.height;
            player.isJumping = false;
            player.velocity.y = 0;
        }
        else if (CheckCollisionRecs(player.rect, platform5.rect) && player.velocity.y > 0) {
            player.rect.y = platform5.rect.y - player.rect.height;
            player.isJumping = false;
            player.velocity.y = 0;
        }
        else if (CheckCollisionRecs(player.rect, platform6.rect) && player.velocity.y > 0) {
            player.rect.y = platform6.rect.y - player.rect.height;
            player.isJumping = false;
            player.velocity.y = 0;
        }


        for (auto& enemy : enemies) {
            enemy.update(GetFrameTime(), platform1.rect);
            if (CheckCollisionRecs(enemy.rect, platform1.rect)) {
                enemy.isMovingRight = !enemy.isMovingRight;
            }

            // Check for collision with bullet
            if (bullet.isActive && CheckCollisionRecs(bullet.rect, enemy.rect)) {
                bullet.isActive = false;
                score++;


                // Respawn enemy on a different platform
                int randomPlatform = GetRandomValue(2, 6); // Choose a random platform index (from 2 to 6)
                switch (randomPlatform) {
                case 2:
                    enemy.rect.x = platform2.rect.x + platform2.rect.width / 2 - enemy.rect.width / 2;
                    enemy.rect.y = platform2.rect.y - enemy.rect.height;
                    break;
                case 3:
                    enemy.rect.x = platform3.rect.x + platform3.rect.width / 2 - enemy.rect.width / 2;
                    enemy.rect.y = platform3.rect.y - enemy.rect.height;
                    break;
                case 4:
                    enemy.rect.x = platform4.rect.x + platform4.rect.width / 2 - enemy.rect.width / 2;
                    enemy.rect.y = platform4.rect.y - enemy.rect.height;
                    break;
                case 5:
                    enemy.rect.x = platform5.rect.x + platform5.rect.width / 2 - enemy.rect.width / 2;
                    enemy.rect.y = platform5.rect.y - enemy.rect.height;
                    break;
                case 6:
                    enemy.rect.x = platform6.rect.x + platform6.rect.width / 2 - enemy.rect.width / 2;
                    enemy.rect.y = platform6.rect.y - enemy.rect.height;
                    break;
                }
            }

            // Check for player/enemy collision
            if (CheckCollisionRecs(player.rect, enemy.rect)) {
                player.rect.x = 100;
                player.rect.y = 500;
                
                toggle = !toggle;
                score = 0;
            }
        }

        if (player.rect.y < 0) {
            exit(0);
        }
        if (IsKeyPressed(KEY_ESCAPE)) {
            toggle = true;
        }
    }

    void draw() {
        BeginDrawing();
        ClearBackground(darkMode ? BLACK: WHITE);

        bullet.draw();
        platform1.draw();
        platform2.draw();
        platform3.draw();
        platform4.draw();
        platform5.draw();
        platform6.draw();

        player.draw();
        for (auto& enemy : enemies) {
            enemy.draw();
        }

        DrawText(TextFormat("Score: %04i", score), 10, 10, 30, darkMode ? WHITE : BLACK);

        EndDrawing();
    }

    bool isGameOver() {
        return WindowShouldClose();
    }

    void close() {
        CloseWindow();
    }
};

class about {
public:
    void about_game() {
		bool toggle_about = true;
        while (!WindowShouldClose()) {
        
			BeginDrawing();
			ClearBackground(darkMode ? BLUE : BLACK);

			DrawText("ABOUT ASYLUM", 20, 20, 40, darkMode ? BLACK : WHITE);
			DrawText("This game is made by Nitesh, Nixan and Utsav.", 20, 100, 20, darkMode ? BLACK : WHITE);
			DrawText("ASYLUM is an arcade game developed for learning the implementation of concepts of OOP.", 20, 120, 20, darkMode ? BLACK : WHITE);
            DrawText("Enemy spawn randomly on the game and the player has to ", 20, 140, 20, darkMode ? BLACK : WHITE);
            DrawText("shoot them to score points.", 20, 160, 20, darkMode ? BLACK : WHITE);
            DrawText("Press 'ESC' to play the game", 20, 200, 20, darkMode ? BLACK : WHITE);
			EndDrawing();
		}
	}
};

class Menu : public Game {
    public:
        about about_game;

    int menu(bool toggle) {
        if (toggle) {
            bool toggle_menu = true;
            const int menu_Items_Count = 4;
            const char* menu_Items[menu_Items_Count] = { "START", "ABOUT GAME", "DARK MODE", "EXIT" };
            int selected_Menu_Item = 0;


            while (!WindowShouldClose()) {
                if (IsKeyPressed(KEY_UP)) {
                    selected_Menu_Item--;
                    if (selected_Menu_Item < 0) {
                        selected_Menu_Item = menu_Items_Count - 1;
                    };
                }
                else if (IsKeyPressed(KEY_DOWN)) {
                    selected_Menu_Item++;
                    if (selected_Menu_Item >= menu_Items_Count) {
                        selected_Menu_Item = 0;
                    };
                }

                BeginDrawing();
                ClearBackground(darkMode ? BLACK : BLUE);

                for (int i = 0; i < menu_Items_Count; i++) {
                    if (i == selected_Menu_Item)
                        DrawText(menu_Items[i], screen_Width / 2 - MeasureText(menu_Items[i], 40) / 2, screen_Height / 2 - 60 + i * 60, 40, darkMode ? BLUE : WHITE);
                    else
                        DrawText(menu_Items[i], screen_Width / 2 - MeasureText(menu_Items[i], 40) / 2, screen_Height / 2 - 60 + i * 60, 40, darkMode ? WHITE : BLACK);
                }

                if (IsKeyPressed(KEY_ENTER)) {
                    toggle = false;
                    switch (selected_Menu_Item) {
                    case 0:
                        update();
                        toggle = false;
                        break;
                    case 1:
                        about_game.about_game();
						break;
                    case 2:
                        darkMode = !darkMode;
                        toggle = false;
                        break;
                    case 3:
                        exit(0);
                        CloseWindow();
                        return 0;
                        break;
                    }
                    // Move the draw() function call inside the switch case
                    draw();
                    break;
                }

                EndDrawing();
            }
        }
        return -1;
    }
};

int main()
{
    Game game;
    Menu menu;
    game.init();
    SetTraceLogLevel(LOG_ERROR);

    while (!game.isGameOver()) {
        if (toggle) {
            menu.menu(toggle);
            toggle = false;
        }
        game.update();
        game.draw();

        
    }

    game.close();
    return 0;
}
