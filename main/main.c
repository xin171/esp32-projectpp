#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

// ===================== 配置区 =====================
// WiFi 信息
const char* WIFI_SSID     = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";

// MQTT 服务器信息
const char* MQTT_SERVER   = "test.mosquitto.org";
const int   MQTT_PORT     = 1880;
const char* MQTT_TOPIC    = "esp32/wokwi/led";
const char* MQTT_CLIENTID = "esp32_wokwi_client_";

// ILI9341 屏幕引脚
#define TFT_CS   5
#define TFT_DC   2
#define TFT_RST  4

// ===================== 实例化对象 =====================
Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// ===================== 函数声明 =====================
void initScreen();
void showStatus(const char* status, uint16_t color);
void connectWiFi();
void mqttCallback(char* topic, byte* payload, unsigned int length);
bool reconnectMQTT();

// ===================== 初始化 =====================
void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 MQTT + ILI9341 启动");

  // 初始化屏幕
  initScreen();
  showStatus("System Starting...", ILI9341_BLUE);

  // 连接WiFi
  connectWiFi();

  // 配置MQTT
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
}

// ===================== 主循环 =====================
void loop() {
  // WiFi断开则重连
  if (WiFi.status() != WL_CONNECTED) {
    showStatus("WiFi Lost", ILI9341_RED);
    connectWiFi();
  }

  // MQTT断开则重连
  if (!mqttClient.connected()) {
    reconnectMQTT();
    delay(1000);
  }

  mqttClient.loop();
}

// ===================== 屏幕初始化 =====================
void initScreen() {
  tft.begin();
  tft.setRotation(3);    // 横屏
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
}

// ===================== 屏幕显示状态 =====================
void showStatus(const char* status, uint16_t color) {
  tft.fillScreen(ILI9341_BLACK);

  // 标题
  tft.setTextSize(2);
  tft.setTextColor(ILI99341_WHITE);
  tft.setCursor(10, 30);
  tft.println("=== MQTT Monitor ===");

  // 状态信息
  tft.setTextSize(3);
  tft.setTextColor(color);
  tft.setCursor(10, 80);
  tft.println(status);

  // IP地址
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_GREEN);
  tft.setCursor(10, 150);
  tft.print("IP: ");
  tft.println(WiFi.localIP().toString());
}

// ===================== WiFi连接 =====================
void connectWiFi() {
  showStatus("Connecting WiFi...", ILI9341_ORANGE);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20) {
    delay(500);
    Serial.print(".");
    retry++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    showStatus("WiFi Connected", ILI9341_GREEN);
    Serial.println("\nWiFi 连接成功！");
  } else {
    showStatus("WiFi Failed", ILI9341_RED);
    Serial.println("\nWiFi 连接失败！");
  }
}

// ===================== MQTT消息回调 =====================
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("收到消息: ");
  
  char msgBuffer[256];
  for (int i = 0; i < length; i++) {
    msgBuffer[i] = (char)payload[i];
  }
  msgBuffer[length] = '\0';

  Serial.println(msgBuffer);
  showStatus(msgBuffer, ILI9341_YELLOW);
}

// ===================== MQTT重连 =====================
bool reconnectMQTT() {
  showStatus("Connecting MQTT...", ILI9341_ORANGE);

  // 生成随机客户端ID，防止冲突
  String clientId = MQTT_CLIENTID;
  clientId += String(random(0xffff), HEX);

  if (mqttClient.connect(clientId.c_str())) {
    mqttClient.subscribe(MQTT_TOPIC);
    showStatus("MQTT Connected!", ILI9341_GREEN);
    Serial.println("MQTT 连接成功！");
    return true;
  } else {
    showStatus("MQTT Failed", ILI9341_RED);
    Serial.print("MQTT 错误码: ");
    Serial.println(mqttClient.state());
    return false;
  }
}
