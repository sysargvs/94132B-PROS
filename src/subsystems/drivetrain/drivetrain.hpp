#pragma once

#include "lemlib/api.hpp"

class Drivetrain {
 public:
  Drivetrain();

  void drive(pros::Controller& controller);

 private:
  pros::MotorGroup leftMotors;
  pros::MotorGroup rightMotors;
  pros::Imu imu;
  pros::Rotation horizontalRotation;
  lemlib::TrackingWheel horizontalWheel;
  lemlib::ExpoDriveCurve throttleCurve;
  lemlib::ExpoDriveCurve steerCurve;

 public:
  lemlib::Chassis chassis;
};
