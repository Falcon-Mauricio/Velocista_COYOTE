#include "BluetoothSerial.h"
#include <QTRSensors.h>
#include <Motor.h>

// #define sensor1 = 13
// #define sensor2 = 27
// #define sensor3 = 26
// #define sensor4 = 25
// #define sensor5 = 33
// #define sensor6 = 32
// #define sensor7 = 35
// #define sensor8 = 34

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define M1A 19
#define M1B 21
#define M2A 23
#define M2B 22
#define M1A_CHANEL 12
#define M1B_CHANEL 14
#define M2A_CHANEL 13
#define M2B_CHANEL 15

int proportional = 0;
int derivative = 0;
int integral = 0;
int lastErr = 0;

float kp = 0;
float ki = 0;
float kd = 0;

float speed = 0;

int velocity = 100;

float pidLeft = 0;
float pidRight = 0;

int maxSpeed = 170;
int minSpeed = 30;

BluetoothSerial SerialBT;

QTRSensors qtr;

MOTOR *motorLeft = new MOTOR(M1B, M1A, M1B_CHANEL, M1A_CHANEL);

MOTOR *motorRight = new MOTOR(M2A, M2B, M2B_CHANEL, M2A_CHANEL);

int setpoint = 3400;

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];

int Led = 5;
int LedB = 2;

void calibration()
{
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]){
                        13, 27, 25, 26, 33, 32, 35, 34},
                    SensorCount);

  delay(500);
  pinMode(Led, OUTPUT);
  digitalWrite(Led, HIGH);
  for (uint16_t i = 0; i < 1000; i++)
  {
    qtr.calibrate();
  }
  digitalWrite(Led, LOW);
}

int getPosition()
{
  int position = qtr.readLineWhite(sensorValues);
  return position;
}

void printPositionAndSensors()
{
  int position = qtr.readLineWhite(sensorValues);

  for (uint8_t i = 0; i < SensorCount; i++)
  {
    SerialBT.print(sensorValues[i]);
    SerialBT.print(" || ");
  }
  SerialBT.println(position);
  SerialBT.print('\n');

  delay(250);
}

void setKp()
{
  SerialBT.println("Ingrese la el valor para Kp");
  float newKp = SerialBT.read();
  delay(3000);
  kp = newKp;
  SerialBT.print("El nuevo valor es: ");
  SerialBT.println(kp);
  SerialBT.println("Sali");
}

void printMainMenu()
{
  SerialBT.println("- M Mostrar el menu");
  SerialBT.println("- A Encender motores");
  SerialBT.println("- B Apagar motores");
  SerialBT.println("- D Menu de configuracion del PID");
}

void printOptions()
{
  // clean the serial
  for (int i = 0; i < 10; i++)
  {
    SerialBT.println("");
  }

  SerialBT.println("Configuracion Actual:");

  SerialBT.print("- KP = ");
  SerialBT.println(kp);

  SerialBT.print("- KI = ");
  SerialBT.println(ki);

  SerialBT.print("- KD = ");
  SerialBT.println(kd);

  SerialBT.println(" (K) KP + 0.1 / (L) KP - 0.1");
  SerialBT.println(" (Q) KP + 0.01 / (W) KP - 0.01");

  SerialBT.println(" (R) KI + 0.1 / (T) KI - 0.1");
  SerialBT.println(" (U) KI + 0.01 / (I) KI - 00.1");

  SerialBT.println(" (Z) KD + 0.1 / (X) KD - 0.1");
  SerialBT.println(" (G) KD + 0.01 / (H) KD - 00.1");
}

void menuBT()
{
  if (SerialBT.available())
  {
    char Menssage = SerialBT.read();
    switch (Menssage)
    {
    case 'M':
    {
      printOptions();
      break;
    }
    case 'A':
    {
      motorLeft->GoAvance(50);
      motorRight->GoAvance(50);
      break;
    }
    case 'B':
    {
      motorLeft->Still();
      motorRight->Still();
      break;
    }
    case 'K':
    {
      kp += 0.1;
      printOptions();
      break;
    }
    case 'L':
    {
      kp -= 0.1;
      printOptions();
      break;
    }
    case 'Q':
    {
      kp += 0.01;
      printOptions();
      break;
    }
    case 'W':
    {
      kp -= 0.01;
      printOptions();
      break;
    }

    case 'R':
    {
      ki += 0.1;
      printOptions();
      break;
    }
    case 'T':
    {
      ki -= 0.1;
      printOptions();
      break;
    }
    case 'U':
    {
      ki += 0.01;
      printOptions();
      break;
    }
    case 'I':
    {
      ki -= 0.01;
      printOptions();
      break;
    }

    case 'Z':
    {
      kd += 0.1;
      printOptions();
      break;
    }
    case 'X':
    {
      kd -= 0.1;
      printOptions();
      break;
    }
    case 'G':
    {
      kd += 0.01;
      printOptions();
      break;
    }
    case 'H':
    {
      kd -= 0.01;
      printOptions();
      break;
    }

    default:
      break;
    }
  }
}

void setup()
{
  Serial.begin(9600);
  SerialBT.begin("ESP32test");
  pinMode(Led, OUTPUT);
  // calibration();
  pinMode(LedB, OUTPUT);
  digitalWrite(LedB, HIGH);
}

void loop()
{
  menuBT();

  // int position = getPosition();

  // proportional = position - setpoint;

  // derivative = proportional - lastErr;

  // integral = integral + proportional;

  // speed = (proportional * kp) + (derivative * kd) + (integral * ki);

  // lastErr = proportional;

  // pidLeft = (velocity + speed);
  // pidRight = (velocity - speed);

  // if (pidLeft > maxSpeed) pidLeft = maxSpeed;
  // else if (pidLeft < minSpeed) pidLeft = minSpeed;

  // if (pidRight < minSpeed) pidRight = minSpeed;
  // else if (pidRight > maxSpeed) pidRight = maxSpeed;

  // SerialBT.println('motor left');
  // SerialBT.println(pidLeft);
  // SerialBT.println('motor right');
  // SerialBT.println(pidRight);
}