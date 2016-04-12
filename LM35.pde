/*
//==========================================================================
//	Author		          : Hanifadinna with another source 
//	Project	            : Arduino 
//	Project description : Temperature sensor LM35 to LCD Serial Monit						
//==========================================================================
*/
int pin = 0; // analog pin
int tempc = 0,tempf=0; // temperature variables
int maxi = -100,mini = 100; // to start max/min temperature

  void setup()
  {
  Serial.begin(9600); // start serial communication
  }

  void loop()
  {
  

  tempc = analogRead(pin)/2;
  delay(1000);
  tempf = (tempc * 9)/ 5 + 32; // converts to fahrenheit
  if(tempc > maxi) {maxi = tempc;} // set max temperature
  if(tempc < mini) {mini = tempc;} // set min temperature

  Serial.print(tempc,DEC);
  Serial.print(" Celsius, ");

  Serial.print(tempf,DEC);
  Serial.print(" fahrenheit -> ");

  Serial.print(maxi,DEC);
  Serial.print(" Max, ");
  Serial.print(mini,DEC);
  Serial.println(" Min");

  tempc = 0;

  delay(1000); // delay before loop
  }
