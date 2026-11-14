#pragma once

#include "api.h"

class Elevator {
 public:
  enum class Motion { UP, DOWN };

  Elevator();

  void setMotion(Motion motion);
  void setHeight(double degrees);
  void holdCurrentHeight();
  void waitUntilAtHeight(std::uint32_t timeoutMs);

 private:
  pros::MotorGroup motors;
  double targetHeight = 0;
};
