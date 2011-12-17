#include <XBee.h>                //http://code.google.com/p/xbee-arduino/
#include <math.h>

/* Debugging! */

#define debuggingsensors false // if true turns off xbee and debuggs the sensors in serial


/* Settings */


// PACHUBE DEF

#define PACHUBE_FEED 25978  //Pachube feed number to send the data to
XBeeAddress64 coordAddr = XBeeAddress64(0x0013A200, 0x407A1FB0);  //Network coordinator forwards data to Pachube

#define transmitpachube 10 // every 2 sensor readings

#define readsensors 1000 // one second

// SENSOR DEF

#define vref 5 // For the computeVolts(); funtcion.

#define micpin A0
#define lightpin A1
#define temppin A2
#define tgspin A3
#define humpin A4 // Pacthed to ENERGY.
#define micspin A5

#define figaroheater 5 // Heat VCC tgs 2442
#define figarocircuit 6 // Sensing VCC tgs 2442


#define sensorreadings 10

#define blinkpin 13  //heartbeat LED must change. Use it by the GPS.

//other global variables
unsigned long msNow, msLast;     //for timing transmissions, etc.

int temp;
int humidity;
int light;
int sound;
float tgs2442;
float tgs4161;

//XBee global variables
#define XBEE_PAYLOAD_LEN 40      //this should agree with the definition in the remote code
XBee xbee = XBee();              //create the XBee object
union {
  byte B; 
  char C;
} 

xbeePayload[XBEE_PAYLOAD_LEN];
ZBTxRequest zbTx = ZBTxRequest();
ZBTxStatusResponse txStatus = ZBTxStatusResponse();
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();




void setup() {
  pinMode(micpin, INPUT);             
  pinMode(lightpin, INPUT);             
  pinMode(temppin, INPUT);            
  pinMode(tgspin, INPUT);  
  pinMode(humpin, INPUT);    
  pinMode(micspin, INPUT);   

  pinMode(figaroheater, OUTPUT);
  pinMode(figarocircuit, OUTPUT);    
  pinMode(blinkpin, OUTPUT);

  if (debuggingsensors == false) {                  
    xbee.begin(9600);               //does Serial.begin()
    delay(5000);                    //give the XBee time to associate
  } 
  else {
    Serial.begin(9600);               //does Serial.begin()
  }

}




void loop(void) {
  static boolean blink;
  static int seconds;             //counts seconds so we can transmit once a minute

  msNow = millis();

  if (debuggingsensors == false) {                  
    readXBee();                                  //check for incoming XBee traffic
  }


  if (msNow > msLast + readsensors || debuggingsensors == true) {                 //defined at readsensors

    msLast = msNow;


    temp = getTemp(temppin, sensorreadings); 
    humidity = getHumidity(humpin, sensorreadings); 
    light = getLight(lightpin, sensorreadings); 
    sound = getSound(micpin); 
//    tgs2442 = getTgs2442(tgspin, sensorreadings); 
    tgs4161 = getTgs4161(tgspin); 


    if (seconds++ == transmitpachube && debuggingsensors == false) {                  
      seconds = 0;
      txPachube();
    }

    if (debuggingsensors == true) { 
      Serial.print("Temp: ");      
      Serial.print(temp);   
      Serial.print(" ");              
      Serial.print("Hum: ");      
      Serial.print(humidity);
      Serial.print(" ");                
      Serial.print("Light: ");      
      Serial.print(light);   
      Serial.print(" ");             
      Serial.print("Sound: ");      
      Serial.print(sound); 
      Serial.print(" ");                 
      Serial.print("Tgs2442: ");      
      Serial.print(tgs2442);  
      Serial.print(" ");              
      Serial.print("tgs4161: ");      
      Serial.println(tgs4161);        
    }

    digitalWrite(blinkpin, blink = !blink);  //blink the heartbeat LED

  }
}


/* Sensor global methods are now on the methods tab */









