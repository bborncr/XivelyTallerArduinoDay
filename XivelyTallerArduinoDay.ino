#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <Xively.h>
#include "DHT.h"

#define DHTPIN 8
#define DHTTYPE DHT11
#define LEDPIN 2

DHT dht(DHTPIN, DHTTYPE);

// MAC address for your Ethernet shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Your Xively key to let you upload data
char xivelyKey[] = "mlsufjPdQXmmxInr8hYPzY7jOYPREIrCEfYEznYIW51nfyE6";

// Define the strings for our datastream IDs
char sensorId1[] = "Temperatura";
char sensorId2[] = "Humedad";
char ledId[] = "Led";
XivelyDatastream datastreams[] = {
  XivelyDatastream(sensorId1, strlen(sensorId1), DATASTREAM_FLOAT),
  XivelyDatastream(sensorId2, strlen(sensorId2), DATASTREAM_FLOAT),
  XivelyDatastream(ledId, strlen(ledId), DATASTREAM_FLOAT),
};
// Finally, wrap the datastreams into a feed
XivelyFeed feed(886492596, datastreams, 3 /* number of datastreams */);

EthernetClient client;
XivelyClient xivelyclient(client);

void setup() {
  Serial.begin(9600);
  
  pinMode(LEDPIN,OUTPUT);
  digitalWrite(LEDPIN,LOW);

  while (Ethernet.begin(mac) != 1)
  {
    Serial.println("Error getting IP address via DHCP, trying again...");
    delay(15000);
  }
  dht.begin();
}

void loop() {
  
  int getReturn = xivelyclient.get(feed, xivelyKey);    //get data from xively
  if(getReturn > 0){
    Serial.println("LED Datastream");
  }else Serial.println("HTTP Error");
  int level = datastreams[2].getFloat();
  if(level < 0){
    level = 0;
  }else if(level > 0){
    level = 1;
  }
  Serial.println(level);
  digitalWrite(LEDPIN, level);
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
  } else {
    
    datastreams[0].setFloat(t);
    Serial.print("Temperatura: ");
    Serial.println(datastreams[0].getFloat());
    
    datastreams[1].setFloat(h);
    Serial.print("Humedad: ");
    Serial.println(datastreams[1].getFloat());
  }
  Serial.println("Uploading it to Xively");
  int ret = xivelyclient.put(feed, xivelyKey);
  Serial.print("xivelyclient.put returned ");
  Serial.println(ret);
  Serial.println();
  
  
  delay(15000);
}
