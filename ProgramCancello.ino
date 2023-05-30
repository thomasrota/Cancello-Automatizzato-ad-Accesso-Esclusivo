#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <Stepper.h>
#include <DHT.h>

 //+CLIP: "

#define FOTORESISTENZA_PARCHEGGIO_0 A0
#define FOTORESISTENZA_PARCHEGGIO_1 A1
#define FOTORESISTENZA_PARCHEGGIO_2 A2
#define FOTORESISTENZA_PARCHEGGIO_3 A3
#define FOTORESISTENZA_INTERNO A4
#define FOTORESISTENZA_ESTERNO A5
#define FOTORESISTENZA_LUMINOSITA A6
#define SEMAFORO_R 2
#define SEMAFORO_G 3
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
#define MOTOR_PIN0 A8
#define MOTOR_PIN1 A9
#define MOTOR_PIN2 A10
#define MOTOR_PIN3 A11

#define RIGHE 4
#define COLONNE 4 
byte pinRighe[RIGHE] = { 50, 49, 48, 47};
byte pinColonne[COLONNE] = { 46, 45, 44, 43};
char tasti[RIGHE][COLONNE] = {{'1', '2', '3', 'A'},{'4', '5', '6', 'B'},{'7', '8', '9', 'C'},{'*', '0', '#', 'D'}};
Keypad tastierino = Keypad(makeKeymap(tasti), pinRighe, pinColonne, RIGHE, COLONNE);
LiquidCrystal lcd (RS, EN, DB4, DB5, DB6, DB7);
SoftwareSerial gsm(TRASMISSIONE_GSM, RICEZIONE_GSM);
Stepper myStepper(1024, MOTOR_PIN1, MOTOR_PIN2, MOTOR_PIN3, MOTOR_PIN4);
DHT dht(SENSORE_TEMPERATURA, DHT11);
SoftwareSerial fingerSerial(TRASMISSIONE_IMPRONTA, RICEZIONE_IMPRONTA);

const int numeroPersone = 2;
struct Persona
{
  char[] nome;
  char[] password;
  char[] telefono;
  int idImpronta;
};
Persona persone[numeroPersone] = {{"Michele", "20190", "+393382831487", 1},{"Thomas", "20196", "+393396773542", 2}};

const int steps = 128;

int postiLiberi;

#define DELAY_CHIUSURA 15000

void setup() {
  Serial.begin(9600);
  gsm.begin(9600);
  dht.begin();
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
  postiLiberi = ContaLiberi();
}

void loop() {
  String orario = RicavaOrarioGSM();
  int temperatura = Temperatura();
  postiLiberi = ContaLiberi();
  AccensioneSemaforo(postiLiberi);
  ModificaLCD(orario, temperatura, postiLiberi);
  if(digitalRead(APERTURA_TASTIERA) == HIGH){
    int persona = VerificaPassword()
    if(persona != -1);{
      MessaggioBenvenuto(persona);
      AperturaCancello();
    }
    else{
      lcd.clear();
      lcd.print("Password");
      lcd.setCursor(0,1);
      lcd.print("Errata");
      delay(2000);
      ModificaLCD(orario, temperatura, postiLiberi);
    }
  }
  if(digitalRead(APERTURA_IMPRONTA) == HIGH){
    int persona = VerificaImpronta()
    if(persona != -1);{
      MessaggioBenvenuto(persona);
      AperturaCancello();
    }
    else{
      lcd.clear();
      lcd.print("Impronta");
      lcd.setCursor(0,1);
      lcd.print("Non trovata");
      delay(2000);
      ModificaLCD(orario, temperatura, postiLiberi);
    }
  }
   if(digitalRead(APERTURA_INTERNO) == HIGH){
    AperturaCancello();
  }
}

String RicavaOrarioGSM(){
  gsm.print("AT+CCLK?");
  String line = "";
  while(true){
    if(gsm.available()){
      char c = gsm.read();
      if (c ==  '\n'){
        line = "";
        if (line.indexOf("+CCLK:") >= 0){
          strtime = line.substring(17, line.lenght()-7);
        }
        if (line.lenght() == 0){
          break;
        }
      }else if (c != '\r'){
        line += c;
      }
    }
  }
  return line;
}

void MessaggioBenvenuto(int persona){
   for (int i = 0; i < numeroPersone; i++){
      if (persona == persone[i].idImpronta){
        lcd.setCursor(0, 0);
        lcd.print("Benvenuto/a");
        lcd.serCursor(persone[i].nome);
        break;
      }
  }
}

void ModificaLCD(int ora, int temperatura, int postiLiberi){
  lcd.setCursor(0,0);
  lcd.print(ora);
  lcd.print("     ");
  lcd.print(temperatura);
  lcd.print("°C");
  lcd.setCursor(0,1);
  lcd.print("Posti Liberi: ");
  lcd.print(postiLiberi);
}

void AccensioneSemaforo(int postiLiberi){
  if(postiLiberi <= 0){
    digitalWrite(SEMAFORO_R, HIGH);
    digitalWrite(SEMAFORO_G, LOW);
  }
  else{
    digitalWrite(SEMAFORO_R, LOW);
    digitalWrite(SEMAFORO_G, HIGH);
  }
}

int VerificaPassword(){
  char passwordInserita[6];
  for (int i = 0; i < 5; i++){
    do{
      passwordInserita[i] = tastierino.getKey();
    }while(passwordInserita[i] < 48 || passwordInserita[i] > 57);
  }
  passwordInserita[5] = '\0';
  for (int i = 0; i < numeroPersone; i++){
      if (strcmp(passwordInserita, persone[i].password) == 0)
        return persone[i].idImpronta;
  }
  return -1;
}

int VerificaImpronta(){
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  return finger.fingerID;
}

int ContaLiberi(){
  int pLiberi = 0;
  if (analogRead(FOTORESISTENZA_PARCHEGGIO_0) > SOGLIA){
    pLiberi++;
    digitalWrite(LED_G_PARCHEGGIO_0, HIGH);
    digitalWrite(LED_R_PARCHEGGIO_0, LOW);
  }
  else{
    digitalWrite(LED_G_PARCHEGGIO_0, LOW);
    digitalWrite(LED_R_PARCHEGGIO_0, HIGH);
  }
  if (analogRead(FOTORESISTENZA_PARCHEGGIO_1) > SOGLIA){
    pLiberi++;
    digitalWrite(LED_G_PARCHEGGIO_1, HIGH);
    digitalWrite(LED_R_PARCHEGGIO_1, LOW);
  }
  else{
    digitalWrite(LED_G_PARCHEGGIO_1, LOW);
    digitalWrite(LED_R_PARCHEGGIO_1, HIGH);
  }
  if (analogRead(FOTORESISTENZA_PARCHEGGIO_2) > SOGLIA){
    pLiberi++;
    digitalWrite(LED_G_PARCHEGGIO_2, HIGH);
    digitalWrite(LED_R_PARCHEGGIO_2, LOW);
  }
  else{
    digitalWrite(LED_G_PARCHEGGIO_2, LOW);
    digitalWrite(LED_R_PARCHEGGIO_2, HIGH);
  }
    if (analogRead(FOTORESISTENZA_PARCHEGGIO_3) > SOGLIA){
    pLiberi++;
    digitalWrite(LED_G_PARCHEGGIO_3, HIGH);
    digitalWrite(LED_R_PARCHEGGIO_3, LOW);
  }
  else{
    digitalWrite(LED_G_PARCHEGGIO_3, LOW);
    digitalWrite(LED_R_PARCHEGGIO_3, HIGH);
  }
  return pLiberi;
}

int RicavaTemperaturaDHT(){
  int t = dht.readTemperature();
  return t;
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
