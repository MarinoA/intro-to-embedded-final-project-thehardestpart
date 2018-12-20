/* Server
*  We write the server first because we need its IP address
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

const char* ssid = "The_Hardest_Part";  // ssid of access point
const char* password = "";              // password of access point
int port = 1174;                        // port number
WiFiServer server(port);                // server configuration

int ledPin = 2; // GPIO2 of Server ESP8266

void setup() 
{
    // UART configuration
    Serial.begin(9600);
    delay(10);

    pinMode(ledPin, OUTPUT);   // set GPIO 2 as an output for debugging

    WiFi.mode(WIFI_AP);  // set mode to access point (server)

    // Start WiFi network
    WiFi.softAP(ssid, password);

    // Tell the server to begin listening for incoming connections
    server.begin();
}  

void loop() 
{
    // Check if a client has connected
    WiFiClient client = server.available();
    if (!client) // if not available, return
    {
        return;
    }

    // Wait until the client sends some data
    digitalWrite(ledPin, LOW); // turn LED off to show connection
    while(!client.available() & !Serial.available())
    {
        // Wait to receive data from client or UART
        delay(500);
        // Return to start if connection breaks
        if(!client.connected() && !client.available())
            return;
    }

    if(Serial.available())
    {
        // Forward received UART character over WiFi
        switch(Serial.read())
        {
            case 1:
                client.println("/1/");
                break;
            case 2:
                client.println("/2/");
                break;
            case 3:
                client.println("/3/");
                break;
            case 4:
                client.println("/4/");
                break;
            case 5:
                client.println("/5/");
                break;
            case 6:
                client.println("/6/");
                break;
            case 7:
                client.println("/7/");
                break;
            case 8:
                client.println("/8/");
                break;
            case 9:
                client.println("/9/");
                break;        
            default:
                break;
        }
    }
    if(client.available())
    {
        // Read the request
        String request = client.readStringUntil('\r');

        // Forward received WiFi character over UART
        if (request.indexOf("/1/") != -1) 
        {
            Serial.write(1);
        } 
        if (request.indexOf("/2/") != -1)
        {
            Serial.write(2);
        }
        if (request.indexOf("/3/") != -1)
        {
            Serial.write(3);
        }
        if (request.indexOf("/4/") != -1)
        {
            Serial.write(4);
        }
        if (request.indexOf("/5/") != -1)
        {
            Serial.write(5);
        }
        if (request.indexOf("/6/") != -1)
        {
            Serial.write(6);
        }
        if (request.indexOf("/7/") != -1)
        {
            Serial.write(7);
        }
        if (request.indexOf("/8/") != -1)
        {
            Serial.write(8);
        }
        if (request.indexOf("/9/") != -1)
        {
            Serial.write(9);
        }
    }

    client.stop(); // disconnect client

    digitalWrite(ledPin, HIGH); // turn LED on to show disconnection

    delay(1);
}
