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
#include "WIFIManagerUtil.h"
#include "config.h"


void wifi_init()
{

    // Retrieve saved networks
    auto savedNetworks = WIFIManagerUtil::GetSavedNetworks();
    Serial.println("Saved networks: " + String(savedNetworks.size()));
    for (const auto &network : savedNetworks)
    {
        Serial.println("Saved network: " + network.ssid);
    }
    bool connected = false;

    // First scan for available networks
    Serial.println("Scanning for available networks...");
    auto availableNetworks = WIFIManagerUtil::GetAvailableNetworks();

    if (availableNetworks.empty())
    {
        Serial.println("No networks found in range");
        return;
    }

    // Try to connect to saved networks that are in range
    for (const auto &network : savedNetworks)
    {
        Serial.println("saved network: " + network.ssid);
        if (WIFIManagerUtil::IsNetworkInRange(network.ssid))
        {
            Serial.print("Found saved network in range: ");
            Serial.println(network.ssid);

            try
            {
                Serial.println("Connecting to saved network: " + network.ssid + " pw:" + network.password);
                WIFIManagerUtil::ConnectToNetwork(network.ssid, network.password);
                connected = true;
                break;
            }
            catch (const std::exception &e)
            {
                Serial.print("Failed to connect to ");
                Serial.print(network.ssid);
                Serial.print(": ");
                Serial.println(e.what());
            }
        }
        else
        {
            Serial.print("Saved network not in range: ");
            Serial.println(network.ssid);
        }
    }

    // WIFIManagerUtil::ConnectToNetwork(WIFI_SSID2, WIFI_PASSWORD2);
    // connected = true;

    if (!connected)
    {
        Serial.println("No saved networks available or failed to connect to any saved network");
    }
}

// TODO: Remove this when we have server interface to add / remove networks
void inject_saved_network(String ssid, String password)
{
    WIFIManagerUtil::SaveNetwork(ssid, password);
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

    epd_init();

    epd_poweron();
    epd_clear();
    epd_poweroff();

    /*
    TODO: Remove this when we have server interface to add / remove networks
    TODO: Don't commit important credentials to the repo
    */

    // SavedNetworks::begin();

    inject_saved_network(WIFI_SSID2, WIFI_PASSWORD2);

    wifi_init();

    try
    {
        std::map<String, String> params;
        params["imageResponseFormat"] = "4bit";
        params["screenWidth"] = "960";
        params["screenHeight"] = "540";
        HTTPRequestUtil::httpGetReqWParmsFixedBufferResponse(API_ENDPOINT, params, 20000, framebuffer, displayFrameBufferSize);

        Serial.println("Drawing image");
        epd_poweron();
        epd_clear();
        delay(1000);
        epd_full_screen();
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

    // delay(5000);
    // int32_t i = 0;

    // Rect_t area = epd_full_screen();
    // epd_poweron();
    // delay(10);
    // epd_clear();
    // for (i = 0; i < 20; i++)
    // {
    //     epd_push_pixels(area, 50, 0);
    //     delay(500);
    // }
    // epd_clear();
    // for (i = 0; i < 40; i++)
    // {
    //     epd_push_pixels(area, 50, 1);
    //     delay(500);
    // }
    // epd_clear();
    // epd_poweroff_all();

    // delay(5000);
}