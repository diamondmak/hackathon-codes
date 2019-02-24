//GPS headerfiles start
#include <SoftwareSerial.h>
#include <TinyGPS.h>
//GPS headerfiles end
//GSM header files start
#include <SoftwareSerial.h>
//GSM header files end

//Gyro header files start
#include<Wire.h>
//Gyro header files end


//GPS Global var start
SoftwareSerial gpsSerial(11,12);
TinyGPS gps;
void gpsdump(TinyGPS &gps);
void printFloat(double f, int digits = 2);
//GPS Global var end

//GSM Global var start
SoftwareSerial gsmSerial(9, 10);
//GSM Global var end

//Gyro Global var start
const int MPU_addr=0x68;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
int minVal=265;
int maxVal=402;
double x;
double y;
double z;
//Gyro Global var end

void setup()  
{
  //GPS setup start
  Serial.begin(9600);
  gpsSerial.begin(9600);
  Serial.println(TinyGPS::library_version());
  Serial.println("by Team ver8tile");
  Serial.print("Sizeof(gpsobject) = "); 
  Serial.println(sizeof(TinyGPS));
  Serial.println();
  //GPS setup end 

 //GSM setup start
 gsmSerial.begin(9600);
 //GSM setup end

//Gyro setup start
 Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
//Gyro setup end
  
}

void loop() // run over and over
{

  int cpx,cpy,cpz;
  //GPS loop start
  gpsSerial.listen();
  Serial.println("Starting of void loop");
  bool newdata = false;
  unsigned long start = millis();
  // Every 5 seconds we print an update
  while (millis() - start < 5000) 
  {
    if (gpsSerial.available()) 
    
    {
      char c = gpsSerial.read();
      //Serial.print(c);  // uncomment to see raw GPS data
      if (gps.encode(c)) 
      {
        newdata = true;
        break;  // uncomment to print new data immediately!
      }
    }
  }
  
  if (newdata) 
  {
    Serial.println("Acquired Data");
    Serial.println("-------------");
    Serial.println("\n");
    gpsdump(gps);
    
    Serial.println("-------------");
    Serial.println();
  }

  //GPS loop end

//Gyro loop start
 Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);
  AcX=Wire.read()<<8|Wire.read();
  AcY=Wire.read()<<8|Wire.read();
  AcZ=Wire.read()<<8|Wire.read();
    int xAng = map(AcX,minVal,maxVal,-90,90);
    int yAng = map(AcY,minVal,maxVal,-90,90);
    int zAng = map(AcZ,minVal,maxVal,-90,90);

       x= RAD_TO_DEG * (atan2(-yAng, -zAng)+PI);
       y= RAD_TO_DEG * (atan2(-xAng, -zAng)+PI);
       z= RAD_TO_DEG * (atan2(-yAng, -xAng)+PI);
       cpx=x;
       cpy=y;
       cpz=z;

     Serial.print("AngleX= ");
     Serial.println(x);

     Serial.print("AngleY= ");
     Serial.println(y);

     Serial.print("AngleZ= ");
     Serial.println(z);
     Serial.println("-----------------------------------------");
     delay(400);
//Gyro loop end

//Debug Gyro start
Serial.print("Debug Gyro");
Serial.println(cpx);
Serial.println(cpy);
Serial.println(cpz);
delay(1000);
//Debug Gyro end

 //GSM loop start
 gsmSerial.listen();
 Serial.println("GSM data from loop:");
 Serial.println("Input S:");
 if (true){
  Serial.println("Inside GSM:");
  
    if((cpx>45&&cpx<315)||(cpy>50&&cpy<280)){
      Serial.println("Message Gone");
      SendMessage();
      delay(100000);
    }
 }

 if (gsmSerial.available()>0)
   Serial.write(gsmSerial.read());
 //GSM loop end

  
}

// GPS function files start
void gpsdump(TinyGPS &gps)
{
  Serial.println("In the gps dump");
  long lat, lon;
  float flat, flon;

  gps.f_get_position(&flat, &flon);
  Serial.print("Lat:"); 
  printFloat(flat, 5); 
  Serial.println("\n");
  Serial.print("Lon:");
  printFloat(flon, 5);
    }

void printFloat(double number, int digits)
{
  // Handle negative numbers
  if (number < 0.0) 
  {
     Serial.print('-');
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;
  
  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  Serial.print(int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0)
    Serial.print("."); 

  // Extract digits from the remainder one at a time
  while (digits-- > 0) 
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    Serial.print(toPrint);
    remainder -= toPrint;
  }
}

//GPS function files end
 void SendMessage()
{
  gsmSerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  gsmSerial.println("AT+CMGS=\"+919616481277\"\r"); // Replace x with mobile number
  delay(1000);
  gsmSerial.println("Emergency Situation at: Lat: ,Lon: ");// The SMS text you want to send
  delay(100);
   gsmSerial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}
//GSM function files start

//GSM function files end
