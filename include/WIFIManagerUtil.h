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

#include <stdexcept>
using namespace std;

class WIFIManagerUtil
{
public:
    static void ConnectToNetwork(String network_ssid, String network_password);
    static void ScanForNetworks();
    static void GetSavedNetworks();

private:
};

#endif
