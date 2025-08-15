/*
Created By: Alex Zidros
Created On: 05/11/2025
*/

#pragma once

#ifndef WIFIManagerUtil_h
#define WIFIManagerUtil_h

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <stdint.h>
#include <map>
#include <algorithm>
#include "SavedNetworks.h"

#include <stdexcept>
using namespace std;

class WIFIManagerUtil
{
public:
    static void ConnectToNetwork(String network_ssid, String network_password);
    static void ConnectToSavedNetwork(String network_ssid);
    static void ScanForNetworks();
    static void SaveNetwork(String network_ssid, String network_password);
    static void RemoveNetwork(String network_ssid);
    static std::vector<SavedNetworks::NetworkCredential> GetSavedNetworks();
    static bool HasSavedNetwork(String network_ssid);
    static void ClearAllSavedNetworks();
    static std::vector<String> GetAvailableNetworks();
    static bool IsNetworkInRange(const String &ssid);

private:
    static bool isConnected;
    static std::vector<String> availableNetworks;
};

#endif
