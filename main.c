#include "raylib.h"
#include "raymath.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450

typedef struct Paddle {
    Rectangle rect;
    int speed;
} Paddle;

typedef struct Ball {
    Vector2 position;
    Vector2 speed;
    float radius;
} Ball;

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "2D Ping Pong - Raylib");
    SetTargetFPS(60);

    // Initialize paddles
    Paddle leftPaddle = {{50, SCREEN_HEIGHT / 2 - 40, 10, 80}, 5};
    Paddle rightPaddle = {{SCREEN_WIDTH - 60, SCREEN_HEIGHT / 2 - 40, 10, 80}, 5};

    // Initialize ball
    Ball ball = {{SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f}, {4, 4}, 8};

    int leftScore = 0;
    int rightScore = 0;

    int leftFouls = 0;
    int rightFouls = 0;
    const int foulLimit = 11;
    bool gameOver = false;

    while (!WindowShouldClose()) {
        if (!gameOver) {
            // Paddle movement
            if (IsKeyDown(KEY_W)) leftPaddle.rect.y -= leftPaddle.speed;
            if (IsKeyDown(KEY_S)) leftPaddle.rect.y += leftPaddle.speed;
            if (IsKeyDown(KEY_UP)) rightPaddle.rect.y -= rightPaddle.speed;
            if (IsKeyDown(KEY_DOWN)) rightPaddle.rect.y += rightPaddle.speed;

            // Prevent paddles from going out of bounds
            leftPaddle.rect.y = Clamp(leftPaddle.rect.y, 0, SCREEN_HEIGHT - leftPaddle.rect.height);
            rightPaddle.rect.y = Clamp(rightPaddle.rect.y, 0, SCREEN_HEIGHT - rightPaddle.rect.height);

            // Ball movement
            ball.position.x += ball.speed.x;
            ball.position.y += ball.speed.y;

            // Ball collision with top/bottom walls
            if (ball.position.y <= ball.radius || ball.position.y >= SCREEN_HEIGHT - ball.radius) {
                ball.speed.y *= -1;
            }

            // Ball collision with paddles
            if (CheckCollisionCircleRec(ball.position, ball.radius, leftPaddle.rect)) {
                ball.speed.x *= -1;
                ball.position.x = leftPaddle.rect.x + leftPaddle.rect.width + ball.radius;
            }
            if (CheckCollisionCircleRec(ball.position, ball.radius, rightPaddle.rect)) {
                ball.speed.x *= -1;
                ball.position.x = rightPaddle.rect.x - ball.radius;
            }

            // Scoring and fouls
            if (ball.position.x < 0) {
                rightScore++;
                leftFouls++;
                ball.position = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
                ball.speed = (Vector2){4, 4};
            } else if (ball.position.x > SCREEN_WIDTH) {
                leftScore++;
                rightFouls++;
                ball.position = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
                ball.speed = (Vector2){-4, 4};
            }

            // Check game over
            if (leftFouls >= foulLimit || rightFouls >= foulLimit) {
                gameOver = true;
            }
        }

        // Draw everything
        BeginDrawing();
        ClearBackground(CLITERAL(Color){154, 205, 50});

        // Center line and circle
        DrawLine(SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT, WHITE);
        DrawCircle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 80, DARKGREEN);

        // Draw paddles and ball
        DrawRectangleRec(leftPaddle.rect, WHITE);
        DrawRectangleRec(rightPaddle.rect, WHITE);
        DrawCircleV(ball.position, ball.radius, WHITE);

        // Draw scores
        DrawText(TextFormat("%d", leftScore), SCREEN_WIDTH / 4, 20, 40, WHITE);
        DrawText(TextFormat("%d", rightScore), 3 * SCREEN_WIDTH / 4, 20, 40, WHITE);

        // Draw fouls
        DrawText(TextFormat("Fouls: %d", leftFouls), 10, SCREEN_HEIGHT - 30, 20, RED);
        DrawText(TextFormat("Fouls: %d", rightFouls), SCREEN_WIDTH - 130, SCREEN_HEIGHT - 30, 20, RED);

        // Game over message
        if (gameOver) {
            const char *winner = (leftFouls >= foulLimit) ? "RIGHT PLAYER WINS!" : "LEFT PLAYER WINS!";
            DrawText(winner, SCREEN_WIDTH / 2 - MeasureText(winner, 40) / 2, SCREEN_HEIGHT / 2 - 20, 40, YELLOW);
            DrawText("Press [ESC] to exit", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 30, 20, WHITE);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
