#pragma once

struct Vector2D
{
    float x = 0.0f;
    float y = 0.0f;

    Vector2D() = default;

    Vector2D(float x, float y)
        : x(x), y(y) {}
};