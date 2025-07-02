#pragma once


struct Vec2 {
    float x;
    float y;

    Vec2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
};

struct IVec2 {
    int x;
    int y;

    IVec2(int x = 0, int y = 0) : x(x), y(y) {}
};
