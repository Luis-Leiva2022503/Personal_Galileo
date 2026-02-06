// ------------------------------- LIBRERIAS -------------------------------
#include <SPI.h>                 //Permite la comunicación entre el RFID y el ESP32
#include <MFRC522.h>             //Permite leer los accesos al RFID por medio de las UID de las tarjetas
#include <Wire.h>                //Permite la comunicación entre la pantalla OLED y el ESP32
#include <Adafruit_GFX.h>        //Maneja los textos, fuentes, formas, etc. que recibe la pantalla OLED
#include <Adafruit_SSD1306.h>    //Es el controlador que le permite funcionar a la pantalla OLED

#define Escribir_MS(pin) Serial.print(pin)
#define Escribirsl_MS(pin) Serial.println(pin)

// ---------- OLED ----------
#define ANCHO 128
#define ALTO 64
#define RESET -1
#define Escribir_OLED(pin) display.print(pin)
#define Escribirsl_OLED(pin) display.println(pin)
#define Tamaño(pin) display.setTextSize(pin)
#define Cursor(pin, PIN) display.setCursor(pin, PIN);
#define Borrar display.clearDisplay();
Adafruit_SSD1306 display(ANCHO, ALTO, &Wire, RESET);

// ---------- RFID ----------
#define SS_PIN 5
#define RST_PIN 27   
MFRC522 RFID(SS_PIN, RST_PIN);

// ---------- DATOS ----------
String uidValido = "D3 2F 06 80";

String nombre = "Luis Fernando";
String apellido = "Leiva Quezada";
String carnet = "26001094";

// ---------- CONTROL ----------
int entradas = 0;
int salidas = 0;
bool dentro = false;

void setup() {
  Serial.begin(115200);

  SPI.begin();
  RFID.PCD_Init();

  Wire.begin(21, 22);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.setTextColor(WHITE);
  Borrar;
  Cursor(0,0);

  Escribirsl_MS("Sistema de asistencia iniciado");
  Escribirsl_MS("Acerque la tarjeta...");
}

void loop() {

  if (!RFID.PICC_IsNewCardPresent()) return;
  if (!RFID.PICC_ReadCardSerial()) return;

  String uidLeido = "";
  for (byte i = 0; i < RFID.uid.size; i++) {
    if (RFID.uid.uidByte[i] < 0x10) uidLeido += "0";
    uidLeido += String(RFID.uid.uidByte[i], HEX);
    if (i < RFID.uid.size - 1) uidLeido += " ";
  }
  uidLeido.toUpperCase();

  if (uidLeido == uidValido) {
    
    Escribir_MS("ACCESO: ");
    Escribirsl_MS(uidLeido);
    Escribir_MS("Nombre: ");
    Escribir_MS(nombre);
    Escribir_MS(" ");
    Escribirsl_MS(apellido);
    Escribir_MS("Carnet: ");
    Escribirsl_MS(carnet);
    
    if (!dentro) {
      entradas++;
      dentro = true;
    } else {
      salidas++;
      dentro = false;
    }

    Borrar;
    Cursor(0,0);
    Tamaño(2);
    Escribir_OLED("NOM:");
    Tamaño(1);
    Cursor(48,0);
    Escribir_OLED(nombre);
    Cursor(48,8);
    Escribir_OLED(apellido);
    Cursor(0, 18);
    Escribir_OLED("Carnet: ");
    Escribir_OLED(carnet);
    Tamaño(2);
    Cursor(0,30);
    Escribir_OLED("E: ");
    Escribir_OLED(entradas);
    Cursor(80,30);
    Escribir_OLED("S: ");
    Escribir_OLED(salidas);

  } else {
    Escribirsl_MS("CARNET NO REGISTRADA");
    Escribir_MS("ACCESO: ");
    Escribirsl_MS(uidLeido);
    Escribir_OLED("NO REGISTRADO");
  }

  display.display();

  RFID.PICC_HaltA();
  RFID.PCD_StopCrypto1();

  delay(2000);
}