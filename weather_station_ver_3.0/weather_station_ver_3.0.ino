
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
float temp[10]={0,0,0,0,0,0,0,0,0,0};
float temp_sum=0;
String webpage;
//_____________________________Record keeping__________________
int myarray[24]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
float minutes=0;
int hours=0;
int pre_hours=-1;
float seconds=0;
float pre_minutes=-1;
int max_val=0;
int avg_val=0;
int  min_val=50;
float sum=0;




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
//input_voltage=analogRead(A0); 
//myarray[0]=int((((input_voltage/1024)*3300))/10);
  
  
}
void loop() {
 seconds=millis()/1000;
 minutes=seconds/60;
 //__________________get temperature____________________
 if(int(minutes)>int(pre_minutes)){
      pre_minutes=minutes;                                       // reads the temperature every minute
      input_voltage=analogRead(A0); 
      temperature=(((input_voltage/1024)*3300))/10;
      place_temp(temperature);
      temp_sum=0;
      for (int i=0;i<10;i++){                                
        temp_sum+=temp[i];
        }
       temperature=temp_sum/10;                                  // taking the average 
       //Serial.print("Average temperature : ");
       //Serial.println(temperature);
      temperature=int(temperature);
 }
//_________________record keeping__________________



hours=minutes/60;
//_____________________
/*Serial.print("this is hours = ");
Serial.println(hours);
Serial.print("int(minutes/6) = ");
Serial.println(int(minutes/6));
*/
/*
Serial.println("max_val   avg_val   min_val");
Serial.print(max_val);
Serial.print("    ");
Serial.print(avg_val);
Serial.print("    ");
Serial.print(min_val);
Serial.println();
Serial.println("testing condition : ");
Serial.print("hours = ");
Serial.println(hours);
Serial.print("pre_hours = ");
Serial.println(pre_hours);
*/

//printarray(temp);

//__________________

if(hours>pre_hours){
  pre_hours=hours;
  place_it(temperature);
  process_history();                                       //process history find averag, minimum and maximum
                                                           // as well as keep record of hourly value
}


//_________________set temperature to webpage_________________________
  webpage=html(String(temperature),String(max_val),String(avg_val),String(min_val));      // create a webpage with the latest values
 



  //______________________________
  server.handleClient();
  delay(100);
  

}

void handle_OnConnect() {
  
  
  server.send(200, "text/html", OnConnectPage()); 
}


void handle_getTemperature(){
  
  
  server.send(200,"text/html",webpage);
  
  
  } 




void handle_NotFound(){
  
  server.send(404, "text/plain", "Not found");
}
//_______________________Record keeping_____________________
void place_it( int in){

//Serial.println("place_it executed");  
//Serial.print("input received as :");
//Serial.println(in);
  for (int i=0;i<23;i++){
    myarray[i]=myarray[i+1];
   
    } 
  
  myarray[23]=in;

 
  }

void place_temp(float t){
  for (int i=0;i<10;i++){
    temp[i]=temp[i+1];
   
    } 
  
  temp[9]=t;
  
  
  }
void process_history(){
//Serial.println("process_history executed");
sum=0;

  for (int j=0;j<24;j++){
     //____________________for average_______________
    sum+=myarray[j];
    //____________________for max___________________
if(myarray[j]>max_val){
  max_val=myarray[j];}
    //____________________for min____________________
    
    if(myarray[j]<min_val& myarray[j]!=0){
      min_val=myarray[j];
      }
    
    }

 if(hours==0){
  avg_val=sum/1;
  }
  else   
  {avg_val=sum/(hours+1);}
  
 
  }

//_________________________
/*void printarray(float a[]){
  for(int i=0;i<10;i++){
    
    Serial.print(a[i]);
    Serial.print(" ");
    }
  Serial.println();
  
  }

*/
//____________________Webpage_______________
String html(String temp, String max_val,String avg_val,String min_val){

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


page+= " <p style = 'font-size:24px; text-align:center'> <b>Maximum = " + max_val+"<sup>o</sup> C</b></p>\n";
page+= " <p style = 'font-size:24px; text-align:center'> <b>Average = "+ avg_val+" <sup>o</sup> C </b></p>\n";
page+= " <p style = 'font-size:24px; text-align:center'> <b>Minimum = "+ min_val+"<sup>o</sup> C </b></p>\n";
page+=" <hr>\n";
page+="<p style='text-align:center;font-size:16'>Time Elapsed since started recording(hh:mm:ss) "+ String(hours) + " : " +String(int(minutes)%60)+ " : " +String(int(seconds)%60)+"</p>\n";

page+=" </body>\n";
page+=" </html>\n";
  return page;
  }
String OnConnectPage(){
  String p="<!DOCTYPE html>\n";
p+="<htlml>";
p+="<head>";
 p+=" <style>";
 p+=" body {background-color:powderblue;}";
 p+=" h1 {color:blue; text-align:center; font-size:36x }";
 p+="       p  {font-size:14x; font:arial;text-align:center; }";
 p+=" </style>  ";
p+="</head>";
p+="  <body>";
  p+="  <h1>Welcome to weather Station</h1>";
    
   p+=" <hr>";
   p+=" <br>";
   p+=" <p>This page was created by sajid ali. <br>Goto Homepage to see the realtime temperature and record of last 24 hours.</p>";
    
   p+=" <br>";
   p+=" <br>  ";
   p+=" <h1><a href='http://192.168.1.1/get_temperature'>Goto Homepage</a> </h1>";

  p+="</body>";

p+="</html>";
return p;  
  }  
