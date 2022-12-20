// Canal Youtube https://youtube.com/alswnet?sub_confirmation=1

//Librerias para ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;


#include "data.h"

int pinLed = 2;
boolean Estado = false;

const uint32_t TiempoEsperaWifi = 5000;

unsigned long TiempoActual = 0;
unsigned long TiempoAnterior = 0;
const long TiempoCancelacion = 500;

WiFiServer servidor(80);

void setup() {
  Serial.begin(115200);
  Serial.println("\nIniciando multi Wifi");

  pinMode(pinLed, OUTPUT);
  digitalWrite(pinLed, 0);

  wifiMulti.addAP(ssid_1, password_1);
  wifiMulti.addAP(ssid_2, password_2);

  WiFi.mode(WIFI_STA);
  Serial.print("Conectando a Wifi ..");
  while (wifiMulti.run(TiempoEsperaWifi) != WL_CONNECTED) {
    Serial.print(".");
  }
  Serial.println(".. Conectado");
  Serial.print("SSID:");
  Serial.print(WiFi.SSID());
  Serial.print(" ID:");
  Serial.println(WiFi.localIP());

  servidor.begin();

}

void loop() {
  WiFiClient cliente = servidor.available();

  if (cliente) {
    Serial.println("Nuevo Cliente");
    TiempoActual = millis();
    TiempoAnterior = TiempoActual;
    String LineaActual = "";

    while (cliente.connected() && TiempoActual - TiempoAnterior <= TiempoCancelacion) {
      if (cliente.available()) {
        TiempoActual = millis();
        char Letra = cliente.read();
        if (Letra == '\n') {
          if (LineaActual.length() == 0) {
            digitalWrite(pinLed, Estado);
            ResponderCliente(cliente);
            break;
          } else {
            Serial.println(LineaActual);
            VerificarMensaje(LineaActual);
            LineaActual = "";
          }
        }  else if (Letra != '\r') {
          LineaActual += Letra;
        }
      }
    }

    cliente.stop();
    Serial.println("Cliente Desconectado");
    Serial.println();
  }
}

void VerificarMensaje(String Mensaje) {
  if (Mensaje.indexOf("GET /encender") >= 0) {
    Serial.println("Encender Led");
    Estado = true;
  } else if (Mensaje.indexOf("GET /apagar") >= 0) {
    Serial.println("Apagar Led");
    Estado = false;
  }
}

void ResponderCliente(WiFiClient& cliente) {
  cliente.print("<h1><center>Servidor Web con Esp8266</center></h1>");
  cliente.print("<hr>");
  cliente.print("<h1>Bienvenido</h1>");
  cliente.print("<br>IP:");
  cliente.print(cliente.remoteIP());
  cliente.print("<br>Estado del led: ");
  cliente.print(Estado ? "Apagado" : "Encendido");
  cliente.print("<br>");
  cliente.print("<br>");
  cliente.print("<button>");
  cliente.print("<a href = '/");
  cliente.print(Estado ? "apagar" : "encender");
  cliente.print("'> Cambiar Estado" );
  cliente.print("</button>");
  cliente.print("</html>");
}
