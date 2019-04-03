
char message[35];
unsigned long lastSignal = 0;
bool transmission = false;
byte state = 1;
int pos;

#define GREENPIN 10
#define REDPIN 9

unsigned long dishka = 8;
unsigned long dishkaFake = 1532661448;



void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  pinMode(GREENPIN, OUTPUT);
  digitalWrite(GREENPIN, HIGH);
  pinMode(REDPIN, OUTPUT);
  digitalWrite(REDPIN, HIGH);
  Serial.println("Start");
  lastSignal = millis();
}

void loop() {
  switch (state) {
    case 1: {

        if (Serial2.available() > 0) {
          lastSignal = millis();
          pos = 0;
          state = 2;
        }
        break;
      }
    case 2: {
        // Reading of message
        if (Serial2.available() > 0 && pos < 35) {
          lastSignal = millis();
          message[pos] = Serial2.read();
          //               Serial.print(message[pos], HEX);
          //               Serial.print(",");
          pos++;
        }
        if (millis() - lastSignal > 100) state = 3;
        if (pos >= 35) {
          delay(300);
          state = 1;
        }
      }
      break;
    case 3: {
        // checksum
        Serial.println();
        byte check = message[1];
        for (int i = 2; i < 27; i++) {
          check = check ^ message[i];
        }
        Serial.println();
        //      Serial.print(check);
        //      Serial.print(",");
        //      Serial.println(message[27], HEX);
        if (check == message[27]) {
          //        Serial.println("Check OK");

          // Decodung
          unsigned long id = hexInDec(message, 1, 10);
          int countryNbr = hexInDec(message, 11, 4);

          Serial.print("CardNumber=");
          Serial.println(id);
          Serial.print("Country=");
          Serial.println(countryNbr);

          if (id == dishka || id == dishkaFake) {
            digitalWrite(GREENPIN, LOW);
            digitalWrite(REDPIN, HIGH);
            Serial.println("It is Dishka!");
          } else {
            digitalWrite(GREENPIN, HIGH);
            digitalWrite(REDPIN, LOW);
            Serial.println("It is not Dishka!");
          }
          state = 4;
        }
        break;
      }
    case 4: {
        delay(2000);
        digitalWrite(GREENPIN, HIGH);
        digitalWrite(REDPIN, HIGH);
        state = 1;
        
        break;
      }
  } //switch
}

unsigned long hexInDec(char message[], int beg , int len) {
  unsigned long mult = 1;
  unsigned long nbr = 0;
  byte nextInt;

  for (int i = beg; i < beg + len; i++) {
    nextInt = message[i];
    if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
    if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
    if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
    nextInt = constrain(nextInt, 0, 15);
    nbr = nbr + (mult * nextInt);
    mult = mult * 16;
  }
  return nbr;
}