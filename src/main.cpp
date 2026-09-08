#include <DHT.h>

// --- Configuración de pines ---
#define DHTPIN 15        // Pin digital del DHT22
#define DHTTYPE DHT22
#define LDR_PIN 34       // Pin analógico del LDR (salida AO)
#define MQ2_PIN 35       // Pin analógico del MQ-2 (salida AO)

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  Serial.println("Iniciando estación ambiental...");
}

int calcularIndiceConfort(float temp, float hum, int luz, int gas) {
  int indice = 100;

  // Penaliza si la temperatura se aleja de un rango ideal (20-26°C)
  if (temp < 20 || temp > 26) indice -= 20;

  // Penaliza si la humedad se aleja de un rango ideal (40-60%)
  if (hum < 40 || hum > 60) indice -= 20;

  // Penaliza si hay poca luz (ambiente cerrado, sin ventilar)
  if (luz < 500) indice -= 15;

  // Penaliza fuerte si el gas está elevado (lo más crítico)
  if (gas > 1500) indice -= 45;

  if (indice < 0) indice = 0;
  return indice;
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int luz = analogRead(LDR_PIN);
  int gas = analogRead(MQ2_PIN);

  if (isnan(h) || isnan(t)) {
    Serial.println("Error leyendo el DHT22");
    delay(2000);
    return;
  }

  int indice = calcularIndiceConfort(t, h, luz, gas);

  Serial.println("----------------------------");
  Serial.printf("Temperatura: %.1f°C\r\n", t);
  Serial.printf("Humedad: %.1f%%\r\n", h);
  Serial.printf("Luz (crudo): %d\r\n", luz);
  Serial.printf("Gas (crudo): %d\r\n", gas);
  Serial.printf("Índice de confort: %d/100\r\n", indice);

  if (indice >= 70) Serial.println("Estado: Ambiente saludable");
  else if (indice >= 40) Serial.println("Estado: Ambiente regular");
  else Serial.println("Estado: Ambiente crítico");

  delay(3000); // lee cada 3 segundos
}