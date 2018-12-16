#include <Keypad.h>


int ledpin = 13;
const byte numRows= 4;
const byte numCols= 4;

char keymap[numRows][numCols]= 
    {
      {'1', '2', '3', 'A'}, 
      {'4', '5', '6', 'B'}, 
      {'7', '8', '9', 'C'},
      {'*', '0', '#', 'D'}
    };


//------------------------------------------------------------
byte rowPins[numRows] = {9,8,7,6};//hubungkamn keypad ke pin Arduino
byte colPins[numCols] = {5,4,3,2};


Keypad keypad= Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numCols); //mapping keypad

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Keypad 4x4");
}

void loop() {

 char key = keypad.getKey();
 if (key) {
  //tampilkan key yang ditekan 
  Serial.println(key) ;
 }


  if (key =='4'){
  digitalWrite(ledpin,HIGH);
  }

  else  if (key =='5') {
    digitalWrite(ledpin, LOW);
  }
    else if(key == '0') {
      Serial.print("Tekan yang lain ya");
    
    } 

 
}

