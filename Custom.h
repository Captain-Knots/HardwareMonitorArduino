typedef struct {
  String key;
  String value;
} flist;

typedef struct {
  String key;
  String value;
  bool baseObj;
} apiOptions;

// globals
const String port = "3000"; // make sure this matches the port you use in your NodeJS API
char ssid[] = "";        // your network SSID (name)
char password[] = "";    // your network password (use for WPA, or use as key for WEP)
const int textsize = 2;
const int numOfDevices = 5; // ensure this matches the number of devices
flist computers[numOfDevices] = { // modify this list to match the devices on your lan and update numOfDevices variable
  {"Server","10.0.0.73"},
  {"MacBook Air","10.0.0.145"},
  {"Main PC","10.0.0.15"},
  {"Desk PC","10.0.0.189"},
  {"Pi4","10.0.0.14"},
};
int spot[4] = {0,0,0,0};
int guiSection = 0;
String myIP = "";
String myOption = "";
int maxDown = numOfDevices;
int refreshCount = 0;
int screenHeight = 240;
int retVals;
String getHardware = "hello";
bool removeBase = true;
bool failed = false; 
const int numOfOptions = 6; // ensure this matches number of options
apiOptions hardwareOptions[numOfOptions] = {
  {"CPU","cpu",true},
  {"CPU Temp","cpuTemperature",true},
  {"CPU Load","currentLoad",true},
  {"Bios","bios",true},
  {"RAM","mem",true},
  {"Network","networkStats",false}
};
