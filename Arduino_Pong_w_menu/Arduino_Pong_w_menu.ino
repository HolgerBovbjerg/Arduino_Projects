// Button Pin Setup
const int r_up_button = 5; //Left player up button pin
const int r_down_button = 11; //Left player down button pin
const int l_up_button = 9; //Right player up button pin
const int l_down_button = 2; //Right player down button pin

//Screen Size Parameters
int x_pixels = 128;
int y_pixels = 32;

//Paddle Parameters
int paddle_height = 10;
int paddle_width = 3;
int speakerPin = 8;

#define maxScore 10 

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

// Ball velocities:
int x_vel = 1;
int y_vel = 1;

// Ball position:
int x_pos = display.width() / 2;
int y_pos = display.height() / 2;

// Paddle positions:
int l_pos = 0;
int r_pos = 0;

// Player scores
int l_score = 0;
int r_score = 0;
int *a = &l_score;
int *b = &r_score;


//Custom characters
// Creates 3 custom characters for the menu display
byte downArrow[8] = {
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b10101, // * * *
  0b01110, //  ***
  0b00100  //   *
};

byte upArrow[8] = {
  0b00100, //   *
  0b01110, //  ***
  0b10101, // * * *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100, //   *
  0b00100  //   *
};

byte menuCursor[8] = {
  B00010, //  *
  B00100, //   *
  B01000, //    *
  B10000, //     *
  B01000, //    *
  B00100, //   *
  B00010, //  *
  B00000  //
};
#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B11111111, B11111111,
  B11111111, B11111111,
  B11111111, B11111111,
  B11111111, B11111111,
  B11111111, B11111111,
  B11111111, B11111111,
  B01111111, B11111111,
  B11111111, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000
};

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


#define KEY_NONE 0
#define KEY_PREV 1
#define KEY_NEXT 2
#define KEY_SELECT 3
#define KEY_BACK 4

int uiKeyPrev = l_up_button;
int uiKeyNext = l_down_button;
int uiKeySelect = r_up_button;

int uiKeyCodeFirst = KEY_NONE;
int uiKeyCodeSecond = KEY_NONE;
int uiKeyCode = KEY_NONE;

void uiStep(void) {
  uiKeyCodeSecond = uiKeyCodeFirst;
  if ( digitalRead(uiKeyPrev))
    uiKeyCodeFirst = KEY_PREV;
  else if ( digitalRead(uiKeyNext))
    uiKeyCodeFirst = KEY_NEXT;
  else if ( digitalRead(uiKeySelect))
    uiKeyCodeFirst = KEY_SELECT;
  else
    uiKeyCodeFirst = KEY_NONE;

  if (uiKeyCodeSecond == uiKeyCodeFirst )
    uiKeyCode = uiKeyCodeFirst;
  else
    uiKeyCode = KEY_NONE;
}

#define MENU_ITEMS 3
const char *menu_strings[MENU_ITEMS] = {"New Game", "Highscores", "Choose Player"};

int menu_current = 0;
int menu_redraw_req = 0;
int last_key_code = KEY_NONE;

void drawMenu() {
  int i, n = 0;
  for (i ; i < MENU_ITEMS; i++) {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, n);
    display.print(menu_strings[i]);
    if (i == menu_current) {
      display.drawBitmap(100, n, menuCursor, 8, 8, 1);
    }
    n = n + 8;
  }
}

void menuUpdate() {
  if ( uiKeyCode != KEY_NONE && last_key_code == uiKeyCode ) {
    return;
  }

  last_key_code = uiKeyCode;

  switch (uiKeyCode) {
    case KEY_NEXT:
      menu_current++;
      if ( menu_current >= MENU_ITEMS )
        menu_current = 0;
      menu_redraw_req = 1;
      break;
    case KEY_PREV:
      if ( menu_current == 0 )
        menu_current = MENU_ITEMS;
      menu_current--;
      menu_redraw_req = 1;
      break;
  }
}

void selectPressed() {
  if (uiKeyCode == KEY_SELECT && last_key_code == uiKeyCode) {
    display.clearDisplay();
    switch (menu_current) {
      case 0:
        while (r_score<= maxScore - 1) {
          updatePaddles();
          checkHitWall();
          checkHitCeiling();
          checkBouncePaddle();
          updateBallPos();
          drawPongElements();
          displayScore();
          display.display();
          checkForWinner();
        }
        break;
      case 1:
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("Morten spiser chips");
        display.display();
        delay(1000);
        menu_redraw_req = 1;
        break;
      case 2:
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("Morten spiser chips");
        display.display();
        delay(1000);
        menu_redraw_req = 1;
        break;
    }
    last_key_code = uiKeyCode;
  }
  
}

void setup() {
  // put your setup code here, to run once:
  pinMode(l_up_button, INPUT);
  pinMode(l_down_button, INPUT);
  pinMode(r_up_button, INPUT);
  pinMode(r_down_button, INPUT);
  Serial.begin(9600);
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // init done
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(200);
  // Clear the buffer.
  display.clearDisplay();
  // force initial redraw.
  menu_redraw_req = 1;
}

void loop() {
  // put your main code here, to run repeatedly:
  uiStep();

  if ( menu_redraw_req != 0 ) {
    display.clearDisplay();
    drawMenu();
    display.display();
    menu_redraw_req = 0;
  }
  menuUpdate();
  selectPressed();
}

void updatePaddles() {
  // Update position of left paddle:
  if (digitalRead(l_down_button) && l_pos < (y_pixels - paddle_height)) {
    l_pos += 1;
  }
  else if (digitalRead(l_up_button) && l_pos > 0) {
    l_pos -= 1;
  }

  // Update position of right paddle:
  if (digitalRead(r_down_button) && r_pos < (y_pixels - paddle_height)) {
    r_pos += 1;
  }
  else if (digitalRead(r_up_button) && r_pos > 0) {
    r_pos -= 1;
  }
}

void checkHitWall() {
  // Check for ball hitting a wall:

  if (x_pos > x_pixels - 1) {
    ball_reset(false);
    l_score += 1;
    tone(speakerPin, 50, 100);
    delay(50);
    tone(speakerPin, 50, 100);
  }

  else if (x_pos < 0) {
    ball_reset(true);
    r_score += 1;
    tone(speakerPin, 50, 100);
    delay(50);
    tone(speakerPin, 50, 100);
  }
}

void checkHitCeiling() {
  // Check for ball bouncing off ceiling:
  if (y_pos > y_pixels - 1 || y_pos < 0) {
    y_vel = -y_vel;
  }
}

// Check for ball bouncing off paddle:
void checkBouncePaddle() {
  // Check for ball bouncing off paddles:
  if (ball_on_right_paddle()) {
    x_vel = -x_vel;
    tone(speakerPin, 300, 100);
  }
  else if (ball_on_left_paddle()) {
    x_vel = -x_vel;
    tone(speakerPin, 250, 100);
  }
}

void updateBallPos() {
  // Update ball position:
  x_pos += x_vel;
  y_pos += y_vel;
}

void drawPongElements() {
  // Draw pong elements to display:
  display.clearDisplay();
  display.drawPixel(x_pos, y_pos, WHITE);
  display.fillRect(0, l_pos, paddle_width, paddle_height, WHITE);
  display.fillRect(x_pixels - paddle_width , r_pos, paddle_width, paddle_height, WHITE);
}

void displayScore() {
  // Display scores
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(display.width() / 4, 0);
  display.println(l_score);
  display.setCursor(display.width() * 3 / 4, 0);
  display.println(r_score);
}

bool ball_on_right_paddle() {
  // If ball is heading towards paddle and is at the surface of paddle between the top and bottom of the paddle, then it's a hit
  return (x_pos == x_pixels - paddle_width - 1 && y_pos >= r_pos && y_pos <= (r_pos + paddle_height) && x_vel == 1);
}

bool ball_on_left_paddle() {
  return (x_pos == paddle_width - 1 && y_pos >= l_pos && y_pos <= (l_pos + paddle_height) && x_vel == -1);
}

void ball_reset(bool left) {
  //If left is true, then ball should launch from the left, otherwise launch from the right
  //Ball should launch at a random Y location and at a random Y velocity

  y_pos = random(display.height());
  if (random(2) > 0) {
    y_vel = 1;
  }
  else {
    y_vel = -1;
  }

  if (left) {
    x_vel = 1;
    x_pos = paddle_width - 1;
  }

  else {
    x_vel = -1;
    x_pos = display.width() - paddle_width;
  }
}

void checkForWinner() {
  if (r_score >= maxScore) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 8);
    display.println("Game Over");
    display.display();
    delay(4000);
    display.clearDisplay();
    display.setCursor(0, 8);
    display.print("Winner: "); display.println("Player 2");
    display.print("Congratulations!");
    display.display();
    playWinnermusic();
    delay(1000);
    display.clearDisplay();
    display.setCursor(0, 8);
    display.print("Loser: "); display.println("Player 1");
    display.print("You suck");
    display.display();
    playLoserMusic();
    delay(4000);
    display.clearDisplay();
    *a = 0;
    *b = 0;
    x_pos = display.width() / 2;
    y_pos = display.height() / 2;
  }
  else if (l_score >= maxScore) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 8);
    display.println("Game Over");
    display.display();
    delay(4000);
    display.clearDisplay();
    display.setCursor(0, 8);
    display.print("Winner: "); display.println("Player 1");
    display.print("Congratulations!");
    display.display();
    playWinnermusic();
    delay(1000);
    display.clearDisplay();
    display.setCursor(0, 8);
    display.print("Loser: "); display.println("Player 2");
    display.print("You suck");
    display.display();
    playLoserMusic();
    delay(4000);
    display.clearDisplay();
    *a = 0;
    *b = 0;
    x_pos = display.width() / 2;
    y_pos = display.height() / 2;
  }
}

void playWinnermusic() {
  // NB: ALL NOTES DEFINED WITH STANDARD ENGLISH NAMES, EXCEPT FROM "A"
  //THAT IS CALLED WITH THE ITALIAN NAME "LA" BECAUSE A0,A1...ARE THE ANALOG PINS ON ARDUINO.
  // (Ab IS CALLED Ab AND NOT LAb)
#define C0 16.35
#define Db0 17.32
#define D0  18.35
#define Eb0 19.45
#define E0  20.60
#define F0  21.83
#define Gb0 23.12
#define G0  24.50
#define Ab0 25.96
#define LA0 27.50
#define Bb0 29.14
#define B0  30.87
#define C1  32.70
#define Db1 34.65
#define D1  36.71
#define Eb1 38.89
#define E1  41.20
#define F1  43.65
#define Gb1 46.25
#define G1  49.00
#define Ab1 51.91
#define LA1 55.00
#define Bb1 58.27
#define B1  61.74
#define C2  65.41
#define Db2 69.30
#define D2  73.42
#define Eb2 77.78
#define E2  82.41
#define F2  87.31
#define Gb2 92.50
#define G2  98.00
#define Ab2 103.83
#define LA2 110.00
#define Bb2 116.54
#define B2  123.47
#define C3  130.81
#define Db3 138.59
#define D3  146.83
#define Eb3 155.56
#define E3  164.81
#define F3  174.61
#define Gb3 185.00
#define G3  196.00
#define Ab3 207.65
#define LA3 220.00
#define Bb3 233.08
#define B3  246.94
#define C4  261.63
#define Db4 277.18
#define D4  293.66
#define Eb4 311.13
#define E4  329.63
#define F4  349.23
#define Gb4 369.99
#define G4  392.00
#define Ab4 415.30
#define LA4 440.00
#define Bb4 466.16
#define B4  493.88
#define C5  523.25
#define Db5 554.37
#define D5  587.33
#define Eb5 622.25
#define E5  659.26
#define F5  698.46
#define Gb5 739.99
#define G5  783.99
#define Ab5 830.61
#define LA5 880.00
#define Bb5 932.33
#define B5  987.77
#define C6  1046.50
#define Db6 1108.73
#define D6  1174.66
#define Eb6 1244.51
#define E6  1318.51
#define F6  1396.91
#define Gb6 1479.98
#define G6  1567.98
#define Ab6 1661.22
#define LA6 1760.00
#define Bb6 1864.66
#define B6  1975.53
#define C7  2093.00
#define Db7 2217.46
#define D7  2349.32
#define Eb7 2489.02
#define E7  2637.02
#define F7  2793.83
#define Gb7 2959.96
#define G7  3135.96
#define Ab7 3322.44
#define LA7 3520.01
#define Bb7 3729.31
#define B7  3951.07
#define C8  4186.01
#define Db8 4434.92
#define D8  4698.64
#define Eb8 4978.03
  // DURATION OF THE NOTES
#define BPM 136    //  you can change this value changing all the others
#define H 2*Q //half 2/4
#define Q 60000/BPM //quarter 1/4 
#define E Q/2   //eighth 1/8
#define S Q/4 // sixteenth 1/16
#define W 4*Q // whole 4/4
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  delay(100);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  delay(100);

  tone(8, E5, S);
  delay(1 + S);
  tone(8, E5, S);
  delay(1 + S);
  tone(8, E5, S);
  delay(1 + S);
  tone(8, E5, S);
  delay(1 + S);
  tone(8, E5, S);
  delay(1 + S);
  tone(8, E5, S);
  delay(1 + S);
  tone(8, E5, S);
  delay(1 + S);
  delay(100);
  tone(8, D5, S);
  delay(1 + S);
  tone(8, D5, S);
  delay(1 + S);
  tone(8, D5, S);
  delay(1 + S);
  tone(8, D5, S);
  delay(1 + S);
  tone(8, D5, S);
  delay(1 + S);
  tone(8, D5, S);
  delay(1 + S);
  tone(8, D5, S);
  delay(1 + S);
  tone(8, D5, S);
  delay(1 + S);
  tone(8, LA5, E);
  delay(1 + E);
  delay(100);

  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  delay(100);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, C6, S);
  delay(1 + S);
  delay(100);

  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  delay(100);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, Bb5, E);
  delay(1 + E);
  delay(100);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  delay(100);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  delay(100);

  tone(8, E5, S);
  delay(1 + S);
  tone(8, E5, S);
  delay(1 + S);
  tone(8, E5, S);
  delay(1 + S);
  tone(8, E5, S);
  delay(1 + S);
  tone(8, E5, S);
  delay(1 + S);
  tone(8, E5, S);
  delay(1 + S);
  tone(8, E5, S);
  delay(1 + S);
  delay(100);
  tone(8, D5, S);
  delay(1 + S);
  tone(8, D5, S);
  delay(1 + S);
  tone(8, D5, S);
  delay(1 + S);
  tone(8, D5, S);
  delay(1 + S);
  tone(8, D5, S);
  delay(1 + S);
  tone(8, D5, S);
  delay(1 + S);
  tone(8, D5, S);
  delay(1 + S);
  tone(8, D5, S);
  delay(1 + S);
  tone(8, LA5, E);
  delay(1 + E);
  delay(100);

  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  delay(100);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, C6, S);
  delay(1 + S);
  delay(100);

  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  delay(100);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, B5, S);
  delay(1 + S);
  tone(8, Bb5, E);
  delay(1 + E);
  delay(100);
}

void playLoserMusic() {
  // NB: ALL NOTES DEFINED WITH STANDARD ENGLISH NAMES, EXCEPT FROM "A"
  //THAT IS CALLED WITH THE ITALIAN NAME "LA" BECAUSE A0,A1...ARE THE ANALOG PINS ON ARDUINO.
  // (Ab IS CALLED Ab AND NOT LAb)
#define C0 16.35
#define Db0 17.32
#define D0  18.35
#define Eb0 19.45
#define E0  20.60
#define F0  21.83
#define Gb0 23.12
#define G0  24.50
#define Ab0 25.96
#define LA0 27.50
#define Bb0 29.14
#define B0  30.87
#define C1  32.70
#define Db1 34.65
#define D1  36.71
#define Eb1 38.89
#define E1  41.20
#define F1  43.65
#define Gb1 46.25
#define G1  49.00
#define Ab1 51.91
#define LA1 55.00
#define Bb1 58.27
#define B1  61.74
#define C2  65.41
#define Db2 69.30
#define D2  73.42
#define Eb2 77.78
#define E2  82.41
#define F2  87.31
#define Gb2 92.50
#define G2  98.00
#define Ab2 103.83
#define LA2 110.00
#define Bb2 116.54
#define B2  123.47
#define C3  130.81
#define Db3 138.59
#define D3  146.83
#define Eb3 155.56
#define E3  164.81
#define F3  174.61
#define Gb3 185.00
#define G3  196.00
#define Ab3 207.65
#define LA3 220.00
#define Bb3 233.08
#define B3  246.94
#define C4  261.63
#define Db4 277.18
#define D4  293.66
#define Eb4 311.13
#define E4  329.63
#define F4  349.23
#define Gb4 369.99
#define G4  392.00
#define Ab4 415.30
#define LA4 440.00
#define Bb4 466.16
#define B4  493.88
#define C5  523.25
#define Db5 554.37
#define D5  587.33
#define Eb5 622.25
#define E5  659.26
#define F5  698.46
#define Gb5 739.99
#define G5  783.99
#define Ab5 830.61
#define LA5 880.00
#define Bb5 932.33
#define B5  987.77
#define C6  1046.50
#define Db6 1108.73
#define D6  1174.66
#define Eb6 1244.51
#define E6  1318.51
#define F6  1396.91
#define Gb6 1479.98
#define G6  1567.98
#define Ab6 1661.22
#define LA6 1760.00
#define Bb6 1864.66
#define B6  1975.53
#define C7  2093.00
#define Db7 2217.46
#define D7  2349.32
#define Eb7 2489.02
#define E7  2637.02
#define F7  2793.83
#define Gb7 2959.96
#define G7  3135.96
#define Ab7 3322.44
#define LA7 3520.01
#define Bb7 3729.31
#define B7  3951.07
#define C8  4186.01
#define Db8 4434.92
#define D8  4698.64
#define Eb8 4978.03
  // DURATION OF THE NOTES
#define BPM 136    //  you can change this value changing all the others
#define H 2*Q //half 2/4
#define Q 60000/BPM //quarter 1/4 
#define E Q/2   //eighth 1/8
#define S Q/4 // sixteenth 1/16
#define W 4*Q // whole 4/4
  tone(8, A5, 10000);
  delay(1 + W);
  delay(10000);
  tone(8, B4, W);
  delay(1 + W);
  delay(100);
}

