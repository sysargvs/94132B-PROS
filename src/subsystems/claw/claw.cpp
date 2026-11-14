#include "claw.hpp"

#include "constants.hpp"

using namespace claw_constants;

Claw::Claw() : motor(PORT, GEARSET) {
  motor.set_brake_mode(pros::MotorBrake::hold);
  motor.set_current_limit(CURRENT_LIMIT);
}

void Claw::setOpen(bool open) {
  isOpen = open;
  motor.move_absolute(open ? OPEN_POSITION : CLOSED_POSITION, MOVE_VELOCITY);
}

void Claw::toggle() {
  setOpen(!isOpen);
}
