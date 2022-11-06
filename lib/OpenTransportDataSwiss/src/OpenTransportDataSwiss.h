#ifndef OpenTransportDataSwiss_H
#define OpenTransportDataSwiss_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <NTPClient.h>
#include <ArduinoJson.h>
#include <UnixTime.h>
#include <math.h>

class OpenTransportDataSwiss
{

public:
    OpenTransportDataSwiss(
        String stopPointBPUIC,
        String openDataUrl,
        String apiKey,
        String numResults);

    String numResultsString;
    String stopPointBPUIC;
    String openDataUrl;
    String apiKey;

    StaticJsonDocument<1000> doc;

    void getWebData(NTPClient timeClient);
    String GetTimeStamp(NTPClient timeClient, String format);
    uint32_t GetTimeToDeparture(String apiCallTime, String departureTime);
    uint32_t GetEpochTime(String dateTimeStamp);

private:
    const char *rootCACertificate;
};

#endif
