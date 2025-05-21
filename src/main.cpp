#ifndef BOARD_HAS_PSRAM
#error "Please enable PSRAM, Arduino IDE -> tools -> PSRAM -> OPI !!!"
#endif

#include <Arduino.h>
#include "epd_driver.h"
#include "utilities.h"
#include "WiFi.h"
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "HTTPRequestUtil.h"

#define network_ssid "1"
#define network_password "2"
#define image_url "http://192.168.1.144:3000/4bit"
#define image_url2 "http://192.168.1.144:3000/4bitsss"

void wifi_init()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(network_ssid, network_password);

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Connecting to WiFi\n");
    }
}

uint8_t *RGBAimageData = NULL; // 2,073,600 B = 2,073.6 kB = 2.025 MB
uint8_t *framebuffer = NULL;   // (EPD_WIDTH * EPD_HEIGHT) / 2 =  259,200bytes
uint8_t *responseBuffer = NULL;

const int displayFrameBufferSize = EPD_WIDTH * EPD_HEIGHT / 2;

void initFramebuffer()
{
    // Custom ESP32 Allocate Memory Function - allocates memory in SPIRAM
    framebuffer = (uint8_t *)heap_caps_malloc(EPD_WIDTH * EPD_HEIGHT / 2, MALLOC_CAP_SPIRAM);
    // Fill Allocated memory with value 0xFF (otherwise previous values at this memory location will be used)
    memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);
}

void setup()
{
    Serial.begin(115200);
    delay(2000);

    framebuffer = (uint8_t *)ps_calloc(sizeof(uint8_t), EPD_WIDTH * EPD_HEIGHT / 2);
    if (!framebuffer)
    {
        Serial.println("alloc memory failed !!!");
        while (1)
            ;
    }
    memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);

    // RGBAimageData = (uint8_t *)ps_calloc(sizeof(uint8_t), EPD_WIDTH * EPD_HEIGHT * 4);
    // if (!RGBAimageData)
    // {
    //     Serial.println("alloc memory failed !!!");
    //     while (1)
    //         ;
    // }
    // memset(RGBAimageData, 0xFF, EPD_WIDTH * EPD_HEIGHT * 4);

    epd_init();

    epd_poweron();
    epd_clear();
    epd_poweroff();

    wifi_init();

    try
    {
        std::map<String, String> params;
        params["imageResponseFormat"] = "4bit";
        params["screenWidth"] = "960";
        params["screenHeight"] = "540";
        HTTPRequestUtil::httpGetReqWParmsFixedBufferResponse(image_url2, params, 10000, framebuffer, displayFrameBufferSize);

        epd_poweron();
        epd_clear();
        delay(1000);
        epd_draw_grayscale_image(epd_full_screen(), framebuffer);
        delay(1000);
        epd_poweroff();
    }
    catch (const std::exception &e)
    {
        Serial.print("Error w/ Request: ");
        Serial.println(e.what());
    }
}

void loop()
{
    // epd_poweron();
    // epd_draw_hline(10, random(10, EPD_HEIGHT), EPD_WIDTH - 20, 0, framebuffer);
    // epd_draw_grayscale_image(epd_full_screen(), framebuffer);
    // delay(1000);

    // epd_draw_rect(10, random(10, EPD_HEIGHT), random(10, 60), random(10, 120), 0, framebuffer);
    // epd_draw_grayscale_image(epd_full_screen(), framebuffer);
    // delay(1000);

    // epd_draw_circle(random(10, EPD_WIDTH), random(10, EPD_HEIGHT), 120, 0, framebuffer);
    // epd_draw_grayscale_image(epd_full_screen(), framebuffer);
    // delay(1000);

    // epd_fill_rect(10, random(10, EPD_HEIGHT), random(10, 60), random(10, 120), 0, framebuffer);
    // epd_draw_grayscale_image(epd_full_screen(), framebuffer);
    // delay(1000);

    // epd_fill_circle(random(10, EPD_WIDTH), random(10, EPD_HEIGHT), random(10, 160), 0, framebuffer);
    // epd_draw_grayscale_image(epd_full_screen(), framebuffer);
    // delay(1000);

    // memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);
    // epd_clear();
    // epd_poweroff();
}