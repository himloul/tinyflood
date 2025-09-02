#include "raylib.h"
#include <vector>
#include <random>
#include <string>

// Configuration
const int BOARD_SIZE = 12;
const int CELL_SIZE = 30;
const int PADDING = 40;
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

public:
    FloodGame() : rng(std::random_device{}()) {
        initBoard();
    }

    void LoadResources() {
        font = LoadFont("assets/fonts/monogram.ttf");
        if (font.texture.id == 0) font = GetFontDefault();
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
        DrawText("TINY FLOOD", GetScreenWidth() / 2 - 120, 100, 40, GAME_COLORS[1]);
        
        // Draw menu items with selection highlight
        Color startColor = (selectedMenuItem == 0) ? GAME_COLORS[1] : GAME_COLORS[0];
        Color instructionsColor = (selectedMenuItem == 1) ? GAME_COLORS[1] : GAME_COLORS[0];
        
        DrawText("Start", GetScreenWidth() / 2 - 30, 300, 20, startColor);
        DrawText("Instructions", GetScreenWidth() / 2 - 60, 350, 20, instructionsColor);
        
        // Draw selection indicator
        int indicatorY = selectedMenuItem == 0 ? 300 : 350;
        DrawText(">", GetScreenWidth() / 2 - 80, indicatorY, 20, GAME_COLORS[1]);
        
        // Draw controls hint
        DrawText("Use UP/DOWN arrows and ENTER to navigate", GetScreenWidth() / 2 - 180, 450, 14, GRAY);
    }

    void DrawInstructions() {
        DrawText("Instructions", PADDING, 100, 30, GAME_COLORS[1]);
        DrawText("Use LEFT/RIGHT arrows to select colors", PADDING, 150, 16, GAME_COLORS[0]);
        DrawText("Press ENTER to flood with selected color", PADDING, 180, 16, GAME_COLORS[0]);
        DrawText("Use UP/DOWN to switch between colors and restart", PADDING, 210, 16, GAME_COLORS[0]);
        DrawText("Fill entire board with one color in few moves", PADDING, 240, 16, GAME_COLORS[0]);
        DrawText("Press ESC to return to menu", PADDING, 270, 16, GAME_COLORS[0]);
        
        DrawText("Press ENTER to return to menu", GetScreenWidth() / 2 - 120, GetScreenHeight() - 50, 16, GAME_COLORS[1]);
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
        DrawText(moveText.c_str(), PADDING, PADDING * 0.2, 20, GAME_COLORS[0]);

        // Draw color buttons with selection highlight
        int buttonY = PADDING + BOARD_SIZE * CELL_SIZE + 10;
        for (int i = 0; i < NUM_COLORS; i++) {
            Rectangle button = {(float)(PADDING + i * 30), (float)buttonY, 25.0f, 25.0f};
            DrawRectangleRounded(button, 0.2f, 0, GAME_COLORS[i]);
            
            // Draw selection indicator for selected color
            if (!onRestartButton && i == selectedColorIndex) {
                DrawRectangleLinesEx(button, 3.0f, WHITE);
                DrawText("^", PADDING + i * 30 + 10, buttonY - 20, 16, WHITE);
            }
        }

        // Draw restart button with selection highlight
        Rectangle restartBtn = {(float)PADDING, (float)(buttonY + 35), 100.0f, 30.0f};
        Color restartColor = onRestartButton ? GAME_COLORS[1] : LIGHTGRAY;
        DrawRectangleRounded(restartBtn, 0.2f, 0, restartColor);
        DrawText("Restart", PADDING + 25, buttonY + 42, 16, onRestartButton ? WHITE : BLACK);
        
        if (onRestartButton) {
            DrawText(">", PADDING - 20, buttonY + 42, 16, GAME_COLORS[1]);
        }

        // Draw controls hint
        if (!gameOver && !win) {
            DrawText("LEFT/RIGHT: Select color", PADDING, buttonY + 75, 12, GRAY);
            DrawText("UP/DOWN: Switch focus", PADDING, buttonY + 90, 12, GRAY);
            DrawText("ENTER: Confirm", PADDING, buttonY + 105, 12, GRAY);
        }

        // Draw win/game over message
        if (win || gameOver) {
            DrawRectangle(0, GetScreenHeight() / 2 - 50, GetScreenWidth(), 100, 
                         win ? Color{0, 128, 0, 200} : Color{128, 0, 0, 200});
            const char* message = win ? "You Win!" : "Game Over!";
            int textWidth = MeasureText(message, 30);
            DrawText(message, GetScreenWidth() / 2 - textWidth / 2, GetScreenHeight() / 2 - 25, 30, WHITE);
            
            const char* restartHint = "Press ENTER to restart";
            int hintWidth = MeasureText(restartHint, 16);
            DrawText(restartHint, GetScreenWidth() / 2 - hintWidth / 2, GetScreenHeight() / 2 + 10, 16, WHITE);
        }
    }

    void Cleanup() {
        if (font.texture.id != GetFontDefault().texture.id) {
            UnloadFont(font);
        }
    }
};

int main() {
    InitWindow(WINDOW_SIZE, WINDOW_SIZE + 100, "Tiny Flood");
    
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
