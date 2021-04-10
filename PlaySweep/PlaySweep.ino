#include "SweepData.h"
#include "SoundData.h"
#include "XT_DAC_Audio.h"
#include "XT_DAC_Audio26.h"
#include "LiquidCrystal_I2C.h"

int debounce=0;
unsigned long start, finished, elapsed;                 //Variables used for timer

int lcdColumns = 20;          //Sets the LCD's columns
int lcdRows = 4;              //Sets the LCD's rows
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);       //Sets the LCD address to the number of columns and rows

int switch_off = 0;
int sound_num = 0;
int red_led_first = 0;        //Used to make sure RED Led is on when switch is on 
const int LEDPIN_RED = 23;      // We assigned a name LED pin to pin number 22
const int LEDPIN_GREEN = 17;  // We assigned a name LED pin to pin number 22
const int PushButton = 15;        // this will assign the name PushButton to pin numer 15
const int PowerSwitch = 14;
                            // This Setup function is used to initialize everything 

XT_Wav_Class One_TwoSweep(__1_2sweep_arduino_wav);     // create an object of type XT_Wav_Class that is used by 
XT_Wav_Class Two_ThreeSweep(__2_3sweep_arduino_wav);     // create an object of type XT_Wav_Class that is used by 

XT_Wav26_Class One_TwoSweep26(__1_2sweep_arduino_wav);     // create an object of type XT_Wav_Class that is used by 
XT_Wav26_Class Two_ThreeSweep26(__2_3sweep_arduino_wav);     // create an object of type XT_Wav_Class that is used by 
                    
                                        // the dac audio class (below), passing wav data as parameter.
                                      
XT_DAC_Audio_Class DacAudio(25,0);    // Create the main player class object. 
                                      // Use GPIO 25, one of the 2 DAC pins and timer 0
XT_DAC_Audio26_Class DacAudio26(26,1); 

uint32_t DemoCounter=0;               // Just a counter to use in the serial monitor
                                      // not essential to playing the sound
                    
XT_Sequence_Class Sequence;               // The sequence object, you add your sounds above to this object (see setup below)                  

XT_Sequence26_Class Sequence26; 


void setup() {
  Serial.begin(115200);               // Not needed for sound, just to demo printing to the serial
                                      // Monitor whilst the sound plays, ensure your serial monitor
                                      // speed is set to this speed also.
  lcd.begin();                         //Initializes the LCD
  lcd.backlight();                    //Turns on the LCD Backlight

  
               
  pinMode(LEDPIN_GREEN, OUTPUT);          // This statement will declare pin 21 as digital output
  pinMode(LEDPIN_RED, OUTPUT);          // This statement will declare pin 22 as digital output
  pinMode(PushButton, INPUT);       // This statement will declare pin 15 as digital input
  pinMode(PowerSwitch,INPUT); 
  Sequence.RepeatForever=true;    //Makes the sound repeat      
  Sequence26.RepeatForever=true;                       
}


void loop() {
  DacAudio.FillBuffer();                // Fill the sound buffer with data
  DacAudio26.FillBuffer26();
  int Push_button_state = digitalRead(PushButton);  // digitalRead function stores the Push button state in variable push_button_state
  int Power_Switch_state = digitalRead(PowerSwitch);
  
  if (Power_Switch_state == HIGH){
  if(red_led_first==0){                     //Marks that device is on
        lcd.clear();
        switch_off = 0;
        digitalWrite(LEDPIN_GREEN,LOW);
        digitalWrite(LEDPIN_RED,HIGH);
        red_led_first = 1;
        lcd.setCursor(0,0);
        lcd.print("WaveMaker ON ");
  }
  
  if (Push_button_state == HIGH) {            //Checks if button is pressed

    
    start = millis();
    delay(200);  
    lcd.clear();     
    sound_num++;                          //Increments counter
    if(sound_num == 3) {                    //Resets counter if needed
      sound_num = 0; 
      red_led_first = 0; 
      }
    PlayNumber(sound_num);                //Plays corresponding sound depending on how many times button is pushed
  }

  if ((sound_num == 1 || sound_num == 2)&&(debounce>100)) {
    finished = millis();
    displayResult();
    debounce = 0;
  } 
  else debounce++;
}
else{
      if (switch_off == 0) {
      sound_num = 0;  
      PlayNumber(sound_num);                      //Plays nothing
      lcd.clear();
      red_led_first = 0;
      digitalWrite(LEDPIN_RED,LOW);
      digitalWrite(LEDPIN_GREEN,LOW);
      lcd.setCursor(0,0);
      lcd.print("WaveMaker OFF");
      switch_off = 1;
      }
}

}

void PlayNumber(int Number){          //Plays new sound from Sequence
  Sequence.RemoveAllPlayItems();      //Stops any sound
  Sequence26.RemoveAllPlayItems();
  AddNumberToSequence(Number);
  DacAudio.Play(&Sequence);
  DacAudio26.Play(&Sequence26);
  Serial.println(Number);
}

void AddNumberToSequence(int TheNumber){                  //Adds sounds depending on the number of times button is pushed
  switch(TheNumber){
    case 1 : Sequence.AddPlayItem(&One_TwoSweep); Sequence26.AddPlayItem(&One_TwoSweep26); digitalWrite(LEDPIN_RED,LOW); digitalWrite(LEDPIN_GREEN,HIGH); lcd.setCursor(0,0); lcd.print("1-2Sweep ON"); break;     
    case 2 : Sequence.AddPlayItem(&Two_ThreeSweep); Sequence26.AddPlayItem(&Two_ThreeSweep26); lcd.setCursor(0,0); lcd.print("2-3Sweep ON"); break;
  }
}

void displayResult()                            //create a function called displayResult
{
 float h, m, s, ms;                             //create float variables (numbers with dec point)
 unsigned long over;                            //create unsigned long integer variable over
 elapsed = finished - start;                               //elapsed time is finished minus start
 h = int(elapsed / 3600000);                    //h is the integer created by dividing elapsed by 360000
 over = elapsed % 3600000;                      //over is created by elapsed modulo 360000
 m = int(over / 60000);                         //m is the integer created by dividing over (seconds over 60) by 60000
 over = over % 60000;                           //new over is over modulo 60000
 s = int(over / 1000);                          //seconds is the integer created by dividing over by 1000
 ms = over % 1000;                              //ms is created by new over modulo 1000
 lcd.setCursor(0,1);
 lcd.print("Run time: ");               //send a title for elapsed to display buffer
 lcd.setCursor(0,2);
 lcd.print(h);                           //send value to display buffer, no decimals
 lcd.print("h ");
 lcd.print(m);                           //send value to display buffer, no decimals
 lcd.print("m ");
 lcd.print(s);                           //send value to display buffer, no decimals
 lcd.print("s ");
// lcd.print(ms);                          //send value to display buffer, no decimals
// lcd.print("ms");
}
