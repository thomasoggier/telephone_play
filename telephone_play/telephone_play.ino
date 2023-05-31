/*************************************************** 
  This is an example for the Adafruit VS1053 Codec Breakout

  Designed specifically to work with the Adafruit VS1053 Codec Breakout 
  ----> https://www.adafruit.com/products/1381

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

// This is a very beta demo of Ogg Vorbis recording. It works...
// Connect a button to digital 7 on the Arduino and use that to
// start and stop recording.

// A mic or line-in connection is required. See page 13 of the 
// datasheet for wiring

// Don't forget to copy the v44k1q05.img patch to your micro SD 
// card before running this example!


// include SPI, MP3 and SD libraries
#include <SPI.h>
#include "Adafruit_VS1053.h"
#include <SD.h>

// define the pins used
#define RESET 9      // VS1053 reset pin (output)
#define CS 10        // VS1053 chip select pin (output)
#define DCS 8        // VS1053 Data/command select pin (output)
#define CARDCS 4     // Card chip select pin
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

#define REC_BUTTON 7
#define PLAY_BUTTON 6


#define PIN_A A4
#define PIN_B A5

  int counter_old;
  int inprogress_old;
  int count;

Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(RESET, CS, DCS, DREQ, CARDCS);

File recording;  // the file we will save our recording to
#define RECBUFFSIZE 128  // 64 or 128 bytes.
uint8_t recording_buffer[RECBUFFSIZE];

void setup() {
  Serial.begin(115200);
  Serial.println(F("Adafruit VS1053 Ogg Recording Test"));


  // initialise the music player
  Serial.println(F("musicPlayer Begin!"));
  if (!musicPlayer.begin()) {
    Serial.println(F("VS1053 not found"));
    while (1);  // don't do anything more
  }
  
  musicPlayer.reset ();
  delay(100);
  musicPlayer.softReset ();
  delay(100);
 musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);
  Serial.println(F("SD Begin!"));
  if (!SD.begin(CARDCS)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }
  Serial.println(F("SD OK!"));
  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(10,10);
  


  pinMode(PIN_A, INPUT);
  digitalWrite(PIN_A, INPUT_PULLUP);
  pinMode(PIN_B, INPUT);
  digitalWrite(PIN_B, INPUT_PULLUP);

  inprogress_old =digitalRead(PIN_A);
  counter_old = digitalRead(PIN_B);
  count = 0;


  
}

uint8_t isRecording = false;

char filename[15];
void loop() {
  //delay(500);
  
  int inprogress = digitalRead(PIN_A);
  int counter = digitalRead(PIN_B);
  //Serial.print("inprogress");
  //Serial.println(inprogress);
  //Serial.print("counter");
  //Serial.println(counter);
  if(!inprogress)
  {
    musicPlayer.pausePlaying(true);
    //Serial.println("pause");
    if (counter != counter_old)
    {
      if (!counter)
      {
        count ++;
        if(count%10==0)
          count = count-10;
        Serial.println("count ++");
        delay(10);
      }
    }
  }
  else
  {
    if(!inprogress_old)
    {

      for(int i = 0; i < 200; i++)
      {
        delay(10);
        //Serial.print("Wait new number: ");
        inprogress = digitalRead(PIN_A);
        //Serial.println(inprogress);
        if(!inprogress) // Add new number
          break;
      }
      
      if(!inprogress)
      {
        count = count*10;
        Serial.print("count * 10: ");
        Serial.println(count);
      }
      else
      {
        // Start to play
        char filename[20];
        sprintf(filename, "task%03d.mp3", count);
        Serial.print("play sound ");
        Serial.println(filename);
        if(!musicPlayer.startPlayingFile(filename))
        {
          Serial.println("not found");
          musicPlayer.sineTest(0x44, 500);
        }
        count = 0;
        delay(10);
       }
    }
  }
  counter_old = counter;
  inprogress_old = inprogress;
}
