/*
Created By: Alex Zidros
Created On: 05/11/2025
*/

#include "SavedNetworks.h"

Preferences SavedNetworks::preferences;
bool SavedNetworks::preferencesInitialized = false;
const char *SavedNetworks::NAMESPACE = "SavedNetworks";
const char *SavedNetworks::NETWORK_COUNT_KEY = "network_count";

bool SavedNetworks::preferencesBegin()
{
    if (preferencesInitialized)
    {
        return true;
    }

    if (!preferences.begin(NAMESPACE, false))
    {
        Serial.println("Failed to open SavedNetworks namespace");
        return false;
    }

    // Initialize network count if it doesn't exist
    if (!preferences.isKey(NETWORK_COUNT_KEY))
    {
        preferences.putInt(NETWORK_COUNT_KEY, 0);
    }

    preferencesInitialized = true;
    return true;
}

void SavedNetworks::preferencesEnd()
{
    preferences.end();
}

String SavedNetworks::getNetworkKey(int index)
{
    if (index < 0)
    {
        throw std::invalid_argument("Network index cannot be negative");
    }
    return "network_" + String(index);
}

void SavedNetworks::saveNetwork(const String &ssid, const String &password)
{
    preferencesBegin();
    if (ssid.length() == 0)
    {
        throw std::invalid_argument("SSID cannot be empty");
    }

    // Check if network already exists
    if (hasNetwork(ssid))
    {
        // Update existing network
        int count = preferences.getInt(NETWORK_COUNT_KEY, 0);
        for (int i = 0; i < count; i++)
        {
            Serial.println("Current Network Count Key: " + String(i));
            String key = getNetworkKey(i);
            String storedSsid = preferences.getString((key + "_ssid").c_str(), "");
            if (storedSsid == ssid)
            {
                Serial.println("Updating Network Password");
                Serial.println("key: " + key);
                Serial.println("New Password: " + password);
                preferences.putString((key + "_pw").c_str(), password);
                return;
            }
        }
    }
    else
    {
        // Add new network
        int count = preferences.getInt(NETWORK_COUNT_KEY, 0);
        Serial.println("Saved Network count: " + String(count));
        Serial.println("Saving as New Network");
        String key = getNetworkKey(count);
        preferences.putString((key + "_ssid").c_str(), ssid);
        preferences.putString((key + "_pw").c_str(), password);
        preferences.putInt(NETWORK_COUNT_KEY, count + 1);
    }
    // preferencesEnd();
}

bool SavedNetworks::removeNetwork(const String &ssid)
{
    if (ssid.length() == 0)
    {
        throw std::invalid_argument("SSID cannot be empty");
    }

    int count = preferences.getInt(NETWORK_COUNT_KEY, 0);
    bool found = false;

    // Find the network and remove it
    for (int i = 0; i < count; i++)
    {
        String key = getNetworkKey(i);
        String storedSsid = preferences.getString((key + "_ssid").c_str(), "");

        if (storedSsid == ssid)
        {
            found = true;
            // Remove the network
            preferences.remove((key + "_ssid").c_str());
            preferences.remove((key + "_pw").c_str());

            // Shift remaining networks down
            for (int j = i; j < count - 1; j++)
            {
                String currentKey = getNetworkKey(j);
                String nextKey = getNetworkKey(j + 1);

                String nextSsid = preferences.getString((nextKey + "_ssid").c_str(), "");
                String nextPassword = preferences.getString((nextKey + "_pw").c_str(), "");

                preferences.putString((currentKey + "_ssid").c_str(), nextSsid);
                preferences.putString((currentKey + "_pw").c_str(), nextPassword);
            }

            // Remove the last network entry
            String lastKey = getNetworkKey(count - 1);
            preferences.remove((lastKey + "_ssid").c_str());
            preferences.remove((lastKey + "_pw").c_str());

            // Update count
            preferences.putInt(NETWORK_COUNT_KEY, count - 1);
            break;
        }
    }

    return found;
}

std::vector<SavedNetworks::NetworkCredential> SavedNetworks::getSavedNetworks()
{
    preferencesBegin();
    std::vector<NetworkCredential> networks;
    int count = preferences.getInt(NETWORK_COUNT_KEY, 0);

    for (int i = 0; i < count; i++)
    {
        String key = getNetworkKey(i);
        NetworkCredential cred;
        cred.ssid = preferences.getString((key + "_ssid").c_str(), "");
        cred.password = preferences.getString((key + "_pw").c_str(), "");
        Serial.println("RETREIVED Network Key: " + key);
        Serial.println("Network SSID: " + cred.ssid);
        Serial.println("Network Password: " + cred.password);
        // Only add networks that have valid SSIDs
        if (cred.ssid.length() > 0)
        {
            networks.push_back(cred);
        }
    }

    // preferencesEnd();
    return networks;
}

bool SavedNetworks::hasNetwork(const String &ssid)
{
    if (ssid.length() == 0)
    {
        return false;
    }

    int count = preferences.getInt(NETWORK_COUNT_KEY, 0);

    for (int i = 0; i < count; i++)
    {
        String key = getNetworkKey(i);
        String storedSsid = preferences.getString((key + "_ssid").c_str(), "");
        if (storedSsid == ssid)
        {
            return true;
        }
    }

    return false;
}

void SavedNetworks::clearAllNetworks()
{
    preferences.clear();
    preferences.putInt(NETWORK_COUNT_KEY, 0);
}