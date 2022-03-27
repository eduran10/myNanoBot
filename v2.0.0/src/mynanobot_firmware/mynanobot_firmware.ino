/**************************************************************************************
* File Name          : mynanobot_firmware.ino
* Author             : Erkan DURAN
* Updated            : Erkan DURAN
* Version            : v2.0.0
* Date               : 09/04/2021
* Description        : Firmware for myNanoBot (Arduino Nano Based Educational Robot) with mBlock 
* License            : CC-BY-SA 3.0 (https://creativecommons.org/licenses/by-sa/3.0/)
* E-mail             : duran.erkan@gmail.com
* Homepage           : https://erkanduran.wordpress.com
* Github             : https://github.com/eduran10/mynanobot		
***************************************************************************************	 
*** This firmware is based Makeblock mBot/orion board firmware by Ander, Mark Yan.
*** https://github.com/Makeblock-official/Makeblock-Libraries/blob/master/examples/Firmware_For_mBlock/mbot_firmware/mbot_firmware.ino
*** https://github.com/Makeblock-official/Makeblock-Libraries/blob/master/examples/Firmware_For_mBlock/orion_firmware/orion_firmware.ino
*** This firmware uses the mBot serial port protocol.
*** mBot serial port protocol and mbot/orion firmware have been adapted to myNanoBot.
***************************************************************************************/


union{
  byte byteVal[2];
  short shortVal;
}valShort;

union{
    byte byteVal[4];
    float floatVal;
    long longVal;
}val;

union{
  byte byteVal[8];
  double doubleVal;
}valDouble;

#if defined(__AVR_ATmega328P__) or defined(__AVR_ATmega168__)
  int analogs[8]={A0,A1,A2,A3,A4,A5,A6,A7};
#endif

#if defined(__AVR_ATmega32U4__) 
  int analogs[12]={A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11};
#endif

#if defined(__AVR_ATmega1280__)|| defined(__AVR_ATmega2560__)
  int analogs[16]={A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15};
#endif


/*myNanoBot Arduino Pins*/  
#define IN1 2
#define IN2 3
#define DLDR 4
#define ENA 5
#define ENB 6
#define IN3 7
#define IN4 8
#define RRGB 9
#define GRGB 10
#define BRGB 11
#define BZZR 12
#define TRIG A0  //Analog - A0
#define ECHO A1  //Analog - A1
#define ALDR A2  //Analog - A2  
#define DSND A3  //Analog - A3
#define ASND A4  //Analog - A4

#define VERSION 0
#define ULTRASONIC_SENSOR 1
#define LIGHT_SENSOR 3
#define JOYSTICK 5
#define SOUND_SENSOR 7
#define RGBLED 8
#define MOTOR 10
#define LIGHT_SENSOR_D 29
#define DIGITAL 30
#define ANALOG 31
#define PWM 32
#define TONE 34
#define SOUND_SENSOR_D 39

#define GET 1
#define RUN 2
#define RESET 4
#define START 5

#define RIGHTDC 1
#define LEFTDC 3

#define BACKWARD 98
#define LEFT 108
#define FORWARD 102
#define RIGHT 114
#define STOP 115


String mVersion = "2.0.0";
boolean isAvailable = false;
boolean isStart = false;
int len = 52;
char buffer[52];
char serialRead;
byte index = 0;
byte dataLen;
unsigned char prevc=0;


void writeBuffer(int index,unsigned char c){
  buffer[index]=c;
}

unsigned char readBuffer(int index){
  return buffer[index];   
}

void writeHead(){
  writeSerial(0xff);
  writeSerial(0x55);
}

void writeEnd(){
  Serial.println(); 
}

void callOK(){
  writeSerial(0xff);
  writeSerial(0x55);
  writeEnd();
}

void writeSerial(unsigned char c){
  Serial.write(c);
}

void readSerial(){
  isAvailable = false;
  if(Serial.available()>0){
    isAvailable = true;
    serialRead = Serial.read();
  }
}

void sendByte(char c){
  writeSerial(1);
  writeSerial(c);
}

void sendShort(double value){
  writeSerial(3);
  valShort.shortVal = value;
  writeSerial(valShort.byteVal[0]);
  writeSerial(valShort.byteVal[1]);
}

void sendFloat(float value){ 
  writeSerial(0x2);
  val.floatVal = value;
  writeSerial(val.byteVal[0]);
  writeSerial(val.byteVal[1]);
  writeSerial(val.byteVal[2]);
  writeSerial(val.byteVal[3]);
}

void sendDouble(double value){
  writeSerial(2);
  valDouble.doubleVal = value;
  writeSerial(valDouble.byteVal[0]);
  writeSerial(valDouble.byteVal[1]);
  writeSerial(valDouble.byteVal[2]);
  writeSerial(valDouble.byteVal[3]);
}

void sendString(String s){
  int l = s.length();
  writeSerial(4);
  writeSerial(l);
  for(int i=0;i<l;i++){
    writeSerial(s.charAt(i));
  }
}

short readShort(int idx){
  valShort.byteVal[0] = readBuffer(idx);
  valShort.byteVal[1] = readBuffer(idx+1);
  return valShort.shortVal; 
}

float readFloat(int idx){
  val.byteVal[0] = readBuffer(idx);
  val.byteVal[1] = readBuffer(idx+1);
  val.byteVal[2] = readBuffer(idx+2);
  val.byteVal[3] = readBuffer(idx+3);
  return val.floatVal;
}

long readLong(int idx){
  val.byteVal[0] = readBuffer(idx);
  val.byteVal[1] = readBuffer(idx+1);
  val.byteVal[2] = readBuffer(idx+2);
  val.byteVal[3] = readBuffer(idx+3);
  return val.longVal;
}

void setup(){  
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  pinMode(ENA,OUTPUT);
  pinMode(ENB,OUTPUT);  
  pinMode(RRGB,OUTPUT);
  pinMode(GRGB,OUTPUT);
  pinMode(BRGB,OUTPUT);  
  pinMode(BZZR,OUTPUT);
  pinMode(TRIG,OUTPUT);
  pinMode(ECHO,INPUT);
  pinMode(DLDR,INPUT);
  pinMode(ALDR,INPUT);
  pinMode(DSND,INPUT);
  pinMode(ASND,INPUT);  
  
  pinMode(13,OUTPUT);
  analogWrite(GRGB,255);
  digitalWrite(13,HIGH);
  delay(500);
  analogWrite(GRGB,0);
  digitalWrite(13,LOW);
  
  Serial.begin(115200);
  delay(500);
  tone(BZZR, 2637, 125);  
  delay(125);
  tone(BZZR, 2349, 125);
  delay(125);
  tone(BZZR, 2093, 125);  
  delay(125);
  
  Serial.print("Version: ");
  Serial.println(mVersion);
}

  /*******************************************
      ff 55 len idx action device pin/data data..
      0  1  2   3   4      5      6        7    
  ********************************************/	
void loop(){	
  readSerial(); 
  if(isAvailable){
    unsigned char c = serialRead&0xff;
    if(c==0x55&&isStart==false){
      if(prevc==0xff){
        index=1;
        isStart = true;
      }
    }else {
      prevc = c;
      if(isStart){
        if(index==2){
          dataLen = c; 
        }else if(index>2){
          dataLen--;
        }
        writeBuffer(index,c);
      }
    }
    index++;
    if(index>51){
      index=0; 
      isStart=false;
    }
    if(isStart&&dataLen==0&&index>3){ 
      isStart = false;
      parseData(); 
      index=0;
    }
  }
}


void parseData(){
  /*******************************************
      ff 55 len idx action device pin/data data..
      0  1  2   3   4      5      6        7    
  ********************************************/	
  isStart = false;
  int idx = readBuffer(3);
  int action = readBuffer(4);
  int device = readBuffer(5);
  switch(action){
    case GET:{ 
      writeHead();
      writeSerial(idx);
      readSensor(device);
      writeEnd();
    }
    break;
    case RUN:{
      runModule(device);
      callOK();
    }
    break;
    case RESET:{
      runMotor(LEFTDC,STOP,0);
      runMotor(RIGHTDC,STOP,0);   
      digitalWrite(RRGB,0);
      digitalWrite(GRGB,0);
      digitalWrite(BRGB,0);
      noTone(BZZR);
      callOK();
    }
    break;
    case START:{
      //start
      callOK();
    }
    break;
  }
}


void readSensor(int device){
  /*******************************************
      ff 55 len idx action device pin/data data..
      0  1  2   3   4      5      6        7    
  ********************************************/	

  switch(device){
    case ULTRASONIC_SENSOR:{     
      digitalWrite(TRIG,LOW);
      delayMicroseconds(2);
      digitalWrite(TRIG,HIGH);
      delayMicroseconds(10);
      digitalWrite(TRIG,LOW);
      double distance = pulseIn(ECHO,HIGH,30000)/58.27;
      if (distance == 0) distance = 400;
      sendFloat(round(distance*1000)/1000.0);
    }
    break;
    case LIGHT_SENSOR_D: {
   	  sendFloat(digitalRead(DLDR));   
    }
    break;
    case LIGHT_SENSOR:{
      sendFloat(analogRead(ALDR));
    }
    break; 
    case SOUND_SENSOR_D:{
      sendFloat(digitalRead(DSND));
    }
    break; 	
    case SOUND_SENSOR:{
      sendFloat(analogRead(ASND));
    }
    break; 	
    case DIGITAL:{
      int pin = readBuffer(6);	
      pinMode(pin,INPUT);
      sendFloat(digitalRead(pin));
    }
    break;
    case ANALOG:{
      int pin = readBuffer(6);	
      pin = analogs[pin];
      pinMode(pin,INPUT);
      sendFloat(analogRead(pin));
    }
    break; 
    case VERSION:{
      sendString(mVersion);
    }
    break;   
  }  
}


void runMotor(int dcMotor, int direction, int speed) {
  if (dcMotor == RIGHTDC) {  		
  	if (direction == FORWARD) {
  	  digitalWrite(IN1, 1);
  	  digitalWrite(IN2, 0);			  
  	}else if (direction == BACKWARD) {
  	  digitalWrite(IN1, 0);
  	  digitalWrite(IN2, 1);	
  	}else { //STOP
  	  digitalWrite(IN1, 0);
  	  digitalWrite(IN2, 0);			
  	}
  	analogWrite(ENA, speed);
  } else if (dcMotor == LEFTDC) {
	if (direction == FORWARD) {
	  digitalWrite(IN3, 1);
	  digitalWrite(IN4, 0);			  		  
	}else if (direction == BACKWARD){ 
	  digitalWrite(IN3, 0);
	  digitalWrite(IN4, 1);			  
	}else { //STOP
	  digitalWrite(IN3, 0);
	  digitalWrite(IN4, 0);		
	}		
	analogWrite(ENB, speed);		
  }
}


void runModule(int device){
  /*******************************************
      ff 55 len idx action device pin/data data..
      0  1  2   3   4      5      6        7    
  ********************************************/	
  
  switch(device){
    case MOTOR:{ 		
      int dcMotor = readBuffer(6);
      int direction = readBuffer(7);	  
      int speed = readBuffer(8);

      runMotor(dcMotor,direction,speed);
    }
    break;	  
    case JOYSTICK:{
      int direction = readBuffer(6);
      int speed = readBuffer(7);
		
      switch(direction) {
        case FORWARD:{
          runMotor(LEFTDC,FORWARD,speed);
          runMotor(RIGHTDC,FORWARD,speed);		
        }
        break;
        case BACKWARD:{
          runMotor(LEFTDC,BACKWARD,speed);
          runMotor(RIGHTDC,BACKWARD,speed);			
        }
        break;
        case RIGHT:{
          runMotor(LEFTDC,FORWARD,speed);
          runMotor(RIGHTDC,BACKWARD,speed);					
        }
        break;
        case LEFT:{
          runMotor(LEFTDC,BACKWARD,speed);
          runMotor(RIGHTDC,FORWARD,speed);				
        }
        break;
        default:{ //STOP
          runMotor(LEFTDC,STOP,0);
          runMotor(RIGHTDC,STOP,0);			
        }				
      }
    }
    break; 
    case RGBLED:{
      int r = readBuffer(6);
      int g = readBuffer(7);
      int b = readBuffer(8);
   
      analogWrite(RRGB,r);
      analogWrite(GRGB,g);
      analogWrite(BRGB,b);  
    }
    break;
    case TONE:{
      int hz = readShort(6);
      int ms = readShort(8);
      if(ms>0){
        tone(BZZR, hz, ms); 
      }else{
        noTone(BZZR); 
      }
    }
    break;
    case DIGITAL:{
      int pin = readBuffer(6);
      int v = readBuffer(7);
      pinMode(pin,OUTPUT);      
      digitalWrite(pin,v);
    }
    break;
    case PWM:{
      int pin = readBuffer(6);
      int v = readBuffer(7);
      pinMode(pin,OUTPUT);      
      analogWrite(pin,v);
    }
    break;     
  }
}
