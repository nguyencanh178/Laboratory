# D22 - NGUYỄN ĐỨC CẢNH & NGUYỄN BÁ HIẾU - BÁO CÁO CÔNG VIỆC NGÀY 30/10/2024

## A. Công việc đã làm

### 1. Nêu rõ các vấn đề gặp phải của báo cáo trước

- Lỗi với string:

| Tiêu chí | Cảnh và Hiếu | Anh Sang |
| --- | --- | --- |
| Phiên bản Ardunio | 2.3.3 | 1.8.16 |
| Additional board mangager URL | chưa có | https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json |
| Board | ESP32 DEV MODULE | ESP32 DEV MODULE |

**Code ban đầu**
```
std::string rxValue = pCharacteristic->getValue();
std::string data = BatteryLevelCharacteristic.getValue();
```

**Code khi complie bị lỗi**
Khi compile gặp lỗi: ```pCharacteristic->getValue()``` không tương ứng với biến String
(máy anh Sang không gặp lỗi này)

```
D:\Self_Learning\Arduino\BLE_wifi\BLE_wifi.ino: In member function 'virtual void MyCallbacks::onWrite(BLECharacteristic*)':
D:\Self_Learning\Arduino\BLE_wifi\BLE_wifi.ino:37:54: error: conversion from 'String' to non-scalar type 'std::string' {aka 'std::__cxx11::basic_string<char>'} requested
   37 |       std::string rxValue = pCharacteristic->getValue();
      |                             ~~~~~~~~~~~~~~~~~~~~~~~~~^~
D:\Self_Learning\Arduino\BLE_wifi\BLE_wifi.ino: In function 'void receiData(String&, String&)':
D:\Self_Learning\Arduino\BLE_wifi\BLE_wifi.ino:190:57: error: conversion from 'String' to non-scalar type 'std::string' {aka 'std::__cxx11::basic_string<char>'} requested
  190 |   std::string data = BatteryLevelCharacteristic.getValue();
      |                      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~^~

exit status 1

Compilation error: conversion from 'String' to non-scalar type 'std::string' {aka 'std::__cxx11::basic_string<char>'} requested
```

**Sửa code**
```
std::string rxValue = pCharacteristic->getValue().c_str();

std::string data = BatteryLevelCharacteristic.getValue().c_str();
```

**Compile bị lỗi quá bộ nhớ khi dùng Board DOIT ESP32 DEVKIT V1**
```
Sketch uses 1729985 bytes (131%) of program storage space. Maximum is 1310720 bytes.
Global variables use 59460 bytes (18%) of dynamic memory, leaving 268220 bytes for local variables. Maximum is 327680 bytes.
Sketch too big; see https://support.arduino.cc/hc/en-us/articles/360013825179 for tips on reducing it.
text section exceeds available space in board

Compilation error: text section exceeds available space in board
```

**Lựa chọn Board ESP32 DEV Module**
```
Sketch uses 1730097 bytes (54%) of program storage space. Maximum is 3145728 bytes.
Global variables use 59460 bytes (18%) of dynamic memory, leaving 268220 bytes for local variables. Maximum is 327680 bytes.
esptool.py v4.6
Serial port COM7
```

==> **Compile thành công**


### 2. Giải thích mã code

- **Khởi tạo web server và LED**
```
WebServer server(80);
uint8_t LED1pin = 4, LED2pin = 5;
bool LED1status = LOW, LED2status = LOW;
```

- **Hàm setup**: ESP32 sẽ kết nối với Wi-Fi sử dụng thông tin được cung cấp qua BLE. Nếu kết nối thất bại, ESP32 sẽ cố gắng kết nối lại bằng cách sử dụng dữ liệu từ receiData.
```
void setup() {
  Serial.begin(115200);
  pinMode(2,OUTPUT);
  InitBLE(); // khởi tạo BLE
  BLE_print("BLE\n");
  BLE_print("Connecting..."); // chuẩn bị kết nối với wifi
  WiFi.mode(WIFI_STA); // khởi tạo wifi
  WiFi.begin(input1,input2);
  while(WiFi.status() != WL_CONNECTED){ // chờ khi wifi đc kết nối
  Serial.print(".");
  receiData(input1,input2);
  WiFi.begin(input1,input2); // khởi tạo wifi
  delay(1000);
  }
  // khi kết nối wifi thành công thì thoát khỏi vòng while
  BLE_print("Connected to " + input1 +"\n");
  BLE_print("IP address: "); 
  // Lấy địa chỉ IP
  IPAddress localIP = WiFi.localIP();

  // Chuyển đổi địa chỉ IP thành chuỗi
  String strIP = localIP.toString();
  BLE_print(strIP+"\n"); // in ra địa chỉ IP 
  server.on("/", handle_OnConnect);
  server.on("/led1on", handle_led1on);
  server.on("/led1off", handle_led1off);
  server.on("/led2on", handle_led2on);
  server.on("/led2off", handle_led2off);
  server.onNotFound(handle_NotFound);
  server.begin();
  BLE_print("HTTP server started\n");
}
```

- **Hàm BLE_Init**: khởi tạo máy chủ BLE với một đặc tính tùy chỉnh để nghe dữ liệu đến để kết nối với Wi-Fi.
```
void InitBLE() {
  BLEDevice::init("ConfigWifi");
  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pBattery = pServer->createService(BatteryService);

  pBattery->addCharacteristic(&BatteryLevelCharacteristic);
  BatteryLevelDescriptor.setValue("Percentage 0 - 100");
  BatteryLevelCharacteristic.addDescriptor(&BatteryLevelDescriptor);
  BatteryLevelCharacteristic.addDescriptor(new BLE2902());


  /* ###############################################################  define callback */
  BLECharacteristic *pWriteCharacteristic = pBattery->createCharacteristic(
                                         CHARACTERISTIC_UUID_RX,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
 
  pWriteCharacteristic->setCallbacks(new MyCallbacks());
  /* ############################################################### */
  
  pServer->getAdvertising()->addServiceUUID(BatteryService);

  pBattery->start();
  // Start advertising
  pServer->getAdvertising()->start();
}
```

- **Hàm BLE_print**
```
void BLE_print(T myVar) {
  String value = String(myVar);
  uint8_t data[value.length()];
  for (size_t i = 0; i < value.length(); i++) {
      data[i] = static_cast<uint8_t>(value.charAt(i));
  }
  BatteryLevelCharacteristic.setValue(data, value.length()); // ghi dữ liệu
  BatteryLevelCharacteristic.notify(); // thông báo cho web có dữ liệu mới
  Serial.print(myVar);
}
```

- **Hàm receiData()** : Nhận dữ liệu từ Web và tách SSID và Password

```
void receiData(String& input1, String& input2){
  std::string data = BatteryLevelCharacteristic.getValue().c_str();
  String receivedData = "";
  for (int i = 0; i < data.length(); i++) {
    receivedData += char(data[i]);
  }
  size_t delimiterPos = receivedData.indexOf(';');

  if (delimiterPos != -1) {
    input1 = receivedData.substring(0, delimiterPos);
    input2 = receivedData.substring(delimiterPos + 1);
  } 
  else {
    input1 = receivedData;
    input2 = "";
  }
}
```

- **Hàm BLE_print**: Gửi dữ liệu lên web (có thể gửi nhiều kiểu dữ liệu khác nhau)

```
template <typename T>
void BLE_print(T myVar) {
  String value = String(myVar);
  uint8_t data[value.length()];
  for (size_t i = 0; i < value.length(); i++) {
      data[i] = static_cast<uint8_t>(value.charAt(i));
  }
  BatteryLevelCharacteristic.setValue(data, value.length());
  BatteryLevelCharacteristic.notify();
  Serial.print(myVar);
}
```

- **Hàm loop**: xử lý các yêu cầu từ web server và điều khiển trạng thái của các đèn LED dựa trên trạng thái hiện tại của chúng

```
void loop() {
  // nhận dữ liệu từ web server để điều khiển led
  server.handleClient(); 
  if(LED1status)  digitalWrite(LED1pin, HIGH);
  else            digitalWrite(LED1pin, LOW);
  if(LED2status)  digitalWrite(LED2pin, HIGH);
  else            digitalWrite(LED2pin, LOW);
}
```

### 3. Thay MQTT vào server

- Sử dụng một cloud có sẵn **(Hive MQ)** đăng ký 1 tài khoản MQTT Broker 

**Kết nối MQTT**
- Mã nguồn sử dụng thư viện PubSubClient để kết nối ESP32 với một MQTT Broker qua cổng 8883 (TLS/SSL)
```
const char *mqtt_broker = "1f6c9630668e42d9969537af3ab11396.s1.eu.hivemq.cloud"; // xác định địa chỉ của MQTT Broker được đặt ở dạng "broker.emqx.io"
const char *mqtt_username = "nguyencanh178"; // tên người dùng để xác thức MQTT
const char *mqtt_password = "canhneee"; // mật khẩu xác thức MQTT
const int mqtt_port = 8883; // số cổng MQTT Broker được mặc định là 8883
```

- **Hàm MQTT_Connect**: Xác thực bằng mqtt_username và mqtt_password.
```
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
    }
    else { // nếu thoả mãn điều kiện thì
      Serial.print ("false with state "); // thông báo trạng thái kết nối thất bại
      Serial.print (client.state()); // thông báo trạng thái của client trên serial
      delay (2000);
    }
  } 
}
```
**Hàm callback()**: Xử lý khi có tin nhắn mới trên một chủ đề đã đăng ký. Tin nhắn sẽ được in ra Serial.
```
void callback (char* topic, byte* payload, unsigned int length){ // khởi tạo hàm callback khi có một chủ đề được đăng ký
  Serial.println ("Message arrived in topic: "); // thông báo tin nhắn đã đc đến trong chủ đề
  Serial.println (topic); // in ra chủ đề tin nhắn
  Serial.println ("Message: "); // in ra tin nhắn
  String message;
  for (int i = 0; i < length; i++){ // vòng lặp để đọc và in ra từng ký tự của message
    message += (char)payload[i];
  }
  Serial.println (message); // kết thúc message
}
```

- **Hàm setup**: 
```
void setup() {
  Serial.begin(115200);
  pinMode(2,OUTPUT);
  InitBLE();
  BLE_print("BLE\n");
  BLE_print("Connecting...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(input1,input2);
  while(WiFi.status() != WL_CONNECTED){
  Serial.print(".");
  receiData(input1,input2);
  WiFi.begin(input1,input2);
  delay(1000);
  }
  BLE_print("Connected to " + input1 +"\n");
  BLE_print("IP address: ");
  // Lấy địa chỉ IP
  IPAddress localIP = WiFi.localIP();
  String strIP = localIP.toString();
  espClient.setInsecure();
  client.setCallback(callback);
  server.begin();
}
```

**Kết quả**: Hiển thị một số bất kì trong khoản từ 10 đến 20 qua giao thức MQTT kết hợp kết nối WiFi qua web BLE

# B. Công việc tiếp theo

- Em muốn xin thầy định hướng sắp tới ạ!