#include "drivetrain.hpp"

#include "constants.hpp"

using namespace drivetrain_constants;

Drivetrain::Drivetrain()
    : leftMotors({LEFT_PORT_1, LEFT_PORT_2}, GEARSET),
      rightMotors({RIGHT_PORT_1, RIGHT_PORT_2}, GEARSET),
      imu(IMU_PORT),
      horizontalRotation(HORIZONTAL_ROTATION_PORT),
      lateralRotation(HORIZONTAL_ROTATION_PORT),
      horizontalWheel(&horizontalRotation, HORIZONTAL_WHEEL_DIAMETER, HORIZONTAL_WHEEL_OFFSET),
      lateralWheel(&lateralRotation, HORIZONTAL_WHEEL_DIAMETER, HORIZONTAL_WHEEL_OFFSET),
      throttleCurve(throttle_curve::DEADBAND, throttle_curve::MIN_OUTPUT, throttle_curve::EXPO),
      steerCurve(steer_curve::DEADBAND, steer_curve::MIN_OUTPUT, steer_curve::EXPO),
      chassis(lemlib::Drivetrain(&leftMotors, &rightMotors, TRACK_WIDTH, WHEEL_DIAMETER, WHEEL_RPM,
                                 HORIZONTAL_DRIFT),
              lemlib::ControllerSettings(lateral::KP, lateral::KI, lateral::KD, lateral::WINDUP_RANGE,
                                         lateral::SMALL_ERROR, lateral::SMALL_ERROR_TIMEOUT,
                                         lateral::LARGE_ERROR, lateral::LARGE_ERROR_TIMEOUT, lateral::SLEW),
              lemlib::ControllerSettings(angular::KP, angular::KI, angular::KD, angular::WINDUP_RANGE,
                                         angular::SMALL_ERROR, angular::SMALL_ERROR_TIMEOUT,
                                         angular::LARGE_ERROR, angular::LARGE_ERROR_TIMEOUT, angular::SLEW),
              lemlib::OdomSensors(nullptr, nullptr, &horizontalWheel, nullptr, &imu), &throttleCurve,
              &steerCurve) {}

void Drivetrain::drive(pros::Controller& controller, bool toggled) {
  int throttle = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
  int steer = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
  chassis.curvature((toggled ? -1 : 1) *throttle, steer);
}
