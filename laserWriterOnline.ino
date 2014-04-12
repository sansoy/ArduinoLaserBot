#include <SPI.h>
#include <Ethernet.h>
#include <Servo.h>
#include <avr/pgmspace.h>

const int MAX_PAGENAME_LEN = 8; // max characters in page name 
char buffer[MAX_PAGENAME_LEN+1]; // additional character for terminating null
#define P(name)   static const prog_uchar name[] PROGMEM  // declare a static string

//SETUP WEBSERVER
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip;  
EthernetServer server(80);
EthernetClient client;

Servo servoYaw;
Servo servoPitch;
int laserPin = 9;

float fontSize;
int wordLength = 1;
int kerning = .41;

float spacing = 40;
float canvasWidth  = 500;           
float canvasHeight = 500;
float distanceToScreen = 1000;

int minYaw = 45;
int maxYaw = 135;
int servoYawHalfAngle = 0;
int minServoYawDegrees = minYaw;
int maxServoYawDegrees = maxYaw;

int minPitch = 45;
int maxPitch = 135;
int servoPitchHalfAngle = 0;
int minServoPitchDegrees = minPitch;
int maxServoPitchDegrees = maxPitch;

int initialServoYaw, initialServoPitch;
int laserYaw, laserPitch; 

PROGMEM  prog_uint16_t strokes[26][10][3] = {
                      {{0,100,0},{0,100,1},{50,0,1},{100,100,1},{25,50,0},{75,50,1},{75,50,0}},
                      {{0,0,0},{0,0,1},{0,100,1},{50,75,1},{0,50,1},{50,25,1},{0,0,1},{0,0,0}}, 
                      {{100,0,0},{100,0,1},{0,0,1},{0,100,1},{100,100,1},{100,0,0}}, 
                      {{0,0,0},{0,100,1},{100,50,0},{0,100,1},{100,50,0},{0,0,1},{0,0,0}}, 
                      {{0,0,0},{0,0,1},{0,100,1},{0,0,0},{100,0,1},{0,50,0},{100,50,1},{0,100,0},{100,100,1},{0,0,0}}, 
                      {{0,0,0},{0,0,1},{0,100,1},{0,0,0},{100,0,1},{0,50,0},{100,50,1},{0,0,0}},
                      {{100,0,0},{100,0,1},{0,0,1},{0,100,1},{100,100,1},{100,50,1},{50,50,1},{50,50,0}},
                      {{0,0,0},{0,0,1},{0,100,1},{100,0,0},{100,100,1},{0,50,0},{100,50,1}, {1,.5, 0}},
                      {{0,0,0},{0,0,1},{100,0,1},{0,100,0},{100,100,1},{50,0,0},{50,100,1},{50,100,0}},
                      {{0,0,0},{0,0,1},{100,0,1},{50,0,0},{50,100,1},{25,100,1},{25,75,1},{25,75,0}},
                      {{0,0,0},{0,0,1},{0,100,1},{100,0,0},{0,50,1},{100,100,1},{100,100,0}},
                      {{0,0,0},{0,0,1},{0,100,1},{100,100,1},{100,100,0}},
                      {{0,100,0},{0,100,1},{0,0,1},{50,100,1},{100,0,1},{100,100,1},{100,100,0}},
                      {{0,100,0},{0,100,1},{0,0,1},{100,100,1},{100,0,1},{100,0,1},{100,0,0}},
                      {{0,0,0},{0,0,1},{0,0,1},{0,100,1},{100,100,1},{100,0,1},{0,0,1},{0,0,0}},
                      {{0,100,0},{0,100,1},{0,0,1},{100,0,1},{100,50,1},{0,50,1},{0,50,0}},
                      {{0,0,0},{0,0,1},{0,80,1},{80,80,1},{80,0,1},{0,0,1},{50,50,0},{100,100,1},{100,100,0}},
                      {{0,100,0},{0,100,1},{0,0,1},{100,0,1},{100,50,1},{0,50,1},{100,100,1},{100,100,0}},
                      {{100,0,0},{100,0,1},{0,0,1},{0,50,1},{100,50,1},{100,100,1},{0,100,1},{0,100,0}}, 
                      {{0,0,0},{0,0,1},{100,0,1},{50,0,0},{50,100,1},{50,100,0}}, 
                      {{0,0,0},{0,0,1},{0,100,1},{100,100,1},{100,0,1},{100,0,0}},
                      {{0,0,0},{0,0,1},{50,100,1},{100,0,1},{100,0,0}},
                      {{0,0,0},{0,0,1},{25,100,1},{50,50,1},{75,100,1},{100,0,1},{100,0,0}},
                      {{0,0,0},{0,0,1},{100,100,1},{100,0,0},{0,100,1},{0,100,0}},
                      {{0,0,0},{0,0,1},{50,50,1},{100,0,1},{50,50,0},{50,100,1},{50,100,0}},
                      {{0,0,0},{0,0,1},{100,0,1},{0,100,1},{100,100,1},{100,100,0}},
                    };
                    
PROGMEM  prog_uint16_t strokes_per_letter[26] = {7,8,6,7,10,8,8,8,8,8,7,5,7,7,8,7,9,8,8,6,6,5,7,6,7,6};
String alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

int currentLetterIndex;
String eachLetter;

int numbOfStrokes; 

String theLetters;
int numbOfChar;

float pitch,yaw;
int laserState;

int i,j;

String POST;
//String POST = "words=deutsch";

void setup() {
  
  Serial.begin(9600);

  // Start the Ethernet connection and the server:z
  Ethernet.begin(mac);
  server.begin();
  Serial.println(F("Ready"));
  Serial.println(F("server is at "));
  Serial.println(Ethernet.localIP());
  
  //SETUP LASER SERVOS
  servoYaw.attach(5);
  servoPitch.attach(6);
  pinMode(laserPin, OUTPUT);
   
  //INITIALIZE to Minimum Angle State
  servoYaw.write(minYaw);
  servoPitch.write(minPitch);
  digitalWrite(laserPin, HIGH);
  
  //YAW SERVO SETUP
  servoYawHalfAngle = int(atan((canvasWidth/2)/distanceToScreen) * 57.2957795);

  minServoYawDegrees = 90 - servoYawHalfAngle;
  if (minServoYawDegrees < minYaw ) minServoYawDegrees = 45;

  maxServoYawDegrees = 90 + servoYawHalfAngle;
  if (maxServoYawDegrees > maxYaw) maxServoYawDegrees = 135;


  //PITCH SERVO SETUP
  servoPitchHalfAngle = int(atan((canvasHeight/2)/distanceToScreen) * 57.2957795);

  minServoPitchDegrees = 90 - servoPitchHalfAngle;
  if (minServoPitchDegrees < minPitch ) minServoPitchDegrees = 45;

  maxServoPitchDegrees = 90 + servoPitchHalfAngle;
  if (maxServoPitchDegrees > maxPitch) maxServoPitchDegrees = 135;
  
  
  delay(1000);
  
}

void loop(){
  
  client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
          memset(buffer,0, sizeof(buffer)); // clear the buffer
          if(client.readBytesUntil('/', buffer,MAX_PAGENAME_LEN)){ 
            if(strcmp(buffer,"POST ") == 0){
             laserWriter();
            }
          }
        }
        break;
      }
      sendHeader("Laser Writer");
      htmlButton();
    }
    delay(1);
    client.stop();
}
          

void sendHeader(char *title)
{
  client.println(F("HTTP/1.1 200 OK"));
  client.println(F("Content-Type: text/html"));
  client.println();
  client.print(F("<html><head><title>"));
  client.println(title);
  client.println(F("</title><body>"));
 
}

void htmlButton()
{
           client.println(F("<h2>Deutsch Laser Writer</h2>"));
           client.println(F("<form action='/' method='post'>"));      
           client.println(F("<input type='text' name='words'>"));
           client.println(F("<br />"));
           client.println(F("<input type='submit' onClick=\"this.form.submit(); this.disabled=true; this.value=\'Sending\';\" >"));
           client.println(F("</form>"));
           client.println(F("</body></html>"));
}

void laserWriter(){
  
    client.find("\n\r"); // skip to the body
    while(client.findUntil("words=", "\n\r")){
          theLetters = client.readString();
    }

    numbOfChar = theLetters.length();
    
    Serial.println(theLetters);
    
    fontSize = int((canvasWidth / numbOfChar)*(1 - kerning));
    
    for (i=0;i<numbOfChar;i++){
      eachLetter = theLetters.substring(i,i+1);
      eachLetter.toUpperCase();

      currentLetterIndex = alphabet.indexOf(eachLetter);
      
      numbOfStrokes = pgm_read_word_near(strokes_per_letter + currentLetterIndex);
  
      for(j=0;j<numbOfStrokes;j++){
        
        yaw = (pgm_read_word_near(&strokes[currentLetterIndex][j][0]))/100.0;
        pitch = (pgm_read_word_near(&strokes[currentLetterIndex][j][1]))/100.0;
        laserState = pgm_read_word_near(&strokes[currentLetterIndex][j][2]);
  
        laserYaw = int(map(round(yaw*fontSize+spacing),0,canvasWidth,minServoYawDegrees,maxServoYawDegrees));
        laserPitch = int(map(round(pitch*fontSize+canvasHeight*.1),0,canvasHeight,minServoPitchDegrees,maxServoPitchDegrees));
   
        servoYaw.write(laserYaw);
        servoPitch.write(laserPitch);
        
        if(laserState == 1){
                  digitalWrite(laserPin, LOW);
        } else {
                  digitalWrite(laserPin, HIGH);
        }
        delay(500);
      }
      
      spacing = spacing + fontSize*(1.1);
     
    }
    spacing = 40;

    servoYaw.write(minYaw);
    servoPitch.write(minPitch);
    digitalWrite(12, HIGH);

}
