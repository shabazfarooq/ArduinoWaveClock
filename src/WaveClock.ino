/**********************************************/
/*  CSE 321 - Project 3 - Fall 2014           */
/*                                            */
/*  Shabaz Farooq  [5002-8309]                */
/*  Nhat   Nguyen  [3586-3080]                */
/**********************************************/


// Include LCD library
#include <LiquidCrystal.h>

/**********************************************/
/*    - Declare global variable               */
/*    - Initialize pins for external devices  */
/*                                            */
/**********************************************/
const int trigPin  = 6;   // Proximity sensor trig pin
const int echoPin  = 7;   // Proximity sensor echo pin
const int ledPin   = 13;  // LED pin
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);  // LCD pin outs

// Variables to keep track of current time
int minutes     = 0;
int seconds     = 0;
int hours       = 12;

// This variable is used for DEMO purposes.
// By setting it to 1 or 2, it will allow the minutes
// to go quicker.
int secondsInMinute = 60;

// Set the alarm time, ex: 12:03
int alarmHour   = 12;
int alarmMinute = 03;

// Alarm variables, to see if the alarm is ringing or turned off
boolean alarmOn      = true;
boolean alarmActive  = false;

// Declare how long the snooze should be,
// and the distance one would wave to snooze the alarm
int snoozeMinutes    = 3;
int snoozeInches     = 10;
boolean snoozeActive = false;

// The difference between the snooze distance and the proximityMax
// distance is the distance required to turn off the alarm.
int proximityMax = 40;

// Variables used to display the time and alarm appropriately
char timeString[20];
char alarmString[20];

int proximity;

String wakeupString;

extern volatile unsigned long timer0_millis;

/**********************************************/
/*                                            */
/*    setup() function:                       */
/*       - Responsible for initializing all   */
/*         output devices                     */
/*                                            */
/**********************************************/
void setup() {
  
  Serial.begin(9600);       // Initialize serial communication
  pinMode(ledPin, OUTPUT);  // Initialize LED communication
  lcd.begin(20, 4);         // Initialize LCD columns and rows
  lcd.clear();
  
  wakeupString = "***** WAKE UP! *****"; // String to run when the alarm goes off

}



/**********************************************/
/*                                            */
/*    loop() function:                        */
/*       - This is the active code that runs  */
/*      for ever when the device is turned on */
/*                                            */
/**********************************************/
void loop() {
  
  // Determine the current seconds
  seconds = millis()/1000;
  
  // Call the following functions to update
  updateTime();
  printTime();
  printAlarm();
  
  // Determine if it is time to set off the alarm
  if (alarmOn == true && hours == alarmHour && minutes == alarmMinute) {
    alarmActive = true;
  }
  
   
  // If the alarm is active
  if (alarmActive == true) {
    // Turn the alarm on
    turnAlarmOn();
    
    // Get the updated distance for the proximity sensor
    proximity = proximityInches();

    // Check for snooze
    if (proximity <= snoozeInches) {
      // Turn te alarm buzzer off if snooze was activated
      alarmActive  = false;
      snoozeActive = true;
      
      // Set the new alarm time
      alarmMinute = minutes + snoozeMinutes;
      
      // If the alarm time goes into the next hour, update accordingly
      if (alarmMinute >= 60) {
        alarmMinute = alarmMinute - 60;
        alarmHour   = alarmHour + 1;
        
        if (alarmHour > 12) {
          alarmHour = alarmHour - 12;
        }
      }
    } 

  }
  
  // If at any point the user waves there hand at the "TURN OFF" point, then deactivate the alarm
  if (alarmActive == true || snoozeActive == true ) {
    proximity = proximityInches();
    
    // Check for turn off
    if (proximity > snoozeInches  && proximity < proximityMax) {
      turnAlarmOff();
    }
  }
 
}



///   SECONARY FUNCTIONS
void turnAlarmOn() {
  led_sound_on();
}

// Turn alarm off function:
// - Responsible for turning off the LED
// - Updating the LCD to reflect that the alarm is not on
// - Turn the buzzer off
void turnAlarmOff() {
  // Call the function responsible for turning the sound and led off
  led_sound_off();
  
  // Update the LCD to reflect that the alarm is no longer on
  alarmOn      = false;
  alarmActive  = false;
  snoozeActive = false;
  lcd.setCursor(0,3);
  lcd.print("      Alarm OFF      ");
  delay(2500);
  lcd.setCursor(0,3);
  lcd.print("                   ");
}


// PrintTime function:
// - Responsible for updating the LCD with the current time
void printTime() {
  lcd.setCursor(0,0);
  lcd.print("          ");
  
  sprintf(timeString, "      %02d:%02d PM      ", hours, minutes);
  
  lcd.setCursor(0,1);
  lcd.print(timeString);
}



// Print alarm function
// - Responsible for most of the LCD display related activities
void printAlarm() {
          
  // If the alarm is on, Print the time and the Alarm
  if (alarmOn == true && alarmActive == false && snoozeActive == false) {
    sprintf(alarmString, " Alarm set at %02d:%02d  ", hours, alarmMinute);
  
    lcd.setCursor(0,3);
    lcd.print(alarmString);
  }
  // If the alarm is active scroll the "wakeup" string across the string
  else if (alarmActive == true) {
    lcd.setCursor(0,0);
    lcd.print("********************");
    
    lcd.setCursor(0,1);
    lcd.print("*");
    lcd.setCursor(19,1);
    lcd.print("*");
    
    lcd.setCursor(0,2);
    lcd.print("*");
    lcd.setCursor(19,2);
    lcd.print("*");
    
    lcd.setCursor(0,3);
    wakeupString = ScrollText(wakeupString);
    lcd.print(wakeupString);
    
    delay(200);
    lcd.setCursor(0,0);
    lcd.print("                    ");
    
    lcd.setCursor(0,1);
    lcd.print(" ");
    lcd.setCursor(19,1);
    lcd.print(" ");
    
    lcd.setCursor(0,2);
    lcd.print(" ");
    lcd.setCursor(19,2);
    lcd.print(" ");
    
    lcd.setCursor(0,3);
    lcd.print("                    ");
    
    
  }
  // If the user calls for a snooze, display SNOOZING
  else if (snoozeActive == true) {
    lcd.setCursor(0,3);
    lcd.print("      SNOOZING     ");
  }  
  
  
}



// Update Time function:
// - Responsible for converting milliseconds into time.
//   Specifically, HOURS:MINUTES:SECONDS
void updateTime() {
  seconds = millis()/1000;
  
  // Update seconds and minutes
  if (seconds >= secondsInMinute) {
    noInterrupts ();
    timer0_millis = 0;
    interrupts ();
    
    minutes = minutes + 1;
  }
  
  // Update hours
  if (minutes == 60) {
    minutes = 0;
    
    if (hours == 12) {
      hours = 1;
    } else {
      hours++;
    }
  }
  

}



// Proximity inches function
// - This function is responsible for tapping into the
//   proximity sensor and returning the value retreived
//   into a usable number. Specifically, converting it
//   into inches
int proximityInches() {
  // Variables used to determine proximity
  long duration;
  long inches;
   
  // -To ensure an accurate measure, we pulse the signal by
  // sending it a low signal followed by a high signal
  // for both the echo pin and the trigger pin.
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
   

  // Measure the promiximity using the ultrasonic sensor
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
   
  // Convert the measured distance to inches
  inches = duration / 74 / 2;
  
  // Return inches
  return inches;

}



// This function turns the LED on and also turns the BUZZER on
void led_sound_on() {
  digitalWrite(ledPin, HIGH);  // Turn the LED ON
  tone(8, 900, 250);
  delay(70);                   // Delay
  digitalWrite(ledPin, LOW);   // Turn the LEF OFF
  delay(70);                   // Delay again
  
}

// This function turns the LED off and also turns the BUZZER off
void led_sound_off() {
  tone(8, 1300, 500);
  delay(500); 
  noTone(8);
  digitalWrite(ledPin, LOW);   // Turn the LED OFF
}


int count = 1;

// This function is simply used to scroll the text across the screen.
// We use this for the "WAKE UP" string as it makes the display nicer. 
String ScrollText(String text){
  count++;
  
  if (count <= 4 || (count >= 13  && count <= 16) ) {
    return text.substring(1,text.length()) + text.substring(0,1);
  }
  else if ( (count >= 5)  && count <= 12) {
    return text.substring(text.length()-1, text.length()) + text.substring(0,text.length()-1);
  } else {
    count = 0;
    return wakeupString = "***** WAKE UP! *****";
  }

}
