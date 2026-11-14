#pragma once

#include "api.h"

namespace claw_constants {

constexpr std::int8_t PORT = 9;
constexpr pros::MotorGears GEARSET = pros::MotorGears::green;

constexpr double OPEN_POSITION = 0;
constexpr double CLOSED_POSITION = -50;
constexpr int MOVE_VELOCITY = 50;
constexpr int CURRENT_LIMIT = 1000;

}
