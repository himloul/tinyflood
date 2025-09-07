#include "raylib.h"
#include <vector>
#include <random>
#include <string>

// Configuration
const int BOARD_SIZE = 12;
const int CELL_SIZE = 35;
const int PADDING = 50;
const int MAX_MOVES = 15;
const int WINDOW_SIZE = BOARD_SIZE * CELL_SIZE + PADDING * 2;

// Game colors
const Color GAME_COLORS[] = {
    {69, 130, 181, 255},   // Steel Blue
    {217, 94, 94, 255},    // Indian Red
    {117, 189, 166, 255},  // Medium Aquamarine
    {242, 176, 87, 255}    // Sandy Brown
};
const int NUM_COLORS = 4;

enum GameState { MENU, PLAYING, INSTRUCTIONS };

class FloodGame {
private:
    std::vector<std::vector<int>> board;
    int moves = 0;
    bool gameOver = false;
    bool win = false;
    GameState state = MENU;
    Font font;
    Font titleFont;
    std::mt19937 rng;

    // Keyboard navigation variables
    int selectedMenuItem = 0;  // For menu navigation
    int selectedColorIndex = 0;  // For color selection in game
    bool onRestartButton = false;  // Whether restart button is selected

    void initBoard() {
        board.assign(BOARD_SIZE, std::vector<int>(BOARD_SIZE));
        std::uniform_int_distribution<int> dist(0, NUM_COLORS - 1);
        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int x = 0; x < BOARD_SIZE; x++) {
                board[y][x] = dist(rng);
            }
        }
    }

    void floodFill(int x, int y, int oldColor, int newColor) {
        if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) return;
        if (board[y][x] != oldColor || oldColor == newColor) return;

        board[y][x] = newColor;
        floodFill(x + 1, y, oldColor, newColor);
        floodFill(x - 1, y, oldColor, newColor);
        floodFill(x, y + 1, oldColor, newColor);
        floodFill(x, y - 1, oldColor, newColor);
    }

    bool checkWin() {
        int firstColor = board[0][0];
        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int x = 0; x < BOARD_SIZE; x++) {
                if (board[y][x] != firstColor) return false;
            }
        }
        return true;
    }

    void restart() {
        initBoard();
        moves = 0;
        gameOver = false;
        win = false;
        selectedColorIndex = 0;
        onRestartButton = false;
    }

    // Helper: draw text centered horizontally using a specific font
    void DrawTextCentered(const Font &f, const char* text, float y, float fontSize, float spacing, Color color) {
        Vector2 sz = MeasureTextEx(f, text, fontSize, spacing);
        Vector2 pos = { (float)GetScreenWidth() / 2.0f - sz.x / 2.0f, y };
        DrawTextEx(f, text, pos, fontSize, spacing, color);
    }

public:
    FloodGame() : rng(std::random_device{}()) {
        initBoard();
    }

    void LoadResources() {
        // Prefer LoadFontEx if you want to set glyph size, but LoadFont works for most cases
        font = LoadFont("assets/fonts/monogram.ttf");
        if (font.texture.id == 0) {
            // Fallback to default font
            font = GetFontDefault();
        }
        titleFont = font; // Using same font for simplicity
    }

    void Update() {
        if (state == PLAYING && !gameOver && !win) {
            if (moves >= MAX_MOVES) {
                gameOver = true;
            } else if (checkWin()) {
                win = true;
            }
        }

        // Handle keyboard input
        HandleKeyboardInput();

        // Handle escape key
        if (IsKeyPressed(KEY_ESCAPE) && state != MENU) {
            state = MENU;
            selectedMenuItem = 0;
        }
    }

    void HandleKeyboardInput() {
        switch (state) {
            case MENU:
                HandleMenuInput();
                break;
            case PLAYING:
                HandleGameInput();
                break;
            case INSTRUCTIONS:
                HandleInstructionsInput();
                break;
        }
    }

    void HandleMenuInput() {
        // Navigate menu with up/down arrows
        if (IsKeyPressed(KEY_UP)) {
            selectedMenuItem = (selectedMenuItem - 1 + 2) % 2;
        } else if (IsKeyPressed(KEY_DOWN)) {
            selectedMenuItem = (selectedMenuItem + 1) % 2;
        }

        // Select menu item with Enter
        if (IsKeyPressed(KEY_ENTER)) {
            if (selectedMenuItem == 0) {
                state = PLAYING;
                selectedColorIndex = 0;
                onRestartButton = false;
            } else if (selectedMenuItem == 1) {
                state = INSTRUCTIONS;
            }
        }
    }

    void HandleGameInput() {
        if (gameOver || win) {
            // Only allow restart when game is over
            if (IsKeyPressed(KEY_ENTER)) {
                restart();
            }
            return;
        }

        // Navigate between color buttons and restart button
        if (IsKeyPressed(KEY_UP)) {
            onRestartButton = !onRestartButton;
        } else if (IsKeyPressed(KEY_DOWN)) {
            onRestartButton = !onRestartButton;
        }

        if (!onRestartButton) {
            // Navigate color selection with left/right arrows
            if (IsKeyPressed(KEY_LEFT)) {
                selectedColorIndex = (selectedColorIndex - 1 + NUM_COLORS) % NUM_COLORS;
            } else if (IsKeyPressed(KEY_RIGHT)) {
                selectedColorIndex = (selectedColorIndex + 1) % NUM_COLORS;
            }

            // Select color with Enter
            if (IsKeyPressed(KEY_ENTER)) {
                int oldColor = board[0][0];
                if (oldColor != selectedColorIndex) {
                    floodFill(0, 0, oldColor, selectedColorIndex);
                    moves++;
                }
            }
        } else {
            // Restart button is selected
            if (IsKeyPressed(KEY_ENTER)) {
                restart();
            }
        }
    }

    void HandleInstructionsInput() {
        // Return to menu with Enter
        if (IsKeyPressed(KEY_ENTER)) {
            state = MENU;
            selectedMenuItem = 0;
        }
    }

    void Draw() {
        BeginDrawing();
        ClearBackground(WHITE);

        switch (state) {
            case MENU:
                DrawMenu();
                break;
            case PLAYING:
                DrawGame();
                break;
            case INSTRUCTIONS:
                DrawInstructions();
                break;
        }

        EndDrawing();
    }

    void DrawMenu() {
        // Title (centered)
        DrawTextCentered(titleFont, "TINY FLOOD", 100.0f, 60.0f, 1.0f, GAME_COLORS[1]);

        // Draw menu items with selection highlight
        Color startColor = (selectedMenuItem == 0) ? GAME_COLORS[1] : GAME_COLORS[0];
        Color instructionsColor = (selectedMenuItem == 1) ? GAME_COLORS[1] : GAME_COLORS[0];

        DrawTextEx(font, "Start", { (float)GetScreenWidth()/2 - 80, 300.0f }, 30.0f, 1.0f, startColor);
        DrawTextEx(font, "Instructions", { (float)GetScreenWidth()/2 - 80, 350.0f }, 30.0f, 1.0f, instructionsColor);

        // Draw selection indicator
        int indicatorY = selectedMenuItem == 0 ? 300 : 350;
        DrawTextEx(font, ">", { (float)GetScreenWidth()/2 - 100, (float)indicatorY }, 30.0f, 1.0f, GAME_COLORS[1]);

        // Draw controls hint
        DrawTextCentered(font, "UP/DOWN to navigate", 450.0f, 20.0f, 1.0f, GRAY);
        DrawTextCentered(font, "ENTER to select", 480.0f, 20.0f, 1.0f, GRAY);

    }

    void DrawInstructions() {
        DrawTextEx(font, "Instructions", { (float)PADDING, 100.0f }, 40.0f, 1.0f, GAME_COLORS[1]);
        DrawTextEx(font, "LEFT/RIGHT: Select colors", { (float)PADDING, 150.0f }, 22.0f, 1.0f, GAME_COLORS[0]);
        DrawTextEx(font, "ENTER: Flood with selected color", { (float)PADDING, 180.0f }, 22.0f, 1.0f, GAME_COLORS[0]);
        DrawTextEx(font, "UP/DOWN: Switch colors and restart", { (float)PADDING, 210.0f }, 22.0f, 1.0f, GAME_COLORS[0]);
        DrawTextEx(font, "ESC: to return to menu", { (float)PADDING, 240.0f }, 22.0f, 1.0f, GAME_COLORS[0]);

        DrawTextEx(font, "---", { (float)PADDING, 270.0f }, 22.0f, 1.0f, GAME_COLORS[0]);
        DrawTextEx(font, "Fill entire board w/ one color", { (float)PADDING, 300.0f }, 22.0f, 1.0f, GAME_COLORS[0]);
        DrawTextEx(font, "in few moves", { (float)PADDING, 330.0f }, 22.0f, 1.0f, GAME_COLORS[0]);


        const char* hint = "Press ENTER to return to menu";
        Vector2 hintSz = MeasureTextEx(font, hint, 22.0f, 1.0f);
        DrawTextEx(font, hint, { (float)GetScreenWidth() / 2.0f - hintSz.x / 2.0f, (float)GetScreenHeight() - 50.0f }, 22.0f, 1.0f, GAME_COLORS[1]);
    }

    void DrawGame() {
        // Draw board
        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int x = 0; x < BOARD_SIZE; x++) {
                Rectangle cell = {
                    (float)(PADDING + x * CELL_SIZE),
                    (float)(PADDING + y * CELL_SIZE),
                    (float)CELL_SIZE,
                    (float)CELL_SIZE
                };
                DrawRectangleRec(cell, GAME_COLORS[board[y][x]]);
            }
        }

        // Draw move counter
        std::string moveText = "Moves: " + std::to_string(moves) + "/" + std::to_string(MAX_MOVES);
        DrawTextEx(font, moveText.c_str(), { (float)PADDING, (float)(PADDING * 0.2) }, 24.0f, 1.0f, GAME_COLORS[0]);

        // Draw color buttons with selection highlight
        int buttonY = PADDING + BOARD_SIZE * CELL_SIZE + 20;
        for (int i = 0; i < NUM_COLORS; i++) {
            Rectangle button = {(float)(PADDING + i * 40), (float)buttonY, 35.0f, 35.0f};
            DrawRectangleRounded(button, 0.2f, 0, GAME_COLORS[i]);

            // Draw selection indicator for selected color
            if (!onRestartButton && i == selectedColorIndex) {
                DrawRectangleLinesEx(button, 4.0f, WHITE);
                DrawTextEx(font, "^", { PADDING + i * 40 + 10.0f, (float)buttonY - 25.0f }, 20.0f, 1.0f, WHITE);
            }
        }

        // Draw restart button with selection highlight
        Rectangle restartBtn = {(float)PADDING, (float)(buttonY + 50), 120.0f, 40.0f};
        Color restartColor = onRestartButton ? GAME_COLORS[1] : LIGHTGRAY;
        DrawRectangleRounded(restartBtn, 0.2f, 0, restartColor);
        DrawTextEx(font, "Restart", { PADDING + 25.0f, (float)buttonY + 60.0f }, 20.0f, 1.0f, onRestartButton ? WHITE : BLACK);

        if (onRestartButton) {
            DrawTextEx(font, ">", { (float)PADDING - 20.0f, (float)buttonY + 60.0f }, 20.0f, 1.0f, GAME_COLORS[1]);
        }

        // Draw controls hint
        if (!gameOver && !win) {
            DrawTextEx(font, "LEFT/RIGHT: Select color", { (float)PADDING, (float)buttonY + 100.0f }, 16.0f, 1.0f, GRAY);
            DrawTextEx(font, "UP/DOWN: Switch focus", { (float)PADDING, (float)buttonY + 120.0f }, 16.0f, 1.0f, GRAY);
            DrawTextEx(font, "ENTER: Confirm", { (float)PADDING, (float)buttonY + 140.0f }, 16.0f, 1.0f, GRAY);
        }

        // Draw win/game over message
        if (win || gameOver) {
            // Define the semi-transparent background rectangle area
            Rectangle overlayRect = { 0, (float)GetScreenHeight() / 2 - 50, (float)GetScreenWidth(), 150 };
            DrawRectangleRec(overlayRect, win ? Color{0, 128, 0, 200} : Color{128, 0, 0, 200});

            // --- MODIFIED SECTION ---
            // Use the DrawTextCentered helper for the main message
            const char* message = win ? "You Win!" : "Game Over!";
            Vector2 msz = MeasureTextEx(titleFont, message, 40.0f, 1.0f);
            // Calculate the Y position to be vertically centered within the overlay
            float messageY = overlayRect.y + (overlayRect.height / 2.0f) - (msz.y / 2.0f);
            DrawTextCentered(titleFont, message, messageY, 40.0f, 1.0f, WHITE);

            // Use the DrawTextCentered helper for the restart hint as well
            // const char* restartHint = "Press ENTER to restart";
            // DrawTextCentered(font, restartHint, (float)GetScreenHeight() / 2 + 50.0f, 20.0f, 1.0f, WHITE);
        }
    }

    void Cleanup() {
        if (font.texture.id != GetFontDefault().texture.id && font.texture.id != 0) {
            UnloadFont(font);
        }
    }
};

int main() {
    InitWindow(WINDOW_SIZE, WINDOW_SIZE + 150, "Tiny Flood");

    // Load and set window icon
    Image icon = LoadImage("assets/icon.png");  // Use PNG instead of ICO for better compatibility
    SetWindowIcon(icon);
    UnloadImage(icon);
    SetTargetFPS(60);

    FloodGame game;
    game.LoadResources();

    while (!WindowShouldClose()) {
        game.Update();
        game.Draw();
    }

    game.Cleanup();
    CloseWindow();
    return 0;
}
