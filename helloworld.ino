//Learn Arduino-Hello world secara berulang

void setup () {
  Serial.begin(9600); //open serial port with speed 9600 bit per second
}

void loop () {
 delay(10000);  // delay time 10 second
 Serial.println("Hello world");  //print hello world
}
