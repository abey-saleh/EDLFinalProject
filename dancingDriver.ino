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


const int leftEncoderPin  = 3;   // INT0
const int rightEncoderPin = 2;   // INT1

int currentMove = -1; 
bool isTimeForNewMove = false;

void setup()
{
//  pinMode(leftEncoderPin, INPUT); DONT NEED
  pinMode(rightEncoderPin, INPUT);


  attachInterrupt(digitalPinToInterrupt(rightEncoderPin), ISR_encoder, RISING);

  pinMode(pinLeftForward, OUTPUT);
  pinMode(pinLeftBackward, OUTPUT);
  pinMode(pinLeftPWM, OUTPUT);
  digitalWrite(pinLeftForward, LOW);
  analogWrite(pinLeftPWM, 0);


  pinMode(pinRightForward, OUTPUT);
  pinMode(pinRightBackward, OUTPUT);
  pinMode(pinRightPWM, OUTPUT);
  digitalWrite(pinRightForward, LOW);
  analogWrite(pinRightPWM, 0);

  pinMode(pinON, INPUT_PULLUP);
  Serial.begin(9600);
}

unsigned long lastTime = 0;
int old_avg_count_value = 0;

float smoothed = 0;   // global or static

void loop() 
{

  unsigned long currentTime = millis();
  long count;
  //every half a second, read edgeCount and reset edgeCount variable
  if (currentTime - lastTime > 700)
   {
    noInterrupts();       // critical section start
    count = edgeCount;    // read
    edgeCount = 0;        // reset
    interrupts();         // critical section end
    lastTime = currentTime;

    int raw = constrain(count / 4, 0, 200);
    float alpha = 0.1;
    smoothed = alpha * raw + (1 - alpha) * smoothed;

    int avg_count_value = int(smoothed);

    //logic for making the robot change dance moves
    if (abs(avg_count_value - old_avg_count_value) >= 40)
      {
        isTimeForNewMove = true;
        pickRandomDanceMove();
        isTimeForNewMove = false;
        old_avg_count_value = avg_count_value;
      }

    //adjust speed
    analogWrite(pinRightPWM, avg_count_value);
    analogWrite(pinLeftPWM, avg_count_value);
    Serial.println(avg_count_value);
  }
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


void ISR_encoder() {
    edgeCount++;   // count rising edges
}
