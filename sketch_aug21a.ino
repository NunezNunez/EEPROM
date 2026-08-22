

#define CS 2
#define SK 3
#define DI 4
#define DO 5

void cmdout(int cmd){ // Implement sending the functions operation to the EEPROM here

    dataOutput(cmd,10);

}

int dataInput(byte size){// Implement read data from EEPROM
    int data = 0;
    for (int i = 0; i < size; i++){
      digitalWrite(SK,HIGH);
      delayMicroseconds(1);

      int bitValue = digitalRead(DO);
      data = (data << 1)| bitValue;

      digitalWrite(SK,LOW);
      delayMicroseconds(1);

    }
    return data;
}

void dataOutput(int data, byte cnt){// Implement write data to EEPROM
     for (int i = cnt - 1 ; i >= 0; i--){

      digitalWrite(DI, (data>>i) & 1);
      digitalWrite(SK, HIGH);
      delayMicroseconds(1);

      digitalWrite(SK, LOW);
      delayMicroseconds(1);
     }
}

void EEPROM_wral(byte data){
  digitalWrite(CS, HIGH);

  cmdout(0b1000100000);
  dataOutput(data,8);

  digitalWrite(CS, LOW);

  delay(10);
  // Implement write all function that takes in a character to populate the whole EEPROM
  //cmdout(wral instruction)
  // dataOutput(int data, byte cnt)
}

void EEPROM_ewds(){
  digitalWrite(CS, HIGH);

  cmdout(0b1000000000);

  digitalWrite(CS, LOW);
  // Implement write and erase disable
  //cmdout(ewds instruction)
}

void EEPROM_ewen(){
  digitalWrite(CS, HIGH);

  cmdout(0b1001100000);

  digitalWrite(CS, LOW);
  // Implement enable write and erase
  //cmdout(ewen instruction)
}

void EEPROM_eral(){
  digitalWrite(CS, HIGH);

  cmdout(0b1001000000);

  digitalWrite(CS, LOW);

  delay(10);
  // Implement erase all
  //cmdout(eral instruction)
}


int EEPROM_read(byte adr){
  int cmd = 0b1100000000 | (adr & 0x7F);

  digitalWrite(CS, HIGH);

  cmdout(cmd);
  dataInput(1);
  int data = dataInput(8);

  digitalWrite(CS, LOW);

  return data;
  // Implement read here from given {adr}
  //cmdout(read instruction)
  // dataInput(byte size)
}

void EEPROM_write(byte adr, int data){
  int cmd = 0b1010000000 | (adr & 0x7F);

  digitalWrite(CS, HIGH);
  cmdout(cmd);
  dataOutput(data,8);

  digitalWrite(CS ,LOW);

  delay(10);
  // Implement write {data} to given {adr}
  //cmdout(write instruction)
  // dataOutput(int data, byte cnt)
}


void EEPROM_erase(byte adr){
  int cmd = 0b1110000000 | (adr & 0x7F);

  digitalWrite(CS, HIGH);

  cmdout(cmd);

  digitalWrite(CS, LOW);

  delay(10);
  // Implement erase the given {adr}
  //cmdout(erase instruction)
}

void setup() {
  pinMode(CS, OUTPUT);
  pinMode(SK, OUTPUT);
  pinMode(DI, OUTPUT);
  pinMode(DO, INPUT_PULLUP);
  Serial.begin(9600);
  digitalWrite(CS, LOW);
  digitalWrite(SK, LOW);
  digitalWrite(DI, LOW);
}



void loop() {
  int ch;
  Serial.println("\n93C46 Test Menu");
  Serial.println("1 - Dump Nvram");
  Serial.println("2 - EWEN Write Enable");
  Serial.println("3 - EWDS Write Disable");
  Serial.println("4 - WRAL 0xA5");
  Serial.println("5 - ERAL Erase ALL");
  Serial.println("6 - Write  0:12,34,56,78,9A,BC,DE,FA");
  Serial.println("7 - Write  0:22,44,66,88,AA,BB,CC,DD");
  Serial.println("8 - Erase  0, 3,4, 7");
  Serial.println("9 - Write Sequential numbers");

  Serial.println("CMD: ");
  while(Serial.available()==0);  // wait for input
  ch= Serial.read();             // Note - Handle CR LF on input

  switch (ch) {

    case '1':
      Serial.println("1 - Dump Nvram");
        for (int adr = 0; adr < 128; adr++) {
        int value = EEPROM_read(adr);

        if (value < 0x10) {
          Serial.print("0");
       }

        Serial.print(value, HEX);
        Serial.print(" ");

        if ((adr + 1) % 16 == 0) {
        Serial.println();
        }
   }
      // Implement code to read all the address and print the result
      //EEPROM_read(byte adr)
      break;

    case '2':
      Serial.print("EWEN Write Enable");
      EEPROM_ewen();
      break;

    case '3':
      Serial.print("EWDS Write Disable");
      EEPROM_ewds();
      break;

    case '4':
      Serial.print("WRAL Write ALL - 0xA5");
      EEPROM_ewen();
      EEPROM_wral(0xA5);
      EEPROM_ewds();

      break;

    case '5':
      Serial.print("ERAL Erase ALL");
      EEPROM_eral();
      break;

    case '6':
      Serial.print("Write  0:12,34,56,78,9A,BC,DE,FA");
       EEPROM_write(0,0x12);
       EEPROM_write(1,0x34);
       EEPROM_write(2,0x56);
       EEPROM_write(3,0x78);
       EEPROM_write(4,0x9A);
       EEPROM_write(5,0xBC);
       EEPROM_write(6,0xDE);
       EEPROM_write(7,0xFA);
      break;

    case '7':
      Serial.print("Write  0:22,44,66,88,AA,BB,CC,DD");
       EEPROM_write(0,0x22);
       EEPROM_write(1,0x44);
       EEPROM_write(2,0x66);
       EEPROM_write(3,0x88);
       EEPROM_write(4,0xAA);
       EEPROM_write(5,0xBB);
       EEPROM_write(6,0xCC);
       EEPROM_write(7,0xDD);
      break;
    case '8':
      Serial.print("Erase  0, 3,4, 7");
       EEPROM_erase(0);
       EEPROM_erase(3);
       EEPROM_erase(4);
       EEPROM_erase(7);
      break;

    case '9':
      Serial.println("9 - Write Sequential numbers");
      EEPROM_ewen();

     for (int adr = 0; adr < 128; adr++) {
       EEPROM_write(adr, adr);
     }

     EEPROM_ewds();

     Serial.println("Done");
      // Implement code to write first 128 ASCII characters to the EEPROM chip
      //EEPROM_write(adr, data)
      break;

    default:
      break;
  }
}
