[Original Project - corbanmailloux](https://github.com/corbanmailloux/esp-mqtt-dht)

# Iot with MQTT + DHT Temperature and Humidity Sensors Using ESP8266 Modules (NODEMCU)
This project gives a simple way to deploy temperature and humidity sensors around your house using cheap components and the MQTT protocol.

The sensor publishes to 2 MQTT topics:
- Temperature topic (example: `home/room/temperature`): The sensor publishes the temperature in Celsius (example: `25.10`).
- Humidity topic (example: `home/room/humidity`): The sensor publishes the relative humidity in percent (example: `37.40`).

### Additional Libraries
- DHT Sensor Library https://github.com/adafruit/DHT-sensor-library
- MQTT Client Library https://github.com/knolleary/pubsubclient
