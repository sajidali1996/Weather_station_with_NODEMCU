
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>



const char* ssid = "Weather_Station";
const char* password = "temperature";

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

//_________________________
float input_voltage;
float temperature;
String webpage;
int history[24]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
int minutes=0;
int hours=0;
int seconds=0;
String max_24="00";
String avg_24="00";
String min_24="00";


//_____________________________



void setup() {
  Serial.begin(115200);
 pinMode(A0,INPUT);
 

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  server.on("/", handle_OnConnect);
  server.on("/get_temperature", handle_getTemperature);
 
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");
}
void loop() {
 //__________________get temperature____________________
  input_voltage=analogRead(A0); 
  temperature=(((input_voltage/1024)*3300))/10;
  temperature=int(temperature);
//_________________set temperature to webpage_________________________
  webpage=html(String(temperature),max_24,avg_24,min_24);

//_________________record keeping________________________
seconds=millis()/1000;
minutes=seconds/60;
/*
if ((minutes/60)>hours)
{
  history[]=temperature;
  
  }


*/
hours=minutes/60;



  //______________________________
  server.handleClient();
  delay(100);
  

}

void handle_OnConnect() {
  
  
  server.send(200, "text/html", "hi there!"); 
}


void handle_getTemperature(){
  
  
  server.send(200,"text/html",webpage);
  
  
  } 




void handle_NotFound(){
  
  server.send(404, "text/plain", "Not found");
}


String html(String temp, String max_24,String avg_24,String min_24){

  String page="<!DOCTYPE html> <html>\n";
page+=" <html>\n";
page+="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\"></head>\n";
page+=" <body style= 'background-color:powderblue'>\n";
page+=" <h1 style='color:blue;font-size:36px;text-align:center'>Weather Station</h1>\n";
page+=" <hr>\n";
page+=" <p style = 'color:Black;text-align:center ;font-size:24px'><b>Current Temperature = " + temp + " <sup>o</sup> C</b> </p>\n";
page+=" <br>\n";
page+=" <h1 style='color:blue;font-size:24px;text-align:center'>Summary of last 24 hours</h1>\n";
page+=" <hr>\n";

page+=" <br>\n";


page+= " <p style = 'font-size:24px; text-align:center'> <b>Maximum = " + max_24+"<sup>o</sup> C</b></p>\n";
page+= " <p style = 'font-size:24px; text-align:center'> <b>Average = "+ avg_24+" <sup>o</sup> C </b></p>\n";
page+= " <p style = 'font-size:24px; text-align:center'> <b>Minimum = "+ min_24+"<sup>o</sup> C </b></p>\n";
page+=" <hr>\n";
page+="<p style='text-align:center;font-size:16'>Time Elapsed since started recording "+ String(hours) + " : " +String(minutes)+ " : " +String(seconds)+"</p>\n";

page+=" </body>\n";
page+=" </html>\n";
  return page;
  }
