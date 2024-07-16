// Micro SD
#include "FS.h"
#include "SD_MMC.h"
#include "SPI.h"

// Wifi
#include <WiFi.h>

// Camera
#include "esp_camera.h"
#include "camera_pins.h"


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
 *      - Necesita que tenga conectada la CAMERA_MODEL_AI_THINKER
 * Post:
 *      - Inicializa la comunicación serial a 115200 baudios
 *      - Inicializa la conexión con la tarjeta SD
 *      - Inicializa la conexión con la cámara
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
  if (!SD_MMC.begin("/sdcard", true)) { // true para usar modo 1-bit (más confiable)
    Serial.println("Error: Iniciando conexion con la tarjeta SD");
    //throw std::runtime_error("...");
  }
  Serial.println("Info: Conexion con la tarjeta SD iniciada correctamente");

  /*
   * Iniciar conexión con la cámara
   */

  camera_config_t config = getCameraConfig();

  Serial.println("Info: Iniciando conexion con la camara");
  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Error: Iniciando conexion con la camara");
    //throw std::runtime_error("...");
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
  //config.pixel_format = PIXFORMAT_RGB565;     // For face detection/recognition

  //config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;  // For ensuring every frame is processed
  config.grab_mode = CAMERA_GRAB_LATEST;        // For capturing the latest image available
  
  //config.fb_location = CAMERA_FB_IN_DRAM;     // Frame buffer is placed in internal DRAM
  config.fb_location = CAMERA_FB_IN_PSRAM;      // Frame buffer is placed in external PSRAM

  config.jpeg_quality = 12;                     // Quality of JPEG output. 0-63 lower means higher quality
  config.fb_count = 2;                          // Number of frame buffers to be allocated. If more than one, then each frame will be acquired (double speed)

  return config;
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




