#include <Display.h>

void Display::begin(int r1_pin,
                    int g1_pin,
                    int b1_pin,
                    int r2_pin,
                    int g2_pin,
                    int b2_pin,
                    int a_pin,
                    int b_pin,
                    int c_pin,
                    int d_pin,
                    int e_pin,
                    int lat_pin,
                    int oe_pin,
                    int clk_pin,
                    int panel_res_x,
                    int panel_res_y,
                    int panel_chain)
{

    HUB75_I2S_CFG mxConfig;

    mxConfig.mx_width = panel_res_x;
    mxConfig.mx_height = panel_res_y;    // we have 64 pix heigh panels
    mxConfig.chain_length = panel_chain; // we have 2 panels chained
    mxConfig.gpio.e = e_pin;

    mxConfig.gpio.r1 = g1_pin;
    mxConfig.gpio.g1 = b1_pin;
    mxConfig.gpio.b1 = r1_pin;
    mxConfig.gpio.r2 = g2_pin;
    mxConfig.gpio.g2 = b2_pin;
    mxConfig.gpio.b2 = r2_pin;

    Display::dma_display = new MatrixPanel_I2S_DMA(mxConfig);
    Display::dma_display->begin();
    Display::dma_display->setTextWrap(false);
    Display::dma_display->setBrightness8(64); //0-255
    Display::dma_display->clearScreen();
    Display::dma_display->fillScreen(Display::myBLACK);
}

void Display::showSplash()
{
    Display::dma_display->clearScreen();
    Display::dma_display->fillScreen(Display::myBLACK);

    // 'ch', 34x34px
    const unsigned char epd_bitmap_ch[] PROGMEM = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0x80, 0x7f, 0xff, 0xff, 0xff, 0x80, 0x7f,
        0xff, 0xff, 0xff, 0x80, 0x7f, 0xff, 0xff, 0xff, 0x80, 0x7f, 0xff, 0xff, 0xff, 0x80, 0x7f, 0xff,
        0xff, 0xff, 0x80, 0x7f, 0xfc, 0x0f, 0xff, 0x80, 0x7f, 0xfc, 0x0f, 0xff, 0x80, 0x7f, 0xfc, 0x0f,
        0xff, 0x80, 0x7f, 0xfc, 0x0f, 0xff, 0x80, 0x7f, 0xfc, 0x0f, 0xff, 0x80, 0x7f, 0xfc, 0x0f, 0xff,
        0x80, 0x7f, 0xfc, 0x0f, 0xff, 0x80, 0x7e, 0x00, 0x00, 0x1f, 0x80, 0x7e, 0x00, 0x00, 0x1f, 0x80,
        0x7e, 0x00, 0x00, 0x1f, 0x80, 0x7e, 0x00, 0x00, 0x1f, 0x80, 0x7e, 0x00, 0x00, 0x1f, 0x80, 0x7e,
        0x00, 0x00, 0x1f, 0x80, 0x7f, 0xfc, 0x0f, 0xff, 0x80, 0x7f, 0xfc, 0x0f, 0xff, 0x80, 0x7f, 0xfc,
        0x0f, 0xff, 0x80, 0x7f, 0xfc, 0x0f, 0xff, 0x80, 0x7f, 0xfc, 0x0f, 0xff, 0x80, 0x7f, 0xfc, 0x0f,
        0xff, 0x80, 0x7f, 0xfc, 0x0f, 0xff, 0x80, 0x7f, 0xff, 0xff, 0xff, 0x80, 0x7f, 0xff, 0xff, 0xff,
        0x80, 0x7f, 0xff, 0xff, 0xff, 0x80, 0x7f, 0xff, 0xff, 0xff, 0x80, 0x7f, 0xff, 0xff, 0xff, 0x80,
        0x7f, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00};

    Display::dma_display->fillRect((128 - 34) / 2, (64 - 34) / 2, 34, 34, Display::vbzWhite);
    Display::dma_display->drawBitmap((128 - 34) / 2, (64 - 34) / 2, epd_bitmap_ch, 34, 34, Display::vbzRed);

    Display::dma_display->setFont();
    Display::dma_display->setTextSize(1);
    Display::dma_display->setCursor(0, 50);
    Display::dma_display->println("opentransportdata.swiss");
}

void Display::displaySetBrightness(int brightness)
{
    Display::dma_display->setBrightness8(brightness); //0-255
}

void Display::showIpAddress(const char *ipAddress)
{
    Display::dma_display->fillScreen(Display::myBLACK);
    Display::dma_display->setFont(&vbzfont);
    Display::dma_display->setTextSize(1);
    Display::dma_display->setCursor(0, 0);
    Display::dma_display->println("configure at:");
    Display::dma_display->println(ipAddress);
}

void Display::connectingMsg()
{
    Display::dma_display->fillScreen(Display::myBLACK);
    Display::dma_display->setFont(&vbzfont);
    Display::dma_display->setTextSize(1);
    Display::dma_display->setCursor(0, 0);
    Display::dma_display->println("connecting...");
}

void Display::connectionMsg(String apName, String password)
{
    Display::dma_display->fillScreen(Display::myBLACK);
    Display::dma_display->setFont(&vbzfont);
    Display::dma_display->setTextSize(1);
    Display::dma_display->setCursor(0, 0);
    Display::dma_display->println("connect to:");
    Display::dma_display->println(apName);
    Display::dma_display->println("pwd: " + password);
}

void Display::printError(String apiError)
{
    Display::dma_display->clearScreen();
    Display::dma_display->fillScreen(Display::vbzBlack);
    Display::dma_display->setFont();
    Display::dma_display->setTextWrap(true);
    Display::dma_display->setTextSize(1);
    Display::dma_display->setTextColor(Display::vbzRed);
    Display::dma_display->setCursor(0, 0);
    Display::dma_display->println(apiError);
    Display::dma_display->setTextWrap(false);
}

void Display::printLines(JsonArray data)
{

    Display::dma_display->fillScreen(Display::myBLACK);

    int index = 0;
    for (const auto &value : data)
    {
        if (value["ttl"].as<int>() >= 0)
        {
            Display::printLine(
                value["line"].as<String>(),
                value["lineRef"].as<String>(),
                value["destination"].as<String>(),
                value["isNF"].as<bool>(),
                value["ttl"].as<int>(),
                value["liveData"].as<bool>(),
                index);
            index++;
        }
    }
}

uint8_t Display::getRightAlignStartingPoint(const char *str, int16_t width)
{

    // int16_t x1 = 0;
    // int16_t y1 = 0;
    // uint16_t w = 0;
    // uint16_t h = 0;
    // dma_display->getTextBounds(str, dma_display->getCursorX(), dma_display->getCursorY(), &x1, &y1, &w, &h);

    // We can't use getTextBounds() because out font is packed as a monospaced font but it isn't. So we need to check how
    // far the text advances to get the width

    GFXcanvas1 canvas(width, 16); // 128x32 pixel canvas
    canvas.setCursor(0, 0);
    canvas.print(str);
    int advance = canvas.getCursorX() + 1;

#ifdef DEBUG
    // Serial.print("----");
    // Serial.print(str);
    // Serial.print(" cx:");
    // Serial.print(advance);
    // Serial.print(" r:");
    // Serial.print(width);
    // Serial.print(" x1:");
    // Serial.print(x1);
    // Serial.print(" y1:");
    // Serial.print(y1);
    // Serial.print(" w:");
    // Serial.print(w);
    // Serial.print(" h:");
    // Serial.print(h);
    // Serial.println("***");
#endif

    return (width - advance);
}

void Display::printLine(String line, String lineRef, String destination, bool accessible, int timeToArrival, bool liveData, int position)
{

    //
    // | Line       | Space     | Destination | Space     | Accessibility  | Space      | TTA        |
    // | <-- 20 --> | <-- 6 --> | <-- X --> | <--   --> | <--      ---> | <--   -->  | <-- 16 --> |
    // | Right Align|           | Left Align|           | Left Align    |            | Right Align|
    //

    // Needs to be set first for correct xPos calculation

    Display::dma_display->setFont(&vbzfont);
    Display::dma_display->setTextSize(1);
    Display::dma_display->setTextWrap(false);

    String NF = "";
    if (accessible)
    {
        NF = "NF";
    }

    String LD = "S";
    if (liveData)
    {
        LD = "L";
    }

#ifdef DEBUG
    Serial.printf("%d|%s|%s|%s|%d|%s|%s\n", position, line.c_str(), lineRef.c_str(), destination.c_str(), timeToArrival, NF.c_str(), LD.c_str());
#endif

    char infoLine[25];
    char lineCh[20];
    char destinationCh[25];
    char accessCh[1];
    char ttlCh[5];

    int lineNumber = position * 13;

    // reset cursor
    dma_display->setCursor(0, 0);

    // Line
    line.replace(" ", "");
    line.trim();
    line.toCharArray(lineCh, 23);
    sprintf(infoLine, "%s", lineCh);

    int xPos = Display::getRightAlignStartingPoint(line.c_str(), 23);

#ifdef DEBUG
    // Serial.print(xPos);
    // Serial.print(",");
    // Serial.print(lineNumber);
    // Serial.print("->");
    // Serial.print(infoLine);
    // Serial.print(":\n");
#endif

    Display::dma_display->fillRect(0, lineNumber, 24, 11, getVbzBackgroundColor(lineRef));
    Display::dma_display->setTextColor(Display::getVbzFontColor(lineRef));
    Display::dma_display->setCursor(xPos, lineNumber);
    Display::dma_display->print(infoLine);

    // Direction
    destination = Display::cropDestination(destination);
    destination.toCharArray(destinationCh, 25);
    sprintf(infoLine, "%s", destinationCh);

    Display::dma_display->setTextColor(Display::vbzYellow);
    Display::dma_display->setCursor(27, lineNumber);
    Display::dma_display->setTextWrap(false);
    Display::dma_display->print(infoLine);

#ifdef DEBUG
    // Serial.print(xPos);
    // Serial.print(",");
    // Serial.print(lineNumber);
    // Serial.print("->");
    // Serial.print(infoLine);
    // Serial.print(":\n\n");
#endif

    // Accessibility
    if (accessible)
    {
        String acces = "\x1F";

        acces.toCharArray(accessCh, 25);
        sprintf(infoLine, "%s", accessCh);

        Display::dma_display->setTextColor(Display::vbzYellow);
        Display::dma_display->setCursor(97, lineNumber);
        Display::dma_display->setTextWrap(false);
        Display::dma_display->print(infoLine);

#ifdef DEBUG
        // Serial.print(infoLine);
        // Serial.print(":");
#endif
    }

    // TTA
    if (timeToArrival <= 0)
    {
        sprintf(ttlCh, "\x1E");

        xPos = 16 - 8;
    }
    else
    {
        sprintf(ttlCh, "%d", timeToArrival);
        xPos = Display::getRightAlignStartingPoint(ttlCh, 16);
    }

    // 128 - 16 = 112

    Display::dma_display->setTextColor(Display::vbzYellow);
    Display::dma_display->setCursor(108 + xPos, lineNumber);
    Display::dma_display->setTextWrap(false);
    Display::dma_display->print(ttlCh);

    if (timeToArrival > 0)
    {
        String liveDataMarker = "'";
        if (liveData)
        {
            liveDataMarker = "`";
        }
        Display::dma_display->setTextColor(Display::vbzYellow);
        Display::dma_display->setCursor(125, lineNumber);
        Display::dma_display->setTextWrap(false);
        Display::dma_display->print(liveDataMarker);
    }
}

uint16_t Display::getVbzFontColor(String lineRef)
{
    int lineId = Display::getLinRefId(lineRef);

    uint8_t r, g, b;

    switch (lineId)
    {
    case 91008:
        // rgb(0, 0, 0)
        r = 0;
        g = 0;
        b = 0;
        break;
    case 91013:
        // rgb(0, 0, 0)
        r = 0;
        g = 0;
        b = 0;
        break;
    default:
        // statements
        r = 255;
        g = 255;
        b = 255;
        break;
    }

    return dma_display->color565(r, g, b);
}

uint16_t Display::getVbzBackgroundColor(String lineRef)
{
    int lineId = Display::getLinRefId(lineRef);

    uint8_t r, g, b;

    switch (lineId)
    {
    case 91002:
        // rgb(229, 0, 0)
        r = 229;
        g = 0;
        b = 0;
        break;
    case 91003:
        // rgb(0, 138, 41)
        r = 0;
        g = 138;
        b = 41;
        break;
    case 91004:
        // rgb(14, 37, 110)
        r = 14;
        g = 37;
        b = 110;
        break;
    case 91005:
        // rgb(116, 69, 30)
        r = 116;
        g = 69;
        b = 30;
        break;
    case 91006:
        // rgb(204, 125, 52)
        r = 204;
        g = 128;
        b = 52;
        break;
    case 91007:
        // rgb(0, 0, 0)
        r = 0;
        g = 0;
        b = 0;
        break;
    case 91008:
        // rgb(137, 183, 0)
        r = 137;
        g = 183;
        b = 0;
        break;
    case 91009:
        // rgb(15, 38, 113)
        r = 15;
        g = 38;
        b = 113;
        break;
    case 91010:
        // rgb(228, 29, 113)
        r = 228;
        g = 29;
        b = 113;
        break;
    case 91011:
        // rgb(0, 138, 41)
        r = 0;
        g = 138;
        b = 41;
        break;
    case 91013:
        // rgb(255, 194, 0)
        r = 255;
        g = 194;
        b = 0;
        break;
    case 91014:
        // rgb(0, 140, 199)
        r = 0;
        g = 140;
        b = 199;
        break;
    case 91015:
        // rgb(228, 0, 0)
        r = 228;
        g = 0;
        b = 0;
        break;
    case 91017:
        // rgb(144, 29, 77)
        r = 144;
        g = 29;
        b = 77;
        break;
    default:
        // statements
        r = 0;
        g = 0;
        b = 0;
        break;
    }

    return dma_display->color565(r, g, b);
}

int Display::getLinRefId(String lineRef)
{
    String refId = lineRef.substring(
        lineRef.indexOf(":") + 1,
        lineRef.indexOf(":") + 6);

    return refId.toInt();
}

String Display::cropDestination(String destination)
{
    // remove extra text
    destination.replace("Zürich,", "");
    destination.replace("Winterthur,", "");
    destination.replace("Bahnhof ", "");

    // fix umlauts being in wrong position in font
    destination.replace("ä", "\x7B");
    destination.replace("ö", "\x7C");
    destination.replace("ü", "\x7D");

    // shorten destination if too long
    destination.trim();

    bool textWasTooLong = (Display::getTextUsedLength(destination) >= Display::maxDestinationPixels);

    // remove characters until text fits in max width
    while (Display::getTextUsedLength(destination) >= Display::maxDestinationPixels)
    {
        destination = destination.substring(0, destination.length() - 1);
    }

    if (textWasTooLong)
    {
        destination = destination + ".";
    }

    return destination;
}

int Display::getTextUsedLength(String text)
{
    GFXcanvas1 canvas(Display::dma_display->width(), 16);
    canvas.setFont(&vbzfont);
    canvas.setTextSize(1);
    canvas.setTextWrap(false);
    canvas.setCursor(0, 0);
    canvas.print(text);
    return canvas.getCursorX() + 1;
}