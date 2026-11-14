#include "main.h"
#include "lemlib/api.hpp" // IWYU pragma: keep
#include "pros_pose/pros_pose.hpp"

pros::Controller controller(pros::E_CONTROLLER_MASTER);
pros::MotorGroup left_mg({1, 2}, pros::MotorGearset::blue);    // Creates a motor group with forwards port and reversed port 
pros::MotorGroup right_mg({-11, -12}, pros::MotorGearset::blue);  // Creates a motor group with forwards port and reversed port

pros::MotorGroup cascade({10, -20}, pros::MotorGearset::green); // Cascade motors are opposite

pros::Motor claw(9);

// drivetrain settings
lemlib::Drivetrain drivetrain(&left_mg, // left motor group
                              &right_mg, // right motor group
                              10, // 10 inch track width
                              lemlib::Omniwheel::OLD_275, // using new 4" omnis
                              450, // drivetrain rpm is 360
                              2 // horizontal drift is 2 (for now)
);

// create an imu on port 13
pros::Imu imu(13);

pros::Rotation horiz_rot(14);
lemlib::TrackingWheel horizontal(&horiz_rot, 2.0, -3);


pros::AIVision camera(7);
ProsPose pose_estimator;


// lateral PID controller
lemlib::ControllerSettings lateral_controller(10, // proportional gain (kP)
                                              0, // integral gain (kI)
                                              3, // derivative gain (kD)
                                              3, // anti windup
                                              1, // small error range, in inches
                                              100, // small error range timeout, in milliseconds
                                              3, // large error range, in inches
                                              500, // large error range timeout, in milliseconds
                                              20 // maximum acceleration (slew)
);

// angular PID controller
lemlib::ControllerSettings angular_controller(2, // proportional gain (kP)
                                              0, // integral gain (kI)
                                              10, // derivative gain (kD)
                                              3, // anti windup
                                              1, // small error range, in degrees
                                              100, // small error range timeout, in milliseconds
                                              3, // large error range, in degrees
                                              500, // large error range timeout, in milliseconds
                                              0 // maximum acceleration (slew)
);

// sensors for odometry
lemlib::OdomSensors sensors(nullptr, //&vertical, // vertical tracking wheel
                            nullptr, // vertical tracking wheel 2, set to nullptr as we don't have a second one
                            nullptr, // horizontal tracking wheel
                            nullptr, // horizontal tracking wheel 2, set to nullptr as we don't have a second one
                            nullptr//&imu // inertial sensor
);

// input curve for throttle input during driver control
lemlib::ExpoDriveCurve throttleCurve(3, // joystick deadband out of 127
                                     10, // minimum output where drivetrain will move out of 127
                                     1.019 // expo curve gain
);

// input curve for steer input during driver control
lemlib::ExpoDriveCurve steerCurve(3, // joystick deadband out of 127
                                  10, // minimum output where drivetrain will move out of 127
                                  1.019 // expo curve gain
);

// create the chassis
lemlib::Chassis chassis(drivetrain, lateral_controller, angular_controller, sensors, &throttleCurve, &steerCurve);

/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {
	static bool pressed = false;
	pressed = !pressed;
	if (pressed) {
		pros::lcd::set_text(2, "I was pressed!");
	} else {
		pros::lcd::clear_line(2);
	} 
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
  pros::lcd::initialize(); // initialize brain screen
  chassis.calibrate(); // calibrate sensors

  camera.reset();
  camera.enable_detection_types(pros::AivisionModeType::tags);
  camera.set_tag_family(pros::AivisionTagFamily::tag_21H7);

  // print position to brain screen
  pros::Task screen_task([&]() {
    while (true) {
      // print robot location to the brain screen
      pros::lcd::print(0, "X: %f", chassis.getPose().x); // x
      pros::lcd::print(1, "Y: %f", chassis.getPose().y); // y
      pros::lcd::print(2, "Theta: %f", chassis.getPose().theta); // heading
      pros::lcd::print(3, "horiz: %f", horizontal.getDistanceTraveled());
      pros::lcd::print(4, "hi orion : %f", horizontal.getDistanceTraveled());

      // delay to save resources
      pros::delay(20);
    }
  });
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
  // chassis.setPose(0, 0, 0);
  // while(cascade.get_torque(0) < 0.4){
  //   cascade.move(100);
  // }
  cascade.move_relative(500, 120);
  chassis.moveToPose(0, 30, 0, 4000);
  printf("hiHIHIHIHI\n");

  // bool locked = false;
  // while(!locked){
  // auto objects = camera.get_all_objects();
  // for (auto &object : objects) {
  //     if (pros::AIVision::is_type(object, pros::AivisionDetectType::tag)) {
  //         // printf("tag\n");
  //         // printf("id %d\n", object.id);
  //         // printf("%d %d %d %d %d %d %d %d\n", object.object.tag.x0, object.object.tag.y0, object.object.tag.x1, object.object.tag.y1, object.object.tag.x2, object.object.tag.y2, object.object.tag.x3, object.object.tag.y3);
  //         double corners[4][2] = {
  //           {object.object.tag.x0, object.object.tag.y0},
  //           {object.object.tag.x1, object.object.tag.y1},
  //           {object.object.tag.x2, object.object.tag.y2},
  //           {object.object.tag.x3, object.object.tag.y3}
  //         };

  //         ProsPoseResult result = pose_estimator.estimate(corners);

  //         if (result.is_valid && !locked) { //result.is_valid
  //           locked = true;
  //           printf("result: %f, %f, %f\nangles: %f, %f, %f\n", result.x*39.370079, result.y*39.370079, result.z*39.370079);
            
  //           chassis.setPose(0, 0, 0);
  //           chassis.moveToPose(result.x*39.370079, result.z*39.370079, 0, 4000);
  //           // chassis.moveToPose(0, 10, 0, 4000);
  //           // result.x, result.y, result.z in meters
  //           // result.yaw, result.pitch, result.roll in degrees
  //           printf("i am here");
  //         }
  //     }
  // }
      
  chassis.waitUntil(30);
  claw.move_relative(-50, 50);
  // }

  // chassis.turnToHeading(90, 1000, {.direction = AngularDirection::CW_CLOCKWISE, .minSpeed = 100});
  
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
  // autonomous();
  
	while (true) {
		pros::lcd::print(0, "%d %d %d", (pros::lcd::read_buttons() & LCD_BTN_LEFT) >> 2,
		                 (pros::lcd::read_buttons() & LCD_BTN_CENTER) >> 1,
		                 (pros::lcd::read_buttons() & LCD_BTN_RIGHT) >> 0);  // Prints status of the emulated screen LCDs

    // get joystick positions
    int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
    int rightX = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
    // move the chassis with curvature drive
    chassis.curvature(rightX, leftY);

    int cascadeUpDown = controller.get_digital(pros::E_CONTROLLER_DIGITAL_UP) - controller.get_digital(pros::E_CONTROLLER_DIGITAL_DOWN);
    cascade.move(cascadeUpDown*50);
    if (cascadeUpDown == 0) {
      cascade.brake();
    }

    int clawAB = controller.get_digital(pros::E_CONTROLLER_DIGITAL_A) - controller.get_digital(pros::E_CONTROLLER_DIGITAL_B);
    claw.move(clawAB*100);

    // delay to save resources
    pros::delay(10);
	}
}