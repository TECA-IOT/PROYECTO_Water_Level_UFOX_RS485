#include <Ufox.h>
#include <LowPower.h>
#include <SoftwareSerial.h>
#include <Arduino.h>
#include <SensorModbusMaster.h>

//Pines empleados
#define btn          13
#define RXLED        17 
#define LED          9 
#define PwrSensor    A0  //pin para encender max485 y elevador de voltaje para sensor 
#define PinRX_485    16 
#define PinTX_485    10
#define PinDERE_485  15
#define PinFRAUD      8  //reed switch a GND
#define Pin_b5        7  //
#define Pin_b4        6  //
#define Pin_b3        5  //
#define Pin_b2        4  //
#define Pin_b1        14  //

Ufox wisol;
SoftwareSerial mySerial(4, 5); //(RX,TX)

bool reboot=0;
    bool rsv1=0;
    bool fraud=0;;
    bool b4=0;
    bool b3=0;
    bool b2=0;
    bool b1=0;
    bool b0=0;

int val=0; //variable contador
float tmp = 80 ; //tiempo que permanecera apagado (Minutos) =  82 *8/60 = 10.9 min


// Define the sensor's modbus address
byte modbusAddress = 0x01;   // The sensor's modbus address, or SlaveID
long modbusBaudRate = 9600;

// Define pin number variables
const int sensorPwrPin = -10;     // The pin sending power to the sensor
const int adapterPwrPin = -22;    // The pin sending power to the RS485 adapter  //encender RS485
const int DEREPin = PinDERE_485;  // The pin controlling Recieve Enable and Driver Enable
                                  // on the RS485 adapter, if applicable (else, -1)
                                  // Setting HIGH enables the driver (arduino) to send text
                                  // Setting LOW enables the receiver (sensor) to send text


const int SSRxPin = PinRX_485; //10; // Recieve pin for software serial (Rx on RS485 adapter)
const int SSTxPin = PinTX_485; //15; // Send pin for software serial (Tx on RS485 adapter)
SoftwareSerial modbusSerial(SSRxPin, SSTxPin);

bool success;
int16_t direccion;
int16_t baud ;
int16_t unit;
int16_t measure;
////////////////////
modbusMaster modbus;

void setup()
{
  pinMode(btn,INPUT);
  pinMode(RXLED,OUTPUT);
  pinMode(PinFRAUD,INPUT_PULLUP); 
  delay(4000);
  Serial.println("Boot");
  Serial.println("Boot");
    
  Serial.begin(115200);
  wisol.begin(9600);
  mySerial.begin(9600); //iniciar serial de usuario pin 5-TX
  mySerial.println("Iniciando Dispositivo Ufox!");
  pinMode(PwrSensor,OUTPUT);
  
    // Set various pins as needed
    if (DEREPin >= 0)
    {
        pinMode(DEREPin, OUTPUT);
    }
    if (sensorPwrPin >= 0)
    {
        pinMode(sensorPwrPin, OUTPUT);
        digitalWrite(sensorPwrPin, HIGH);
    }
    if (adapterPwrPin >= 0)
    {
        pinMode(adapterPwrPin, OUTPUT);
        digitalWrite(adapterPwrPin, HIGH);
    }

    // Turn on the "main" serial port for debugging via USB Serial Monitor
  
   // while(!Serial);
       
  Serial.print("ID: ");
  Serial.println(wisol.ID());
    
  Serial.print("PAC: ");
  Serial.println(wisol.PAC());

  modbusSerial.begin(modbusBaudRate);
  modbus.begin(modbusAddress, modbusSerial, DEREPin);
  delay(1000);

  wisol.SLEEP();
  pinMode(LED,OUTPUT);
  pinMode(RXLED,OUTPUT);
  
  for(int i=0; i<10;i++){
    digitalWrite(RXLED,LOW);
    delay(30);
    digitalWrite(RXLED,HIGH);  
    delay(30);
    }
    
  reboot=1; //bandera que indica reinicio
   if(digitalRead(PinFRAUD)==1)fraud=0;
   if(digitalRead(PinFRAUD)==0)fraud=1;
  TASK(); //realizar tareas, lectura de sensores, actuadores, enviar por sigfox, recibir de sigfox,etc...
  
  mySerial.print("Apagar UFOX (Min): ");
  mySerial.println((tmp*8)/60);

  
}

// ---------------------------------------------------------------------------
// Main setup function
// ---------------------------------------------------------------------------
void loop()
{
 val++;
 mySerial.print(".");
 if(digitalRead(PinFRAUD)==1)fraud=0;
 if(digitalRead(PinFRAUD)==0)fraud=1;

 if(val >= tmp){  //preguntar si es tiempo de enviar
     digitalWrite(LED,LOW);
     mySerial.println("Despertar UFOX! ");

     //reboot=0,rsv1=0,rsv2=0,b4=0,b3=0,b2=0,b1=0,b0=0;
     reboot=0;

   rsv1=0;
   
   if(digitalRead(PinFRAUD)==1)fraud=0;
   if(digitalRead(PinFRAUD)==0)fraud=1;
   
   b4=0;
   b3=0;
   b2=0;
   b1=0;
   b0=0;
   
     TASK(); //realizar tareas, lectura de sensor, actuadores, enviar por sigfox, recibir de sigfox,etc...
 
     val=0;
     mySerial.print("Apagar UFOX (Min): ");
     mySerial.println((tmp*8)/60);
     LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); //APAGAR
  }   
     LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF); //APAGAR
}


void TASK(){
  
    wisol.RST(); //reinicializar (despertar) radio sigfox
      //digitalWrite(RXLED,LOW); //encender led
    mySerial.println("Enviar dato Sigfox...");  
    digitalWrite(PwrSensor,HIGH); //ENCEDER TRANSISTOR
    
    for(int i=0; i<3;i++){TXLED0;  delay(100); TXLED1;   delay(100);}
  Serial.print("ID: ");
  Serial.println(wisol.ID());  
  Serial.print("PAC: ");
  Serial.println(wisol.PAC());
  
    meassure_rs485(); //leer RS485
    
    digitalWrite(PwrSensor,LOW); //APAGAR TRANSISTOR
    digitalWrite(DEREPin,HIGH);
       
   uint32_t S1 = measure;   //int24=24bits=3bytes  nivel de agua en mm 
   uint32_t S2 = 0;        //uint24=24bits=3bytes
   uint16_t S3 = 0;          //uint16=16bits=2bytes
   uint16_t S4 = 0;       //uint16=16bits=2bytes   
   
   uint16_t  volt =  wisol.VOLT();
   volt = volt/100;
   uint8_t bateria = volt;     //int=8bits=2bytes  0.0V -4.5V

   Serial.print("Voltaje:  ");
   Serial.println(bateria);
  
    uint8_t SumBit = SumBit<<1 |reboot;  //1bytes
    SumBit = SumBit<<1 |rsv1;
    SumBit = SumBit<<1 |fraud;
    SumBit = SumBit<<1 |b4;
    SumBit = SumBit<<1 |b3;
    SumBit = SumBit<<1 |b2;
    SumBit = SumBit<<1 |b1;
    SumBit = SumBit<<1 |b0;

    Serial.print("S1:");Serial.print(S1);Serial.print(" S2:");Serial.print(S2);Serial.print(" S3:");Serial.print(S3);Serial.print(" S4:");Serial.println(S4);
    Serial.print(" Reboot:");Serial.print(reboot); Serial.print(" Fraude:");Serial.print(fraud);Serial.print(" Battery:");Serial.println(bateria);  
   
    //SumBit=255;
    char buff[30]="";
    //formatear a cadena, convertir los datos a valores hexagesimales
    sprintf(buff,"%06lx%06lx%04x%04x%02x%02x",S1,S2,S3,S4,SumBit,bateria);  
    Serial.println(buff);
    //String valor2 ="10";
    Serial.println(wisol.SEND(buff)); 
     //digitalWrite(RXLED,HIGH);

    for(int i=0; i<3;i++){TXLED1;  delay(100);TXLED0;   delay(100);}
    wisol.SLEEP();//apagar radio sigfox
}


void meassure_rs485(){
Serial.print("Status:");
success = modbus.getRegisters(0x03, 0x04, 2);
Serial.print(success);
Serial.print("  ");

Serial.print("Address:");
direccion = modbus.int16FromRegister(0x03, 0x00, bigEndian);
Serial.print(direccion);
Serial.print(" ");

Serial.print("Baud:");
baud = modbus.int16FromRegister(0x03, 0x01, bigEndian);
Serial.print(baud);
Serial.print(" ");

Serial.print("Unit:");
unit = modbus.int16FromRegister(0x03, 0x02, bigEndian);
Serial.print(unit);
Serial.print(" ");

Serial.print("Measure (mm):");
measure = modbus.int16FromRegister(0x03, 0x04, bigEndian);
Serial.print(measure);
Serial.println(" ");

delay(500);
}
