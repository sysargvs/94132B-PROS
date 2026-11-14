#include "elevator.hpp"

#include <cmath>

#include "constants.hpp"

using namespace elevator_constants;

Elevator::Elevator() : motors({LEFT_PORT, RIGHT_PORT}, GEARSET) {
  motors.set_brake_mode_all(pros::MotorBrake::hold);
}

void Elevator::setMotion(Motion motion) {
  if (motors.get_position() < 0 && motion == Motion::DOWN) {
    return;
  }
  motors.move_voltage(motion == Motion::UP ? UP_VOLTAGE : -DOWN_VOLTAGE);
}

void Elevator::setHeight(double degrees) {
  targetHeight = degrees;
  motors.move_absolute(degrees, MOVE_VELOCITY);
}

double Elevator::getPosition() {
  return motors.get_position();
}

void Elevator::holdCurrentHeight() {
  motors.move_velocity(0);
}

void Elevator::waitUntilAtHeight(std::uint32_t timeoutMs) {
  std::uint32_t start = pros::millis();
  while (std::abs(motors.get_position() - targetHeight) > HEIGHT_TOLERANCE &&
         pros::millis() - start < timeoutMs) {
    pros::delay(10);
  }
}
