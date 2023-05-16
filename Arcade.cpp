#include "raylib.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#define GRAVITY 900
#define JUMP_HEIGHT 400
#define MOVE_SPEED 200
#define ENEMY_MOVE_SPEED 100

const int screen_Width = 1280;
const int screen_Height = 720;

bool toggle = true;

class Player {
public:
    Rectangle rect;
    Vector2 velocity;
    bool isJumping;            //why bool is used is cause to keep track wether the player is jumping or not
    Color color;

    Texture2D texture;

    Player(Rectangle r, Vector2 v, bool j, Color c) : rect(r), velocity(v), isJumping(j), color(c) {
        texture = LoadTexture("Images/player.png");
    }

    void control(bool left, bool right) {
        if (left) {
            rect.x -= MOVE_SPEED * GetFrameTime();
        }
        else if (right) {
            rect.x += MOVE_SPEED * GetFrameTime();
        }

        // Check if the player is on the ground to allow jumping
        if (IsKeyPressed(KEY_UP) && !isJumping && rect.y >= screen_Height - 80 - rect.height) {
            isJumping = true;
            velocity.y = -JUMP_HEIGHT;
        }

        // Apply gravity and update player position
        velocity.y += GRAVITY * GetFrameTime();
        rect.y += velocity.y * GetFrameTime();

        // Check if the player has landed on the ground
        if (rect.y >= screen_Height - rect.height) {
            rect.y = screen_Height - rect.height;
            velocity.y = 0;
            isJumping = false;
        }

        // Limit player position to stay within the screen height
        if (rect.y > screen_Height - rect.height) {
            rect.y = screen_Height - rect.height;
        }
    }

    void draw() {
        DrawRectangleRec(rect, color);
        DrawTexture(texture, static_cast<int>(rect.x), static_cast<int>(rect.y), WHITE);
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

        // Move the enemy horizontally
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
        DrawRectangleRec(rect, color);
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
        DrawRectangleRec(rect, color);
    }
};

class Game {
public:


public:
    Bullet bullet;
    std::vector<Bullet> bullets;
    Player player;
    Platform ground;
    Platform platform2;
    Platform platform3;
    Platform platform4;
    Platform platform5;
    Platform platform6;
    Enemy enemy;

    Game() :
        player(Rectangle{ 0, 0, 40, 40 }, Vector2{ 0, 0 }, false, BLUE),
        enemy(Rectangle{ 400, 350, 30, 30 }, Vector2{ 0, 0 }, true, RED),
        bullet(Rectangle{ 0, 0, 10, 10 }, Vector2{ 600, 20 }, false, RED),
        ground(Rectangle{ 0, 700, 1280, 50 }, DARKGRAY),      //PLatform or ground
        platform2(Rectangle{ 200, 600, 200, 25 }, DARKGRAY),    //first,second postition,
        platform3(Rectangle{ 500, 500, 200, 25 }, DARKGRAY),    //fourth size
        platform4(Rectangle{ 300, 400, 300, 25 }, DARKGRAY),    //third length
        platform5(Rectangle{ 150, 300, 300, 25 }, DARKGRAY),
        platform6(Rectangle{ 600, 200, 250, 25 }, DARKGRAY)
    {

    }

    void in_it() {
        InitWindow(screen_Width, screen_Height, "Asylum");
        SetTargetFPS(60);
    }

    void update() {
        player.control(IsKeyDown(KEY_LEFT), IsKeyDown(KEY_RIGHT));

        if (IsKeyPressed(KEY_SPACE) && !player.isJumping) {
            // Shoot a new bullet
            Bullet newBullet(
                Rectangle{ player.rect.x + player.rect.width / 2 - bullet.rect.width / 2,
                           player.rect.y - bullet.rect.height,
                           bullet.rect.width,
                           bullet.rect.height },
                Vector2{ 600, 20 },
                true,
                RED
            );
            bullets.push_back(newBullet);
        }



        // Update all active bullets
        for (auto& bullet : bullets) {
            bullet.update();

            // Check for collision between bullet and platforms
            if (bullet.isActive) {
                if (CheckCollisionRecs(bullet.rect, ground.rect) ||
                    CheckCollisionRecs(bullet.rect, platform2.rect) ||
                    CheckCollisionRecs(bullet.rect, platform3.rect) ||
                    CheckCollisionRecs(bullet.rect, platform4.rect) ||
                    CheckCollisionRecs(bullet.rect, platform5.rect) ||
                    CheckCollisionRecs(bullet.rect, platform6.rect)) {
                    bullet.isActive = false;
                }
            }
        }

        // Remove inactive bullets
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& bullet) {
            return !bullet.isActive;
            }), bullets.end());

        //PLATFORM COLLISION different to bullet collision
        if (CheckCollisionRecs(player.rect, ground.rect) && player.velocity.y > 0) {
            player.rect.y = ground.rect.y - player.rect.height;
            player.isJumping = false;
            player.velocity.y = 0;
        }
        else if (CheckCollisionRecs(player.rect, platform2.rect)) {
            player.rect.y = platform2.rect.y - player.rect.height;
            player.isJumping = false;
            player.velocity.y = 0;
        }
        else if (CheckCollisionRecs(player.rect, platform3.rect)) {
            player.rect.y = platform3.rect.y - player.rect.height;
            player.isJumping = false;
            player.velocity.y = 0;
        }
        else if (CheckCollisionRecs(player.rect, platform4.rect)) {
            player.rect.y = platform4.rect.y - player.rect.height;
            player.isJumping = false;
            player.velocity.y = 0;
        }
        else if (CheckCollisionRecs(player.rect, platform5.rect)) {
            player.rect.y = platform5.rect.y - player.rect.height;
            player.isJumping = false;
            player.velocity.y = 0;
        }
        else if (CheckCollisionRecs(player.rect, platform6.rect)) {
            player.rect.y = platform6.rect.y - player.rect.height;
            player.isJumping = false;
            player.velocity.y = 0;
        }

        // Move enemy and check for collisions
        enemy.update(GetFrameTime(), ground.rect);
        if (CheckCollisionRecs(enemy.rect, ground.rect)) {
            enemy.isMovingRight = !enemy.isMovingRight;
        }
        else if (CheckCollisionRecs(enemy.rect, platform2.rect)) {
			enemy.isMovingRight = !enemy.isMovingRight;
		}
        else if (CheckCollisionRecs(enemy.rect, platform3.rect)) {
			enemy.isMovingRight = !enemy.isMovingRight;
		}
        else if (CheckCollisionRecs(enemy.rect, platform4.rect)) {
			enemy.isMovingRight = !enemy.isMovingRight;
		}
        else if (CheckCollisionRecs(enemy.rect, platform5.rect)) {
			enemy.isMovingRight = !enemy.isMovingRight;
		}
        else if (CheckCollisionRecs(enemy.rect, platform6.rect)) {
			enemy.isMovingRight = !enemy.isMovingRight;
		}

        // Check for player/enemy collision
        if (CheckCollisionRecs(player.rect, enemy.rect)) {
            toggle = !toggle;
        }

        if (player.rect.y < 0) {
            exit(0);
        }

        if (IsKeyPressed(KEY_P)) {
            toggle = true;
        }
    }

    void draw() {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        bullet.draw();
        for (auto& bullet : bullets) {
			bullet.draw();
		}
        ground.draw();
        platform2.draw();
        platform3.draw();
        platform4.draw();
        platform5.draw();
        platform6.draw();

        player.draw();
        enemy.draw();

        EndDrawing();
    }

    bool isGameOver() {
        return WindowShouldClose();
    }

    void close() {
        CloseWindow();
    }
};

class CreditScreen {
public:
    void Draw() const {
        BeginDrawing();

        ClearBackground(BLUE);

        // Calculate the center position of the screen
        int centerX = screen_Width / 2;
        int centerY = screen_Height / 2;

        // Calculate the position of the credit text
        int textWidth = MeasureText("Credits", 40);
        int textHeight = 40;
        int textX = centerX - textWidth / 2;
        int textY = centerY - textHeight / 2;

        // Draw the credit text
        DrawText("Credits", textX, textY - 150, 50, BLACK);
        DrawText("Developers:", textX, textY - 100, 40, BLACK);
        DrawText("Nixan Gauli", textX, textY - 50, 25, BLACK);
        DrawText("Nitesh Poudel", textX, textY, 25, BLACK);
        DrawText("Utsav Thapa", textX, textY + 50, 25, BLACK);
        DrawText("Artworks", textX, textY + 100, 50, BLACK);
        DrawText("Google", textX, textY + 150, 25, BLACK);

        EndDrawing();
    }
};

class Menu : public Game {
public:
    int menu(bool toggle) // Menu system used int for  value return if error occures
    {
        if (toggle == true) {
            bool toggle_menu = true;
            const int menu_Items_Count = 3;
            const char* menu_Items[menu_Items_Count] = { "START", "CREDITS", "EXIT" };
            int selected_Menu_Item = 0;

            while (!WindowShouldClose()){

                if (IsKeyPressed(KEY_UP)) //KeyPressed meaning press a key (Raylib.h)
                {
                    selected_Menu_Item--;
                    if (selected_Menu_Item < 0){
                        selected_Menu_Item = menu_Items_Count - 1;
                    }
                }
                else if (IsKeyPressed(KEY_DOWN)){
                    selected_Menu_Item++;

                    if (selected_Menu_Item >= menu_Items_Count){
                        selected_Menu_Item = 0;
                    }
                }
                // Draw
                BeginDrawing();
                ClearBackground(BLUE);

                for (int i = 0; i < menu_Items_Count; i++){
                    // Highlight the selected menu item
                    if (i == selected_Menu_Item)
                        DrawText(menu_Items[i], screen_Width / 2 - MeasureText(menu_Items[i], 40) / 2, screen_Height / 2 - 60 + i * 60, 40, WHITE);
                    else
                        DrawText(menu_Items[i], screen_Width / 2 - MeasureText(menu_Items[i], 40) / 2, screen_Height / 2 - 60 + i * 60, 40, BLACK);
                }

                // Check if the Enter key was pressed on the selected menu item
                if (IsKeyPressed(KEY_ENTER)) {
                    switch (selected_Menu_Item) {
                    case 0:
                        toggle = false;
                        return 0;
                    case 1:
                        toggle = false;
                        return 1;
                    case 2:
                        toggle = false;
                        return 2;
                    }
                }

                EndDrawing();
            }
        }
        return -1; // Error
    }

    void showCreditScreen() {
        CreditScreen creditScreen;
        bool exitCreditScreen = false;

        while (!exitCreditScreen) {
            BeginDrawing();
            ClearBackground(BLUE);

            creditScreen.Draw();

            EndDrawing();

            if (IsKeyPressed(KEY_ENTER))
                exitCreditScreen = true;
        }
    }
};



int main()
{
    Game game;
    Menu menu;
    game.in_it();

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
