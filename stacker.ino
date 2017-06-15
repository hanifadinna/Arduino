/*
 * Project 1: LED Matrix Game
 * Kevin McMillin
 * 05-833 Gadgets, Spring 2012
 *
 * This is an implementation of the arcade / carnival game
 * "Stackers," in which a player is tasked with stacking
 * sets of moving blocks on top of each other using an 
 * input button to freeze the blocks of a given row at their
 * current position.
 *
 * Hardware requirements:
 * - 1 8x8 LED Matrix
 * - 1 push button
 * - wiring patience
 *  
 * Credits:
 * - http://arduino.cc/playground/Main/DirectDriveLEDMatrix, for a drawing function. 
 * - Scott Hudson, for his button debouncing code
 *
 * Revisions:
 * 02/23 Initial revision 0.5.
 */

// wiring setup (so we can easily reference certain pins)
// pin[xx] on led matrix connected to nn on Arduino (-1 is dummy to make array start at pos 1)
int pins[17]= {-1, 13, 14, 15, 16, 17, 18, 19, 4, 5, 6, 7, 8, 9, 10, 11, 12};
int cols[8] = {pins[13], pins[3], pins[4], pins[10], pins[6], pins[11], pins[15], pins[16]};
int rows[8] = {pins[9], pins[14], pins[8], pins[12], pins[1], pins[7], pins[2], pins[5]};
int switchPin = 3;

/* Structure to track and debounce the state of a push button switch */
typedef struct switchTracker {
  int lastReading;       // last raw value read
  long lastChangeTime;   // last time the raw value changed
  byte pin;              // the pin this is tracking changes on
  byte switchState;      // debounced state of the switch
} 
switchTrack;

// number of milliseconds between block movement (decrements as level increases)
int LEVEL_SPEEDS[8] = {
  //  800, 800, 700, 700, 650, 600, 550, 500};
  80, 80, 70, 70, 65, 60, 55, 50};
int currentSpeed = LEVEL_SPEEDS[0];
// maximum number of blocks to display (decrements as level increases)
int MAX_WIDTHS[8] = {
  3, 3, 3, 2, 2, 1, 1, 1};

// The main board matrix, displaying the current state of the game.
int board[8][8] = { \
  {0, 0, 0, 0, 0, 0, 0, 0}, \
  {0, 0, 0, 0, 0, 0, 0, 0}, \
  {0, 0, 0, 0, 0, 0, 0, 0}, \
  {0, 0, 0, 0, 0, 0, 0, 0}, \
  {0, 0, 0, 0, 0, 0, 0, 0}, \
  {0, 0, 0, 0, 0, 0, 0, 0}, \
  {0, 0, 0, 0, 0, 0, 0, 0}, \
  {0, 0, 0, 0, 0, 0, 0, 0} \
};

// A static integer matrix representing a winning state.
int winningBoard[8][8] = { \
  {0, 0, 0, 0, 0, 0, 0, 0}, \
  {0, 0, 1, 0, 0, 1, 0, 0}, \
  {0, 0, 1, 0, 0, 1, 0, 0}, \
  {0, 0, 0, 0, 0, 0, 0, 0}, \
  {0, 0, 0, 0, 0, 0, 0, 0}, \
  {0, 1, 0, 0, 0, 0, 1, 0}, \
  {0, 0, 1, 1, 1, 1, 0, 0}, \
  {0, 0, 0, 0, 0, 0, 0, 0} \
};

// A static integer matrix representing a losing state.
int losingBoard[8][8] = { \
  {0, 0, 0, 0, 0, 0, 0, 0}, \
  {0, 0, 1, 0, 0, 1, 0, 0}, \
  {0, 0, 1, 0, 0, 1, 0, 0}, \
  {0, 0, 0, 0, 0, 0, 0, 0}, \
  {0, 0, 0, 0, 0, 0, 0, 0}, \
  {0, 0, 1, 1, 1, 1, 0, 0}, \
  {0, 1, 0, 0, 0, 0, 1, 0}, \
  {0, 0, 0, 0, 0, 0, 0, 0} \
};

// configuration constants and game states
const int WIN_LEVEL = 8;
const int BOARD_HEIGHT = 8;
const int BOARD_WIDTH = 8;
const int PLAYING = 0;
const int LOSE = 1;
const int WIN = 2;

/* 
 * Global variables
 *
 * - currentDirection: are the blocks moving left (-1) or right (1)?
 * - currentX: what's the current x position of the leftmost moving block?
 * - currentY: what row are we currently moving on?
 * - currentWidth: how many blocks are we drawing and moving on this row?
 * - currentLevel: for convenience to check for a win state. It's always equal to BOARD_HEIGHT-1-currentY.
 * - lastTime: to keep track of block motion
 * - gameState: to make logical decisions
 * - switchTrack: to track button changes
 */
int currentDirection = 1;
int currentX = 2;
int currentY = BOARD_HEIGHT-1;
int currentWidth = 3;
int currentLevel = 0;
unsigned long lastTime = 0;
int gameState = PLAYING;
switchTrack switchInput;

void setup(){
  initSwitchTrack(switchInput, switchPin);
  pinMode(switchPin,INPUT);
  for(int i=0;i<8;i++){
    pinMode(cols[i],OUTPUT);
    pinMode(rows[i],OUTPUT);
    digitalWrite(cols[i],LOW);
    digitalWrite(rows[i],HIGH);
  } 
}

void loop(){
  /* 
   * We do three things here: calculate the next currentX position (which might reverse
   * depending on BOARD_WIDTH and currentWidth), update the board[][] matrix with that 
   * info, and then draw the board in a separate function. Separating the functions
   * seems most readable and also ensures timing concerns are decently mitigated.
   */
  calculateMove();
  updateBoard();
  drawBoard();

  // If we detect the button is pressed, then we handle input accordingly.
  if(switchChange(switchInput) && switchInput.switchState == 1 /* pressed */)
    buttonEvent();
}

void buttonEvent(){
  // make sure we're PLAYING first
  if (gameState==PLAYING){
    // only do this check when there's something to check for below
    if(currentLevel > 0){
      // check each block to see if there's one below
      int maxBlockToCheck = currentX+currentWidth;
      for(int x = currentX; x < maxBlockToCheck; x++){
        // if there's no block beneath this block...
        int debug = currentY+1;
        if(board[currentY+1][x] == 0){
          // ...then give them one fewer block next level
          currentWidth -= 1;
          board[currentY][x] = 0;
        }
      }
    }
    currentY -= 1;
    currentLevel += 1;
    checkForWinOrLoss();
  }
  // else maybe we're in WIN or LOSE, and the button should reset the game
  else if(gameState == WIN || gameState == LOSE){
    resetGame();
  }
}

void checkForWinOrLoss(){
  // check if they've lost
  if (currentWidth == 0) {
    gameState = LOSE;
  }
  // else maybe they won?
  else if (currentLevel == WIN_LEVEL) {
    gameState = WIN;
  }
  // else they've got some levels to go.
  else {
    // these are real Stackers constraints as nearly as I can tell: levels speed up very slightly
    // and, even if you're stacking perfectly, you begin to lose width at a certain level.
    currentSpeed = LEVEL_SPEEDS[currentLevel];
    currentWidth = min(currentWidth,MAX_WIDTHS[currentLevel]);
  }
}

void calculateMove(){
  if (gameState == PLAYING){
    unsigned long currentTime = millis();
    // only allow moves every currentSpeed msec
    if(lastTime + currentSpeed<=currentTime){
      // do we need to switch direction?
      if(currentX+currentDirection < 0 || (currentX+currentDirection+currentWidth > BOARD_WIDTH))
        currentDirection = -currentDirection;

      currentX += currentDirection;
      lastTime = currentTime;
    }
  }
}

void updateBoard(){
  // set everything in the row off first
  if(gameState==PLAYING){
    for(int x = 0; x < BOARD_WIDTH; x++)
      board[currentY][x] = 0;
    // then set things on!
    for(int x = currentX; x < currentX+currentWidth; x++)
      board[currentY][x] = 1;
  }
}

void resetGame(){
  // clear the board
  for(int i = 0; i < BOARD_WIDTH; i++){
    for(int j = 0; j < BOARD_HEIGHT; j++){
      board[i][j] = 0;
    }
  }

  // reset all the globals
  currentDirection = -1;
  currentX = 2;
  currentY = BOARD_HEIGHT-1;
  currentWidth = 3;
  currentLevel = 0;
  lastTime = millis();
  gameState = PLAYING;
}

byte col = 0;
void drawBoard(){
  digitalWrite(cols[col], LOW);  // Turn whole previous column off
  col++;
  if (col == 8) {
    col = 0;
  }
  for (int row = 0; row < 8; row++) {
    if (gameState == PLAYING){
      if (board[col][7 - row] == 1)
        digitalWrite(rows[row], LOW);
      else
        digitalWrite(rows[row], HIGH); // Turn off this led
    }
    else if(gameState == WIN){
      if (winningBoard[col][7 - row] == 1)
        digitalWrite(rows[row], LOW);
      else
        digitalWrite(rows[row], HIGH); // Turn off this led
    }
    else if(gameState == LOSE){
      if (losingBoard[col][7 - row] == 1)
        digitalWrite(rows[row], LOW);
      else
        digitalWrite(rows[row], HIGH); // Turn off this led
    }
  }
  digitalWrite(cols[col], HIGH); // Turn whole column on at once (for equal lighting times)
}

/* Fill in a switchTrack structure to start tracking/debouncing a switch
 on the given pin.
 */
void initSwitchTrack(struct switchTracker &sw, int swPin) {  
  pinMode(swPin, INPUT);
  sw.lastReading = digitalRead(swPin);
  sw.lastChangeTime = millis();
  sw.pin = swPin;
  sw.switchState = sw.lastReading;
}

/*-------------------------------------------*/
/* Using a time based debounce, test whether a switch should be considered to have 
 changed state since the last call to this routine for the switch, and track the 
 current state of the switch in the given switch tracking structure (sw).  The switch 
 will be consided to be in a new state only after it has held that state for debounceTime msec 
 or longer.  sw.switchState will be set to 1 when the switch should be considered pressed 
 and 0 when it should be considered released.  This routine will return true when the 
 switch is in a new state compared to the last call to this routine, false otherwise.
 
 Note: this routine must be called frequently to operate correctly.
 Note: this code is here to make the demo a little more stand alone (nothing to import),
 but in other cases you might want to consider using the code from Bounce.h instead.
 */
boolean switchChange(struct switchTracker &sw) { 
  // switch must stay stable this long (in msec) to register
  // could probably perceptually get away with 100, but since this is a game where winning requires hitting the 
  // right 50 msec interval (i.e. this is how this game is rigged...), we have to at least let the 
  // user have a chance of winning.
  const long debounceTime = 50;   

  // default to no change until we find out otherwise
  boolean result = false; 

  // get the current raw reading from the switch
  int reading = digitalRead(sw.pin);

  // if the raw reading has changed (by noise or actual press/release) reset change time to now
  if (reading != sw.lastReading) sw.lastChangeTime = millis();
  sw.lastReading = reading;

  // if time since the last change is longer than the required dwell
  if ((millis() - sw.lastChangeTime) > debounceTime) {
    // note whether we are changing
    result = (reading != sw.switchState);
    // in any case the value has been stable and so the reported state 
    // should now match the current raw reading
    sw.switchState = reading;
  }
  return result;
}

