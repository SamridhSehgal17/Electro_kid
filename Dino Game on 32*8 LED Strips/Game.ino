/*
 * Person Runner Game for 32x8 WS2812B LED Matrix
 * Hardware: Arduino Uno + WS2812B 32x8 + 2 Buttons
 *
 * Features:
 * - Random obstacle shapes each time
 * - Speed increases after every 1 obstacle
 * - Person has a 4-row-tall shape with helmet head
 */

#include <Adafruit_NeoPixel.h>

// Matrix setup
#define LED_PIN 6
#define MATRIX_WIDTH 32
#define MATRIX_HEIGHT 8
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)

// Buttons
#define JUMP_BTN 2
#define DUCK_BTN 3

// Game Settings
#define GAME_SPEED_START 100
#define GRAVITY 1
#define JUMP_FORCE 3

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Colors
uint32_t COLOR_PERSON = strip.Color(0, 255, 0);      // Green
uint32_t COLOR_OBSTACLE = strip.Color(255, 0, 0);    // Red
uint32_t COLOR_GROUND = strip.Color(100, 100, 100);  // Gray
uint32_t COLOR_SCORE = strip.Color(0, 0, 255);       // Blue

// Person
int personX = 3;
int personY = 5;  // slightly raised
int personVelocity = 0;
bool isJumping = false;
bool isDucking = false;

// Obstacle
int obstacleX = MATRIX_WIDTH - 1;
int obstacleY = 7;
int obstacleHeight = 2;
int obstacleWidth = 1;

// Game state
unsigned long lastUpdate = 0;
unsigned long score = 0;
int gameSpeed = GAME_SPEED_START;
bool gameOver = false;

void setup() {
  Serial.begin(9600);
  pinMode(JUMP_BTN, INPUT_PULLUP);
  pinMode(DUCK_BTN, INPUT_PULLUP);

  strip.begin();
  strip.setBrightness(60);
  strip.show();

  randomSeed(analogRead(0));
}

void loop() {
  if (gameOver) {
    displayGameOver();
    if (digitalRead(JUMP_BTN) == LOW) {
      resetGame();
      delay(200);
    }
    return;
  }

  if (millis() - lastUpdate > gameSpeed) {
    lastUpdate = millis();

    // Read inputs
    if (digitalRead(JUMP_BTN) == LOW && !isJumping && !isDucking) {
      isJumping = true;
      personVelocity = JUMP_FORCE;
    }
    isDucking = (digitalRead(DUCK_BTN) == LOW);

    // Physics
    if (isJumping) {
      personY -= personVelocity;
      personVelocity -= GRAVITY;
      if (personY >= 5) {
        personY = 5;
        isJumping = false;
        personVelocity = 0;
      }
    }

    // Move obstacle
    obstacleX--;
    if (obstacleX < -obstacleWidth) {
      obstacleX = MATRIX_WIDTH - 1;
      score++;

      // ✅ Speed up after every 1 obstacle
      if (gameSpeed > 30) {
        gameSpeed -= 5; // gradually gets faster
      }

      // Random obstacle shape
      obstacleHeight = random(1, 4);     // 1 to 3 tall
      obstacleWidth = random(1, 3);      // 1 or 2 wide
      obstacleY = 7;                     // stays grounded
    }

    // Collision
    if (checkCollision()) gameOver = true;

    drawGame();
  }
}

// --- Utility functions ---

int getPixelIndex(int x, int y) {
  if (x < 0 || x >= MATRIX_WIDTH || y < 0 || y >= MATRIX_HEIGHT) return -1;
  if (y % 2 == 0) return y * MATRIX_WIDTH + x;
  else return y * MATRIX_WIDTH + (MATRIX_WIDTH - 1 - x);
}

void setPixel(int x, int y, uint32_t color) {
  int index = getPixelIndex(x, y);
  if (index >= 0 && index < NUM_LEDS) strip.setPixelColor(index, color);
}

// --- Draw Person (with helmet top) ---
void drawPerson(int x, int y, bool ducking) {
  if (ducking) {
    // Crouch (compact)
    setPixel(x - 1, y, COLOR_PERSON);
    setPixel(x,     y, COLOR_PERSON);
    setPixel(x + 1, y, COLOR_PERSON);
    setPixel(x,     y + 1, COLOR_PERSON);
  } else {
    // Tall with helmet
    setPixel(x, y - 1, COLOR_PERSON);    // top helmet pixel
    setPixel(x - 1, y, COLOR_PERSON);
    setPixel(x,     y, COLOR_PERSON);
    setPixel(x + 1, y, COLOR_PERSON);
    setPixel(x,     y + 1, COLOR_PERSON);
    setPixel(x - 1, y + 2, COLOR_PERSON);
    setPixel(x + 1, y + 2, COLOR_PERSON);
  }
}

void drawGame() {
  strip.clear();

  // Ground line
  for (int xc = 0; xc < MATRIX_WIDTH; xc++) setPixel(xc, 7, COLOR_GROUND);

  // Draw person
  drawPerson(personX, personY, isDucking && !isJumping);

  // Draw obstacle
  for (int w = 0; w < obstacleWidth; w++) {
    for (int h = 0; h < obstacleHeight; h++) {
      setPixel(obstacleX - w, obstacleY - h, COLOR_OBSTACLE);
    }
  }

  // Score (blue top row)
  int displayScore = min(score, (unsigned long)MATRIX_WIDTH);
  for (int i = 0; i < displayScore; i++) setPixel(i, 0, COLOR_SCORE);

  strip.show();
}

bool checkCollision() {
  int left = personX - 1;
  int right = personX + 1;
  int top = personY - 1;
  int bottom = personY + ((isDucking && !isJumping) ? 1 : 2);

  for (int w = 0; w < obstacleWidth; w++) {
    int obsX = obstacleX - w;
    if (obsX < left || obsX > right) continue;

    for (int h = 0; h < obstacleHeight; h++) {
      int obsY = obstacleY - h;
      if (obsY >= top && obsY <= bottom) return true;
    }
  }
  return false;
}

void displayGameOver() {
  static bool blink = false;
  static unsigned long lastBlink = 0;
  if (millis() - lastBlink > 300) {
    lastBlink = millis();
    blink = !blink;
  }

  strip.clear();
  if (blink) {
    for (int y = 2; y < 6; y++) {
      for (int x = 0; x < MATRIX_WIDTH; x++) {
        if ((x + y) % 2 == 0) setPixel(x, y, COLOR_OBSTACLE);
      }
    }
  }

  // Score on ground line
  int displayScore = min(score, (unsigned long)MATRIX_WIDTH);
  for (int i = 0; i < displayScore; i++) setPixel(i, 7, COLOR_SCORE);

  strip.show();
  Serial.print("Game Over! Score: ");
  Serial.println(score);
}

void resetGame() {
  personY = 5;
  personVelocity = 0;
  isJumping = false;
  isDucking = false;
  obstacleX = MATRIX_WIDTH - 1;
  score = 0;
  gameSpeed = GAME_SPEED_START;
  gameOver = false;
  Serial.println("Game Started!");
}

