#include <YoutubeApi.h>

/*******************************************************************
    An example of bot that echos back any messages received
 *                                                                 *
    written by Giacarlo Bacchio (Gianbacchio on Github)
    adapted by Brian Lough
    OLED code added by TheMiguelBi
 *******************************************************************/

// ESP8266
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// OLED
#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

// OLED: SCL = D1, SDA = D2
U8G2_SH1106_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display

//------- Replace the following! ------
char ssid[] = "xxxx";       // your network SSID (name)
char password[] = "yyyy";  // your network key
#define API_KEY "yourApiToken"  // your google apps API Token
#define CHANNEL_ID "YourChannelId" // makes up the url of channel
IPAddress ip(0, 0, 0, 0);          // Your IP Address
IPAddress gateway(0, 0, 0, 0);       // Your GateWay Address
IPAddress subnet(255, 255, 255, 0);   // Your Subnet Address

WiFiClientSecure client;
YoutubeApi api(API_KEY, client);

int api_mtbs = 60000; // mean time between api requests (60s)
long api_lasttime;   // last time api request has been done

long subs = 0;

int activeScreen = 1; // The currently displayed OLED screen

int subscriberCount;
int viewCount;
int commentCount;
int videoCount;

//
// =======================================================================================================
// MAIN ARDUINO SETUP (1x during startup)
// =======================================================================================================
//

void setup() {

  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  WiFi.config(ip, gateway, subnet);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());

  // Display setup
  Wire.setClock(400); // 400kHz I2C clock
  u8g2.begin();
  u8g2.setFontRefHeightExtendedText();
  u8g2.setFontPosTop();

  // First get Data
  getApiData();

}

//
// =======================================================================================================
// GET YOUTUBE API 3 DATA
// =======================================================================================================
//

void getApiData() {
  if (api.getChannelStatistics(CHANNEL_ID)) {
    subscriberCount = api.channelStats.subscriberCount;
    viewCount = api.channelStats.viewCount;
    commentCount = api.channelStats.commentCount;
    videoCount = api.channelStats.videoCount;
  }
}

//
// =======================================================================================================
// PRINT SERIAL DATA
// =======================================================================================================
//

void printData() {
  if (millis() > api_lasttime + api_mtbs)  {

    getApiData();

    Serial.println("---------Stats---------");
    Serial.print("Subscriber Count: ");
    Serial.println(subscriberCount);
    Serial.print("View Count: ");
    Serial.println(viewCount);
    Serial.print("Comment Count: ");
    Serial.println(commentCount);
    Serial.print("Video Count: ");
    Serial.println(videoCount);
    Serial.println("------------------------");

    api_lasttime = millis();
  }
}

//
// =======================================================================================================
// DRAW DISPLAY
// =======================================================================================================
//

void drawDisplay() {

  u8g2.firstPage();  // clear screen
  do {
    switch (activeScreen) {
      case 1: // Screen # 1 -----------------------------------

        oledHeader();

        // Subscribers
        u8g2.setFont(u8g2_font_courB14_tf); // u8g_font_9x15
        u8g2.setCursor(3, 20);
        u8g2.print("Subscribers:");
        u8g2.setFont(u8g_font_helvR24); // helvR24
        u8g2.setCursor(3, 35);
        u8g2.print(subscriberCount);
        break;

        case 2: // Screen # 2 -----------------------------------

        oledHeader();

        // Views
        u8g2.setFont(u8g2_font_courB14_tf); // u8g_font_9x15
        u8g2.setCursor(3, 20);
        u8g2.print("Views:");
        u8g2.setFont(u8g_font_helvR24); // helvR24
        u8g2.setCursor(3, 35);
        u8g2.print(viewCount);
        break;

        case 3: // Screen # 3 -----------------------------------

        oledHeader();

        // Views
        u8g2.setFont(u8g2_font_courB14_tf); // u8g_font_9x15
        u8g2.setCursor(3, 20);
        u8g2.print("Comments:");
        u8g2.setFont(u8g_font_helvR24); // helvR24
        u8g2.setCursor(3, 35);
        u8g2.print(commentCount);
        break;

        case 4: // Screen # 4 -----------------------------------

        oledHeader();

        // Videos
        u8g2.setFont(u8g2_font_courB14_tf); // u8g_font_9x15
        u8g2.setCursor(3, 20);
        u8g2.print("Videos:");
        u8g2.setFont(u8g_font_helvR24); // helvR24
        u8g2.setCursor(3, 35);
        u8g2.print(videoCount);
        break;
    }
  } while ( u8g2.nextPage() ); // show display queue
}

void oledHeader() {
  // Header
  u8g2.setFont(u8g_font_6x13); // u8g_font_9x15B
  u8g2.drawStr(5, -2, "The XYZ"); //Your YouTube Channel Name
  // Dividing Line
  u8g2.drawLine(0, 13, 128, 13);
}

//
// =======================================================================================================
// SWICH OLED PAGE
// =======================================================================================================
//

void switchPage() {
  static unsigned long lastSwitch;
  if (millis() - lastSwitch >= 2000) { // Every 2s
    lastSwitch = millis();
    
    // increase the page number
    activeScreen++;
    if ( activeScreen >= 5 )
      activeScreen = 1;
  }
}

//
// =======================================================================================================
// MAIN LOOP
// =======================================================================================================
//

void loop() {

  printData();

  drawDisplay();

  switchPage();

}
