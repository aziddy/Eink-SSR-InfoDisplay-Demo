/*
Created By: Alex Zidros
Created On: 01/22/2025
*/

#include "HTTPRequestUtil.h"

String HTTPRequestUtil::makeHttpGetRequest(String url, int maxPayloadSize, int maxTimeOut)
{

    // CHECK IF WIFI IS CONNECTED
    checkIfWifiIsConnected();

    // CustomSerial::println("Start Making Web Request");

    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;

    http.begin(client, url);     // Your Domain name with URL path or IP address with path
    http.setTimeout(maxTimeOut); // Set timeout (in milliseconds)

    int httpResponseCode = http.GET();

    String payload = "{}";

    checkPayloadSize(http, maxPayloadSize, PayloadSizeCheckType::MAX_PAYLOAD_SIZE);
    payload = http.getString();
    checkHttpResponseCode(http, httpResponseCode);

    http.end();
    // CustomSerial::println(payload);
    return payload;
}

String HTTPRequestUtil::makeHttpGetRequest(String url, int maxPayloadSize)
{
    return HTTPRequestUtil::makeHttpGetRequest(url, maxPayloadSize, 30000);
}

void HTTPRequestUtil::makeHttpGetRequestForImageData(String url, int maxPayloadSize, int maxTimeOut, char *buffer)
{
    // CHECK IF WIFI IS CONNECTED
    checkIfWifiIsConnected();

    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;

    http.begin(client, url);     // Your Domain name with URL path or IP address with path
    http.setTimeout(maxTimeOut); // Set timeout (in milliseconds)

    int httpResponseCode = http.GET();

    checkPayloadSize(http, maxPayloadSize, PayloadSizeCheckType::MAX_PAYLOAD_SIZE);

    // Get the response stream
    WiFiClient *stream = http.getStreamPtr();
    int bytesRead = 0;

    // Read data while available
    while (stream->available())
    {
        buffer[bytesRead] = stream->read();
        bytesRead++;
    }

    // Check response code after reading the data
    checkHttpResponseCode(http, httpResponseCode);

    http.end();
}

uint8_t *HTTPRequestUtil::httpGetRequestDynamicBuffer(String url, int maxTimeOut)
{
    checkIfWifiIsConnected();

    WiFiClient *client = checkIfHttps(url);

    HTTPClient http;
    http.begin(*client, url);    // Your Domain name with URL path or IP address with path
    http.setTimeout(maxTimeOut); // Set timeout (in milliseconds)

    int httpResponseCode = http.GET();

    checkHttpResponseCode(http, httpResponseCode);

    int contentLength = http.getSize(); // Get the size of the response

    Serial.print("Content Length: ");
    Serial.println(contentLength);

    uint8_t *buffer = (uint8_t *)ps_calloc(sizeof(uint8_t), contentLength);

    // Get the response stream
    WiFiClient *stream = http.getStreamPtr();
    int bytesRead = 0;

    // Read data while available
    while (stream->available())
    {
        buffer[bytesRead] = stream->read();
        bytesRead++;
    }

    Serial.print("Bytes Read: ");
    Serial.println(bytesRead);

    http.end();
    delete client; // Clean up the client, because it's a pointer

    return buffer;
}

void HTTPRequestUtil::checkIfWifiIsConnected()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        throw runtime_error("WiFi is not connected");
    }
}

void HTTPRequestUtil::checkPayloadSize(HTTPClient &http, int maxPayloadSize, enum PayloadSizeCheckType checkType)
{
    int contentLength = http.getSize(); // Get the size of the response

    if (checkType == PayloadSizeCheckType::MAX_PAYLOAD_SIZE && contentLength > maxPayloadSize)
    {
        // CustomSerial::println("Response size too large, aborting.");
        String errorMessage = "Response size too large - " + String(contentLength) + " Bytes\n\n" + "Current Limit: " + String(maxPayloadSize) + " Bytes";

        http.end();
        throw runtime_error(errorMessage.c_str());
    }
    else if (checkType == PayloadSizeCheckType::EXPECTED_PAYLOAD_SIZE && contentLength != maxPayloadSize)
    {
        throw runtime_error("Response size does not match expected size");
    }
}

void HTTPRequestUtil::checkHttpResponseCode(HTTPClient &http, int httpResponseCode)
{
    if (httpResponseCode != 200)
    {
        http.end();
        throw runtime_error("Error HTTP Resp Status Not 200");
    }
}

WiFiClient *HTTPRequestUtil::checkIfHttps(String url)
{
    WiFiClient *client;

    if (url.startsWith("https://"))
    {
        WiFiClientSecure *secureClient = new WiFiClientSecure();
        secureClient->setInsecure();
        client = secureClient;
    }
    else
    {
        client = new WiFiClient();
    }

    return client;
}

// ... existing code ...

String HTTPRequestUtil::buildUrlWithParams(const String &baseUrl, const std::map<String, String> &params)
{
    if (params.empty())
    {
        return baseUrl;
    }

    String finalUrl = baseUrl;
    bool isFirstParam = true;

    for (const auto &param : params)
    {
        if (isFirstParam)
        {
            finalUrl += "?";
            isFirstParam = false;
        }
        else
        {
            finalUrl += "&";
        }

        // URL encode the key and value
        String encodedKey = param.first;
        String encodedValue = param.second;

        // Basic URL encoding for special characters
        encodedKey.replace(" ", "%20");
        encodedKey.replace("+", "%2B");
        encodedValue.replace(" ", "%20");
        encodedValue.replace("+", "%2B");

        finalUrl += encodedKey + "=" + encodedValue;
    }

    return finalUrl;
}

void HTTPRequestUtil::httpGetReqWParmsFixedBufferResponse(String url, const std::map<String, String> &params, int maxTimeOut, uint8_t *buffer, int expectedPayloadSize)
{
    String finalUrl = buildUrlWithParams(url, params);
    return httpGetReqFixedBufferResponse(finalUrl, maxTimeOut, buffer, expectedPayloadSize);
}

void HTTPRequestUtil::httpGetReqFixedBufferResponse(String url, int maxTimeOut, uint8_t *buffer, int expectedPayloadSize)
{
    checkIfWifiIsConnected();

    WiFiClient *client = checkIfHttps(url);
    HTTPClient http;

    http.begin(*client, url);
    http.setTimeout(maxTimeOut);

    int httpResponseCode = http.GET();
    checkHttpResponseCode(http, httpResponseCode);
    checkPayloadSize(http, expectedPayloadSize, PayloadSizeCheckType::EXPECTED_PAYLOAD_SIZE);

    // Get the response stream
    WiFiClient *stream = http.getStreamPtr();
    int bytesRead = 0;

    // Read data while available
    while (stream->available())
    {
        buffer[bytesRead] = stream->read();
        bytesRead++;
    }

    http.end();
    delete client;
}
