#pragma once

#include "lemlib/api.hpp"

class Drivetrain {
 public:
  Drivetrain();

  void drive(pros::Controller& controller, bool toggled);

 private:
  pros::MotorGroup leftMotors;
  pros::MotorGroup rightMotors;
  pros::Imu imu;
  pros::Rotation horizontalRotation;
  pros::Rotation lateralRotation;
  lemlib::TrackingWheel horizontalWheel;
  lemlib::TrackingWheel lateralWheel;
  lemlib::ExpoDriveCurve throttleCurve;
  lemlib::ExpoDriveCurve steerCurve;

 public:
  lemlib::Chassis chassis;
};
