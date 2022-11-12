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
        String direction,
        String openDataUrl,
        String apiKey,
        String numResults);

    String numResultsString;
    String stopPointBPUIC;
    String direction;
    String openDataUrl;
    String apiKey;
    String httpLastError;

    StaticJsonDocument<1000> doc;

    int getWebData(NTPClient timeClient);
    String FormatTimeStamp(String formattedDate, String format);
    uint32_t GetTimeToDeparture(String apiCallTime, String departureTime);
    uint32_t GetEpochTime(String dateTimeStamp);
    String getXmlValue(String xmlStartElement, String xmlEndElement, String xmlDocument);

private:
    const char *rootCACertificate;
};

#endif
