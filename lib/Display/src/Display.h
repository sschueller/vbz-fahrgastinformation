#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <vbzfont.h>
#include <Adafruit_GFX.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

class Display
{

public:
    void begin(int r1_pin,
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
               int panel_chain);

    uint8_t getRightAlignStartingPoint(const char *str, int16_t width);

    void printLine(String line, String direction, bool accessible, int timtToArrival, bool liveData, int position);
    void printLines(JsonArray data);

    void showIpAddress(const char *ipAddress);
    void connectingMsg();
    void connectionMsg(String apName, String password);

    void displaySetBrightness(int brightness);

    uint16_t getVbzFontColor(int line);
    uint16_t getVbzBackgroundColor(int line);

private:
    MatrixPanel_I2S_DMA *dma_display = nullptr;
    uint16_t myBLACK = dma_display->color565(0, 0, 0);
    uint16_t vbzYellow = dma_display->color565(255, 255, 255); // 252, 249, 110
    uint16_t vbzWhite = dma_display->color565(255, 255, 255);
    uint16_t vbzBlack = dma_display->color565(0, 0, 0);
};

#endif