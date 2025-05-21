/*
Created By: Alex Zidros
Created On: 05/11/2025
*/

#include "WIFIManagerUtil.h"

void WIFIManagerUtil::ConnectToNetwork(String network_ssid, String network_password)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(network_ssid, network_password);

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Connecting to WiFi\n");
    }
}

void WIFIManagerUtil::ScanForNetworks()
{
    int n = WiFi.scanNetworks();
    Serial.println("Scanning for networks...");
    for (int i = 0; i < n; i++)
    {
        Serial.println(WiFi.SSID(i));
    }
}