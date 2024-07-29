/******************************************************************************\
* Archivo: arduino-camera.ino                                                  *
* Fecha: Julio 2024                                                            *
* Autor: davidrseves                                                           *
* Repositorio: https://github.com/davidrseves/security-system                  *
* Descripción: Este archivo contiene el código fuente para la cámara           *
\******************************************************************************/


#include "FS.h"           // Micro SD
#include "SD_MMC.h"
#include "SPI.h"
#include <WiFi.h>         // Wifi
#include "esp_camera.h"   // Camera
#include "camera_pins.h"
#include <ArduinoJson.h>  // JSON
#include <stdexcept>      // Excepciones


void setup() {}


void loop() {
  
  try {

    initModules();  // Inicializar conexiones con los modulos
    DynamicJsonDocument configDoc = getConfigObject();  // Obtener config de SD
    connectWiFi(configDoc["ssid"],  configDoc["password"]); // Conectar WiFi

    while (true) {
      takeAndUploadPhoto(configDoc["serverAddress"], configDoc["serverPort"],
                                configDoc["serverPath"]); // Tomar y subir foto

      goToSleep(configDoc["captureInterval"].as<int>());  // Duerme ms
    }
    
  } catch (const std::runtime_error &e) {
    Serial.println(e.what());
    Serial.println("Reinicie el dispoitivo");
    goToSleep(); // Modo de dormir profundo

  }
}


/*******************************************************************************
 *                           FUNCIONES AUXILIARES                              *
 ******************************************************************************/


/*
 * Pre:
 *      - Necesita que tenga conectado el lector de tarjetas micro SD
 *      - Necesita que tenga conectada la CAMERA_MODEL_AI_THINKER
 * Post:
 *      - Inicializa la comunicación serial a 115200 baudios
 *      - Inicializa la conexión con la tarjeta SD
 *      - Inicializa la conexión con la cámara
 * Throws:
 *      - std::runtime_error si hay algún error, sin solución
 */
void initModules() {

  /*
   * Iniciar la comunicación serial
   */
  
  Serial.begin(115200);
  Serial.println();

  /*
   * Iniciar conexión con la tarjeta SD
   */
  
  Serial.println("Info: Iniciando conexion con la tarjeta SD");
  if (!SD_MMC.begin("/sdcard", true)) { // true para usar modo más confiable
    throw std::runtime_error("Error: Iniciando conexion con la tarjeta SD");
  }
  Serial.println("Info: Conexion con la tarjeta SD iniciada correctamente");

  /*
   * Iniciar conexión con la cámara
   */

  camera_config_t config = getCameraConfig();

  Serial.println("Info: Iniciando conexion con la camara");
  if (esp_camera_init(&config) != ESP_OK) {
    throw std::runtime_error("Error: Iniciando conexion con la camara");
  }
  Serial.println("Info: Conexion con la camara iniciada correctamente");

  // Esperar para asegurar que se inicia correctamente
  delay(500);
}


/*
 * Pre: ---
 * Post:
 *      - Devuelve la configuración para la CAMERA_MODEL_AI_THINKER
 */
camera_config_t getCameraConfig() {

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  
  config.frame_size = FRAMESIZE_HD;             // 1280x720
  //config.frame_size = FRAMESIZE_UXGA;         // 1600x1200
  
  config.pixel_format = PIXFORMAT_JPEG;         // For streaming
  //config.pixel_format = PIXFORMAT_RGB565;     // For face recognition

  //config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;  // Every frame is processed
  config.grab_mode = CAMERA_GRAB_LATEST;  // Just the latest frame is processed
  
  //config.fb_location = CAMERA_FB_IN_DRAM; // Frame buffer in internal DRAM
  config.fb_location = CAMERA_FB_IN_PSRAM;  // Frame buffer in external PSRAM

  config.jpeg_quality = 12; // Quality of JPEG. 0-63 lower means higher quality
  config.fb_count = 2;  // Number of frame buffers to be allocated

  return config;
}


/*
 * Pre:
 *      - Necesita un fichero en la tarjeta SD "/config.json" que contenga la
 *        configuración necesaria
 * Post:
 *      - Devuelve objeto (diccionario) con las configuraciones mapeadas
 * Throws:
 *      - std::runtime_error si hay algún error, sin solución
 */
DynamicJsonDocument getConfigObject() {

  /*
   * Acceso a "/config.json"
   */

  String configFileRaw;

  // Leer el fichero /config.json
  File configFile = SD_MMC.open("/config.json"); // Abrir fichero
  if (!configFile) {
    throw std::runtime_error("Error: Abriendo el fichero /config.json");
  }

  // Leer todo el fichero y guardar en configFileRaw
  while (configFile.available()) {
    configFileRaw += char(configFile.read());
  }

  configFile.close(); // Cerrar fichero
  Serial.println("Info: Contenido del fichero /config.json: " + configFileRaw);


  /*
   * Mapear JSON a objeto
   */

  // Estimar y reservar tamaño inicial del DynamicJsonDocument
  DynamicJsonDocument doc(1024);  // Si necesita más espacio se auto ajusta

  // Parsear el JSON
  DeserializationError error = deserializeJson(doc, configFileRaw);

  // Verifica si hubo un error durante el parseo
  if (error) {
    throw std::runtime_error(std::string("Error: Parseando JSON de configuración, ") + error.c_str());
  }


  /*
   * Comporbar parámetros obligatorios
   */

  // Wifi
  if (doc["ssid"].isNull() || doc["password"].isNull()) {
    throw std::runtime_error("Error: Parametros sin configurar: ssid o password");
  }

  // Server
  if (doc["serverAddress"].isNull() || doc["serverPort"].isNull() || doc["serverPath"].isNull()) {
    throw std::runtime_error("Error: Parametros sin configurar: serverAddress o serverPort o serverPath");
  }

  // Sleep or capture interval
  if (doc["captureInterval"].isNull()) {
    throw std::runtime_error("Error: Parametro sin configurar: captureInterval");
  }

  return doc; // Devuelve el documento JSON mapeado en un diccionario
}


/*
 * Pre:
 *      - Necesita los strings ssid y password del wifi
 * Post:
 *      - Establece la conexión WiFi
 * Throws:
 *      - std::runtime_error si hay algún error, sin solución
 */
void connectWiFi(const String ssid, const String password) {

  WiFi.begin(ssid, password);
  //WiFi.setSleep(false); // Mantener siempre activo

  Serial.println("Info: Estableciendo conexión WiFi");

  // Intentos para establecer conexión WiFi (máximo 5 intentos)
  for (unsigned int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++) {
    Serial.println("Info: Fallo en la conexión WiFi. Reintentando");
    delay(500); // Esperar 500 ms antes de volver a intentar
  }

  // Verificar si se estableció la conexión WiFi
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Info: WiFi conectado, con IP asignada: "
                                                  + WiFi.localIP().toString());
  } else {  // Si no se ha podido establecer conexión
    throw std::runtime_error("Error: No se pudo establecer conexión WiFi");
  }
}


/*
 * Pre:
 *      - Necesita haber inicializado la cámara
 *      - Necesita los strings de configuración del servidor
 * Post:
 *      - Captura una foto y la envía al servidor mediante HTTP POST en
 *        el campo "imageFile"
 * Throws:
 *      - std::runtime_error si hay algún error, sin solución
 */
void takeAndUploadPhoto(const String serverAddress, const String serverPort,
                                                      const String serverPath) {

  /*
   * Capturar una imagen
   */
  
  Serial.println("Info: Capturando foto");
  camera_fb_t* fb = esp_camera_fb_get();;   // Puntero que apunta al buffer
  if (!fb) {
    esp_camera_fb_return(fb); // Liberar buffer de cámara
    throw std::runtime_error("Error: No se pudo capturar la foto");
  }
  Serial.println("Info: Foto capturada correctamente");


  /*
   * Enviar foto al endpoint
   */

  WiFiClient client;

  Serial.println("Info: Conectado con el servidor: " + serverAddress + ":"
                                                    + serverPort + serverPath);
  if (client.connect(serverAddress.c_str(), serverPort.toInt())) {
    
    // Preparar encabezados y cuerpo de la solicitud
    String head = "--davidrseves\r\nContent-Disposition: form-data; name=\"imageFile\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--davidrseves--\r\n";
    
    // Enviar solicitud HTTP POST
    client.println("POST " + serverPath + " HTTP/1.1");
    client.println("Host: " + serverAddress);
    client.println("Content-Length: " + String(fb->len + head.length() + tail.length()));
    client.println("Content-Type: multipart/form-data; boundary=davidrseves");
    client.println();
    client.print(head);


    // Enviar datos de la imagen en bloques de 1024 bytes
    uint8_t* fbBuf = fb->buf;  // Puntero al inicio del buffer de la imagen
    size_t fbLen = fb->len;    // Longitud total de los datos de la imagen
    size_t blockSize = 1024;   // Tamaño del bloque en bytes

    // Iterar a través de los datos de la imagen en bloques de 1024 bytes
    for (size_t n = 0; n < fbLen; n += blockSize) {
      // Calcular el tamaño del bloque actual
      size_t currentBlockSize = (n + blockSize < fbLen) ? blockSize : fbLen - n;

      // Enviar el bloque actual al cliente
      client.write(fbBuf + n, currentBlockSize);
    }

    client.print(tail);
    
    esp_camera_fb_return(fb); // Liberar buffer de cámara
    
    // Leer respuesta del servidor
    int timeoutTimer = 10000; // 10 segundos
    long startTimer = millis();
    boolean state = false;  // Para empezar a capturar el body de la respuesta
    String getAll;
    String getBody;
    
    while (!(getBody.length() > 0) && ((startTimer + timeoutTimer) > millis())) {
      while (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (getAll.length() == 0) { state = true; }
          getAll = "";
        } else if (c != '\r') { getAll += String(c); }
        if (state == true) { getBody += String(c); }
        startTimer = millis();  // Reiniciar el temporizador
      }
    }
    client.stop();
    Serial.println("Info: HTTP Response: " + getBody);

  } else {
    throw std::runtime_error("Error: Conectando con el servidor");
  }
}


/*
 * Pre:
 *      - Necesita el tiempo de dormir en milisegundos
 * Post:
 *      - Programa el temporizador y entra en modo de dormir ligero
 */
void goToSleep(int sleepTime) {

  Serial.println("Info: Durmiendo por " + String(sleepTime) + " ms");
  delay(sleepTime);
  Serial.println("Info: Despertando del modo dormir ligero");
}


/*
 * Pre: ---
 * Post:
 *      - Entra en modo de dormir profundo indefinidamente
 */
void goToSleep() {

  Serial.println("Info: Entrando en modo de dormir profundo indefinidamente");

  while (true) {
    delay(1000000);
  }
}



