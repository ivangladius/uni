
#include <OneWire.h>

#define OneWirePin 2

#define BUTTON PB5

UTFT screen(ITDB32S,19,18,17,16);

OneWire one_wire_ds(OneWirePin);

void showAddress(int number, byte address[8])
{
  // Im Array address enthaltene Daten kompakt sedezimal ausgeben
  byte i;

  Serial.print(number);
  Serial.print(" - ");
  if (OneWire::crc8(address, 7) != address[7])
    { Serial.println("hat keinen gueltigen CRC!"); }
  else
    {
      //alle ist ok, anzeigen
      for (i = 0; i < 8; i++)
        {
          if (address[i] <= 0xF)
            { Serial.print("0"); }
          Serial.print(address[i],HEX);
        }
      Serial.println("");
    }
}

float getTemperature(bool type_s, byte address[8])
{
  // Temperaturwert des adressieren Sensors auslesen
  // Dabei wird zwischen DS18S20 udn den anderen unterschieden
  byte data[12];
  int16_t raw;
  byte i;
  
  one_wire_ds.reset();
  one_wire_ds.select(address);
  one_wire_ds.write(0x44, 1);        // Start Messung, parasitaere Versorgung an
  delay(1000);                  // eventuell reichen auch 750 ms
  one_wire_ds.reset();
  one_wire_ds.select(address);    
  one_wire_ds.write(0xBE);           // Read Scratchpad
  for ( i = 0; i < 9; i++)
    { data[i] = one_wire_ds.read(); }
  raw = (data[1] << 8) | data[0];
  if (type_s) 
    {
      raw = raw << 3;
      if (data[7] == 0x10)
        // Vorzeichen expandieren 
        { raw = (raw & 0xFFF0) + 12 - data[6]; }
    }
  else
    {
      byte cfg = (data[4] & 0x60);
      // Aufloesung bestimmen, bei niedrigerer Aufloesung sind
      // die niederwertigen Bits undefiniert -> auf 0 setzen
      if (cfg == 0x00) raw = raw & ~7;      //  9 Bit Aufloesung,  93.75 ms
      else if (cfg == 0x20) raw = raw & ~3; // 10 Bit Aufloesung, 187.5 ms
      else if (cfg == 0x40) raw = raw & ~1; // 11 Bit Aufloesung, 375.0 ms
      // Default ist 12 Bit Aufloesung, 750 ms Wandlungszeit
    }
  return ((float)raw / 16.0);
}

void lookUpSensors()
{
  // Alle DS18x20-Sensoren suchen, Adresse ausgeben
  byte address[8];
  bool type_s = false;
  bool found = false;
  bool detect = false;
  int count = 0;
  
  while (one_wire_ds.search(address))
    {
      // Adresse anzeigen, wenn ein Temperatursensor gefunden wurde
      if ((address[0] == 0x10) or (address[0] == 0x22) or (address[0] == 0x28))
        {
          found = true;
          count++;
          showAddress(count, address);
        }
    } // ... while
  if (!found)
    { Serial.println("Keine Sensoren gefunden"); }
}

void readSensors()
{
  // Bei allen DS18x20-Sensoren die Temperatur auslesen und ausgeben
  byte address[8];
  bool type_s = false;
  bool detect = false;
  float celsius;
  int count = 0;
  char temp[5];
  

  while (one_wire_ds.search(address))
    {
      detect = false;
      // Ist es ein DS18x20? Wenn ja, welcher Typ?
      switch (address[0])
        {
        case 0x10:
          detect = true;
          type_s = true;
          break;
        case 0x28:
          detect = true;
          type_s = false;
          break;
        case 0x22:
          detect = true;
          type_s = false;
          break;
        } 
      if (detect)
        {
          snprintf(temp, "%.2lf°C", getTemperature(type_s, address));
          screen.print(temp,
                       getDisplayXSize() / 2,
                       getDisplayYSize() - 10,
                       0);
                       

          // Temperatur auslesen und anzeigen
        }
    } // ... while
  one_wire_ds.reset_search();   
}


void danger() {
  screen.setColor(255, 0, 0);
  screen.drawRect(
      (screen.getDisplayXSize() / 2) - 20,
      (screen.getDisplayYSize() / 2) - 10,
      (screen.getDisplayXSize() / 2) + 20,
      (screen.getDisplayYSize() / 2) + 20);
  screen.setColor(0, 0, 0);
}

void setup() {

  Serial.begin(9600);

  pinMode(BUTTON, INPUT_PULLUP);

  attachInterupt(digitalPinToInterrupt(BUTTON), danger, RISING);

  lookUpSensors();

  screen.fillScr(0, 0, 0);
}

void loop() {

  readSensors(); // draws temperature
  delay(1000);

  // if button is pressed interrupt -> danger screen comes up
}
