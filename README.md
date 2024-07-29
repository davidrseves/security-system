# security-system

Proyecto de código abierto (Licencia GPL).

## Resumen

Este proyecto se está desarrollando para crear un sistema de seguridad de código abierto.

## Componentes

### 1. Cámara

Cámara que utiliza el dispositivo ESP32-CAM, como el siguiente: [ESP32-CAM en Aliexpress](https://es.aliexpress.com/item/1005006299363624.html?spm=a2g0o.order_list.order_list_main.5.55e9194dtZtBBs&gatewayAdapt=glo2esp).

La cámara está programada con Arduino. En la tarjeta microSD debe haber un archivo de texto llamado `config.json`, que se utiliza para configurar los parámetros de la cámara. Para actualizar estos parámetros, es necesario reiniciar el Arduino.

Ejemplo de `config.json`:

```json
{
  "ssid": "myWifi",
  "password": "myWifiPassword",
  "serverAddress": "192.168.0.18",
  "serverPort": "8080",
  "serverPath": "/upload",
  "captureInterval": "5000"
}
```

### 2. Servidor Backend

(Más detalles sobre el servidor backend se añadirán aquí.)

### 3. Servidor Frontend

(Más detalles sobre el servidor frontend se añadirán aquí.)

## Etapas de Implementación

### 1. Hacer Funcionar lo Básico

- [x] Primera implementación de la cámara, que suba imágenes sin autenticación y sin seguridad.
- [ ] Primera implementación del backend, que guarde las imágenes y las sirva en un endpoint.

### 2. Añadir Autenticación y Seguridad

- [ ] Cifrar imágenes (extremo a extremo).
- [ ] Autenticar cámaras mediante JWT.

### 3. Añadir Usuarios

- [ ] Autenticar usuarios mediante JWT.
- [ ] Asociar usuarios a cámaras.

(Más etapas se añadirán a medida que avance la implementación.)
