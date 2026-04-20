const int pinON = 6;
const int pinRightForward = 7;
const int pinRightBackward = 8;
const int pinRightPWM = 9;
const int pinLeftForward = 11;
const int pinLeftBackward = 12;
const int pinLeftPWM = 10;

volatile long edgeCount = 0;
volatile long leftCount = 0;
volatile long rightCount = 0;

const int leftEncoderPin = 3;  
const int rightEncoderPin = 2; 

unsigned long lastTime = 0;

// Variable to track the current random move so it doesn't repeat the same move twice
int currentMove = -1; 

void setup() {
  pinMode(leftEncoderPin, INPUT);
  pinMode(rightEncoderPin, INPUT);

  attachInterrupt(digitalPinToInterrupt(rightEncoderPin), ISR_encoder, RISING);
  attachInterrupt(digitalPinToInterrupt(leftEncoderPin), ISR_encoder, RISING);

  pinMode(pinLeftForward, OUTPUT);
  pinMode(pinLeftBackward, OUTPUT);
  pinMode(pinLeftPWM, OUTPUT);
  analogWrite(pinLeftPWM, 0);

  pinMode(pinRightForward, OUTPUT);
  pinMode(pinRightBackward, OUTPUT);
  pinMode(pinRightPWM, OUTPUT);
  analogWrite(pinRightPWM, 0);

  pinMode(pinON, INPUT_PULLUP);
  Serial.begin(9600);
  
  // Initialize the random number generator using noise from an unconnected analog pin
  randomSeed(analogRead(A5)); 
  
  // Pick the very first move to start the dance
  pickRandomDanceMove();
}

void loop() {
  // John's math to control the speed 
  unsigned long currentTime = millis();
  
  // every half a second, read edgeCount and reset edgeCount variable 
  // (Note: your code says 1000, which is 1 second, but leaving it as you wrote it!)
  if (currentTime - lastTime > 1000) {
    long count;
    noInterrupts(); // critical section start
    count = edgeCount; // read
    edgeCount = 0; // reset
    interrupts(); // critical section end
    lastTime = currentTime;

    int avg_count_value = count / 4;
    
    analogWrite(pinRightPWM, avg_count_value);
    analogWrite(pinLeftPWM, avg_count_value); 
    
    Serial.println(count);
  }
// checking here to see if it is time for a new dance 
  if (isTimeForNewMove() == true) {
      pickRandomDanceMove();
  }
}

void ISR_encoder() {
  edgeCount++; // count rising edges
}

// Placeholder for future logic
bool isTimeForNewMove() {
  return false; 
}

void pickRandomDanceMove() {
  // Pick a random number between 0 and 3
  int nextMove = random(0, 4); 
  
  // Prevent the robot from picking the exact same move twice in a row
  while (nextMove == currentMove) {
      nextMove = random(0, 4);
  }
  
  currentMove = nextMove;
  
  switch (currentMove) {
    case 0:
      // Move 0: Go Forward
      Serial.println("Dancing: FORWARD");
      digitalWrite(pinLeftForward, HIGH);
      digitalWrite(pinLeftBackward, LOW);
      digitalWrite(pinRightForward, HIGH);
      digitalWrite(pinRightBackward, LOW);
      break;
      
    case 1:
      // Move 1: Go Backward
      Serial.println("Dancing: BACKWARD");
      digitalWrite(pinLeftForward, LOW);
      digitalWrite(pinLeftBackward, HIGH);
      digitalWrite(pinRightForward, LOW);
      digitalWrite(pinRightBackward, HIGH);
      break;
      
    case 2:
      // Move 2: Spin Left (Counter-Clockwise)
      Serial.println("Dancing: SPIN LEFT");
      digitalWrite(pinLeftForward, LOW);
      digitalWrite(pinLeftBackward, HIGH);
      digitalWrite(pinRightForward, HIGH);
      digitalWrite(pinRightBackward, LOW);
      break;
      
    case 3:
      // Move 3: Spin Right (Clockwise)
      Serial.println("Dancing: SPIN RIGHT");
      digitalWrite(pinLeftForward, HIGH);
      digitalWrite(pinLeftBackward, LOW);
      digitalWrite(pinRightForward, LOW);
      digitalWrite(pinRightBackward, HIGH);
      break;
  }
}
