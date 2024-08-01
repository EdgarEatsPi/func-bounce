#include "raylib.h"
#include <stdbool.h>
#include <math.h>

#define EPSILON 0.01

float f(float);

float line(float x, Vector2* a, Vector2* b) {
    if (a->x == b->x)
        return (a->y + b->y)/2;
    return (x - b->x) * ((b->y - a->y)/(b->x - a->x)) + b->y;
}

// Bisection method to find the intersection
Vector2 findIntersection(Vector2* start, Vector2* end) {
    if (fabs(start->x - end->x) < EPSILON)
        return (Vector2) {start->x, f(start->x)};

    float xa = start->x, xb = end->x, xm, ya, yb, ym;
    ya = f(xa) - start->y;
    yb = f(xb) - end->y;

    if (ya*yb > 0)
        return *start;

    while (fabs(xb - xa) > EPSILON) {
        xm = (xa + xb) / 2.0;
        ym = f(xm) - line(xm, start, end);

        if (fabs(ym) < EPSILON) {
            return (Vector2){xm, f(xm)};  // Found the intersection
        } else if (ya * ym < 0) {
            xb = xm;
            yb = ym;
        } else {
            xa = xm;
            ya = ym;
        }
    }

    float intersectionX = (xa + xb) / 2.0;
    float intersectionY = f(intersectionX);
    return (Vector2){intersectionX, intersectionY};  // Return the midpoint as the intersection
}

Vector2 graphToScreenV(Vector2 *graphCoords, int width, int height) {
    Vector2 screenCoords = { width/2 + graphCoords->x, height/2 - graphCoords->y };
    return screenCoords;
}

Vector2 graphToScreen(float x, float y, int width, int height) {
    Vector2 screenCoords = { width/2 + x, height/2 - y };
    return screenCoords;
}

float f(float x) {
    return x*x/80 - 80;
}

int main() {
    const int scrWidth = 800;
    const int scrHeight = 800;
    InitWindow(scrWidth, scrHeight, "func-bounce");

    const float g = 100;
    const float elasticity = 1;

    bool clip = false;
    Vector2 ball = { 0, 200 };
    Vector2 nextBall = ball;
    Vector2 ballVel = { 0, 0 };
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mousePos = GetMousePosition();
            ball.x = mousePos.x - scrWidth/2;
            ball.y = scrHeight/2 - mousePos.y;
            nextBall = ball;
            ballVel.x = 0;
            ballVel.y = 0;
        }
        float dt = GetFrameTime();
        ballVel.y -= g*dt;
        nextBall.y += ballVel.y*dt;
        nextBall.x += ballVel.x*dt;

        // Point-On-Graph
        float pog = f(ball.x);
        if (nextBall.y <= pog && !clip) {
            Vector2 clipPoint = findIntersection(&ball, &nextBall);

            float clipYTime = ballVel.y != 0 ? (nextBall.y - clipPoint.y) / ballVel.y : 0;
            float clipXTime = ballVel.x != 0 ? (nextBall.x - clipPoint.x) / ballVel.x : 0;

            nextBall.y -= (ballVel.y + 1) * clipYTime;
            nextBall.x -= (ballVel.x + 1) * clipXTime;
            if (nextBall.y < f(nextBall.x)) {
                nextBall.y = f(nextBall.x);
            }
            float m = (f(ball.x + 0.1) - f(ball.x))/0.1; 
            Vector2 normal;
            if (m == 0) {
                normal = (Vector2){0, 1};
            } else if (m != 0) {
                float mag = 1/sqrt(1 + 1/(m*m));
                normal = (Vector2){mag, -mag/m};
                //Vector2 normal = {mag, m*mag};
            }
            
            float dotProd = ballVel.x * normal.x + ballVel.y * normal.y;
            ballVel.x = (ballVel.x - 2 * normal.x * dotProd) * elasticity;
            ballVel.y = (ballVel.y - 2 * normal.y * dotProd) * elasticity;
            clip = true;
        } else {
            clip = false;
        }

        ball.x = nextBall.x;
        ball.y = nextBall.y;

        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawText("yay", 10, 10, 20, DARKGRAY);
            DrawLine(scrWidth/2, scrHeight, scrWidth/2, 0, BLACK);
            DrawLine(scrWidth, scrHeight/2, 0, scrHeight/2, BLACK);

            DrawCircleV(graphToScreenV(&ball, scrWidth, scrHeight), 5, MAROON);

            for (int x = -scrWidth/2; x <= scrWidth/2; x++) {
                float y = f(x); 
                Vector2 scrCoords = graphToScreen(x, y, scrWidth, scrHeight);
                if (scrCoords.y < 0 || scrCoords.y > scrHeight) {
                    continue;
                }
                Vector2 scrCoordsNext = graphToScreen(x+1, f(x+1), scrWidth, scrHeight);
                //DrawCircle(px, py, 10, BLACK);
                DrawLineV(scrCoords, scrCoordsNext, BLACK);
            }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
