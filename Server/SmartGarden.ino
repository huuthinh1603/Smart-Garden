#include <ESP8266WiFi.h>

//Kết nối với mạng nhà bạn.
IPAddress staticIP(192,168,1,22);
IPAddress gateway(192,168,1,9);
IPAddress subnet(255,255,255,0);
const char* ssid     = "Samsung J7";
const char* password = "123456789";

WiFiServer server(80);

//Chuỗi lưu trữ dữ liêu HTTP.
String header;

//Chuỗi lưu trang thái.
String LIGHT = "off";
String PUMP = "off";
String AUTO = "off";

//Biến nhận dữ liệu từ STM32F1
char Temp;
int i = 0;
char Receive[4];

//Biến lưu giá trị sensor.
int NhietDoKK = 0;
int DoAmKK;
int DoAmDat;


void setup() {
  //UART baud rate = 9600
  Serial.begin(9600);

  //Thông báo kết nối vs network.
  Serial.print("Connecting to ");
  Serial.println(ssid);
  //WiFi.config(staticIP, gateway, subnet);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // In ra địa chỉ IP của Web server.
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //Khởi động Web server.
  server.begin();
}

void loop(){

  
          if (Serial.available() > 0) {
          Temp = Serial.read();
          if(Temp == '#')
          {
              i = 0;
          }
          else if(Temp == '@')
          {
              DoAmDat = Receive[0];
              DoAmKK = Receive[1];
              NhietDoKK = Receive[2];
          }
          else if(i <= 3)
          {
              Receive[i] = Temp;
              i++;
          }
          Serial.println(DoAmDat);
}
  
  WiFiClient client = server.available();   

  if (client) {                             
    String currentLine = "";                
    while (client.connected()) {           
      if (client.available()) {            
        char c = client.read();             
        //Serial.write(c);                 
        header += c;
        if (c == '\n') {                    

          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /Light/on") >= 0) {
              Serial.print("*ON_LIGHT#");
              LIGHT = "on";
            } else if (header.indexOf("GET /Light/off") >= 0) {
              Serial.print("*OFF_LIGHT#");
              LIGHT = "off";
            } else if (header.indexOf("GET /Pump/on") >= 0) {
              Serial.print("*ON_PUMP#");
              PUMP = "on";
            } else if (header.indexOf("GET /Pump/off") >= 0) {
              Serial.print("*OFF_PUMP#");
              PUMP = "off";
            } else if (header.indexOf("GET /Auto/on") >= 0) {
              Serial.print("*NOT_AUTO#");
              AUTO = "on";
            } else if (header.indexOf("GET /Auto/off") >= 0) {
              Serial.print("*AUTO#");
              LIGHT = "off";
              PUMP = "off";
              AUTO = "off";
            }
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta charset='utf-8'><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 12px 30px;");
            client.println("text-decoration: none; font-size: 15px; margin: 1px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<h2>Nhiệt độ & độ ẩm.</h2> ");
            client.println("<h3>Nhiệt độ: ");
            client.println(NhietDoKK);
            client.println("*C</h3><h3>Độ ẩm không khí: ");
            client.println(DoAmKK);
            client.println("%</h3><h3>Độ ẩm đất: ");
            client.println(DoAmDat);
            client.println("%</h3>");

            
            client.println("<br>");
            client.println("<h2>Điều khiển thiết bị</h2>. "); 
            // Display current state, and ON/OFF auto 
            client.println("<p>AUTO</p>");
            // If the output5State is off, it displays the ON button       
            if (AUTO=="off") {
              client.println("<p><a href=\"/Auto/on\"><button class=\"button\">ON</button></a></p>");
            } else { 
              client.println("<p><a href=\"/Auto/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
            if(AUTO == "on")
            {
                client.println("<p>LIGHT</p>");      
                if (LIGHT=="off") {
                  client.println("<p><a href=\"/Light/on\"><button class=\"button button2\">OFF</button></a></p>");
                } else {
                  client.println("<p><a href=\"/Light/off\"><button class=\"button \">ON</button></a></p>");
                }
    
                client.println("<p>PUMP</p>");
                // If the output4State is off, it displays the ON button       
                if (PUMP=="off") {
                  client.println("<p><a href=\"/Pump/on\"><button class=\"button button2\">OFF</button></a></p>");
                } else {
                  client.println("<p><a href=\"/Pump/off\"><button class=\"button\">ON</button></a></p>");
                }
            }
            client.println("</body></html>");
            
            client.println();
            // Break out of the while loop
            break;
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;     
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
  }
}
