// use code with caution for your sanity. it might work on hardware?
// also i don't like arduino code so this code absolutely sucks.
// circuitpython ftw.

#include <Arduino.h>

#define BIT0_PIN 0
#define BIT1_PIN 1
#define BIT2_PIN 2
#define BIT3_PIN 3
#define BIT4_PIN 4
#define BIT5_PIN 5
#define BIT6_PIN 6
#define BIT7_PIN 7

#define BTN_DOWN 28
#define BTN_UP 27
#define BTN_SPECIAL 26

int ledPins[] = {
  BIT0_PIN, BIT1_PIN, BIT2_PIN, BIT3_PIN,
  BIT4_PIN, BIT5_PIN, BIT6_PIN, BIT7_PIN
};

enum GameState {
  START_NEW_ROUND,
  SHOW_SEQUENCE,
  PLAYER_TURN,
  CHECK_ANSWER,
  SHOW_RESULT
};

GameState currentState = START_NEW_ROUND;

int targetNumber;
int playerInput = 0;
bool playerWasCorrect;

int lastBtnUpState = HIGH;
int lastBtnDownState = HIGH;
int lastBtnSpecialState = HIGH;

void displayBinaryOnLeds(int n) {
  for (int i = 0; i < 8; i++) {
    if ((n >> i) & 1) {
      digitalWrite(ledPins[i], HIGH);
    } else {
      digitalWrite(ledPins[i], LOW);
    }
  }
}

void flashAllLeds(int count, int duration) {
  for (int i = 0; i < count; i++) {
    displayBinaryOnLeds(255);
    delay(duration);
    displayBinaryOnLeds(0);
    delay(duration);
  }
}

void showBitsOneByOne(int n, int duration) {
  displayBinaryOnLeds(0);
  delay(500);
  for (int i = 0; i < 8; i++) {
    if ((n >> i) & 1) {
      digitalWrite(ledPins[i], HIGH);
    }
    delay(duration);
    digitalWrite(ledPins[i], LOW);
  }
}

void showSuccess() {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 8; j++) {
      digitalWrite(ledPins[j], HIGH);
      delay(40);
    }
    for (int j = 0; j < 8; j++) {
      digitalWrite(ledPins[j], LOW);
      delay(40);
    }
  }
}

void showFailure() {
  for (int i = 0; i < 3; i++) {
    displayBinaryOnLeds(playerInput);
    delay(300);
    displayBinaryOnLeds(0);
    delay(150);
    displayBinaryOnLeds(targetNumber);
    delay(500);
    displayBinaryOnLeds(0);
    delay(150);
  }
}

void setup() {
  for (int i = 0; i < 8; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_SPECIAL, INPUT_PULLUP);

  Serial.begin(9600);
  randomSeed(analogRead(A0));
}

void loop() {
  int btnUpRead = digitalRead(BTN_UP);
  int btnDownRead = digitalRead(BTN_DOWN);
  int btnSpecialRead = digitalRead(BTN_SPECIAL);

  switch (currentState) {
    
    case START_NEW_ROUND:
      targetNumber = random(1, 256);
      playerInput = 0;
      displayBinaryOnLeds(0);
      delay(1000);
      currentState = SHOW_SEQUENCE;
      break;

    case SHOW_SEQUENCE:
      Serial.println(targetNumber); // why serial not working???!?
      
      showBitsOneByOne(targetNumber, 400);
      flashAllLeds(3, 100);
      displayBinaryOnLeds(targetNumber);
      delay(1500);
      displayBinaryOnLeds(0);
      flashAllLeds(2, 150);
      
      currentState = PLAYER_TURN;
      break;

    case PLAYER_TURN:
      if (btnUpRead == LOW && lastBtnUpState == HIGH) {
        playerInput++;
        if (playerInput > 255) playerInput = 0;
        displayBinaryOnLeds(playerInput);
        Serial.println(playerInput); // why serial not working???!?
        delay(50);
      }

      if (btnDownRead == LOW && lastBtnDownState == HIGH) {
        playerInput--;
        if (playerInput < 0) playerInput = 255;
        displayBinaryOnLeds(playerInput);
        Serial.println(playerInput); // why serial not working???!?
        delay(50);
      }

      if (btnSpecialRead == LOW && lastBtnSpecialState == HIGH) {
        currentState = CHECK_ANSWER;
      }
      break;

    case CHECK_ANSWER:
      playerWasCorrect = (playerInput == targetNumber);
      currentState = SHOW_RESULT;
      break;
      
    case SHOW_RESULT:
      if (playerWasCorrect) {
        showSuccess();
      } else {
        showFailure();
      }
      delay(1500);
      currentState = START_NEW_ROUND;
      break;
  }

  lastBtnUpState = btnUpRead;
  lastBtnDownState = btnDownRead;
  lastBtnSpecialState = btnSpecialRead;
}
