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

ESP8266WebServer server(80); // เปิด webserver ที่ port 80

Servo servo1;  // create servo object to control a servo
Servo servo2;
Servo servo3;
Servo servo4;

// home page
String homePage = " \
<!DOCTYPE html>\ 
  <head>\ 
    <script src='https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js'></script>   \ 
 </head>\ 
 <body>\ 
   <button id='btLed'>LED ON</button> \ 
   <br>\ 
   <br>\ 
   <div>\ 
     <input type='text' id='lname'>\ 
     <br>\ 
     <button id='btSave'>SAVE</button>  \ 
   </div>   \ 
   <script>     \ 
     $( document ).ready(function() {       \ 
       $( '#btLed' ).click(function() {         \ 
         if($( '#btLed' ).text() == 'LED ON') { \ 
           $.post( '/on', function( data ) {\ 
             $( '#btLed' ).text(data);\ 
           });                   \ 
         } else {           \ 
           $.post( '/off', function( data ) {\ 
             $( '#btLed' ).text(data);\ 
           });      \ 
         }       \ 
       });  \ 
       $( '#btSave' ).click(function() {  \ 
         let inputData = $( '#lname' ).val();  \ 
         $.post( '/config/set', {msg: inputData} , function( data ) {\ 
           console.log('data => ' + data);\ 
         });                   \ 
       }); \ 
     });   \ 
   </script>    \ 
 </body>\ 
 <html>\ 
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

  // controll servor 1
  server.on("/1/l", [](){ 
    servo1.write(0);
    String txt = "{servo:1, turn: left}";
    Serial.println(txt);
    server.send ( 200, "text/plain", txt ); 
    digitalWrite(LED, LOW); delay(10); digitalWrite(LED, HIGH);
  });
  server.on("/1/r", [](){ 
    servo1.write(90);
    String txt = "{servo:1, turn: left}";
    Serial.println(txt);
    server.send ( 200, "text/plain", txt ); 
    digitalWrite(LED, LOW); delay(10); digitalWrite(LED, HIGH);
  });

  // controll servor 2
  server.on("/2/l", [](){ 
    servo2.write(0);
    String txt = "{servo:2, turn: left}";
    Serial.println(txt);
    server.send ( 200, "text/plain", txt ); 
    digitalWrite(LED, LOW); delay(10); digitalWrite(LED, HIGH);
  });
  server.on("/2/r", [](){ 
    servo2.write(90);
    String txt = "{servo:2, turn: right}";
    Serial.println(txt);
    server.send ( 200, "text/plain", txt ); 
    digitalWrite(LED, LOW); delay(10); digitalWrite(LED, HIGH);
  });

  // controll servor 3
  server.on("/3/l", [](){ 
    servo3.write(0);
    String txt = "{servo:3, turn: left}";
    Serial.println(txt);
    server.send ( 200, "text/plain", txt ); 
    digitalWrite(LED, LOW); delay(10); digitalWrite(LED, HIGH);
  });
  server.on("/3/r", [](){ 
    servo3.write(90);
    String txt = "{servo:3, turn: right}";
    Serial.println(txt);
    server.send ( 200, "text/plain", txt ); 
    digitalWrite(LED, LOW); delay(10); digitalWrite(LED, HIGH);
  });
   
  // controll servor 4
  server.on("/4/l", [](){ 
    servo4.write(0);
    String txt = "{servo:4, turn: left}";
    Serial.println(txt);
    server.send ( 200, "text/plain", txt ); 
    digitalWrite(LED, LOW); delay(10); digitalWrite(LED, HIGH);
  });
  server.on("/4/r", [](){ 
    servo4.write(90);
    String txt = "{servo:4, turn: right}";
    Serial.println(txt);
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
