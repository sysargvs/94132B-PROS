#include "main.h"

#include "subsystems/claw/claw.hpp"
#include "subsystems/drivetrain/drivetrain.hpp"
#include "subsystems/elevator/elevator.hpp"

pros::Controller controller(pros::E_CONTROLLER_MASTER);
Drivetrain drivetrain;
Elevator elevator;
Claw claw;


void initialize() {
  pros::lcd::initialize();
  drivetrain.chassis.calibrate();

  pros::Task screenTask([] {
    while (true) {
      lemlib::Pose pose = drivetrain.chassis.getPose();
      pros::lcd::print(0, "X: %.2f", pose.x);
      pros::lcd::print(1, "Y: %.2f", pose.y);
      pros::lcd::print(2, "Theta: %.2f", pose.theta);
      pros::delay(50);
    }
  });
}

void disabled() {}

void competition_initialize() {}

void autonomous() {
  drivetrain.chassis.setPose(0, 0, 0);
  drivetrain.chassis.moveToPose(0, 20, 0, 4000);
}

void opcontrol() {
  while (true) {
    drivetrain.drive(controller);

    if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_L1)) {
      elevator.setMotion(Elevator::Motion::UP);
    } else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_L2)) {
      elevator.setMotion(Elevator::Motion::DOWN);
    } else {
      elevator.holdCurrentHeight();
    }

    if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_A)) {
      claw.setMotion(Claw::Motion::INTAKE);
    } else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_B)) {
      claw.setMotion(Claw::Motion::OUTTAKE);
    } else {
      claw.stop();
    }

    pros::delay(10);
  }
}
