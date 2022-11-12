#include <OpenTransportDataSwiss.h>

OpenTransportDataSwiss::OpenTransportDataSwiss(String stopPointBPUIC,
                                               String direction,
                                               String openDataUrl,
                                               String apiKey,
                                               String numResults)
{

    OpenTransportDataSwiss::numResultsString = numResults;
    OpenTransportDataSwiss::stopPointBPUIC = stopPointBPUIC;
    OpenTransportDataSwiss::direction = direction;
    OpenTransportDataSwiss::openDataUrl = openDataUrl;
    OpenTransportDataSwiss::apiKey = apiKey;
}

int OpenTransportDataSwiss::getWebData(NTPClient timeClient)
{
    WiFiClientSecure *client = new WiFiClientSecure;
    if (client)
    {
        // If we only one one direction we need to get the double amount of results in order to fill the display
        int resultsToGet = OpenTransportDataSwiss::numResultsString.toInt();
        if (OpenTransportDataSwiss::direction != "A")
        {
            resultsToGet = resultsToGet * 2;
        }

        // client->setCACert(rootCACertificate);
        client->setInsecure(); // should not be required

        // get a timestamp to "now"
        String formattedDate = timeClient.getFormattedDate();

        // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
        HTTPClient https;

        String PostData = "<Trias version=\"1.1\"";
        PostData += "xmlns=\"http://www.vdv.de/trias\"";
        PostData += "xmlns:siri=\"http://www.siri.org.uk/siri\"";
        PostData += "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">";
        PostData += "<ServiceRequest>";
        PostData += "    <siri:RequestTimestamp>" + OpenTransportDataSwiss::FormatTimeStamp(formattedDate, "RequestTimestamp") + "</siri:RequestTimestamp>";
        PostData += "    <siri:RequestorRef>API-Explorer</siri:RequestorRef>";
        PostData += "    <RequestPayload>";
        PostData += "        <StopEventRequest>";
        PostData += "            <Location>";
        PostData += "                <LocationRef>";
        PostData += "                    <StopPointRef>" + OpenTransportDataSwiss::stopPointBPUIC + "</StopPointRef>";
        PostData += "                </LocationRef>";
        PostData += "                <DepArrTime>" + OpenTransportDataSwiss::FormatTimeStamp(formattedDate, "DepArrTime") + "</DepArrTime>";
        PostData += "            </Location>";
        PostData += "            <Params>";
        PostData += "                <NumberOfResults>" + (String)resultsToGet + "</NumberOfResults>";
        PostData += "                <StopEventType>departure</StopEventType>";
        PostData += "                <IncludePreviousCalls>false</IncludePreviousCalls>";
        PostData += "                <IncludeOnwardCalls>false</IncludeOnwardCalls>";
        PostData += "                <IncludeRealtimeData>true</IncludeRealtimeData>";
        PostData += "            </Params>";
        PostData += "        </StopEventRequest>";
        PostData += "    </RequestPayload>";
        PostData += "</ServiceRequest>";
        PostData += "</Trias>";

        // Serial.println(url);
        // Serial.println(PostData);

        https.addHeader("Authorization", OpenTransportDataSwiss::apiKey);
        https.addHeader("Content-Type", "text/XML");

        // Serial.println(https.getString());

#ifdef DEBUG
        Serial.print("[HTTPS] begin...\n");
#endif
        if (https.begin(*client, OpenTransportDataSwiss::openDataUrl))
        { // HTTPS
#ifdef DEBUG
            Serial.print("[HTTPS] POST...\n");
#endif
            // start connection and send HTTP header
            int httpCode = https.POST(PostData);

            // httpCode will be negative on error
            if (httpCode > 0)
            {
// HTTP header has been send and Server response header has been handled
#ifdef DEBUG
                Serial.printf("[HTTPS] POST... code: %d\n", httpCode);
#endif

                // file found at server
                if (httpCode == HTTP_CODE_OK)
                {
                    // Parse
#ifdef DEBUG
                    Serial.printf("[HTTPS] Parse\n");
#endif

                    // get length of document (is -1 when Server sends no Content-Length header)
                    // int len = https.getSize();
                    // Serial.printf("len: %d\n", len);

                    String result = https.getString();

                    int index = result.indexOf("</trias:StopEvent>");
                    int counter = 0;

                    JsonArray data = doc.to<JsonArray>();

                    // Serial.printf("xml: %s\n", result.c_str());

                    while (index != -1)
                    {
                        String part = OpenTransportDataSwiss::getXmlValue("<trias:StopEvent>", "</trias:StopEvent>", result);

                        result.remove(0, result.indexOf("</trias:StopEvent>") + 19);
                        index = result.indexOf("</trias:StopEvent>");

                        // Serial.printf("xml %d: %s\n", index,  part.c_str());

                        // find live dep time <trias:EstimatedTime>2022-11-04T16:02:00Z</trias:EstimatedTime>
                        // find sched dep time <trias:TimetabledTime>2022-11-05T19:43:00Z</trias:TimetabledTime>
                        String departureTime;
                        bool liveData = false;
                        bool isLate = false;
                        if (part.indexOf("<trias:EstimatedTime>") != -1)
                        {
                            // Has live data
                            liveData = true;
                            departureTime = OpenTransportDataSwiss::getXmlValue(
                                "<trias:EstimatedTime>",
                                "</trias:EstimatedTime>",
                                part);

                            // check if late
                            String scheduledTime = OpenTransportDataSwiss::getXmlValue(
                                "<trias:TimetabledTime>",
                                "</trias:TimetabledTime>",
                                part);

                            uint32_t drift = OpenTransportDataSwiss::GetTimeToDeparture(scheduledTime, departureTime);
                            // Serial.printf("drift: %d\n", drift);

                            if (drift >= OpenTransportDataSwiss::lateMinCutoff) {
                                isLate = true;
                            }

                        }
                        else
                        {
                            // Has no live data, use scheduled
                            departureTime = OpenTransportDataSwiss::getXmlValue(
                                "<trias:TimetabledTime>",
                                "</trias:TimetabledTime>",
                                part);
                        }

                        // Serial.printf("departureTime: %d: %s\n", index, departureTime.c_str());

                        // find destination <trias:DestinationText><trias:Text>Zürich, Rehalp</trias:Text><trias:Language>de</trias:Language></trias:DestinationText>
                        String destination = OpenTransportDataSwiss::getXmlValue(
                            "<trias:DestinationText><trias:Text>",
                            "</trias:Text><trias:Language>de</trias:Language></trias:DestinationText>",
                            part);

                        // Serial.printf("destination: %d: %s\n", index, destination.c_str());

                        // find Line <trias:PublishedLineName><trias:Text>11</trias:Text><trias:Language>de</trias:Language></trias:PublishedLineName>
                        String line = OpenTransportDataSwiss::getXmlValue(
                            "<trias:PublishedLineName><trias:Text>",
                            "</trias:Text><trias:Language>de</trias:Language></trias:PublishedLineName>",
                            part);

                        // Serial.printf("line: %d: %s\n", index, line.c_str());

                        // find NF <trias:Code>A__NF</trias:Code>
                        bool isNF = false;
                        if (part.indexOf("<trias:Code>A__NF</trias:Code>") != -1)
                        {
                            isNF = true;
                        }

                        String lineRef = OpenTransportDataSwiss::getXmlValue(
                            "<trias:LineRef>",
                            "</trias:LineRef>",
                            part);

                        // Serial.printf("NF: %d: %s\n", index, isNF);

                        // match direction if set and skip if it doesn't match
                        String refDirection = lineRef.substring(lineRef.lastIndexOf(":") + 1, lineRef.length());
                        if (OpenTransportDataSwiss::direction != "A" && refDirection != OpenTransportDataSwiss::direction)
                        {
                            continue;
                        }

                        StaticJsonDocument<200> doc2;
                        JsonObject item = doc2.to<JsonObject>();

                        item["departureTime"] = departureTime;
                        item["ttl"] = OpenTransportDataSwiss::GetTimeToDeparture(OpenTransportDataSwiss::FormatTimeStamp(formattedDate, "RequestTimestamp"), departureTime);
                        item["liveData"] = liveData;
                        item["line"] = line;
                        item["lineRef"] = lineRef;
                        item["isNF"] = isNF;
                        item["isLate"] = isLate;
                        item["destination"] = destination;

                        data.add(item);

                        counter++;
                    }

                    if (data.isNull())
                    {
                        Serial.printf("No data: %s\n", result.c_str());
                    }
                    https.end();
                    delete client;
                    return 0;
                }
                else
                {
                    Serial.printf("[HTTPS] POST... failed, http code error: %s\n", https.errorToString(httpCode).c_str());

                    if (httpCode == 403)
                    {
                        OpenTransportDataSwiss::httpLastError = "ERROR: Authentication Failed. API key may be incorrect or expired. Code: " + (String)httpCode;
                    }
                    else
                    {
                        OpenTransportDataSwiss::httpLastError = "ERROR: http code error: " + (String)httpCode;
                    }
                }
            }
            else
            {
                Serial.printf("[HTTPS] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
                OpenTransportDataSwiss::httpLastError = "ERROR: http code error: " + (String)httpCode;
            }

            https.end();
        }
        else
        {
            Serial.printf("[HTTPS] Unable to connect\n");
            OpenTransportDataSwiss::httpLastError = "Unable to connect";
        }

        // End extra scoping block

        delete client;
    }
    else
    {
        Serial.println("Unable to create client or no stations set");
        OpenTransportDataSwiss::httpLastError = "Unable to create client or no stations set";
    }

    return 1;
}

String OpenTransportDataSwiss::FormatTimeStamp(String formattedDate, String format)
{
    // RequestTimestamp>2022-11-04T15:38:26.611Z
    // DepArrTime>2022-11-04T16:38:26

    if (format == "RequestTimestamp")
    {
        return formattedDate;
    }

    return formattedDate.substring(0, formattedDate.lastIndexOf("."));
}

uint32_t OpenTransportDataSwiss::GetTimeToDeparture(String apiCallTime, String departureTime)
{

    // Serial.printf("apiCallTime: %s\n", apiCallTime.c_str());
    // Serial.printf("departureTime: %s\n", departureTime.c_str());

    uint32_t now = OpenTransportDataSwiss::GetEpochTime(apiCallTime);
    uint32_t dep = OpenTransportDataSwiss::GetEpochTime(departureTime);

    // Serial.printf("now: %d\n", now);
    // Serial.printf("dep: %d\n", dep);

    if ((dep <= now) || (dep - now) <= 60)
    {
        return 0;
    }

    // Serial.printf("res: %d\n", round((dep - now) / 60));

    return round((dep - now) / 60);
}

uint32_t OpenTransportDataSwiss::GetEpochTime(String dateTimeStamp)
{

    String dayStamp = dateTimeStamp.substring(0, dateTimeStamp.indexOf("T"));
    int year = dayStamp.substring(0, dayStamp.indexOf("-")).toInt();
    int month = dayStamp.substring(dayStamp.indexOf("-") + 1, dayStamp.lastIndexOf("-")).toInt();
    int day = dayStamp.substring(dayStamp.lastIndexOf("-") + 1).toInt();

    String timeStamp = dateTimeStamp.substring(dateTimeStamp.indexOf("T") + 1, dateTimeStamp.indexOf("Z"));
    int hour = timeStamp.substring(0, timeStamp.indexOf(":")).toInt();
    int minute = timeStamp.substring(timeStamp.indexOf(":") + 1, timeStamp.lastIndexOf(":")).toInt();
    int seconds = timeStamp.substring(timeStamp.lastIndexOf(":") + 1).toInt();

    UnixTime stamp(0);

    stamp.setDateTime(year, month, day, hour, minute, seconds);

    return stamp.getUnix();
}

String OpenTransportDataSwiss::getXmlValue(String xmlStartElement, String xmlEndElement, String xmlDocument)
{
    return xmlDocument.substring(
        xmlDocument.indexOf(xmlStartElement) + xmlStartElement.length(),
        xmlDocument.indexOf(xmlEndElement));
}