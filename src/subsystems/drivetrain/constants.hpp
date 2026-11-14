#pragma once

#include "lemlib/api.hpp"

namespace drivetrain_constants {

constexpr std::int8_t LEFT_PORT_1 = 1;
constexpr std::int8_t LEFT_PORT_2 = 2;
constexpr std::int8_t RIGHT_PORT_1 = -11;
constexpr std::int8_t RIGHT_PORT_2 = -12;
constexpr pros::MotorGears GEARSET = pros::MotorGears::blue;

constexpr float TRACK_WIDTH = 10;
constexpr float WHEEL_DIAMETER = lemlib::Omniwheel::OLD_275;
constexpr float WHEEL_RPM = 450;
constexpr float HORIZONTAL_DRIFT = 2;

constexpr std::uint8_t IMU_PORT = 13;
constexpr std::int8_t HORIZONTAL_ROTATION_PORT = 14;
constexpr float HORIZONTAL_WHEEL_DIAMETER = lemlib::Omniwheel::NEW_2;
constexpr float HORIZONTAL_WHEEL_OFFSET = -3;

namespace lateral {
constexpr float KP = 10;
constexpr float KI = 0;
constexpr float KD = 3;
constexpr float WINDUP_RANGE = 3;
constexpr float SMALL_ERROR = 1;
constexpr float SMALL_ERROR_TIMEOUT = 100;
constexpr float LARGE_ERROR = 3;
constexpr float LARGE_ERROR_TIMEOUT = 500;
constexpr float SLEW = 20;
}

namespace angular {
constexpr float KP = 2;
constexpr float KI = 0;
constexpr float KD = 10;
constexpr float WINDUP_RANGE = 3;
constexpr float SMALL_ERROR = 1;
constexpr float SMALL_ERROR_TIMEOUT = 100;
constexpr float LARGE_ERROR = 3;
constexpr float LARGE_ERROR_TIMEOUT = 500;
constexpr float SLEW = 0;
}

namespace throttle_curve {
constexpr float DEADBAND = 3;
constexpr float MIN_OUTPUT = 10;
constexpr float EXPO = 1.019;
}

namespace steer_curve {
constexpr float DEADBAND = 3;
constexpr float MIN_OUTPUT = 10;
constexpr float EXPO = 1.019;
}

}
