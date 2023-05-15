#include <raylib.h>
#include <iostream>
#include <string>
#include <vector>

const int screenWidth = 1280;
const int screenHeight = 720;

class GameObject {
protected:
    Vector2 position;
    Texture2D texture;

public:
    GameObject(Vector2 pos, const std::string& texturePath)
        : position(pos)
    {
        texture = LoadTexture(texturePath.c_str());
    }

    virtual void Update() = 0;

    void Draw()
    {
        DrawTextureV(texture, position, WHITE);
    }

    Vector2 GetPosition() const
    {
        return position;
    }
};

class Player : public GameObject {
private:
    const float moveSpeed = 300.0f;
    const float jumpForce = 600.0f;
    Vector2 velocity;
    Vector2 position;
    bool isJumping;

public:
    Player(Vector2 pos)
        : GameObject(pos, "Images/Player.png")
        , velocity({ 0.0f, 0.0f })
        , isJumping(false)
    {
        position.x = 0;  // Set the x-coordinate to the left side of the screen
        position.y = screenHeight / 2;  // Set the y-coordinate to the center of the screen
    }

    void Update() override
    {
        // Player movement
        if (IsKeyDown(KEY_A))
        {
            position.x -= moveSpeed * GetFrameTime();
        }
        else if (IsKeyDown(KEY_D))
        {
            position.x += moveSpeed * GetFrameTime();
        }

        // Player jumping
        if (IsKeyPressed(KEY_SPACE) && !isJumping)
        {
            velocity.y = -jumpForce;
            isJumping = true;
        }

        // Apply gravity
        velocity.y += 1000.0f * GetFrameTime();

        // Apply velocity to position
        position.y += velocity.y * GetFrameTime();

        // Check if player is on the ground
        if (position.y >= screenHeight - texture.height)
        {
            position.y = screenHeight - texture.height;
            velocity.y = 0.0f;
            isJumping = false;
        }
    }
};

class Platform : public GameObject {
public:
    Platform(Vector2 pos, const std::string& texturePath)
        : GameObject(pos, texturePath)
    {
    }

    void Update() override
    {
        // Platforms do not require any update logic
    }
};

class Game {
private:
    Player player;
    std::vector<Platform> platforms;

public:
    Game()
        : player({ screenWidth / 2.0f, screenHeight / 2.0f })
    {
        platforms.push_back(Platform({ 0, screenHeight - 20 }, "Images/Mainplatform.png"));
        platforms.push_back(Platform({ screenWidth / 2.0f, screenHeight - 20 }, "Images/Mainplatform.png"));
        platforms.push_back(Platform({ screenWidth - 100, screenHeight - 20 }, "Images/Mainplatform.png"));
    }

    void Update()
    {
        player.Update();

        for (auto& platform : platforms)
        {
            platform.Update();
        }
    }

    void Draw()
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        player.Draw();

        for (auto& platform : platforms)
        {
            platform.Draw();
        }

        EndDrawing();
    }
};

class Menu {
public:
    enum class MenuItem {
        StartGame,
        Credits,
        Exit
    };

    MenuItem GetSelectedMenuItem() const {
        return selectedMenuItem;
    }

    void Update() {
        if (IsKeyPressed(KEY_UP)) {
            int selectedItem = static_cast<int>(selectedMenuItem);
            selectedItem--;
            if (selectedItem < 0)
                selectedItem = static_cast<int>(MenuItem::Exit);
            selectedMenuItem = static_cast<MenuItem>(selectedItem);
        }
        else if (IsKeyPressed(KEY_DOWN)) {
            int selectedItem = static_cast<int>(selectedMenuItem);
            selectedItem++;
            if (selectedItem > static_cast<int>(MenuItem::Exit))
                selectedItem = static_cast<int>(MenuItem::StartGame);
            selectedMenuItem = static_cast<MenuItem>(selectedItem);
        }
    }

    void Draw() const {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        // Calculate the center position of the screen
        int centerX = screenWidth / 2;
        int centerY = screenHeight / 2;

        // Calculate the position of the menu text
        int textWidth = MeasureText("Start Game", 30);
        int textHeight = 30;
        int textX = centerX - textWidth / 2;
        int textY = centerY - textHeight / 2;

        // Draw menu items
        DrawText("Start Game", textX, textY-50, 30, selectedMenuItem == MenuItem::StartGame ? BLUE : BLACK);
        DrawText("Credits", textX, textY, 30, selectedMenuItem == MenuItem::Credits ? BLUE : BLACK);
        DrawText("Exit", textX, textY+50, 30, selectedMenuItem == MenuItem::Exit ? BLUE : BLACK);

        EndDrawing();
    }

private:
    MenuItem selectedMenuItem = MenuItem::StartGame;
};

int main()
{
    InitWindow(screenWidth, screenHeight, "Asylum");
    Menu menu;
    Game game;

    bool inMainMenu = true;
    bool inGame = false;

    while (!WindowShouldClose()) {
        if (inMainMenu) {
            menu.Update();
            menu.Draw();

            if (IsKeyPressed(KEY_ENTER)) {
                switch (menu.GetSelectedMenuItem()) {
                case Menu::MenuItem::StartGame:
                    inMainMenu = false;
                    inGame = true;
                    break;
                case Menu::MenuItem::Credits:
                    // Show credits
                    break;
                case Menu::MenuItem::Exit:
                    CloseWindow();
                    break;
                }
            }
        }
        else if (inGame) {
            game.Update();
            game.Draw();

            if (IsKeyPressed(KEY_ESCAPE)) {
                inMainMenu = true;
                inGame = false;
            }
        }
    }

    CloseWindow();

    return 0;
}