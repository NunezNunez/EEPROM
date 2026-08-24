

#define CS 2
#define SK 3
#define DI 4
#define DO 5


void dataOutput(int data, byte cnt){
  for (int i = cnt - 1; i >= 0; i--){

    digitalWrite(DI, (data >> i) & 1);

    digitalWrite(SK, HIGH);
    delayMicroseconds(1);

    digitalWrite(SK, LOW);
    delayMicroseconds(1);
  }
}


int dataInput(byte size){
  int data = 0;

  for (int i = 0; i < size; i++){

    digitalWrite(SK, HIGH);
    delayMicroseconds(1);

    int bitValue = digitalRead(DO);
    data = (data << 1) | bitValue;

    digitalWrite(SK, LOW);
    delayMicroseconds(1);
  }

  return data;
}


void cmdout(int cmd){
  dataOutput(cmd, 10);
}


void EEPROM_wral(byte data){

  digitalWrite(CS, HIGH);

  // WRAL = 1 00 01 XXXXX
  cmdout(0b1000100000);

  dataOutput(data, 8);

  digitalWrite(CS, LOW);

  delay(10);
}


void EEPROM_ewds(){

  digitalWrite(CS, HIGH);

  // EWDS = 1 00 00 XXXXX
  cmdout(0b1000000000);

  digitalWrite(CS, LOW);
}


void EEPROM_ewen(){

  digitalWrite(CS, HIGH);

  // EWEN = 1 00 11 XXXXX
  cmdout(0b1001100000);

  digitalWrite(CS, LOW);
}


void EEPROM_eral(){

  digitalWrite(CS, HIGH);

  // ERAL = 1 00 10 XXXXX
  cmdout(0b1001000000);

  digitalWrite(CS, LOW);

  delay(10);
}


int EEPROM_read(byte adr){

  // READ = 1 10 AAAAAAA
  int cmd = 0b1100000000 | (adr & 0x7F);

  digitalWrite(CS, HIGH);

  cmdout(cmd);

  // Discard dummy bit
  dataInput(1);

  // Read 8 data bits
  int data = dataInput(8);

  digitalWrite(CS, LOW);

  return data;
}


void EEPROM_write(byte adr, int data){

  // WRITE = 1 01 AAAAAAA
  int cmd = 0b1010000000 | (adr & 0x7F);

  digitalWrite(CS, HIGH);

  cmdout(cmd);

  dataOutput(data, 8);

  digitalWrite(CS, LOW);

  delay(10);
}


void EEPROM_erase(byte adr){

  // ERASE = 1 11 AAAAAAA
  int cmd = 0b1110000000 | (adr & 0x7F);

  digitalWrite(CS, HIGH);

  cmdout(cmd);

  digitalWrite(CS, LOW);

  delay(10);
}


void setup(){

  pinMode(CS, OUTPUT);
  pinMode(SK, OUTPUT);
  pinMode(DI, OUTPUT);
  pinMode(DO, INPUT_PULLUP);

  Serial.begin(9600);

  digitalWrite(CS, LOW);
  digitalWrite(SK, LOW);
  digitalWrite(DI, LOW);
}


void loop(){

  int ch;

  Serial.println("\n93C46 Test Menu");
  Serial.println("1 - Dump Nvram");
  Serial.println("2 - EWEN Write Enable");
  Serial.println("3 - EWDS Write Disable");
  Serial.println("4 - WRAL 0xA5");
  Serial.println("5 - ERAL Erase ALL");
  Serial.println("6 - Write 0:12,34,56,78,9A,BC,DE,FA");
  Serial.println("7 - Write 0:22,44,66,88,AA,BB,CC,DD");
  Serial.println("8 - Erase 0, 3,4, 7");
  Serial.println("9 - Write Sequential numbers");

  Serial.println("CMD: ");

  while (Serial.available() == 0);

  ch = Serial.read();


  switch (ch){


    case '1':

      Serial.println("1 - Dump Nvram");

      for (int row = 0; row < 128; row += 16){

        // Print row starting address
        if (row < 0x10){
          Serial.print("0");
        }

        Serial.print(row, HEX);
        Serial.print("  ");


        // Print 16 bytes in HEX
        for (int i = 0; i < 16; i++){

          int value = EEPROM_read(row + i);

          if (value < 0x10){
            Serial.print("0");
          }

          Serial.print(value, HEX);
          Serial.print(" ");
        }


        Serial.print("   ");


        // Print same 16 bytes as ASCII
        for (int i = 0; i < 16; i++){

          int value = EEPROM_read(row + i);

          if (value >= 32 && value <= 126){
            Serial.write((char)value);
          }
          else{
            Serial.print(".");
          }
        }

        Serial.println();
      }

      break;


    case '2':

      Serial.println("EWEN Write Enable");

      EEPROM_ewen();

      break;


    case '3':

      Serial.println("EWDS Write Disable");

      EEPROM_ewds();

      break;


    case '4':

      Serial.println("WRAL Write ALL - 0xA5");

      EEPROM_wral(0xA5);

      break;


    case '5':

      Serial.println("ERAL Erase ALL");

      EEPROM_eral();

      break;


    case '6':

      Serial.println("Write 0:12,34,56,78,9A,BC,DE,FA");

      EEPROM_write(0, 0x12);
      EEPROM_write(1, 0x34);
      EEPROM_write(2, 0x56);
      EEPROM_write(3, 0x78);
      EEPROM_write(4, 0x9A);
      EEPROM_write(5, 0xBC);
      EEPROM_write(6, 0xDE);
      EEPROM_write(7, 0xFA);

      break;


    case '7':

      Serial.println("Write 0:22,44,66,88,AA,BB,CC,DD");

      EEPROM_write(0, 0x22);
      EEPROM_write(1, 0x44);
      EEPROM_write(2, 0x66);
      EEPROM_write(3, 0x88);
      EEPROM_write(4, 0xAA);
      EEPROM_write(5, 0xBB);
      EEPROM_write(6, 0xCC);
      EEPROM_write(7, 0xDD);

      break;


    case '8':

      Serial.println("Erase 0, 3, 4, 7");

      EEPROM_erase(0);
      EEPROM_erase(3);
      EEPROM_erase(4);
      EEPROM_erase(7);

      break;


    case '9':

      Serial.println("9 - Write Sequential numbers");

      for (int adr = 0; adr < 128; adr++){
        EEPROM_write(adr, adr);
      }

      Serial.println("Done");

      break;


    default:

      break;
  }
}