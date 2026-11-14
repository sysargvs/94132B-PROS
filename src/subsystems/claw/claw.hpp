#pragma once

#include "api.h"

class Claw {
 public:
  Claw();

  void setOpen(bool open);
  void toggle();

 private:
  pros::Motor motor;
  bool isOpen = true;
};
