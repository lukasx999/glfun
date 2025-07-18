#pragma once

#include "camera.hh"

static constexpr int WIDTH  = 1600;
static constexpr int HEIGHT = 900;

#define PRINT(x) std::println("{}: {}", #x, x)

struct State {
    Camera cam { { 0.0f, 0.0f, 3.0f } };
    float fov_deg = 45.0f;
    bool polygon_mode = false;
};
