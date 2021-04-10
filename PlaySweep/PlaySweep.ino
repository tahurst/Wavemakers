#include "SweepData.h"
#include "MusicDefinitions.h"
#include "SoundData.h"
#include "XT_DAC_Audio.h"
#include "XT_DAC_Audio26.h"
#include "LiquidCrystal_I2C.h"
#include <WiFi.h>
#include <HTTPClient.h>

int debounce = 0;                                       //Variables used for timer
unsigned long start, finished, elapsed;
float h, m, s, ms;                             //create float variables (numbers with dec point)                 

int lcdColumns = 20;          //Sets the LCD's columns      //Variables used for LCD
int lcdRows = 4;              //Sets the LCD's rows
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);       //Sets the LCD address to the number of columns and rows

int switch_off = 0;         //Variables used for switches and buttons
int sound_num = 0;
int red_led_first = 0;        //Used to make sure RED Led is on when switch is on 
const int LEDPIN_RED = 23;      // We assigned a name LED pin to pin number 22
const int LEDPIN_GREEN = 17;  // We assigned a name LED pin to pin number 22
const int PushButton = 15;        // this will assign the name PushButton to pin numer 15
const int PowerSwitch = 14;

                                                      //Variables used to implement sound to be played via speakers
XT_Wav_Class One_TwoSweep(__1_2sweep_arduino_wav);     // create an object of type XT_Wav_Class that is used by 
XT_Wav_Class Two_ThreeSweep(__2_3sweep_arduino_wav);     // create an object of type XT_Wav_Class that is used by 
XT_Wav26_Class One_TwoSweep26(__1_2sweep_arduino_wav);     // create an object of type XT_Wav_Class that is used by 
XT_Wav26_Class Two_ThreeSweep26(__2_3sweep_arduino_wav);     // create an object of type XT_Wav_Class that is used by 
                    
// the dac audio class (below), passing wav data as parameter.
XT_DAC_Audio_Class DacAudio(25,0);    // Create the main player class object. Use GPIO 25, one of the 2 DAC pins and timer 0
XT_DAC_Audio26_Class DacAudio26(26,1);  // Create the main player class object. Use GPIO 26, one of the 2 DAC pins and timer 1
                    
XT_Sequence_Class Sequence;               // The sequence object, you add your sounds above to this object               
XT_Sequence26_Class Sequence26; 

String SoundFile;
const char * ssid = "ATTV87Nays";               //CHANGE THIS TO WIFI USERNAME
const char * password = "%maerirs7568";       //CHANGE THIS TO WIFI PASSWORD
String GOOGLE_SCRIPT_ID = "AKfycbzrU8E9q7NODo8dpO3MU_bklztTFDcNNHoleDN4pQJ0yZPSRpyBZmuF8M8HmgzMD-ec"; // Replace by your GAS service id

const char * root_ca=\
"-----BEGIN CERTIFICATE-----\n" \
"MIIDujCCAqKgAwIBAgILBAAAAAABD4Ym5g0wDQYJKoZIhvcNAQEFBQAwTDEgMB4G\n" \
"A1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjIxEzARBgNVBAoTCkdsb2JhbFNp\n" \
"Z24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDYxMjE1MDgwMDAwWhcNMjExMjE1\n" \
"MDgwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEG\n" \
"A1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZI\n" \
"hvcNAQEBBQADggEPADCCAQoCggEBAKbPJA6+Lm8omUVCxKs+IVSbC9N/hHD6ErPL\n" \
"v4dfxn+G07IwXNb9rfF73OX4YJYJkhD10FPe+3t+c4isUoh7SqbKSaZeqKeMWhG8\n" \
"eoLrvozps6yWJQeXSpkqBy+0Hne/ig+1AnwblrjFuTosvNYSuetZfeLQBoZfXklq\n" \
"tTleiDTsvHgMCJiEbKjNS7SgfQx5TfC4LcshytVsW33hoCmEofnTlEnLJGKRILzd\n" \
"C9XZzPnqJworc5HGnRusyMvo4KD0L5CLTfuwNhv2GXqF4G3yYROIXJ/gkwpRl4pa\n" \
"zq+r1feqCapgvdzZX99yqWATXgAByUr6P6TqBwMhAo6CygPCm48CAwEAAaOBnDCB\n" \
"mTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUm+IH\n" \
"V2ccHsBqBt5ZtJot39wZhi4wNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2NybC5n\n" \
"bG9iYWxzaWduLm5ldC9yb290LXIyLmNybDAfBgNVHSMEGDAWgBSb4gdXZxwewGoG\n" \
"3lm0mi3f3BmGLjANBgkqhkiG9w0BAQUFAAOCAQEAmYFThxxol4aR7OBKuEQLq4Gs\n" \
"J0/WwbgcQ3izDJr86iw8bmEbTUsp9Z8FHSbBuOmDAGJFtqkIk7mpM0sYmsL4h4hO\n" \
"291xNBrBVNpGP+DTKqttVCL1OmLNIG+6KYnX3ZHu01yiPqFbQfXf5WRDLenVOavS\n" \
"ot+3i9DAgBkcRcAtjOj4LaR0VknFBbVPFd5uRHg5h6h+u/N5GJG79G+dwfCMNYxd\n" \
"AfvDbbnvRG15RjF+Cv6pgsH/76tuIMRQyV+dTZsXjAzlAcmgQWpzU/qlULRuJQ/7\n" \
"TBj0/VLZjmmx6BEP3ojY+x1J96relc8geMJgEtslQIxq/H5COEBkEveegeGTLg==\n" \
"-----END CERTIFICATE-----\n";

WiFiClientSecure client;

void setup() {
  Serial.begin(115200);               // Not needed for sound, just to demo printing to the serial
                                      // Monitor whilst the sound plays, ensure your serial monitor speed is set to this speed also.
  
  lcd.begin();                         //Initializes the LCD
  lcd.backlight();                    //Turns on the LCD Backlight

  
               
  pinMode(LEDPIN_GREEN, OUTPUT);          // This statement will declare pin 21 as digital output
  pinMode(LEDPIN_RED, OUTPUT);          // This statement will declare pin 22 as digital output
  pinMode(PushButton, INPUT);       // This statement will declare pin 15 as digital input
  pinMode(PowerSwitch,INPUT); 
  Sequence.RepeatForever=true;    //Makes the sound repeat      
  Sequence26.RepeatForever=true;

  //delay(10);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  lcd.setCursor(0,3);
  lcd.print(" Connected to WIFI ");
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
    if (elapsed > 5){
        sendData("Duration=" + String(h) + "h_" + String(m) + "m_" + String(s) + "s" + "&Sound_File=" + SoundFile);     
      }
  }

  if ((sound_num == 1 || sound_num == 2)&&(debounce>100)) {
    finished = millis();
    displayResult();
    debounce = 0;
      if (sound_num == 1) {
        SoundFile = "1-2Sweep";
      }
      else {
        SoundFile = "2-3Sweep";
      }
      
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
}

void sendData(String params) {
   HTTPClient http;
   String url="https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+params;
   Serial.print(url);
    Serial.print("Making a request");
    http.begin(url, root_ca); //Specify the URL and certificate
    int httpCode = http.GET();  
    http.end();
    Serial.println(": done "+httpCode);
    lcd.setCursor(0,3);
    lcd.print("Data sent to Log");
}
