#pragma once

#include "api.h"

namespace elevator_constants {

constexpr std::int8_t LEFT_PORT = 10;
constexpr std::int8_t RIGHT_PORT = -20;
constexpr pros::MotorGears GEARSET = pros::MotorGears::red;

constexpr int UP_VOLTAGE = 12000;
constexpr int DOWN_VOLTAGE = 12000;
constexpr int MOVE_VELOCITY = 120;
constexpr double HEIGHT_TOLERANCE = 10;

}
