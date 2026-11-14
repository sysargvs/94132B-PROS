#include "main.h"
#include "pros_pose/pros_pose.hpp"

#include "subsystems/claw/claw.hpp"
#include "subsystems/drivetrain/drivetrain.hpp"
#include "subsystems/elevator/elevator.hpp"

pros::Controller controller(pros::E_CONTROLLER_MASTER);
Drivetrain drivetrain;
Elevator elevator;
Claw claw;


pros::AIVision camera(7);
ProsPose pose_estimator;

void initialize() {
  pros::lcd::initialize();
  drivetrain.chassis.calibrate();

  camera.reset();
  camera.enable_detection_types(pros::AivisionModeType::tags);
  camera.set_tag_family(pros::AivisionTagFamily::tag_21H7);

  pros::Task screenTask([] {
    while (true) {
      lemlib::Pose pose = drivetrain.chassis.getPose();
      pros::lcd::print(0, "X: %.2f", pose.x);
      pros::lcd::print(1, "Y: %.2f", pose.y);
      pros::lcd::print(2, "Theta: %.2f", pose.theta);
      pros::lcd::print(3, "Height: %.2f", elevator.getPosition());
      pros::delay(50);
    }
  });
}

void disabled() {}

void competition_initialize() {}

void autonomous() {
  claw.setMotion(Claw::Motion::INTAKE);
  elevator.setHeight(530);
  drivetrain.chassis.setPose(0, 0, 0);
  drivetrain.chassis.moveToPose(0, 15, 0, 4000);
  // drivetrain.chassis.waitUntilDone();
  bool lockedGoal = false;
  while(!lockedGoal) {
    auto objects = camera.get_all_objects();
    for (auto &object : objects) {
      if (pros::AIVision::is_type(object, pros::AivisionDetectType::tag) && !lockedGoal) {
        // printf("tag\n");
        // printf("id %d\n", object.id);
        // printf("%d %d %d %d %d %d %d %d\n", object.object.tag.x0, object.object.tag.y0, object.object.tag.x1, object.object.tag.y1, object.object.tag.x2, object.object.tag.y2, object.object.tag.x3, object.object.tag.y3);

        double corners[4][2] = {
          {object.object.tag.x0, object.object.tag.y0},
          {object.object.tag.x1, object.object.tag.y1},
          {object.object.tag.x2, object.object.tag.y2},
          {object.object.tag.x3, object.object.tag.y3}
        };
        printf("%d\n", object.id);
        // lockedGoal = true;

        ProsPoseResult result = pose_estimator.estimate(corners);
        if(result.is_valid){
          lockedGoal = true;
        //   printf("result: %f, %f, %f\nangles: %f, %f, %f\n", result.x*39.370079, result.y*39.370079, result.z*39.370079);

          drivetrain.chassis.cancelAllMotions();
          drivetrain.chassis.setPose(0, 0, 0);
          if ((result.roll == 180.0 && result.yaw == -180.0) || (result.roll == -180.0 && result.yaw == 180.0)) {
            lockedGoal = false;
          } else {
            drivetrain.chassis.moveToPose(result.x*39.370079, result.z*39.370079+30, 0, 4000);
            drivetrain.chassis.waitUntilDone();
            pros::lcd::print(4, "Angles: %.2f %.2f %.2f", result.roll, result.pitch, result.yaw);
          }
        }
      }
    }
  }
  elevator.setHeight(330);
  claw.setMotion(Claw::Motion::OUTTAKE);
  pros::delay(1500);
  elevator.setHeight(530);
  pros::delay(500);
  drivetrain.chassis.curvature(-127, 0);
  pros::delay(500);
  drivetrain.chassis.curvature(0, -127);
  pros::delay(250);
  drivetrain.chassis.curvature(0, 0);
  claw.stop();
  
}

void opcontrol() {
  bool toggled = false;
  while (true) {
    if (controller.get_digital_new_press(pros::E_CONTROLLER_DIGITAL_LEFT)) {
      toggled = !toggled;
    }
    drivetrain.drive(controller, toggled);

    if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_L1)) {
      elevator.setMotion(Elevator::Motion::UP);
    } else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_L2)) {
      elevator.setMotion(Elevator::Motion::DOWN);
    } else {
      elevator.holdCurrentHeight();
    }

    if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1)) {
      claw.setMotion(Claw::Motion::INTAKE);
    } else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_R2)) {
      claw.setMotion(Claw::Motion::OUTTAKE);
    } else {
      claw.stop();
    }
    pros::delay(10);
  }
}
