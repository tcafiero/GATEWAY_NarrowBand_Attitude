/*********************************************************************
  Esempio di Gateway in grado di rilevare il proprio assetto e la presenza di devices bluetooth circostanti (MAC ADDRESS) e di inviare le relative informazioni 
  ad un broker in cloud sfruttando la rete cellulare (modulo SIM7020E - simcom).
  created by
  Mariano Iadaresta - Topview Srl
  11/07/2018
*********************************************************************/

#include <bluefruit.h>

#define LED 3
#define STOP_PIN 25
#define ACC_INTERRUPT_PIN 17
#define ALL_BUTTON 20
#define NB_ON_OFF_CHIP 1
#define RESET_NBCHIP 4
#define PWRKEY_NBCHIP 5

#include <Wire.h>
#include "Adafruit_LIS3DH.h"
#include "Adafruit_Sensor.h"

// I2C
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

const char* ManDown_IN_HEX =  "4d616e446f776e";
const char* MAW_IN_HEX = "4d616e4174576f726b";
const char* TOKEN_IN_HEX = "544f4b454e";
//const char* BT_COIN_IN_HEX = "42545f434f494e";

String MAC_BT;
//String RSSI_TOKEN;
String TOPIC_IMEI;
String  TOPIC;
String PAYLOAD;
String stringPub;
//String IMEI_MODULE;

unsigned int PAYLOAD_LENGTH;

int AcX, AcY = 0;
int a = 0;
int b = 0;
int c = 0;
int d = 0;
int e = 0;
int f = 0;

int counter = 0;

void setup()
{
  Serial.setPins(12, 11); //Serial to talk with SIM7020E
  Serial.begin(115200);
  lis.begin(0x19);
  pinMode(LED, OUTPUT);
  pinMode(STOP_PIN, OUTPUT);
  pinMode(NB_ON_OFF_CHIP, OUTPUT);
  pinMode(RESET_NBCHIP, OUTPUT);
  pinMode(PWRKEY_NBCHIP, OUTPUT);
  pinMode(ACC_INTERRUPT_PIN, INPUT);
  pinMode(ALL_BUTTON, INPUT);
  digitalWrite(RESET_NBCHIP, HIGH);
  digitalWrite(NB_ON_OFF_CHIP, HIGH);
  digitalWrite(PWRKEY_NBCHIP, HIGH);
  digitalWrite(STOP_PIN, HIGH);
  digitalWrite(LED, LOW); // LED ON
  WakeUP_NB_module(); // accendo il modulo NB
  Serial.println("AT+GSN");
  String exString;
  exString = Serial.readString();
  exString.remove(0, 9);
  exString.remove(15);
  TOPIC_IMEI = exString;
  Reg_rq_NB_module(); // richiedo la registrazione alla rete

  // Initialize Bluefruit with maximum connections as Peripheral = 0, Central = 1
  // SRAM usage required by SoftDevice will increase dramatically with number of connections
  Bluefruit.begin(0, 1);

  // Set max power. Accepted values are: -40, -30, -20, -16, -12, -8, -4, 0, 4
  Bluefruit.setTxPower(4);
  Bluefruit.setName("Bluefruit52");

  // Start Central Scan
  Bluefruit.setConnLedInterval(250);
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.start(10);
}


void scan_callback(ble_gap_evt_adv_report_t* report)
{
  digitalWrite(LED, HIGH); // LED OFF

  a = report->peer_addr.addr[5];
  b = report->peer_addr.addr[4];
  c = report->peer_addr.addr[3];
  d = report->peer_addr.addr[2];
  e = report->peer_addr.addr[1];
  f = report->peer_addr.addr[0];

  MAC_BT = " " ; // clean string

  // converto i valori in formato esadecimale in stringa di caratteri

  MAC_BT =  String(a , HEX) + ':' + String(b , HEX) + ':' + String(c , HEX) + ':' + String(d , HEX) + ':' + String(e , HEX) + ':' + String(f , HEX);
  MAC_BT.toUpperCase();
  TOPIC = MAC_BT;
  PAYLOAD = TOKEN_IN_HEX;
  PAYLOAD_LENGTH = PAYLOAD.length();
  a = 0;
  b = 0;
  c = 0;
  d = 0;
  e = 0;
  f = 0;
  //  Serial.println("Timestamp Addr              Rssi Data");
  //
  //  Serial.printf("%09d ", millis());
  //
  //  // MAC is in little endian --> print reverse
  //  Serial.printBufferReverse(report->peer_addr.addr, 6, ':');
  //  Serial.print(" ");
  //
  //  Serial.print(report->rssi);
  //  Serial.print("  ");
  //
  //  Serial.printBuffer(report->data, report->dlen, '-');
  //  Serial.println();
  //
  //  // Check if advertising contain BleUart service
  //  if ( Bluefruit.Scanner.checkReportForUuid(report, BLEUART_UUID_SERVICE) )
  //  {
  //    Serial.println("                       BLE UART service detected");
  //  }
  //
  //  Serial.println();

  digitalWrite(LED, LOW);
  Serial.println("AT+CMQNEW=""\"54.76.137.235\",\"18224\",2400,100");
  delay(2000);
  Serial.println("AT+CMQCON=0,3"",\"myclient\",1200,0,0,\"test\",\"test\"");
  delay(2000);
  stringPub = " ";
  stringPub = "AT+CMQPUB=0"",\"";
  stringPub += TOPIC;
  stringPub += "\",1,0,0,";
  stringPub += PAYLOAD_LENGTH;
  stringPub += ",\"";
  stringPub += PAYLOAD;
  stringPub += "\"";
  Serial.println(stringPub);
  stringPub = " ";
  delay(2000);
  Serial.println("AT+CMQDISCON=0");
  digitalWrite(LED, HIGH);
  delay(1000);
  TOPIC = " ";
  PAYLOAD = " ";
  PAYLOAD_LENGTH = 0;
  assetto();
//  counter++;
//  if (counter > 4)
//  {
  Serial.println("AT+COPS=?");
  delay(1000);
  Serial.println("AT+COPS=?");
  delay(1000);
  Serial.println("AT+COPS=?");
  delay(15000);
  Serial.println("AT+CSQ");
  delay(500);
  Serial.println("AT+CGREG?");
  delay(500);
  Serial.println("AT+CGREG?");
  delay(500);
  Serial.println("AT+CSQ");
  delay(500);
  Serial.println("AT+CREG?");
  delay(500);
  Serial.println("AT+CREG?");
  delay(500);
  Serial.println("AT+CGREG?");
  digitalWrite(LED, LOW); // LED ON
  int i = 0 ;
  while (i < 10 )
  {
    digitalWrite(LED, LOW);
    delay(200);
    digitalWrite(LED, HIGH);
    delay(200);
    i++;
    counter = 0;
  }
//  }
}

void assetto()
{
  lis.read();      // get X Y and Z data at once
  sensors_event_t event;
  lis.getEvent(&event);
  AcX = event.acceleration.x;
  AcY = event.acceleration.y;
  if (abs (AcX + AcY) > 2)
  {
    PAYLOAD = MAW_IN_HEX;
    PAYLOAD_LENGTH = PAYLOAD.length();
    digitalWrite(LED, LOW);
    Serial.println("AT+CMQNEW=""\"54.76.137.235\",\"18224\",2400,100");
    delay(2000);
    Serial.println("AT+CMQCON=0,3"",\"myclient\",1200,0,0,\"test\",\"test\"");
    delay(2000);
    stringPub = " ";
    stringPub = "AT+CMQPUB=0"",\"";
    stringPub += TOPIC_IMEI;
    stringPub += "\",1,0,0,";
    stringPub += PAYLOAD_LENGTH;
    stringPub += ",\"";
    stringPub += PAYLOAD;
    stringPub += "\"";
    Serial.println(stringPub);
    stringPub = " ";
    delay(2000);
    Serial.println("AT+CMQDISCON=0");
    digitalWrite(LED, HIGH);
  }
  else {
    PAYLOAD = ManDown_IN_HEX;
    PAYLOAD_LENGTH = PAYLOAD.length();
    digitalWrite(LED, LOW);
    Serial.println("AT+CMQNEW=""\"54.76.137.235\",\"18224\",2400,100");
    delay(2000);
    Serial.println("AT+CMQCON=0,3"",\"myclient\",1200,0,0,\"test\",\"test\"");
    delay(2000);
    stringPub = " ";
    stringPub = "AT+CMQPUB=0"",\"";
    stringPub += TOPIC_IMEI;
    stringPub += "\",1,0,0,";
    stringPub += PAYLOAD_LENGTH;
    stringPub += ",\"";
    stringPub += PAYLOAD;
    stringPub += "\"";
    Serial.println(stringPub);
    stringPub = " ";
    delay(2000);
    Serial.println("AT+CMQDISCON=0");
  }
}

void loop()
{
}

void WakeUP_NB_module() // accendo il modulo NB
{
  digitalWrite(NB_ON_OFF_CHIP, LOW);
  digitalWrite(RESET_NBCHIP, LOW);
  digitalWrite(PWRKEY_NBCHIP, HIGH);
  delay(2000);
  digitalWrite(PWRKEY_NBCHIP, LOW);
  //  Serial.println("AT");
  delay(3000);
}

void Reg_rq_NB_module() // richiedo la registrazione alla rete
{
  Serial.println("AT+COPS=?");
  delay(1000);
  Serial.println("AT+COPS=?");
  delay(1000);
  Serial.println("AT+COPS=?");
  delay(30000);
  Serial.println("AT+CSQ");
  delay(500);
  Serial.println("AT+CGREG?");
  delay(500);
  Serial.println("AT+CGREG?");
  delay(500);
  Serial.println("AT+CSQ");
  delay(500);
  Serial.println("AT+CREG?");
  delay(500);
  Serial.println("AT+CREG?");
  delay(500);
  Serial.println("AT+CGREG?");
  digitalWrite(LED, LOW); // LED ON
  int i = 0 ;
  while (i < 10 )
  {
    digitalWrite(LED, LOW);
    delay(200);
    digitalWrite(LED, HIGH);
    delay(200);
    i++;
  }
}

void Sleep_NB_module() // addormenta e spegni il modulo NB
{
  Serial.println("AT+CPSMS=1");
  delay(2000);
  digitalWrite(RESET_NBCHIP, HIGH);
  delay(50);
  digitalWrite(NB_ON_OFF_CHIP, HIGH);
}

