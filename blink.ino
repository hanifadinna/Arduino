*/

const int buttonPin = 2;     // pin tempat ditancapkannya pushbutton
const int ledPin =  13;      // pin tempat ditancapkannya LED
int buttonState = 0;         // pembacaan status pushbutton

void setup() {
  pinMode(ledPin, OUTPUT); // deklarasikan LED sebagai output
  pinMode(buttonPin, INPUT);     // deklarasikan button sebagai output  
}

void loop(){
  buttonState = digitalRead(buttonPin);  // baca keadaan nilai pushbutton

  // check apakah pushbutton ditekan, jika IYA, buttonState is HIGH:
  if (buttonState == HIGH) {         // LED on:    
    digitalWrite(ledPin, HIGH);  
  }
  else {
    digitalWrite(ledPin, LOW);     // LED off:
  }
}
