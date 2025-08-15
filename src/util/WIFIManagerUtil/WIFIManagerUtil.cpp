/*
Created By: Alex Zidros
Created On: 05/11/2025
*/

#include "WIFIManagerUtil.h"

bool WIFIManagerUtil::isConnected = false;
std::vector<String> WIFIManagerUtil::availableNetworks;

void WIFIManagerUtil::ConnectToNetwork(String network_ssid, String network_password)
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(network_ssid, network_password);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20)
    {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED)
    {
        isConnected = true;
        Serial.println("Connected to WiFi");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    }
    else
    {
        Serial.println("Failed to connect to WiFi");
    }
}

void WIFIManagerUtil::ConnectToSavedNetwork(String network_ssid)
{
    if (!HasSavedNetwork(network_ssid))
    {
        throw runtime_error("Network not found in saved networks");
    }

    auto networks = GetSavedNetworks();
    for (const auto &network : networks)
    {
        if (network.ssid == network_ssid)
        {
            ConnectToNetwork(network.ssid, network.password);
            return;
        }
    }
}

std::vector<String> WIFIManagerUtil::GetAvailableNetworks()
{
    std::vector<String> availableNetworks;
    int n = WiFi.scanNetworks();

    // Wait for scan to complete and add extra delay for better network detection
    delay(5000); // Wait 2 seconds for scan completion

    Serial.println("Number of scanned networks: " + String(n));

    for (int i = 0; i < n; i++)
    {
        availableNetworks.push_back(WiFi.SSID(i));
        Serial.println("Network " + String(i) + ": " + WiFi.SSID(i));
    }

    return availableNetworks;
}

bool WIFIManagerUtil::IsNetworkInRange(const String &ssid)
{
    if (availableNetworks.empty())
    {
        availableNetworks = GetAvailableNetworks();
    }
    return std::find(availableNetworks.begin(), availableNetworks.end(), ssid) != availableNetworks.end();
}

void WIFIManagerUtil::ScanForNetworks()
{
    int n = WiFi.scanNetworks();
    Serial.println("Scanning for networks...");
    for (int i = 0; i < n; i++)
    {
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(WiFi.SSID(i));
        Serial.print(" (");
        Serial.print(WiFi.RSSI(i));
        Serial.print(")");
        Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
        delay(10);
    }
}

void WIFIManagerUtil::SaveNetwork(String network_ssid, String network_password)
{
    SavedNetworks::saveNetwork(network_ssid, network_password);
}

void WIFIManagerUtil::RemoveNetwork(String network_ssid)
{
    SavedNetworks::removeNetwork(network_ssid);
}

std::vector<SavedNetworks::NetworkCredential> WIFIManagerUtil::GetSavedNetworks()
{
    return SavedNetworks::getSavedNetworks();
}

bool WIFIManagerUtil::HasSavedNetwork(String network_ssid)
{
    return SavedNetworks::hasNetwork(network_ssid);
}

void WIFIManagerUtil::ClearAllSavedNetworks()
{
    SavedNetworks::clearAllNetworks();
}