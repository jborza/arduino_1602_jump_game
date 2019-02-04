#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line

#define BUTTON_PIN 50
#define LED_PIN 13
#define SMILEY 0
#define HEART 1

enum STATE {
  MENU,
  PLAY,
  GAME_OVER
} state;

byte smiley[8] = {
  B00000,
  B10001,
  B00000,
  B00000,
  B01110,
  B10001,
  B00000,
};

byte heart[8] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B11111,
  B01110,
  B00100,
};

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.createChar(SMILEY, smiley);
  lcd.createChar(HEART, heart);
  lcd.backlight();
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  state = MENU;
}

int isButtonPressed() {
  return digitalRead(BUTTON_PIN) == LOW;
}

int getButton() {
  return digitalRead(BUTTON_PIN);
}

void firstline() {
  lcd.setCursor(0, 0);
}

void newline() {
  lcd.setCursor(0, 1);
}

void print_menu() {
  lcd.home();
  lcd.print(">> JUMP GAME <<");
  newline();
  lcd.print(" 1P PRESS START");
}

void handle_button_menu() {
  if (isButtonPressed()) {
    state = PLAY;
    lcd.clear();
  }
}
#define OBSTACLE '^'
#define COIN '\xA1'
#define PLAYER '\xFC'
#define GROUND '_'
#define WALL '\xFF'
#define MINE 'x'
#define AIR ' '
#define SPIKE '^'

int player_y = 0;

//obstacles - 0xff - full block

int hearts = 3;
int coins = 0;

bool playerOnGround() {
  return player_y == 0;
}

bool playerInAir() {
  return player_y == 1;
}

bool isObstacle(char thing) {
  return thing == SPIKE || thing == MINE || thing == WALL;
}

void print_play() {
  Serial.println("play");
  const int frame_delay = 150;
  int position = 0;
  char level[] = "_____________\xFF__\xA1\xA1\xA1__^__\xFF___x__x__x__\xFF\xA1_\xA1_xx___\xFF_____\xA1___x_\xA1x\xA1x\xA1____\xFF__x_\xA1_\xA1\xFF\xA1\xFF\xA1\xFF\xA1____\xA1^\xA1^\xA1^\xA1^___xxx__\xA1^\xA1^\xA1\xA1_\xA1\xFF\xA1\xFF\xA1______";
  //  Serial.println(level);
  char charBuf[16];

  //render
  for (int x = 0; x < strlen(level); x++) {
    firstline();
    //line 1 - player and hearts
    lcd.write(AIR);
    if (playerInAir()) {
      lcd.write(PLAYER);
    }
    else {
      lcd.write(AIR);
    }
    digitalWrite(LED_PIN, player_y);
    for (int i = 0; i < 4; i++) {
      lcd.write(AIR);
    }
    lcd.setCursor(8, 0);
    lcd.print(coins, DEC);

    //write hearts or overwrite with air
    lcd.setCursor(13, 0);
    for (int i = 13; i < 16 - hearts; i++) {
      lcd.write(AIR);
    }
    lcd.setCursor(16 - hearts, 0);
    for (int i = 0; i < hearts; i++) {
      lcd.write(HEART);
    }
    newline();
    //ground and player
    lcd.write(GROUND);
    if (playerOnGround()) {
      lcd.write(PLAYER);
    }
    else {
      lcd.write(level[x + 1]);
    }
    //rest of the level
    for (int i = 2; i < 16; i++) {
      lcd.write(level[i + x]);
    }
    //if the player hits an obstacle
    char currentThing = level[x + 2];
    if (playerOnGround() && isObstacle(currentThing)) {
      hearts--;
    }
    handle_gameover();
    if (hearts < 0)
      return;
    if (playerOnGround() && currentThing == COIN) {
      coins++;
    }
    //Serial.println(currentThing);
    //drop
    if (playerInAir())
      player_y = 0;
    if (isButtonPressed()) {
      player_y = 1;
    }
    delay(frame_delay);
  }
}

void handle_gameover() {
  if (hearts < 0)
  {
    lcd.clear();
    state = GAME_OVER;
  }
}

void print_gameover() {
  firstline();
  lcd.print("      GAME");
  newline();
  lcd.print("      OVER  ");
  lcd.write(SMILEY);
}

void handle_button_gameover() {

}

void handle_button_play() {
  //todo jump on the button
}

void loop() {
  switch (state) {
    case MENU:
      print_menu();
      handle_button_menu();
      delay(10);
      break;
    case PLAY:
      print_play();
      handle_button_play();
      delay(250);
      break;
    case GAME_OVER:
      print_gameover();
      handle_button_gameover();
      delay(10);
      break;
  }
  digitalWrite(LED_PIN, isButtonPressed());
}
