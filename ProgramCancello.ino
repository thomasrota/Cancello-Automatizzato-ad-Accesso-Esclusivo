#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <Stepper.h>

#define FOTORESISTENZA_PARCHEGGIO_0 A0
#define FOTORESISTENZA_PARCHEGGIO_1 A1
#define FOTORESISTENZA_PARCHEGGIO_2 A2
#define FOTORESISTENZA_PARCHEGGIO_3 A3
#define FOTORESISTENZA_INTERNO A4
#define FOTORESISTENZA_ESTERNO A5
#define FOTORESISTENZA_LUMINOSITA A6
#define SEMAFORO_R 2
#define SEMAFORO_G 3
#define SEMAFORO_B 4
#define TRASMISSIONE_GSM 5
#define RICEZIONE_GSM 6
#define LAMPEGGIANTE 22
#define LAMPIONI 23
#define PULSANTE_INTERNO 24
#define PULSANTE_TASTIERA 25
#define PULSANTE_IMPRONTA 26
#define FINE_CORSA_APERTURA 27
#define FINE_CORSA_CHIUSURA 28
#define LASER_PARCHEGGIO_0 29
#define LASER_PARCHEGGIO_1 30
#define LASER_PARCHEGGIO_2 31
#define LASER_PARCHEGGIO_3 32
#define LASER_INTERNO 33
#define LASER_ESTERNO 34
#define LED_R_PARCHEGGIO_0 35
#define LED_G_PARCHEGGIO_0 36
#define LED_R_PARCHEGGIO_1 37
#define LED_G_PARCHEGGIO_1 38
#define LED_R_PARCHEGGIO_2 39
#define LED_G_PARCHEGGIO_2 40
#define LED_R_PARCHEGGIO_3 41
#define LED_G_PARCHEGGIO_3 42
#define RS 51
#define EN 52
#define DB4 7
#define DB5 8
#define DB6 9
#define DB7 10
#define TRASMISSIONE_IMPRONTA 11
#define RICEZIONE_IMPRONTA 12
#define SENSORE_TEMPERATURA 13
#define MOTOR_PIN_0 A8
#define MOTOR_PIN_1 A9
#define MOTOR_PIN_2 A10
#define MOTOR_PIN_3 A11

#define RIGHE 4
#define COLONNE 4 
byte pinRighe[RIGHE] = { 50, 49, 48, 47};
byte pinColonne[COLONNE] = { 46, 45, 44, 43};
char tasti[RIGHE][COLONNE] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
Keypad tastierino = Keypad(makeKeymap(tasti), pinRighe, pinColonne, RIGHE, COLONNE);
LiquidCrystal lcd (RS, EN, DB4, DB5, DB6, DB7);
SoftwareSerial gsm(5, 6);
Stepper myStepper(1024, MOTOR_PIN1, MOTOR_PIN2, MOTOR_PIN3, MOTOR_PIN4);

int postiLiberi = 0;
const int steps = 128;

#define DELAY_CHIUSURA 15000

void setup() {
  Serial.begin(9600);
  gsm.begin(9600);
  pinMode(LAMPEGGIANTE, OUTPUT);
  pinMode(FINE_CORSA_APERTURA, INPUT);
  pinMode(FINE_CORSA_CHIUSURA, INPUT);
  pinMode(APERTURA_INTERNO, INPUT);
  pinMode(APERTURA_ESTERNO, INPUT);
  pinMode(MOTOR_PIN1, OUTPUT);
  pinMode(MOTOR_PIN2, OUTPUT);
  pinMode(MOTOR_PIN3, OUTPUT);
  pinMode(MOTOR_PIN4, OUTPUT);
  myStepper.setSpeed(25);
  //postiLiberi = ContaLiberi();
}

void loop() {
  if(digitalRead(APERTURA_INTERNO) == HIGH){
    AperturaCancello();
    delay(2000); //possibilmente 20000
    ChiusuraCancello();
    postiLiberi++;
  }
  if(digitalRead(APERTURA_ESTERNO) == HIGH){
    AperturaCancello();
    delay(2000); //possibilmente 20000
    ChiusuraCancello();
    postiLiberi--;
  }
}

void ContaLiberi(){
  
}

void AperturaCancello(){
  while(digitalRead(FINE_CORSA_APERTURA) == LOW){
    myStepper.step(steps);
    digitalWrite(LAMPEGGIANTE, !digitalRead(LAMPEGGIANTE));
  }
  digitalWrite(LAMPEGGIANTE, LOW);
}

void ChiusuraCancello(){
  while(digitalRead(FINE_CORSA_CHIUSURA) == LOW){
    myStepper.step(-steps);
    digitalWrite(LAMPEGGIANTE, !digitalRead(LAMPEGGIANTE));
  }
  digitalWrite(LAMPEGGIANTE, LOW);
}
