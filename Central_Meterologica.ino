#include <ThingerESP8266.h>
#include "DHT.h"

// libreria de la balanza
#include "HX711.h"
// HX711.DOUT - pin #A1
// HX711.PD_SCK - pin #A0
HX711 scale(D5, D6); // declaramos los pines de DT y SCK
//------------------------------------------------------------

#define USERNAME "usuario"
#define DEVICE_ID "Dispositivo"
#define DEVICE_CREDENTIAL "Credencial"

#define SSID "Nombre de la red "
#define SSID_PASSWORD "Contraseña de la red"

ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

//........................................Configuracion del Sensor DHT11
#define DHTPIN D1 //conectado al pin D1
#define DHTTYPE DHT11
DHT dht11(DHTPIN, DHTTYPE);
//.......................................

float temperatura, humedad, precipita; //--> Variables
unsigned long tiempoAnterior;
unsigned long periodo=5000;
void setup() {
  tiempoAnterior = millis();
  Serial.begin(9600);
  scale.set_scale(660); // aquí declaramos la ESCALA 660
  scale.tare(20); // reset la escala a 0

  pinMode(LED_BUILTIN, OUTPUT);
  thing.add_wifi(SSID, SSID_PASSWORD);
  digitalWrite(LED_BUILTIN, HIGH); // apago el LED de la placa
  dht11.begin(); //--> Comienza a leer el Sensor de temperatura y humedad

  //..................................................Enviar datos de temperatura y humedad a Thinger.io
  // El simbolo ">>" significa que transmite datos
  thing["dht11"] >> [](pson & out) {
    out["temperatura"] = temperatura;
    out["humedad"] = humedad;
  };
  //..................................................

  thing["HX711"] >> [] (pson & out) {
    out["precipita"] = precipita;
  };

}

void loop() {
  
  if ((millis() - tiempoAnterior) >= periodo) {
    thing.handle();
    tiempoAnterior = millis();
    //----------------------------------------
    temperatura = dht11.readTemperature();
    humedad = dht11.readHumidity();
    //----------------------------------------

    if (((scale.get_units(10)) / 1.375) < 1)
      precipita = 0;
    else
      precipita = ((scale.get_units(20)) / 1.375); // divido por la superficie del pluviometro 138

    //------- Muestro los mm en el monitor
    //Serial.print("\t| mm lluvia:\t");
    // Serial.println (precipita);

    scale.power_down(); // “duerme el sensor” para reiniciar
  }

  scale.power_up();
}
