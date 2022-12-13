#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>

#include <ESP8266HTTPClient.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Arduino_JSON.h>
#include <TimeLib.h>

#define SCREEN_ADDRESS 0x3C //Адрес дисплея
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32 

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char* ssid = "Wi-Fi_login";
const char* password = "Wi-Fi_password";

void setup() {
  Serial.begin(115200);
  //==================
  //Подключение к Wi-Fi
  //==================
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Non Connecting to WiFi..");
  }
  else  
  {
    // Удалось подключиться. Отправляем сообщение
    // о подключении и выводим адрес IP
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }

  //=================
  //Настройка дисплея
  //=================
  // инициализация с I2C адресом 0x3C
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  // очистить буфер
  display.clearDisplay();

}

void loop() {
  double temp;
  String main;
  JSONVar myObject;

  //================
  //===WiFi часть===
  //================
  if (WiFi.status() == WL_CONNECTED) { //Проверка состояния подключения Wi-Fi

    WiFiClient client;
    HTTPClient http;  //Объект класса HttpClient

    http.begin(client, "http://api.openweathermap.org/data/2.5/weather?lat=55.7522&lon=37.6156&appid=326545c004c1e20536e07e0f3738f96e");  //Укажите адрес назначения запроса
    int httpCode = http.GET();  //Отправка запроса

    if (httpCode > 0) { //Проверка кода возврата

      String payload = http.getString();    //Получения результата GET-запроса в виде строки
      Serial.println(payload);
      myObject = JSON.parse(payload);
      temp = (double)myObject["main"]["temp"] - 273.15;
      main = (String)myObject["weather"][0]["main"];
    }
    else {
      Serial.println("Error with get request");
    }

    http.end();   //Закрыть соединение

  }

  //==================================
  //Получение и преобразование времени
  //==================================
  setTime((int)(myObject["dt"]));
  char str_time[12];
  sprintf(str_time, "%02d:%02d %d/%d/%d", hour(), minute(), day(), month(), year());
  
  //======================
  //===Вывод на дисплей===
  //======================
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println(str_time);
  display.println(main);
  display.setTextSize(2);
  display.println("  " + (String)temp + (char)247 + 'C');
  display.display();
  
  delay(30000);
}
