//==========================================================================
//	Author		    : Hanifadinna with another source 
//	Project	            : Hook Up Temperature Sensor 
//	Project description : Temperature sensor LM35 to LCD display						
//==========================================================================

#include <LiquidCrystal.h>
/*
  The circuit:
 * LCD RS pin to digital pin 8
 * LCD Enable pin to digital pin 9
 * LCD D4 pin to digital pin 4
 * LCD D5 pin to digital pin 5
 * LCD D6 pin to digital pin 6
 * LCD D7 pin to digital pin 7
 * LCD R/W pin to ground
*/

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int temp;
int celsius;
int kelvin;
int fahrenheit;
int reamur;

int val = A1; // read sensor from analog pin A1              
                    
void setup ()
{
  lcd.begin(16, 2);                   // set the lcd dimension
  lcd.clear();                        // LCD screen clear
  lcd.print("  HELLO WORLD  ");        // display the text
  lcd.setCursor(0,1);                 // set lcd.setCursor (column,row) 
  lcd.print("LM35 Temp Sensor");  
  delay(3000);                        // delay 3000ms
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("C:      K:");            // C:Celsius  K:Kelvin
  lcd.setCursor(0,1);        
  lcd.print("F:      R:");            // F:Fahrenheit  R:Reamur
}


void loop()
{
  temp = analogRead(val);    //get analog value
  
  celsius = temp/2;                    //refer to LM35 datasheet, celsius can get by devide by 2.
  lcd.setCursor(2, 0);                 // set cursor position
  lcd.print(celsius,DEC);              // print the tempreture value in DECIMAL
  
  kelvin = celsius + 273;              // kelvin = celsius + 273
  lcd.setCursor(10, 0);                // set cursor position 
  lcd.print(kelvin,DEC);
  
  fahrenheit = (celsius*9/5)+32;      // fahrenheit = 9/5*celcius +32
  lcd.setCursor(2, 1);
  lcd.print(fahrenheit,DEC);
 
  reamur = celsius*4/5;                // reamur = celsius*4/
  lcd.setCursor(10, 1);;  
  lcd.print(reamur,DEC);
  
  delay(1000);               // delay 1000ms
  
}
  
