/*
Created By: Alex Zidros
Created On: 05/11/2025
*/

#pragma once

#ifndef SavedNetworks_h
#define SavedNetworks_h

#include <Arduino.h>
#include <Preferences.h>
#include <vector>
#include <string>

class SavedNetworks
{
public:
    struct NetworkCredential
    {
        String ssid;
        String password;
    };

    static bool preferencesBegin();
    static void preferencesEnd();
    static void saveNetwork(const String &ssid, const String &password);
    static bool removeNetwork(const String &ssid);
    static std::vector<NetworkCredential> getSavedNetworks();
    static bool hasNetwork(const String &ssid);
    static void clearAllNetworks();

private:
    static Preferences preferences;
    static bool preferencesInitialized;
    static const char *NAMESPACE;
    static const char *NETWORK_COUNT_KEY;
    static String getNetworkKey(int index);
};

#endif