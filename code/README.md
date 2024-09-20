# ELE-D22-NguyenDucCanh

```
#include <WiFi.h> // khai báo thư viện wifi của esp
#include <PubSubClient.h> // khai báo thư viện pubsubclient để cho phép ESP32 kết nối với một MQTT Broker để Publish và Subcribe 
#include <DHTesp.h> // thư viện cảm biến
#include <ArduinoJson.h> // thư viện xử lý Json
#include <WiFiClientSecure.h>

// WiFi
/// WiFi
// const char *ssid = "Linh Canh"; // tên wifi  
// const char *password = "Dung4gdi";  // nhập passwifi

const char *ssid = "giabao";
const char *password = "nguyengiabao12";

// const char *ssid = "Xuong";
// const char *password = "68686868";
// DHT22

#define DHTpin 4
#define DHTTYPE DHT22

const int led = 2;
const int button = 13;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50; // Delay for debounce

bool lastButtonState = HIGH;
bool buttonState = HIGH;
bool ledState = LOW;

// const int time_state = 1000;

DHTesp dht; // khởi tạo đối tượng DHTesp là dht cho phép sử dụng các chức năng của thư viện cảm biến

unsigned long timeUpdata = millis();

// MQTT Broker
const char *mqtt_broker = "1f6c9630668e42d9969537af3ab11396.s1.eu.hivemq.cloud"; // xác định địa chỉ của MQTT Broker được đặt ở dạng "broker.emqx.io"
const char *mqtt_username = "nguyencanh178"; // tên người dùng để xác thức MQTT
const char *mqtt_password = "canhneee"; // mật khẩu xác thức MQTT
const int mqtt_port = 8883; // số cổng MQTT Broker được mặc định là 8883

WiFiClientSecure espClient; // khởi tạo đối tượng wificlient với tên là espClient
PubSubClient client (espClient); // khởi tạo đối tượng pubsubclient sử dụng đối tượng wificlient 

void setup_Wifi(){
  Serial.println("Connecting to ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.print("...");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void publishMessage(const char* topic, String payload, bool retained){ // xuất bản một tin nhắn đến một chủ đề cụ thể
  if (client.publish(topic, payload.c_str(), true)){ // nếu publish thành công sẽ trả về giá trị true
    Serial.println("Message published ["+String(topic)+"]: " + payload); // khi xuất bản thành công thì in thông báo có dạng "Message published [topic]: payload"
  }
}

void callback (char* topic, byte* payload, unsigned int length){ // khởi tạo hàm callback khi có một chủ đề được đăng ký
  int buttonState = digitalRead(button);
  Serial.println ("Message arrived in topic: "); // thông báo tin nhắn đã đc đến trong chủ đề
  Serial.println (topic); // in ra chủ đề tin nhắn
  Serial.println ("Message: "); // in ra tin nhắn
  String message;
  for (int i = 0; i < length; i++){ // vòng lặp để đọc và in ra từng ký tự của message
    message += (char)payload[i];
  }
  Serial.println (message); // kết thúc message

  if (strcmp(topic, "Led Control") == 0){
    if (message == "on"){ 
      digitalWrite (led, HIGH); // bật LED khi nhận đc tin nhắn on
      publishMessage ("led", "LED is turn ON", true); // phát hành tin nhắn LED is turn on
    }
    else if (message == "off") {
      digitalWrite (led, LOW);
      publishMessage ("led", "LED is turn OFF", true);
    }
  }
}

void setup (){
  Serial.begin(115200); // baurd rate của esp
  while (!Serial){
    delay (100);
  }
  dht.setup(DHTpin, DHTesp::DHT22);
  setup_Wifi();
  espClient.setInsecure();
  client.setCallback(callback);
  pinMode (led, OUTPUT);
  pinMode (button, INPUT_PULLUP);
  digitalWrite(led, ledState);
}

void MQTT_Connect (void){
  // kết nối với một MQTT Broker
  client.setServer (mqtt_broker, mqtt_port); // thiết lập mqqtt_broker và cổng kết nối
  client.setCallback (callback); // thiết lập hàm callback cho client
  while (!client.connected()){ // vòng lặp sẽ chạy khi client chưa kết nối
    String client_id = "esp32-client"; // khởi tạo chuỗi client_id với giá trị ban đầu là esp32-client
    client_id += String(WiFi.macAddress()); // kết nối vào chuỗi client_id với địa chỉ MAC của wifi
    Serial.printf ("The Client %s connects to the public mqtt broker", client_id.c_str()); // in ra thông báo kết nối với client ID trên serial
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)){ // kiểm tra điều kiện nếu client được kết nối với MQTT Broker, người dùng và mật khẩu
      Serial.println ("Public emqx mqtt broker connected"); // in ra thông báo kết nối thành công trên serial
      client.subscribe ("Led Control");
      client.subscribe ("esp32/client");
    }
    else { // nếu thoả mãn điều kiện thì
      Serial.print ("false with state "); // thông báo trạng thái kết nối thất bại
      Serial.print (client.state()); // thông báo trạng thái của client trên serial
      delay (2000);
    }
  } 
}

void handleButtonPress() {
  bool reading = digitalRead(button); // Read button state
  
  if (reading != lastButtonState) {
    lastDebounceTime = millis(); // Reset debounce timer
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) { 
        ledState = !ledState; 
        digitalWrite(led, ledState);
        publishMessage("led", ledState ? "LED is turn ON" : "LED is turn OFF", true);
      }
    }
  }
  lastButtonState = reading;
}

void loop (){
  client.loop();
  MQTT_Connect();
  handleButtonPress();
  // DHT22
  if (millis() - timeUpdata > 5000){ 
    delay(dht.getMinimumSamplingPeriod());
    float h = dht.getHumidity(); // khai báo biến độ ẩm
    float t = dht.getTemperature (); // khai báo biến nhiệt độ

    DynamicJsonDocument doc (1024); // tạo một dynamicjsondocument với bộ nhớ 1024 byte
    doc["humidity"] = h; 
    doc["temperature"] = t;
    char mqtt_message [128]; // chuỗi tin nhắn gồm 128 ký tự
    serializeJson (doc, mqtt_message); // tự chuyển json về một chuỗi từ doc và message
    publishMessage("ESP32/DHT22", mqtt_message, true); // phát hành tin nhắn esp32/dht22 nhập từ chuỗi mqtt_message

    timeUpdata = millis();
  }

}
```