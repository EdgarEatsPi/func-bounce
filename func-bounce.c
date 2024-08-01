#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define EPSILON 0.01

typedef struct Vector2 {
    float x;
    float y;
} Vector2;

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

void DrawCircle(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius)
{
const int32_t diameter = (radius * 2);

int32_t x = (radius - 1);
int32_t y = 0;
int32_t tx = 1;
int32_t ty = 1;
int32_t error = (tx - diameter);

while (x >= y)
{
// Each of the following renders an octant of the circle
SDL_RenderDrawLine(renderer, centreX, centreY, centreX + x, centreY - y);
SDL_RenderDrawLine(renderer, centreX, centreY, centreX + x, centreY + y);
SDL_RenderDrawLine(renderer, centreX, centreY, centreX - x, centreY - y);
SDL_RenderDrawLine(renderer, centreX, centreY, centreX - x, centreY + y);
SDL_RenderDrawLine(renderer, centreX, centreY, centreX + y, centreY - x);
SDL_RenderDrawLine(renderer, centreX, centreY, centreX + y, centreY + x);
SDL_RenderDrawLine(renderer, centreX, centreY, centreX - y, centreY - x);
SDL_RenderDrawLine(renderer, centreX, centreY, centreX - y, centreY + x);

  if (error <= 0)
  {
  	++y;
  	error += ty;
  	ty += 2;
  }

  if (error > 0)
  {
  	--x;
  	tx += 2;
  	error += (tx - diameter);
  }
}
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
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(scrWidth, scrHeight, 0, &window, &renderer);

    const float g = 100;
    const float elasticity = 0.9;

    bool clip = false;
    Vector2 ball = { 0, 200 };
    Vector2 nextBall = ball;
    Vector2 ballVel = { 0, 0 };

    unsigned int start = SDL_GetTicks();
    bool stop = false;

    while (!stop) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    stop = true;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    Vector2 mousePos = {event.button.x, event.button.y};
                    ball.x = mousePos.x - scrWidth/2;
                    ball.y = scrHeight/2 - mousePos.y;
                    if (ball.y < f(ball.x)) {
                        ball.y = f(ball.x);
                    }
                    nextBall = ball;
                    ballVel.x = 0;
                    ballVel.y = 0;
                default:
                    break;
            }
        }

        unsigned int end = SDL_GetTicks();
        float dt = ((float) start - end)/1000;
        start = SDL_GetTicks();
        ballVel.y -= g*dt;
        nextBall.y += ballVel.y*dt;
        nextBall.x += ballVel.x*dt;

        // Point-On-Graph
        float pog = f(ball.x);
        //if (((ball.y - f(ball.x)) * (nextBall.y - f(nextBall.x))) < 0 && !clip) {
        if (ball.y < pog && !clip) {
            Vector2 clipPoint = findIntersection(&ball, &nextBall);

            float clipYTime = ballVel.y != 0 ? (nextBall.y - clipPoint.y) / ballVel.y : 0;
            float clipXTime = ballVel.x != 0 ? (nextBall.x - clipPoint.x) / ballVel.x : 0;

            nextBall.y -= (ballVel.y + 1) * (clipYTime);
            nextBall.x -= (ballVel.x + 1) * (clipXTime);
            ballVel.y += g*clipYTime;
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
            }

            float dotProd = ballVel.x * normal.x + ballVel.y * normal.y;
            ballVel.x -= 2 * normal.x * dotProd * elasticity;
            ballVel.y -= 2 * normal.y * dotProd * elasticity;
            clip = true;
        } else {
            clip = false;
        }

        ball.x = nextBall.x;
        ball.y = nextBall.y;

        //Start Drawing
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            Vector2 ballScr = graphToScreenV(&ball, scrWidth, scrHeight);
            // Define circle parameters
            int radius = 5; // radius of the circle

            // Fill the circle with the specified color
            SDL_SetRenderDrawColor(renderer, 190, 33, 55, 255);
            DrawCircle(renderer, ballScr.x, ballScr.y, radius);

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawLine(renderer, scrWidth/2, scrHeight, scrWidth/2, 0);
            SDL_RenderDrawLine(renderer, scrWidth, scrHeight/2, 0, scrHeight/2);


            for (int x = -scrWidth/2; x <= scrWidth/2; x++) {
                float y = f(x);
                Vector2 scrCoords = graphToScreen(x, y, scrWidth, scrHeight);
                if (scrCoords.y < 0 || scrCoords.y > scrHeight) {
                    continue;
                }
                Vector2 scrCoordsNext = graphToScreen(x+1, f(x+1), scrWidth, scrHeight);
                SDL_RenderDrawLine(renderer, scrCoords.x, scrCoords.y, scrCoordsNext.x, scrCoordsNext.y);
            }
            SDL_RenderPresent(renderer);
        // End Drawing
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
