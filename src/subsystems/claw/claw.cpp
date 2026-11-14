#include "claw.hpp"

#include "constants.hpp"

using namespace claw_constants;

Claw::Claw() : motor(PORT, GEARSET) {}

void Claw::setMotion(Motion motion) {
  motor.move_voltage(motion == Motion::INTAKE ? INTAKE_VOLTAGE : -OUTTAKE_VOLTAGE);
}

void Claw::stop() {
  motor.brake();
}

pros::Motor Claw::getMotor() {
  return motor;
}