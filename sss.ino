#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Joystick pins
const int joyX = A0;
const int joyY = A1;
const int joySW = 2;

// Paddle
int paddleX = 54;
const int paddleY = 28;
const int paddleW = 20;
const int paddleH = 3;

// Ball
float ballX = 64;
float ballY = 20;
float ballDX = 0.8;
float ballDY = -0.8;

// Bricks: 2 rows, 16 columns (each brick is 8x4)
bool bricks[2][16];

// Timing
unsigned long lastSpeedIncrease = 0;
int frameDelay = 30;

// Score
int score = 0;
bool gameOver = false;

void setup() {
  pinMode(joySW, INPUT_PULLUP);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  resetBricks();
  lastSpeedIncrease = millis();
}

void loop() {
  if (gameOver) {
    showGameOver();
    if (digitalRead(joySW) == LOW) {
      resetGame();
    }
    return;
  }

  // Joystick
  int xValue = analogRead(joyX);
  if (xValue < 400 && paddleX > 0) paddleX -= 2;
  else if (xValue > 600 && paddleX + paddleW < SCREEN_WIDTH) paddleX += 2;

  // Ball movement
  ballX += ballDX;
  ballY += ballDY;

  // Wall collision
  if (ballX <= 0 || ballX >= SCREEN_WIDTH - 2) ballDX = -ballDX;
  if (ballY <= 0) ballDY = -ballDY;

  // Paddle collision
  if (ballY >= paddleY - 1 && ballX >= paddleX && ballX <= paddleX + paddleW) {
    ballDY = -ballDY;
  }

  // Brick collision
  for (int row = 0; row < 2; row++) {
    for (int col = 0; col < 16; col++) {
      if (bricks[row][col]) {
        int bx = col * 8;
        int by = row * 6;
        if (ballX >= bx && ballX <= bx + 7 && ballY >= by && ballY <= by + 4) {
          bricks[row][col] = false;
          ballDY = -ballDY;
          score++;
        }
      }
    }
  }

  // Refill bricks if all destroyed
  if (allBricksCleared()) {
    resetBricks();
  }

  // Ball missed
  if (ballY > SCREEN_HEIGHT) {
    gameOver = true;
  }

  // Speed up
  if (millis() - lastSpeedIncrease > 6000) {
    if (ballDX > 0) ballDX += 0.1; else ballDX -= 0.1;
    if (ballDY > 0) ballDY += 0.1; else ballDY -= 0.1;
    if (frameDelay > 10) frameDelay -= 2;
    lastSpeedIncrease = millis();
  }

  drawGame();
  delay(frameDelay);
}

void drawGame() {
  display.clearDisplay();

  // Draw bricks
  for (int row = 0; row < 2; row++) {
    for (int col = 0; col < 16; col++) {
      if (bricks[row][col]) {
        display.fillRect(col * 8, row * 6, 7, 4, SSD1306_WHITE);
      }
    }
  }

  // Draw paddle
  display.fillRect(paddleX, paddleY, paddleW, paddleH, SSD1306_WHITE);

  // Draw ball
  display.fillRect((int)ballX, (int)ballY, 2, 2, SSD1306_WHITE);

  // Draw score
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Score:");
  display.print(score);

  display.display();
}

void resetBricks() {
  for (int row = 0; row < 2; row++) {
    for (int col = 0; col < 16; col++) {
      bricks[row][col] = true;
    }
  }
}

bool allBricksCleared() {
  for (int row = 0; row < 2; row++) {
    for (int col = 0; col < 16; col++) {
      if (bricks[row][col]) return false;
    }
  }
  return true;
}

void showGameOver() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(30, 10);
  display.println("GAME OVER!");
  display.setCursor(20, 20);
  display.print("Score: ");
  display.print(score);
  display.display();
}

void resetGame() {
  paddleX = 54;
  ballX = 64;
  ballY = 20;
  ballDX = 0.8;
  ballDY = -0.8;
  frameDelay = 30;
  score = 0;
  gameOver = false;
  resetBricks();
  lastSpeedIncrease = millis();
}
