/*
  Blink
  LED ON selama 1 detik dan OFF selama 1 detik secara berulang
 */

void setup() {                
  // Deklarari pin sebagai output
  // LED dihubungkan dengan pin 13 Arduino
  pinMode(13, OUTPUT);     
}

void loop() {
  digitalWrite(13, HIGH);   // set the LED on
  delay(1000);              // lama ON selama 1 detik
  digitalWrite(13, LOW);    // set the LED off
  delay(1000);              // lama OFF selama 1 detik
}
