#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <Cosm.h>

// MAC address for your Ethernet shield
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x74, 0x93 };

// Your Cosm key to let you upload data
char cosmKey[] = "dVthyUCRchJA8npQhY98nsnmeu6SAKxOZVRmdG4rWFd2dz0g";

// Analog pin which we're monitoring (0 and 1 are used by the Ethernet shield)

int indicator = 7;
int indicator2 = 8;
int upperpower = 6;
int lowerpower = 5;

// Define the strings for our datastream IDs
char sensorId[] = "moisture";
char sensor2Id[] = "light";
CosmDatastream datastreams[] = {
  CosmDatastream(sensorId, strlen(sensorId), DATASTREAM_FLOAT),
  CosmDatastream(sensor2Id, strlen(sensor2Id), DATASTREAM_FLOAT),
};
// Finally, wrap the datastreams into a feed
CosmFeed feed(87022, datastreams, 2 /* number of datastreams */);

EthernetClient client;
CosmClient cosmclient(client);

void setup() {
  // put your setup code here, to run once:
  pinMode(indicator, OUTPUT);
  pinMode(indicator2, OUTPUT);
  pinMode(upperpower, OUTPUT);
  pinMode(lowerpower, OUTPUT);
  
  Serial.begin(9600);
  
  Serial.println("Starting multiple datastream upload to Cosm...");
  Serial.println();

  while (Ethernet.begin(mac) != 1)
  {
    Serial.println("Error getting IP address via DHCP, trying again...");
    delay(15000);
  }
}

void loop() {
  int sensorValue = getMoisture();
  if (sensorValue < 200) {
    digitalWrite(indicator2, LOW);
    digitalWrite(indicator, HIGH);
  }
  else {
    digitalWrite(indicator, LOW);
    digitalWrite(indicator2, HIGH);
  }
  datastreams[0].setFloat(sensorValue);

  Serial.print("Read sensor value ");
  Serial.println(datastreams[0].getFloat());

  sensorValue = analogRead(A3);
  datastreams[1].setFloat(sensorValue);

  Serial.print("Read sensor value ");
  Serial.println(datastreams[1].getFloat());

  Serial.println("Uploading it to Cosm");
  int ret = cosmclient.put(feed, cosmKey);
  Serial.print("cosmclient.put returned ");
  Serial.println(ret);

  Serial.println();
  Serial.println("Sleeping for 10 minutes");
  delay(600000);
  Serial.println("Sleep finished...");
}

int getMoisture () {
  digitalWrite(upperpower, HIGH);
  digitalWrite(lowerpower, LOW);
  delay(2000);
  int moisture = analogRead(A2);
  delay(1000);
  digitalWrite(upperpower, LOW);
  digitalWrite(lowerpower, HIGH);
  delay(3000);
  digitalWrite(lowerpower, LOW);
  return moisture;
}
