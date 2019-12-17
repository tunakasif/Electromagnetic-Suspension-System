// PINS
//  Motor A
int const ENA = 9;
int const IN1 = 8;
int const IN2 = 7;
//  Motor B
int const ENB = 3;
int const IN3 = 5;
int const IN4 = 4;

// CONSTANTS
int const MIN_SPEED = 27;   // Set to minimum PWM value that will make motors turn
int const ACCEL_DELAY = 50; // delay between steps when ramping motor speed up or down.
int const inc_dec_amount = 2;

// VARIABLES
char bluetooth;
char curr_direction_right;
char curr_direction_left;
int curr_speed_right;
int curr_speed_left;

//===============================================================================
//  Initialization
//===============================================================================
void setup()
{
  pinMode(ENA, OUTPUT); // set all the motor control pins to outputs
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Turn off motors - Initial state
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  // Default selections
  curr_direction_right = 'F'; // forward
  curr_direction_left = 'F';  // forward
  curr_speed_right = 0;       // stop
  curr_speed_left = 0;        // stop

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
    setDirAndSpeed('F', curr_direction_right, curr_speed_right);
    setDirAndSpeed('F', curr_direction_left, curr_speed_left);
    break;
  case 'B':
    // set preference using bluetooth input
    setDirAndSpeed('B', curr_direction_right, curr_speed_right);
    setDirAndSpeed('B', curr_direction_left, curr_speed_left);
    break;

  // forward-right & back-right respectively
  case 'I':
    setDirAndSpeed('F', curr_direction_right, curr_speed_right);
    break;
  case 'J':
    setDirAndSpeed('B', curr_direction_right, curr_speed_right);
    break;

  // forward-left & back-left respectively
  case 'G':
    setDirAndSpeed('F', curr_direction_left, curr_speed_left);
  case 'H':
    setDirAndSpeed('B', curr_direction_left, curr_speed_left);
    break;

  // stop
  case 'X':
  case 'W':
  case 'w':
    curr_direction_right = 'F'; // forward
    curr_speed_right = 0;       // stop
    curr_direction_left = 'F';  // forward
    curr_speed_left = 0;        // stop
    break;
  }

  // apply selected state
  Motor('A', curr_direction_right, curr_speed_right);
  Motor('B', curr_direction_left, curr_speed_left);
  Serial.println(curr_direction_right);
  Serial.println(curr_direction_left);
  Serial.println(curr_speed_right);
  Serial.println(curr_speed_left);
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
void Motor(char mot, char dir, int speed)
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
    analogWrite(ENA, newspeed);
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
    analogWrite(ENB, newspeed);
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
    analogWrite(ENA, newspeed);
    analogWrite(ENB, newspeed);
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
