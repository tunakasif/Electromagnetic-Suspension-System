// PINS
//  Motor A for Wheels
int const ENA_WHEEL = 10;
int const INA_WHEEL = 12;
//  Motor B for Wheels
int const ENB_WHEEL = 11;
int const INB_WHEEL = 13;

//  Motor A for Suspension
int const ENA_SUSP = 9;
int const IN1 = 8;
int const IN2 = 7;
//  Motor B for Suspension
int const ENB_SUSP = 3;
int const IN3 = 5;
int const IN4 = 4;

// CONSTANTS
int const MIN_SPEED = 27;   // Set to minimum PWM value that will make motors turn
int const ACCEL_DELAY = 50; // delay between steps when ramping motor speed up or down.
int const inc_dec_amount = 2;

// VARIABLES
char bluetooth;
bool suspension_mode;
// Suspension
char susp_curr_direction_right;
char susp_curr_direction_left;
int susp_curr_speed_right;
int susp_curr_speed_left;
// Wheels
char wheel_curr_direction_right;
char wheel_curr_direction_left;
int wheel_curr_speed_right;
int wheel_curr_speed_left;

//===============================================================================
//  Initialization
//===============================================================================
void setup()
{
  // Wheels
  pinMode(ENA_WHEEL, OUTPUT);
  pinMode(ENB_WHEEL, OUTPUT);
  pinMode(INA_WHEEL, OUTPUT);
  pinMode(INB_WHEEL, OUTPUT);

  // Suspension
  pinMode(ENA_SUSP, OUTPUT); // set all the motor control pins to outputs
  pinMode(ENB_SUSP, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Turn off motors - Initial state
  digitalWrite(INA_WHEEL, LOW);
  digitalWrite(INB_WHEEL, LOW);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  // Default selections for Suspension
  suspension_mode = false;
  susp_curr_direction_right = 'F'; // forward
  susp_curr_direction_left = 'F';  // forward
  susp_curr_speed_right = 0;       // stop
  susp_curr_speed_left = 0;        // stop

  // Default selections for Suspension
  wheel_curr_direction_right = 'F'; // forward
  wheel_curr_direction_left = 'F';  // forward
  wheel_curr_speed_right = 0;       // stop
  wheel_curr_speed_left = 0;        // stop

  Serial.begin(9600); // Set comm speed for serial monitor messages and bluetooth
}
//===============================================================================
//  Main
//===============================================================================
void loop()
{
  Serial.println(bluetooth);
  if (Serial.available())
  {
    bluetooth = Serial.read();
    Serial.println(bluetooth);
  }

  switch (bluetooth)
  {
  case 'F':
    // set preference using bluetooth input
    setDirAndSpeed('F', susp_curr_direction_right, susp_curr_speed_right);
    setDirAndSpeed('F', susp_curr_direction_left, susp_curr_speed_left);
    break;
  case 'B':
    // set preference using bluetooth input
    setDirAndSpeed('B', susp_curr_direction_right, susp_curr_speed_right);
    setDirAndSpeed('B', susp_curr_direction_left, susp_curr_speed_left);
    break;

  // forward-right & back-right respectively
  case 'I':
    setDirAndSpeed('F', susp_curr_direction_right, susp_curr_speed_right);
    break;
  case 'J':
    setDirAndSpeed('B', susp_curr_direction_right, susp_curr_speed_right);
    break;

  // forward-left & back-left respectively
  case 'G':
    setDirAndSpeed('F', susp_curr_direction_left, susp_curr_speed_left);
  case 'H':
    setDirAndSpeed('B', susp_curr_direction_left, susp_curr_speed_left);
    break;

  // stop
  case 'X':
  case 'W':
  case 'w':
    susp_curr_direction_right = 'F'; // forward
    susp_curr_speed_right = 0;       // stop
    susp_curr_direction_left = 'F';  // forward
    susp_curr_speed_left = 0;        // stop
    break;
  }

  // apply selected state
  // DriverMotor('A', susp_curr_direction_right, susp_curr_speed_right);
  // DriverMotor('B', susp_curr_direction_left, susp_curr_speed_left);
  DriverMotor('A', 'F', 50);
  DriverMotor('B', 'F', 50);
  ShieldMotor('A', 'F', 50);
  ShieldMotor('B', 'F', 50);
  delay(10);
}
/*
 * Motor function does all the heavy lifting of controlling the motors
 * mot = motor to control either 'A' or 'B'.  'C' controls both motors.
 * dir = Direction either 'F'orward or 'R'everse
 * speed = Speed.  Takes in 1-100 percent and maps to 0-255 for PWM control.  
 * Mapping ignores speed values that are too low to make the motor turn.
 * In this case, anything below 27, but 0 still means 0 to stop the motors.
 */
void DriverMotor(char mot, char dir, int speed)
{
  // remap the speed from range 0-100 to 0-255
  int newspeed;
  if (speed == 0)
    newspeed = 0; // Don't remap zero, but remap everything else.
  else
    newspeed = map(speed, 1, 100, MIN_SPEED, 255);

  switch (mot)
  {
  case 'A': // Controlling Motor A
    if (dir == 'F')
    {
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
    }
    else if (dir == 'R')
    {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
    }
    analogWrite(ENA_SUSP, newspeed);
    break;

  case 'B': // Controlling Motor B
    if (dir == 'F')
    {
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
    }
    else if (dir == 'R')
    {
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
    }
    analogWrite(ENB_SUSP, newspeed);
    break;

  case 'C': // Controlling Both Motors
    if (dir == 'F')
    {
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
    }
    else if (dir == 'R')
    {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
    }
    analogWrite(ENA_SUSP, newspeed);
    analogWrite(ENB_SUSP, newspeed);
    break;
  }
}

void ShieldMotor(char mot, char dir, int speed)
{
  // remap the speed from range 0-100 to 0-255
  int newspeed;
  if (speed == 0)
    newspeed = 0; // Don't remap zero, but remap everything else.
  else
    newspeed = map(speed, 1, 100, MIN_SPEED, 255);

  switch (mot)
  {
  case 'A': // Controlling Motor A
    if (dir == 'F')
    {
      digitalWrite(INA_WHEEL, HIGH);
    }
    else if (dir == 'R')
    {
      digitalWrite(INB_WHEEL, LOW);
    }
    analogWrite(ENA_WHEEL, newspeed);
    break;

  case 'B': // Controlling Motor B
    if (dir == 'F')
    {
      digitalWrite(INB_WHEEL, HIGH);
    }
    else if (dir == 'R')
    {
      digitalWrite(INA_WHEEL, LOW);
    }
    analogWrite(ENB_WHEEL, newspeed);
    break;

  case 'C': // Controlling Both Motors
    if (dir == 'F')
    {
      digitalWrite(INA_WHEEL, HIGH);
      digitalWrite(INB_WHEEL, HIGH);
    }
    else if (dir == 'R')
    {
      digitalWrite(INA_WHEEL, LOW);
      digitalWrite(INB_WHEEL, LOW);
    }
    analogWrite(ENA_WHEEL, newspeed);
    analogWrite(ENB_WHEEL, newspeed);
    break;
  }
}

void setDirAndSpeed(char requested_dir, char &curr_direction, int &curr_speed)
{
  switch (requested_dir)
  {
  case 'F':
    if (curr_direction == 'F' && 100 - inc_dec_amount < curr_speed)
      curr_speed = 100;

    else if (curr_direction == 'F' && 0 <= curr_speed)
      curr_speed += inc_dec_amount;

    else if (curr_direction == 'R' && curr_speed <= MIN_SPEED + inc_dec_amount)
    {
      curr_direction = 'F'; // forward
      curr_speed = 0;
    }

    else
      curr_speed -= inc_dec_amount;
    break;

  case 'B':
    if (curr_direction == 'R' && 100 - inc_dec_amount < curr_speed)
      curr_speed = 100;

    else if (curr_direction == 'R' && 0 <= curr_speed)
      curr_speed += inc_dec_amount;

    else if (curr_direction == 'F' && curr_speed <= MIN_SPEED + inc_dec_amount)
    {
      curr_direction = 'R'; // forward
      curr_speed = 0;
    }

    else
      curr_speed -= inc_dec_amount;
    break;

  default:
    // preserve current state
    break;
  }
}
