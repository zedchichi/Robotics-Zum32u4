#include <Wire.h>
#include <Zumo32U4.h>

// Tunable parameters
#define QTR_THRESHOLD     1000
#define REVERSE_SPEED     100
#define TURN_SPEED        80
#define FORWARD_SPEED     100
#define REVERSE_DURATION  100  // ms
#define TURN_DURATION     300  // ms

#define TOTAL_HOUSES 4

Zumo32U4LCD display;
Zumo32U4ButtonA buttonA;
Zumo32U4Buzzer buzzer;
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;

#define NUM_SENSORS 3
unsigned int lineSensorValues[NUM_SENSORS];

String path = "";
String returnPath = "";
bool deliveryMade = false;
bool returning = false;
int housecounter = 0;


void waitForButtonAndCountDown() 
{
  ledYellow(1);
  display.clear();
  display.print(F("Press A"));
  buttonA.waitForButton();
  ledYellow(0);
  display.clear();
}

void setup() 
{
  Serial.begin(9600);
  lineSensors.initThreeSensors();
  proxSensors.initFrontSensor();
  waitForButtonAndCountDown();

  maze.init();
}

void loop() 
{

  if (buttonA.isPressed())
  {
    motors.setSpeeds(0, 0);
    Serial.println(path);
    buttonA.waitForRelease();
    if(!deliveryMade)
    {
      waitForButtonAndCountDown();
    }
  }

  if(!deliveryMade)
  {
    explore();
  }
  else
  {
    followOptiPath();
  }
  
}

void explore()
{
  while (!deliveryMade && housecounter < TOTAL_HOUSES) {
    navigateMaze();

    // Check for a house
    if (houseDetected()) 
    {
      performDelivery();
      // After delivering, turn around to continue exploring
      turnAround();
    }
  }  

}

void followOptiPath()
{
  for (unsigned int i = 0; i < path.length(); i++) {
    char action = path.charAt(i);
    executeMovement(action);
  }
  // Reset state after following the path
  deliveryMade = false;
  housecounter = 0;
  path = "";
}

void executePath(char action)
{
  switch (action) 
  {
    case 'L':
        turnLeft();
        break;
    case 'R':
        turnRight();
        break;
    case 'F':
        moveForwardShort();
        break;
  }
}

void turnAround()
{
  unsigned long startTime = millis();
  unsigned long turnTime = TURN_DURATION * 80; 
  bool isTurning = true;

  while (isTurning) {
    unsigned long currentTime = millis();

    // Break the loop if the turn has taken the expected duration
    if (currentTime - startTime >= turnTime) {
      isTurning = false;
      break;
    }

    // Continuously read line sensor values during the turn
    lineSensors.read(lineSensorValues);
    
    // Adjust turning speed based on line sensor readings to stay on track
    if (lineSensorValues[0] > QTR_THRESHOLD) 
    {
      // If the left sensor detects the line, slightly decrease the left motor speed
      motors.setSpeeds(-TURN_SPEED / 2, TURN_SPEED);
    } 
    else if (lineSensorValues[NUM_SENSORS - 1] > QTR_THRESHOLD) 
    {
      // If the right sensor detects the line, slightly decrease the right motor speed
      motors.setSpeeds(-TURN_SPEED, TURN_SPEED / 2);
    } 
    else 
    {
      // If no sensors detect the line, proceed with the normal turn
      motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
      delay(TURN_DURATION * 80);
    }
    
    // Small delay to ensure sensor readings are processed smoothly
    delay(10);
  }
  
}

void navigateMaze() 
{
  lineSensors.read(lineSensorValues);

  if (lineSensorValues[0] > QTR_THRESHOLD)
  {
    getOut(TURN_SPEED, -TURN_SPEED, 'R');
  }
  else if (lineSensorValues[NUM_SENSORS - 1] > QTR_THRESHOLD)
  {
    getOut(-TURN_SPEED, TURN_SPEED, 'L');
  }
 else if (lineSensorValues[0] < QTR_THRESHOLD && lineSensorValues[NUM_SENSORS - 1] < QTR_THRESHOLD)
  {
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
  else
  {
    // Otherwise, go straight.
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
    path += 'F';
  }
}

// function to help robot navigate corners and dead ends
void getOut(int leftSpeed, int rightSpeed, char direction)
{
  const int incrementDuration = 100;  // Time for each incremental turn
  const int maxTurns = 10;  // no of incremnet turns to try
  int turnCount = 0;

  motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
  delay(REVERSE_DURATION);
  path += 'B';

  while(turnCount < maxTurns)
  {
    motors.setSpeeds(leftSpeed, rightSpeed);
    delay(incrementDuration);

    lineSensors.read(lineSensorValues);
    if (lineSensorValues[0] < QTR_THRESHOLD && lineSensorValues[NUM_SENSORS - 1] < QTR_THRESHOLD)
    {
      motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
      path += direction; //Track direction of turn
      break;

    }

    turnCount++;
  }

  if (turnCount >= maxTurns) {
    int turn45Duration = TURN_DURATION * 10;
    motors.setSpeeds(leftSpeed, rightSpeed);
    delay(turn45Duration); 

    path += direction; // Track direction of turn
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);


  }

}

// house detecting logic using the proximity sensors while naviagating the maze
bool houseDetected() {
  const uint8_t proximityThreshold = 6;
  const uint8_t closeproximityThreshold = 5;

  proxSensors.read();
  uint8_t leftValue = proxSensors.countsFrontWithLeftLeds();
  uint8_t rightValue = proxSensors.countsFrontWithRightLeds();
  
  // Object detected in front
  if ((leftValue >= proximityThreshold || rightValue >= proximityThreshold) && housecounter < TOTAL_HOUSES) 
  {
  // Move forward until the robot is close to the object
    motors.setSpeeds(200, 200); // Move forward
    // proxSensors.read(); // Update sensor readings
    // leftValue = proxSensors.countsFrontWithLeftLeds();
    // rightValue = proxSensors.countsFrontWithRightLeds();

    delay(1000);
    motors.setSpeeds(0, 0);
    housecounter++;
    
    return true; // Object detected
  }
  return false; // No object detected
}

void performDelivery() {
  // motors.setSpeeds(0, 0);
  //buzzer.play("L16 cdegreg4");

  if(housecounter == 1)
  {
    buzzer.play("L16 cdegreg4");
    display.print(F("House 1"));

    // delay(5000);
    // buzzer.stopPlaying();
  }
  else if (housecounter == 2)
  {
    buzzer.play("L16 efgefga");
    display.print(F("House 1"));

    // delay(5000);
    // buzzer.stopPlaying();
  }
  delay(5000);
  buzzer.stopPlaying();
  
  if (housecounter >= TOTAL_HOUSES)
  {
    deliveryMade = true;
    generateReturnPath();
    Serial.println("Reaturn Path: " + returnPath);
  }
}

void generateReturnPath() {
  for (int i = path.length() - 1; i >= 0; i--) {
    char step = path[i];
    switch(step) {
      case 'L': returnPath += 'R'; break;
      case 'R': returnPath += 'L'; break;
      case 'F': returnPath += 'F'; break;
      case 'B': break;
    }
  }
  returning = true;
  //path = returnPath; // Set the path to returnPath for following back
}

void turnLeft()
{
  motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
  delay(TURN_DURATION);
  //motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  motors.setSpeeds(0,0);
}

void turnRight()
{
  motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
  delay(TURN_DURATION);
  //motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  motors.setSpeeds(0,0);

}

void moveForwardShort() 
{
  motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  delay(500); // Adjust this value based on the distance between intersections in your maze
  motors.setSpeeds(0, 0); // Stop the robot
}

void followReturnPath() 
{
  // Logic to follow the return path
  if (returnPath.length() > 0) 
  {
    char action = returnPath.charAt(0); // Get the next action
    returnPath = returnPath.substring(1); // Remove this action from the path

    switch (action) 
    {
      case 'L':
          turnLeft();
          break;
      case 'R':
          turnRight();
          break;
      case 'F':
          moveForwardShort();
          break;
    }
  } 
  else if (returning)
  {
    // Once the robot has finished the return path, stop and reset the state
   motors.setSpeeds(0, 0); // Stop the robot
    Serial.println("Returned to start");
    display.print(F("Returned"));

    returning = false; // Reset for the next run
    deliveryMade = false; // Reset delivery status
    waitForButtonAndCountDown(); // Ready for next run
  
  }
}





