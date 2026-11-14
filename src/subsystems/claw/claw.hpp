#pragma once

#include "api.h"

class Claw {
 public:
  enum class Motion { INTAKE, OUTTAKE };

  Claw();

  void setMotion(Motion motion);
  void stop();

 private:
  pros::Motor motor;
};
