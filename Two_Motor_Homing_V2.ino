// Define stepper motor connections and steps per revolution
#define x_dir_pin 2
#define x_step_pin 3
#define y_dir_pin 4
#define y_step_pin 5

const int COL0 = 0;
const int COL1 = 1;
const int COL2 = 2;
const int COL3 = 3;
const int COL4 = 4;

const int ROW1 = 1;
const int ROW2 = 2;
const int ROW3 = 3;

////////////////////
//     OPTIONS    //
////////////////////
const int COL = COL1;       // change to the column you want to shoot at
const int ROW = ROW1;       // change to the row you want to shoot at
const int STEP_DELAY = 100;  // change to adjust speed of the motor, higher is slower
/////////////////////

typedef struct Motor {
  int step_pin;
  int dir_pin;
  int steps;
  bool turned_clockwise;
};


void setup() {

  // declare pins as output
  pinMode(x_step_pin, OUTPUT);
  pinMode(x_dir_pin, OUTPUT);
  pinMode(y_step_pin, OUTPUT);
  pinMode(y_dir_pin, OUTPUT);

  Serial.begin(9600);
  delay(1000);        // delay for stability, without this the serial monitor prints garbage
}

void loop() {
  input_validation();

  // initialize motors
  Motor motor_x, motor_y;
  motor_x.step_pin = x_step_pin;
  motor_x.dir_pin = x_dir_pin;
  motor_y.step_pin = y_step_pin;
  motor_y.dir_pin = y_dir_pin;

  // figure out how many steps we need to move
  determine_x_motion(&motor_x);
  determine_y_motion(&motor_y);

  // now that we know how many steps we need to move, move that number of steps
  move_n_steps(&motor_x);
  delay(1000);
  move_n_steps(&motor_y);
  Serial.println(motor_y.steps);

  // delay to give the gun time to shoot
  delay(3000);

  // go back to the starting position
  reset_to_home_position(&motor_y);
  delay(2000);
  reset_to_home_position(&motor_x);

  // exit with no errors, this ensures the code runs just once 
  exit(0);
}

// Determine if the user input a valid column
void input_validation() {
  bool col_is_valid = (COL == COL1 || COL == COL2 || COL == COL3 || COL == COL4 || COL == COL0);
  bool row_is_valid = (ROW == ROW1 || ROW == ROW2 || ROW == ROW3);
  if (col_is_valid == false || row_is_valid == false) {
    Serial.println("Invalid input");
    delay(1000);
    exit(1);
  }
}
// Figure out how many steps we need to move and in what direction (clockwise or counterclockwise) based on user input
void determine_x_motion(Motor *motor_x) {
  if (COL == COL1) {
    motor_x->steps = 11;
    set_mode_counterclockwise(motor_x);
  }
  else if (COL == COL2) {
    motor_x->steps = 4;
    set_mode_counterclockwise(motor_x);
  }
  else if (COL == COL0) { 
    motor_x->steps = 0;
    set_mode_clockwise(motor_x);
  }
  else if (COL == COL3) {
    motor_x->steps = 4;
    set_mode_clockwise(motor_x);
  }
  else if (COL == COL4) {
    motor_x->steps = 11;
    set_mode_clockwise(motor_x);
  }
}

void determine_y_motion(Motor *motor_y) {
  if (ROW == ROW1) {
    motor_y->steps = 8;
    set_mode_counterclockwise(motor_y);
  }
  else if (ROW == ROW2) {
    motor_y->steps = 0;
    set_mode_clockwise(motor_y);
  }
  else if (ROW == ROW3) {
    motor_y->steps = 8;
    set_mode_clockwise(motor_y);
  }
}

// Move "n" number of steps
void move_n_steps(Motor *motor) {
  // Ensures pin is low on start up 
  digitalWrite(motor->step_pin, LOW);
 
  for (int i = 0; i < motor->steps; i++) {
    // These four lines result in 1 step
    digitalWrite(motor->step_pin, HIGH);
    delay(STEP_DELAY);
    digitalWrite(motor->step_pin, LOW);
    delay(STEP_DELAY);
  }
}

// Go back to the starting position
void reset_to_home_position(Motor *motor) {
  // if the motor just moved clockwise, set it to move counterclockwise so it will move back where it came from
  if (motor->turned_clockwise) {
    set_mode_counterclockwise(motor);
    move_n_steps(motor);
  }
  // else, the motor moved counterclockwise, so set the direction to clockwise and then move back
  else {
    set_mode_clockwise(motor);
    move_n_steps(motor);
  }
}

// Set the direction to clockwise
void set_mode_clockwise(Motor *motor) {
  digitalWrite(motor->dir_pin, HIGH);
  motor->turned_clockwise = true;
}

// Set the direction to counterclockwise
void set_mode_counterclockwise(Motor *motor) {
  digitalWrite(motor->dir_pin, LOW);
  motor->turned_clockwise = false;
}

