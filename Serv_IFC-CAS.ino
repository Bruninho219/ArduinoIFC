/*
5V= DHT+
GND = DHT-
A1 = DHT_Date
GND = Buzzer-
D7 = Buzzer+
*/

#include <SPI.h>
#include <Ethernet.h>
#include <DHT.h>
 
#define DHTPIN A1
#define DHTTYPE DHT11
#define BUZZERPIN 7
 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
float h = 0;
float h0 = 0;
float t = 0;
float t0 = 0;
float tmax = 26;
float seno;
int frequencia;

IPAddress ip(10,0,200,219);
IPAddress gateway(10,0,0,1);
IPAddress subnet(255, 255, 0, 0);
DHT dht(DHTPIN, DHTTYPE);
EthernetServer server(80);

void setup()
{
  pinMode(BUZZERPIN,OUTPUT);
  Ethernet.begin(mac, ip, gateway, subnet);
  dht.begin();
  server.begin();
}
 
void loop()
{
  h=dht.readHumidity();
  t=dht.readTemperature();

  if(t>tmax)
  {
    sirene(1);
  }
  else
  {
    sirene(0);
  }
  
  EthernetClient client = server.available();
  if (client)
  {
    Serial.println("new client");
    boolean currentLineIsBlank = true;
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        Serial.write(c);
        if (c == 'n' && currentLineIsBlank)
        {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println("Refresh: 5");
          client.println();
          client.println("<!DOCTYPE HTML><html><head><title>Serv. IFC-CAS [");
          client.print(t);
          client.println("&#186;C]</title></head><boby><b>Temperatura: ");
          client.print(t);
          client.print("&#186;C");
          t0=View(client, t, t0);
          client.print("<br/>Umidade: ");
          client.print(h);
          client.print("%");
          h0=View(client, h, h0);
          client.println("<br/></b></body></html>");
          break;
        }
        if (c == 'n')
        {
          currentLineIsBlank = true;
        } 
        else if (c != 'r')
        {
          currentLineIsBlank = false;
        }
      }
    }
    delay(50);
    client.stop();
  }
}

void sirene(int x)
{
  if(x==1)
  {
    for(int x=0;x<180;x++)
    {
      seno=(sin(x*3.1416/180));
      frequencia = 2000+(int(seno*1000));
      tone(BUZZERPIN,frequencia);
      delay(5);
    }
  }
  else
  {
    noTone(BUZZERPIN);
  }
}

float View(EthernetClient client, float x, float x0)
{
  if(x>x0)
  {
    client.println("&#9650;");
  }
  if(x<x0)
  {
    client.println("&#9660;");
  }
  if(x==x0)
  {
    client.println("&#9654;");
  }
  return x;  
}