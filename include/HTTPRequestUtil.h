/*
Created By: Alex Zidros
Created On: 01/22/2025
*/

#pragma once

#ifndef HTTPRequestUtil_h
#define HTTPRequestUtil_h

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <stdint.h>
#include <map>

#include <stdexcept>
using namespace std;

class HTTPRequestUtil
{
public:
    static String makeHttpGetRequest(String url, int maxPayloadSize);
    static String makeHttpGetRequest(String url, int maxPayloadSize, int maxTimeOut);

    static void makeHttpGetRequestForImageData(String url, int maxPayloadSize, int maxTimeOut, char *buffer);

    static uint8_t *httpGetRequestDynamicBuffer(String url, int maxTimeOut);

    static void httpGetReqFixedBufferResponse(String url, int maxTimeOut, uint8_t *buffer, int expectedPayloadSize);

    static void httpGetReqWParmsFixedBufferResponse(String url, const std::map<String, String> &params, int maxTimeOut, uint8_t *buffer, int expectedPayloadSize);

private:
    enum PayloadSizeCheckType
    {
        EXPECTED_PAYLOAD_SIZE,
        MAX_PAYLOAD_SIZE
    };
    static void checkIfWifiIsConnected();

    static WiFiClient *checkIfHttps(String url);

    static void checkPayloadSize(HTTPClient &http, int maxPayloadSize, enum PayloadSizeCheckType checkType);
    static void checkHttpResponseCode(HTTPClient &http, int httpResponseCode);
    static String buildUrlWithParams(const String &baseUrl, const std::map<String, String> &params);
};

#endif