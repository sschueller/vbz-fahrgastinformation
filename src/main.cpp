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

OpenTransportDataSwiss api(
    OPEN_DATA_STATION,
    OPEN_DATA_URL,
    OPEN_DATA_API_KEY,
    OPEN_DATA_RESULTS);
    
Display display;

WebServer Server;
AutoConnect Portal(Server);
AutoConnectConfig config(AP_NAME, AP_PASSWORD);

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, timeServer, timeOffset, 60000);
WiFiClient client;

bool startCP(IPAddress ip)
{
  display.connectionMsg();
  delay(10000);
  return true;
}

void setup()
{
  delay(1000);

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

  display.connectingMsg();

#ifdef DEBUG
  Serial.begin(MONITOR_SPEED);
  Serial.setDebugOutput(true);
  Serial.println("Debug Enabled");
#endif

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

  while (!timeClient.update())
  {
    timeClient.forceUpdate();
  }

#ifdef DEBUG
  Serial.println("Time: " + timeClient.getFormattedDate());
#endif

  api.getWebData(timeClient);

  display.printLines(api.doc.as<JsonArray>());

  delay(30000);
}
