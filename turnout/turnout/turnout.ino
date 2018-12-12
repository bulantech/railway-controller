#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h> // web server
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <Servo.h> // servo motor

#define SW_PIN D3 //0 //ปุ่ม FLASH 
#define LED D0 //led บนบอร์ด 
#define SERVO_1 D5
#define SERVO_2 D6
#define SERVO_3 D7
#define SERVO_4 D8

int status[] = {0, 0, 0, 0};

ESP8266WebServer server(80); // เปิด webserver ที่ port 80

Servo servo1;  // create servo object to control a servo
Servo servo2;
Servo servo3;
Servo servo4;

// home page
String homePage = " \
<!DOCTYPE html>  \
<head>  \
  <title>Railroad Switch</title> \
  <meta charset='utf-8'> \
  <meta name='viewport' content='width=device-width, initial-scale=1'> \
  <link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/4.1.3/css/bootstrap.min.css'> \
  <script src='https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js'></script> \
  <script src='https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.3/umd/popper.min.js'></script> \
  <script src='https://maxcdn.bootstrapcdn.com/bootstrap/4.1.3/js/bootstrap.min.js'></script>  \
</head>  \
<body>  \
  <nav class='navbar navbar-dark bg-primary' data-toggle='affix'> \
    <div class='mx-auto d-sm-flex d-block flex-sm-nowrap'> \
      <a class='navbar-brand' href='#'>Railroad Switch </a> \
    </div> \
  </nav> \
  <div class='container'> \
    <div class='card-deck mt-3'> \
      <div class='card text-center' name='1'> \
        <img class='card-img-top' src='https://raw.githubusercontent.com/bulantech/railway-controller/master/turnout/doc/l.jpg' alt='Card image cap'> \
        <div class='card-body'> \
          <h5 class='card-title'>Turnout 1</h5> \
          <button type='button' class='btn btn-danger btn-sm mx-1' name='l'><h2><</h2></button> \
          <button type='button' class='btn btn-primary btn-sm mx-1' name='r'><h2>></h2></button> \
        </div> \
      </div> \
      <div class='card text-center' name='2'> \
        <img class='card-img-top' src='https://raw.githubusercontent.com/bulantech/railway-controller/master/turnout/doc/l.jpg' alt='Card image cap'> \
        <div class='card-body'> \
          <h5 class='card-title'>Turnout 2</h5> \
          <button type='button' class='btn btn-danger btn-sm mx-1' name='l'><h2><</h2></button> \
          <button type='button' class='btn btn-primary btn-sm mx-1' name='r'><h2>></h2></button> \
        </div> \
      </div> \
      <div class='card text-center' name='3'> \
        <img class='card-img-top' src='https://raw.githubusercontent.com/bulantech/railway-controller/master/turnout/doc/l.jpg' alt='Card image cap'> \
        <div class='card-body'> \
          <h5 class='card-title'>Turnout 3</h5> \
          <button type='button' class='btn btn-danger btn-sm mx-1' name='l'><h2><</h2></button> \
          <button type='button' class='btn btn-primary btn-sm mx-1' name='r'><h2>></h2></button> \
        </div> \
      </div> \
      <div class='card text-center' name='4'> \
        <img class='card-img-top' src='https://raw.githubusercontent.com/bulantech/railway-controller/master/turnout/doc/l.jpg' alt='Card image cap'> \
        <div class='card-body'> \
          <h5 class='card-title'>Turnout 4</h5> \
          <button type='button' class='btn btn-danger btn-sm mx-1' name='l'><h2><</h2></button> \
          <button type='button' class='btn btn-primary btn-sm mx-1' name='r'><h2>></h2></button> \
        </div> \
      </div> \
    </div> \
  </div> \
  \
  <script>       \
    $( document ).ready(function() {   \
      let ori = window.location.origin; \
      $.get( ori+'/status', function( data ) { \
        console.log( data ); \
      }); \
 \
      $('button').click(function(e) {   \
        let name = $(this).attr('name'); \
        let cardNumber = $(this).parent().parent().attr('name'); \
        let ip = $('#ip').val(); \
        let checked = $('#defaultCheck1').is(':checked'); \
        let url = 'http://'+ip+'/'+cardNumber+'/'+name; \
        let origin = window.location.origin; \
 \
        if(!checked) { \
          url = origin+'/'+cardNumber+'/'+name; \
        } \
        if(ip == '' && checked) {  \
          alert('Enter Railroad Switch ip'); \
          return; \
        } \
 \
        $(this).parent().find('button').removeClass().addClass('btn btn-primary btn-sm mx-1'); \
        $(this).removeClass().addClass('btn btn-danger btn-sm mx-1'); \
        $(this).parent().parent().find('img').attr('src','https://raw.githubusercontent.com/bulantech/railway-controller/master/turnout/doc/l.jpg'); \
        if(name=='r') { \
          $(this).parent().parent().find('img').attr('src','https://raw.githubusercontent.com/bulantech/railway-controller/master/turnout/doc/r.jpg'); \
        } \
         \
        console.log(name, cardNumber, ip, url, origin)   ;      \
        $.get( url, function( data ) { \
          console.log( data ); \
        }); \
      });  \
 \
      $('#defaultCheck1').click(function() { \
        if ($(this).is(':checked')) { \
          $('#ip').prop('disabled', false); \
        } else { \
          $('#ip').prop('disabled', true); \
        } \
      }); \
   });     \
  </script>      \
</body>  \
<html>  \
 ";

void handleRoot() {
  server.send(200, "text/html", homePage);
  digitalWrite(LED, LOW); delay(10); digitalWrite(LED, HIGH);
}

// หน้า page ไม่มีหน้าเว็บที่ขอมา
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(LED, LOW); delay(10); digitalWrite(LED, HIGH); delay(100); 
  digitalWrite(LED, LOW); delay(10); digitalWrite(LED, HIGH);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // set in out pin
  pinMode(SW_PIN, INPUT_PULLUP);  
  pinMode(LED, OUTPUT); 

  // servo init
//  servo1.attach(LED); //test
  servo1.attach(SERVO_1);
  servo2.attach(SERVO_2);
  servo3.attach(SERVO_3);
  servo4.attach(SERVO_4);
  // reset servo to 0 degree
  servo1.write(0);
  servo2.write(0);
  servo3.write(0);
  servo4.write(0);
  
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager; //เรียกใช้ WiFiManager library

  // รอ 5 วินาที่
  Serial.println("Reset wifi config?:");
  for(int i=5; i>0; i--){
    Serial.print(String(i)+" "); 
    digitalWrite(LED, LOW);
    delay(100);
    digitalWrite(LED, HIGH);
    delay(900);
  }
  
  // ถ้ามีการกด จะรีเซตค่า wifi ที่เคยตั้งไา้
  if(digitalRead(SW_PIN) == LOW) // Press button
  {
    Serial.println();
    Serial.println("Reset wifi config");
    wifiManager.resetSettings(); 
  }    
  
  //set custom ip for portal
  //wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  //fetches ssid and pass from eeprom and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  
  // ถ้า จะรีเซตค่า wifi จะสามารถเข้าเซต wifi ด้วย wifiManager
  // ถ้าไม่ จะต่อ wifi
  wifiManager.autoConnect("AutoConnectAP"); // AutoConnectAP คือชื่อ wifi ของ NodeMCU
  
  //or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();

  
  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  // web router  
  server.on("/", handleRoot);

  server.on("/status", [](){ 
    String txt = "{\"1\":" + String(status[0]) + ", \"2\":" + String(status[1]) + ", \"3\":" + String(status[2]) + ", \"4\":" + String(status[3]) + "}";
    Serial.println(txt);
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send ( 200, "text/plain", txt ); 
    digitalWrite(LED, LOW); delay(10); digitalWrite(LED, HIGH);
  }); 
  
  // controll servor 1
  server.on("/1/l", [](){ 
    status[0] = 0;
    servo1.write(0);
    String txt = "{servo:1, turn: left}";
    Serial.println(txt);
    server.sendHeader("Access-Control-Allow-Origin", "*"); //ให้สามารถเรียกจาก jquery ได้
    server.send ( 200, "text/plain", txt ); 
    digitalWrite(LED, LOW); delay(10); digitalWrite(LED, HIGH); // led กระพริบแสดงสถานะการเรียก url นี้
  });
  server.on("/1/r", [](){ 
    status[0] = 90;
    servo1.write(90);
    String txt = "{servo:1, turn: right}";
    Serial.println(txt);
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send ( 200, "text/plain", txt ); 
    digitalWrite(LED, LOW); delay(10); digitalWrite(LED, HIGH);
  });

  // controll servor 2
  server.on("/2/l", [](){ 
    status[1] = 0;
    servo2.write(0);
    String txt = "{servo:2, turn: left}";
    Serial.println(txt);
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send ( 200, "text/plain", txt ); 
    digitalWrite(LED, LOW); delay(10); digitalWrite(LED, HIGH);
  });
  server.on("/2/r", [](){ 
    status[1] = 90;
    servo2.write(90);
    String txt = "{servo:2, turn: right}";
    Serial.println(txt);
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send ( 200, "text/plain", txt ); 
    digitalWrite(LED, LOW); delay(10); digitalWrite(LED, HIGH);
  });

  // controll servor 3
  server.on("/3/l", [](){ 
    status[2] = 0;
    servo3.write(0);
    String txt = "{servo:3, turn: left}";
    Serial.println(txt);
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send ( 200, "text/plain", txt ); 
    digitalWrite(LED, LOW); delay(10); digitalWrite(LED, HIGH);
  });
  server.on("/3/r", [](){ 
    status[2] = 90;
    servo3.write(90);
    String txt = "{servo:3, turn: right}";
    Serial.println(txt);
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send ( 200, "text/plain", txt ); 
    digitalWrite(LED, LOW); delay(10); digitalWrite(LED, HIGH);
  });
   
  // controll servor 4
  server.on("/4/l", [](){ 
    status[3] = 0;
    servo4.write(0);
    String txt = "{servo:4, turn: left}";
    Serial.println(txt);
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send ( 200, "text/plain", txt ); 
    digitalWrite(LED, LOW); delay(10); digitalWrite(LED, HIGH);
  });
  server.on("/4/r", [](){ 
    status[3] = 90;
    servo4.write(90);
    String txt = "{servo:4, turn: right}";
    Serial.println(txt);
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send ( 200, "text/plain", txt ); 
    digitalWrite(LED, LOW); delay(10); digitalWrite(LED, HIGH);
  }); 
  
  server.onNotFound(handleNotFound);

  server.begin(); // เริ่มใช้งาน web server
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient(); // web server รอรับค่าจาก web browser
    
}
