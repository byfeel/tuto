


/*
 * webserver dynamique - temperature
 */

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>   //Include SPIFFS
// bibliotheque temperature
#include <DHTesp.h>
#include <ArduinoJson.h>

// FTPserver
#include "ESP8266FtpServer.h"

unsigned long previousMillis=0 ;
unsigned long interval = 10000;

//WiFi Connection configuration
const char *ssid = "nom wifi";
const char *password = "pass wifi";

#define led 2
// init dht
DHTesp dht;
ComfortState cf;
// PIN ou est connecté l'info data du dht
int dhtPin = 16;  // GPIO16  egale a D2 sur WEMOS D1R1  ou D0 pour les autres ( averifier selon esp )

  float humidity ;
  float temperature;
 float heatIndex;
  float dewPoint;
  byte per;
      String comfortStatus;

      
// on crée l'instance
FtpServer ftpSrv;

//json


StaticJsonBuffer<200> jsonBuffer;  
JsonObject& JSONInfo = jsonBuffer.createObject();

//***********************************
//************* Gestion du serveur WEB
//***********************************
ESP8266WebServer server(80);

void handleRoot(){
  server.sendHeader("Location", "/index.html",true);   //Redirige vers page index.html sur SPIFFS
  server.send(302, "text/plane","");
}

// fonction de traitement SETLED
void handleLED(){
  String SetLED = server.arg("LED");
  Serial.println(SetLED);
  if (SetLED=="on")  digitalWrite(led, LOW);
  else  digitalWrite(led, HIGH);
  
  server.send(200, "text/plane",SetLED);
}

// fonction de traitement SETLED
void handleInfo() {
  String Info;
   JSONInfo["T"] = temperature;
  JSONInfo["H"] = humidity;
  JSONInfo["Confort"] = comfortStatus;
  JSONInfo.printTo(Info);
  
  server.send(200, "application/json",Info);
}

void handleWebRequests(){
  if(loadFromSpiffs(server.uri())) return;
  String message = "File Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " NAME:"+server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.println(message);
}

//********** fin serveur web

void setup() {

  Serial.begin(115200);
  Serial.println();

 // Conexion WIFI
  WiFi.begin(ssid, password);    
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //demarrage file system
  if (SPIFFS.begin()) {
    Serial.println("Demarrage file System");
    ftpSrv.begin("esp8266","pass");   // demarrage serveur ftp avec user = esp8266 et mdp = password
  }
  else Serial.println("Erreur file System");
  
  //Si connexion affichage info dans console
  Serial.println("");
  Serial.print("Connection ok sur le reseau :  ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); 



  //Initialize Webserver
  server.on("/",handleRoot);
  server.on("/setLED",handleLED); //Rpour gerer led
  server.on("/getInfo",handleInfo); //Rpour récupérer info module
  server.onNotFound(handleWebRequests); //Set setver all paths
  server.begin(); 

  pinMode(led, OUTPUT); 


// Initialize temperature sensor
dht.setup(dhtPin, DHTesp::DHT11);
delay(1000);
humidity = dht.getHumidity();
temperature = dht.getTemperature();


  JSONInfo["T"] = temperature;
  JSONInfo["H"] = humidity;
}

void loop() {
//service web 
 server.handleClient();
 ftpSrv.handleFTP();

 // recup temp
  if( millis() - previousMillis >= interval) {
    previousMillis = millis();   
 humidity = dht.getHumidity();
temperature = dht.getTemperature();
 heatIndex = dht.computeHeatIndex(temperature,humidity);
 dewPoint = dht.computeDewPoint(temperature,humidity);
  float cr = dht.getComfortRatio(cf,temperature,humidity);
  per = dht.computePerception(temperature, humidity);

  switch(cf) {
    case Comfort_OK:
      comfortStatus = "OK";
      break;
    case Comfort_TooHot:
      comfortStatus = "Trop Chaud";
      break;
    case Comfort_TooCold:
      comfortStatus = "Trop froid";
      break;
    case Comfort_TooDry:
      comfortStatus = "Trop Sec";
      break;
    case Comfort_TooHumid:
      comfortStatus = "Trop humide";
      break;
    case Comfort_HotAndHumid:
      comfortStatus = "Chaud et Humide";
      break;
    case Comfort_HotAndDry:
      comfortStatus = "Chaud et Sec";
      break;
    case Comfort_ColdAndHumid:
      comfortStatus = "Froid et humide";
      break;
    case Comfort_ColdAndDry:
      comfortStatus = "Froid et Sec";
      break;
    default:
      comfortStatus = "Unknown:";
      break;
  }
Serial.println(" T:" + String(temperature) + "°C  H:" + String(humidity) + "%  I:" + String(heatIndex) + " D:" + String(dewPoint) + " " + comfortStatus+" et per :"+ String(per));
    }  
}

// gestion du not found.
bool loadFromSpiffs(String path){
  String dataType = "text/plain";
  if(path.endsWith("/")) path += "index.html";

  if(path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if(path.endsWith(".html")) dataType = "text/html";
  else if(path.endsWith(".htm")) dataType = "text/html";
  else if(path.endsWith(".css")) dataType = "text/css";
  else if(path.endsWith(".js")) dataType = "application/javascript";
  else if(path.endsWith(".png")) dataType = "image/png";
  else if(path.endsWith(".gif")) dataType = "image/gif";
  else if(path.endsWith(".jpg")) dataType = "image/jpeg";
  else if(path.endsWith(".ico")) dataType = "image/x-icon";
  else if(path.endsWith(".xml")) dataType = "text/xml";
  else if(path.endsWith(".pdf")) dataType = "application/pdf";
  else if(path.endsWith(".zip")) dataType = "application/zip";
  File dataFile = SPIFFS.open(path.c_str(), "r");
  if (server.hasArg("download")) dataType = "application/octet-stream";
  if (server.streamFile(dataFile, dataType) != dataFile.size()) {
  }

  dataFile.close();
  return true;
}
