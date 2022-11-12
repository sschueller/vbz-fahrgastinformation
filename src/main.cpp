#include <Config.h>
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <WiFi.h>
#include <AutoConnect.h>
#include <WebServer.h>
#include <NTPClient.h>

#include <Display.h>
#include <OpenTransportDataSwiss.h>

// Set offset time in seconds to adjust for your timezone, for example:
// GMT +1 = 3600
// GMT +8 = 28800
// GMT -1 = -3600
// GMT 0 = 0
int timeOffset = 3600;
const char timeServer[11] = TIME_SERVER;
int sensorValue;
int loopCounter = 0;

OpenTransportDataSwiss api(
    OPEN_DATA_STATION,
    OPEN_DATA_DIRECTION,
    OPEN_DATA_URL,
    OPEN_DATA_API_KEY,
    OPEN_DATA_RESULTS);

Display display;

WebServer Server;
AutoConnect Portal(Server);
AutoConnectConfig Config;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, timeServer, timeOffset, 60000);
WiFiClient client;

bool startCP(IPAddress ip)
{
  display.connectionMsg(AP_NAME, AP_PASSWORD);
  delay(10000);
  return true;
}

void setup()
{
  delay(1000);

#ifdef DEBUG
  Serial.begin(MONITOR_SPEED);
  Serial.setDebugOutput(true);
  Serial.println("Debug Enabled");
#endif

  display.begin(
      R1_PIN,
      G1_PIN,
      B1_PIN,
      R2_PIN,
      G2_PIN,
      B2_PIN,
      A_PIN,
      B_PIN,
      C_PIN,
      D_PIN,
      E_PIN,
      LAT_PIN,
      OE_PIN,
      CLK_PIN,
      PANEL_RES_X,
      PANEL_RES_Y,
      PANEL_CHAIN);

  display.showSplash();

  delay(3000);

  display.connectingMsg();

  Config.title = "VBZ-Anzeige";
  Config.apid = AP_NAME;
  Config.psk = AP_PASSWORD;
  // Config.staip = IPAddress(192,168,4,100);
  // Config.staGateway = IPAddress(192,168,1,1);
  // Config.staNetmask = IPAddress(255,255,255,0);
  // Config.dns1 = IPAddress(192,168,1,1);

  Portal.config(Config);
  Portal.onDetect(startCP);

  if (Portal.begin())
  {

#ifdef DEBUG
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
#endif

    display.showIpAddress(WiFi.localIP().toString().c_str());
    delay(2000);

    // Initialize a NTPClient to get time
    timeClient.begin();
    timeClient.setTimeOffset(timeOffset);
  }
}

void loop()
{
  Portal.handleClient();

  int timeTimer = 0;
  while (!timeClient.update())
  {
    if (timeTimer > 30) {
      display.printError("Unable to get time\nfrom NTP Server:\n" + (String) TIME_SERVER);
    }

    timeClient.forceUpdate();
    timeTimer++;
  }

#ifdef DEBUG
  Serial.println("Time: " + timeClient.getFormattedDate());
#endif

  // brightness sensor
  sensorValue = analogRead(A0);
  sensorValue = map(sensorValue, 0, 4095, 12, 255);
  display.displaySetBrightness(sensorValue);

  // Serial.println(touchRead(touchRead(GPIO_NUM_32)));

  if (loopCounter == 0)
  {
    if (int apiCode = api.getWebData(timeClient) == 0)
    {
      // Serial.println(api.doc.as<String>());
      display.printLines(api.doc.as<JsonArray>());
    }
    else
    {
      display.printError(api.httpLastError);
    }
  }

  loopCounter++;

  if (loopCounter > 31)
  {
    loopCounter = 0;
  }

  delay(1000);
}
