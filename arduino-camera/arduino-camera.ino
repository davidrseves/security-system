// Micro SD
#include "FS.h"
#include "SD_MMC.h"
#include "SPI.h"

// Wifi
#include <WiFi.h>

void setup() {

  // try catch: si excepcion mensaje de reiniciar y parar todo
  initModules();  // Inicializar conexiones con los modulos
  connectWiFi();  // Iniciar conexión WiFi

}

void loop() {

}


/*******************************************************************************
 *                           FUNCIONES AUXILIARES                              *
 *******************************************************************************/


/*
 * Pre:
 *      - Necesita que tenga conectado el lector de tarjetas micro SD
 * Post:
 *      - Inicializa la comunicación serial a 115200 baudios
 *      - Inicializa la coneción con la tarjeta SD
 */
void initModules() {

  // Iniciar la comunicación serial
  Serial.begin(115200);

  // Iniciar conexión con la tarjeta SD
  Serial.println("Info: Iniciando conexion con la tarjeta SD");
  if (!SD_MMC.begin("/sdcard", true)) { // true para usar modo 1-bit (más confiable)
    Serial.println("Error: Iniciando conexion con la tarjeta SD");
    //throw std::runtime_error("...");
  }
  Serial.println("Info: Conexion con la tarjeta SD iniciada correctamente");

  delay(500); // Esperar para asegurar que se inicia correctamente
}


/*
 * Pre:
 *      - Necesita un fichero en la tarjeta SD "/config.txt" que contenga en la
 *        primera línea el ssid del WiFi y en la segunda línea el password
 * Post:
 *      - Establece la conexión WiFi
 */
void connectWiFi() {

  /*
   * Acceso a "/config.txt"
   */

  String ssid, password;

  // Leer el fichero /config.txt
  File configFile = SD_MMC.open("/config.txt"); // Abrir fichero
  if (!configFile) {
    Serial.println("Error: Abriendo el fichero /config.txt");
    //throw std::runtime_error("...");
  }

  // Leer la primera línea y la guardar en la variable ssid
  if (configFile.available()) {
    ssid = configFile.readStringUntil('\n');
  } else {
    Serial.println("Error: Leyendo la primera línea del fichero /config.txt ");
    //throw std::runtime_error("...");
  }

  // Leer la segunda línea y la guardar en la variable password
  if (configFile.available()) {
    password = configFile.readStringUntil('\n');
  } else {
    Serial.println("Error: Leyendo la segunda línea del fichero /config.txt ");
    //throw std::runtime_error("...");
  }

  Serial.println("Info: Contenido del fichero /config.txt, ssid: " + ssid + ", password: " + password);

  configFile.close(); // Cerrar fichero


  /*
   * Establecer conexión WiFi
   */

  WiFi.begin(ssid, password);
  //WiFi.setSleep(false); // Mantener siempre activo

  Serial.println("Info: Intentando establecer conexión WiFi");

  // Intentos para establecer conexión WiFi (máximo 5 intentos)
  for (unsigned int i = 0; i < 5 && WiFi.status() != WL_CONNECTED; i++) {
    Serial.println("Info: Fallo en la conexión WiFi. Reintentando...");
    delay(500); // Esperar 500 ms antes de volver a intentar
  }

  // Verificar si se estableció la conexión WiFi
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Info: WiFi conectado, con IP asignada: " + WiFi.localIP().toString());
  } else {  // Si no se ha podido establecer conexión
    Serial.println("Error: No se pudo establecer conexión WiFi");
    //throw std::runtime_error("...");
  }
}

