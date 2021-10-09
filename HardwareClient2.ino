
#include <rpcWiFi.h>
#include <HTTPClient.h>
#include "Custom.h" 
#include <ArduinoJson.h>

#include"seeed_line_chart.h" //include the library
#include"TFT_eSPI.h"
//#include"Free_Fonts.h" //include the header file
TFT_eSPI tft;

// for LCD line chart
#define max_size 50 //maximum size of data
doubles data; //Initilising a doubles type to store data
TFT_eSprite spr = TFT_eSprite(&tft);  // Sprite 

String getValue(String extras) {
  if((WiFi.status() == WL_CONNECTED)) {
        String payload = "Error";
        HTTPClient http;
        String url = "http://" + myIP + ":" port + "/system/" + getHardware + extras;
        http.begin(url); //HTTP
        Serial.print("[HTTP] GET...\n");
        Serial.print(url);
        // start connection and send HTTP header       
        int httpCode = http.GET();
        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);
            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                payload = http.getString();
                Serial.println(payload);
                return payload;
            }
        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
            failed = true;
            return "Error";
        }
        http.end();
    } else {
      return "Nothing";
    }
}

void drawing() {
  tft.setTextColor(TFT_WHITE);
  if (guiSection == 0) {
    maxDown = numOfDevices;
    drawPCs();
  } else if (guiSection == 1) {
    maxDown = numOfOptions;
    drawHardwareOptions();
  } else if (guiSection == 2) {   
    drawHardwareSpecs();
    maxDown = retVals;
    if(failed) {
      Serial.println("Failed...going back...");
      guiSection--;
      drawing();
    }
  } else if (guiSection == 3) {
    drawGraph();
  }
}

void drawPCs() {
  tft.fillScreen(TFT_BLACK);
  int drawline = 0;
  int nextline = 10 * textsize;
  int start = ((spot[0] * nextline) / screenHeight) * (screenHeight / nextline);
  for (int i = start; i < start + (screenHeight / nextline); i++) {
    if (i >= numOfDevices) {
      break;
    }
    if(i == spot[0]) {
      tft.drawString("- " + computers[i].key, 0, drawline);
    } else {
      tft.drawString(computers[i].key, 0, drawline);
    }    
    drawline += nextline;
  }
}

void drawHardwareOptions() {
  tft.fillScreen(TFT_BLACK);
  int drawline = 0;
  int nextline = 10 * textsize;
  for (int i = 0; i < numOfOptions; i++) {
    if(i == spot[1]) {
      getHardware = hardwareOptions[i].value;
      removeBase = hardwareOptions[i].baseObj;
      tft.drawString("- " + hardwareOptions[i].key, 0, drawline);
    } else {
      tft.drawString(hardwareOptions[i].key, 0, drawline);
    }    
    drawline += nextline;
  }
}

void drawHardwareSpecs() {
    
     // clear old data
     while(!data.empty()) {
      data.pop();
     }
     failed = false;
   
    String payload = getValue("");
    StaticJsonDocument<800> doc;
    DeserializationError error = deserializeJson(doc, payload);
  
    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      failed = true;
      return;
    }
    
    tft.fillScreen(TFT_BLACK);
    int drawline = 0;
    int nextline = 10 * textsize;
    JsonObject json;
    if(removeBase) {
      json = doc[getHardware];
    } else {
      json = doc.as<JsonObject>();
    }
    retVals = json.size();
    Serial.println(retVals);
    int i = 0;
    for (auto kv : json) {
      if(i == spot[2]) {
        tft.drawString("- " + String(kv.key().c_str()) + ":" + kv.value().as<String>(), 0, drawline);
        myOption = kv.key().c_str();
      } else {
        tft.drawString(String(kv.key().c_str()) + ":" + kv.value().as<String>(), 0, drawline);
      }
      //tft.drawString(String(kv.key().c_str()) + ":" + kv.value().as<String>(), 0, drawline);
      drawline += nextline;
      i++;
    }
}

void drawGraph() {    
   tft.fillScreen(TFT_WHITE); 
   tft.setTextColor(TFT_BLACK);
   
   String payload = getValue("/" + myOption);
    StaticJsonDocument<800> doc;
    DeserializationError error = deserializeJson(doc, payload);
  
    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      failed = true;
      return;
    }
    JsonObject json;
    if(removeBase) {
      json = doc[getHardware];
    } else {
      json = doc.as<JsonObject>();
    }
    retVals = json.size();
    
    // should only be one
    float plotval = 0.0;
    for (auto kv : json) {
      plotval = kv.value().as<float>();
    }  
   spr.fillSprite(TFT_WHITE); 
   if (data.size() == max_size) {
        data.pop();//this is used to remove the first read variable
    }
    data.push(plotval); //read variables and store in data
    
  //Settings for the line graph
    auto content = line_chart(20, 10); //(x,y) where the line graph begins
         content
                .height(tft.height() - 80) //header.height() * 1.5) //actual height of the line chart
                .width(tft.width() - content.x() * 2) //actual width of the line chart
                .based_on(0.0) //Starting point of y-axis, must be a float
                .show_circle(false) //drawing a cirle at each point, default is on.
                .value(data) //passing through the data to line graph
                .color(TFT_PURPLE) //Setting the color for the line
                .draw();
                
    spr.pushSprite(0, 0);

    String strval = myOption + ": " + String(plotval);
    tft.drawString(strval,60,200); 
  
}

void setup() {
  tft.begin();
  tft.setRotation(3);
  tft.setTextSize(textsize);
  //tft.fillScreen(TFT_WHITE); //Black background
  spr.createSprite(320,180); //try half screen
  //tft.setFreeFont(FF19); //select font
  //tft.setTextColor(TFT_BLACK);
  //tft.drawString("Light Sensor",10,120);//prints string at (10,10)

  pinMode(WIO_LIGHT, INPUT);  //Wio Terminal internal light sensor
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(500);
    Serial.println("Connecting..");
  }
  Serial.print("Connected to the WiFi network with IP: ");
  Serial.println(WiFi.localIP()); 

  // pin stuff
  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);

  drawPCs();
}
 
void loop() {
  // wait for input
   if (digitalRead(WIO_5S_UP) == LOW) {
    Serial.println("5 Way Up");
    spot[guiSection]--;
    if(spot[guiSection] < 0 ) {
      spot[guiSection] = 0;
      //return;
      goto breakif;
      // DONT MOVE UP AND DOWN ON guiSection 3 OR 4
    } else if (guiSection == 3) {
      goto breakif;
    }   
   }
   else if (digitalRead(WIO_5S_DOWN) == LOW) {
    Serial.println("5 Way Down");
    spot[guiSection]++;
    if(spot[guiSection] >= maxDown) {
      spot[guiSection] = maxDown - 1;
      goto breakif;
      // DONT MOVE UP AND DOWN ON guiSection 3 OR 4
    } else if (guiSection == 3) {
      goto breakif;
    }
   }
   else if (digitalRead(WIO_5S_LEFT) == LOW) {
    Serial.println("5 Way Left");
    refreshCount = 0;
    Serial.println(guiSection);
    guiSection--;
    Serial.println(guiSection);
    if(guiSection < 0) {
      guiSection = 0;
      goto breakif;
    } else if (guiSection == 1) {
      // reset the spot pointer fore the api values
      spot[2] = 0;
    }
   }
   else if (digitalRead(WIO_5S_RIGHT) == LOW) {
    Serial.println("5 Way Right");
    if (guiSection == 0) {
      myIP = String(computers[spot[0]].value);
    }
    guiSection++;
    if(guiSection > 3) {
      guiSection = 3;
      goto breakif;
    }
   }
   else if (digitalRead(WIO_5S_PRESS) == LOW) {
    Serial.println("5 Way Press");
    if (guiSection == 0) {
      myIP = String(computers[spot[0]].value);
    }
    guiSection++;
    if(guiSection > 3) {
      guiSection = 3;
      goto breakif;
    }
   } else {
    // NOTHING PRESSED SO DON'T DRAW
    if(guiSection == 3) {
      if(refreshCount == 10) {
        refreshCount = 0;
        drawing();
      }
      refreshCount++;
    }
    goto breakif;
   }
   
   drawing();

   breakif:
   delay(200);      
}
