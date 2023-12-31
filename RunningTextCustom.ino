#define RUN_DEMO 1
#define DEBUG 0  // Enable or disable (default) debugging output

#if DEBUG
#define PRINT(s, v) \
  { \
    Serial.print(s); \
    Serial.print(v); \
  }  // Print a string followed by a value (decimal)
#define PRINTX(s, v) \
  { \
    Serial.print(s); \
    Serial.print(v, HEX); \
  }  // Print a string followed by a value (hex)
#define PRINTB(s, v) \
  { \
    Serial.print(s); \
    Serial.print(v, BIN); \
  }  // Print a string followed by a value (binary)
#define PRINTC(s, v) \
  { \
    Serial.print(s); \
    Serial.print((char)v); \
  }  // Print a string followed by a value (char)
#define PRINTS(s) \
  { Serial.print(s); }  // Print a string
#define PRINTLN(s) \
  { Serial.println(s); }  // Print a string
#else
#define PRINT(s, v)   // Print a string followed by a value (decimal)
#define PRINTX(s, v)  // Print a string followed by a value (hex)
#define PRINTB(s, v)  // Print a string followed by a value (binary)
#define PRINTC(s, v)  // Print a string followed by a value (char)
#define PRINTS(s)     // Print a string
#define PRINTLN(s)    // Print a string
#endif

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <FS.h>
#include <EEPROM.h>

#include <functional>
#include <map>
#include <vector>

#define DEMO_DELAY 15  // time to show each demo element in seconds
#define UNIT_DELAY 25
#define SCROLL_DELAY (4 * UNIT_DELAY)
#define MIDLINE_DELAY (6 * UNIT_DELAY)
#define SCANNER_DELAY (2 * UNIT_DELAY)
#define RANDOM_DELAY (6 * UNIT_DELAY)
#define FADE_DELAY (8 * UNIT_DELAY)
#define SPECTRUM_DELAY (4 * UNIT_DELAY)
#define HEARTBEAT_DELAY (1 * UNIT_DELAY)
#define HEARTS_DELAY (28 * UNIT_DELAY)
#define EYES_DELAY (20 * UNIT_DELAY)
#define WIPER_DELAY (1 * UNIT_DELAY)
#define ARROWS_DELAY (3 * UNIT_DELAY)
#define ARROWR_DELAY (8 * UNIT_DELAY)
#define INVADER_DELAY (6 * UNIT_DELAY)
#define PACMAN_DELAY (4 * UNIT_DELAY)
#define SINE_DELAY (2 * UNIT_DELAY)

#define CHAR_SPACING 1  // pixels between characters
#define BUF_SIZE 75     // character buffer size

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

#define CLK_PIN D5
#define DATA_PIN D7
#define CS_PIN D0

// MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);                      // SPI hardware interface
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);  // Arbitrary pins

uint32_t prevTimeAnim = 0;  // Used for remembering the millis() value in animations
#if RUN_DEMO
uint32_t prevTimeDemo = 0;      //  Used for remembering the millis() time in demo loop
uint8_t timeDemo = DEMO_DELAY;  // number of seconds left in this demo loop
#endif

ESP8266WebServer server(80);
const char* ssid = "RobotikID";
const char* password = "123456789";
const byte DNS_PORT = 53;
IPAddress apIP(172, 217, 28, 1);
DNSServer dnsServer;

String param_pesan[] = {
  "pesan_pertama",
  "pesan_kedua",
  "pesan_ketiga"
};
String param_efek_in[] = {
  "efek_in_pertama",
  "efek_in_kedua",
  "efek_in_ketiga"
};
String param_efek_out[] = {
  "efek_out_pertama",
  "efek_out_kedua",
  "efek_out_ketiga"
};

String pesan[3], tmp_pesan;
File file;
String nama_file[] = {
  "/text_pertama.txt",
  "/text_kedua.txt",
  "/text_ketiga.txt",
};
FS* fileSystem = &SPIFFS;

const String configHtml = R"""(
<!DOCTYPE html>
<html>
<head>
  <title>Running Text Manager</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  
</head>
<body>
  <div class="topnav">
    <h1 style="text-align: center;">Running Text Manager</h1>
  </div>
  <div class="content" >
      <form action="/config">
        <table style="margin-left: auto; margin-right: auto; text-align: center;">
          <tr>
            <th> Urutan Ke </th>
            <th> Pesan </th>
            <th> Efek Awal</th>
            <th> Efek Akhir </th>
          </tr>
          <tr>
            <td> <label for="pertama">Pertama</label> </td> 
            <td> <input type="text" id ="pertama" name="pesan_pertama" value="{pesan_pertama}" placeholder="Pesan"> </td>
            <td> 
              <select name="efek_in_pertama" >
                <option value="0">Scroll Up</option>
                <option value="1">Scroll Down</option>
                <option value="2">Scroll Left</option>
                <option value="3">Scroll Right</option>
                <option value="4">Opening Cursor</option>
                <option value="5">Closing Cursor</option>
              </select>
            </td>
            <td> 
              <select name="efek_out_pertama" >
                <option value="0">Scroll Up</option>
                <option value="1">Scroll Down</option>
                <option value="2">Scroll Left</option>
                <option value="3">Scroll Right</option>
                <option value="4">Opening Cursor</option>
                <option value="5">Closing Cursor</option>
              </select>
            </td>
          </tr>
          <tr>
            <td> <label for="kedua">Kedua</label> </td>
            <td> <input type="text" id ="kedua" name="pesan_kedua" value="{pesan_kedua}" placeholder="Pesan"></td>
            <td> 
              <select name="efek_in_kedua" >
                <option value="0">Scroll Up</option>
                <option value="1">Scroll Down</option>
                <option value="2">Scroll Left</option>
                <option value="3">Scroll Right</option>
                <option value="4">Opening Cursor</option>
                <option value="5">Closing Cursor</option>
              </select>
            </td>
            <td> 
              <select name="efek_out_kedua" >
                <option value="0">Scroll Up</option>
                <option value="1">Scroll Down</option>
                <option value="2">Scroll Left</option>
                <option value="3">Scroll Right</option>
                <option value="4">Opening Cursor</option>
                <option value="5">Closing Cursor</option>
              </select>
            </td>
          </tr>
          <tr>
            <td> <label for="ketiga">Ketiga</label> </td>
            <td> <input type="text" id ="ketiga" name="pesan_ketiga" value="{pesan_ketiga}" placeholder="Pesan"></td>
            <td> 
              <select name="efek_in_ketiga" >
                <option value="0">Scroll Up</option>
                <option value="1">Scroll Down</option>
                <option value="2">Scroll Left</option>
                <option value="3">Scroll Right</option>
                <option value="4">Opening Cursor</option>
                <option value="5">Closing Cursor</option>
              </select>
            </td>
            <td> 
              <select name="efek_out_ketiga" >
                <option value="0">Scroll Up</option>
                <option value="1">Scroll Down</option>
                <option value="2">Scroll Left</option>
                <option value="3">Scroll Right</option>
                <option value="4">Opening Cursor</option>
                <option value="5">Closing Cursor</option>
              </select>
            </td>
          </tr>
          <tr>
            <td> <input type ="submit" value ="Submit"> </td>
          </tr>
        </table>
      </form>
  </div>
</body>
</html>
)""";

void simpan_pesan(String nama_file, String isi_file);
String baca_pesan(String nama_file);
bool graphicMidline1(bool bInit);
bool graphicMidline2(bool bInit);
bool graphicScanner(bool bInit);
bool graphicRandom(bool bInit);
bool graphicScroller(bool bInit);
bool graphicSpectrum1(bool bInit);
bool graphicSpectrum2(bool bInit);
bool graphicHeartbeat(bool bInit);
bool graphicFade(bool bInit);
bool graphicHearts(bool bInit);
bool graphicEyes(bool bInit);
bool graphicBounceBall(bool bInit);
bool graphicArrowScroll(bool bInit);
bool graphicWiper(bool bInit);
bool graphicInvader(bool bInit);
bool graphicPacman(bool bInit);
bool graphicArrowRotate(bool bInit);
bool graphicSinewave(bool bInit);
void resetMatrix(void);
void runMatrixAnimation(void);
String generateConfigHtml();

void setup() {
  mx.begin();
  prevTimeAnim = millis();
#if RUN_DEMO
  prevTimeDemo = millis();
#endif
  Serial.begin(9600);
  EEPROM.begin(10);
  if (!SPIFFS.begin()) {
    PRINTLN("Gagal Memulai SPIFFS");
    return;
  }

  pesan[0] = baca_pesan(nama_file[0]);
  pesan[1] = baca_pesan(nama_file[1]);
  pesan[2] = baca_pesan(nama_file[2]);

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, password);
  dnsServer.start(DNS_PORT, "*", apIP);
  server.on("/", []() {
    server.send(200, "text/html", generateConfigHtml());
  });
  server.on("/config", []() {
    for (int a = 0; a < 3; a++) {
      if (server.hasArg(param_pesan[a].c_str())) {
        tmp_pesan = server.arg(param_pesan[a].c_str());
        pesan[a] = tmp_pesan;
        simpan_pesan(nama_file[a], tmp_pesan);
        Serial.print("Pesan: ");
        Serial.println(tmp_pesan);
      }
    }
    server.send(200, "text/html", generateConfigHtml());
  });
  server.onNotFound([]() {
    server.send(200, "text/html", generateConfigHtml());
  });

  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();

  runMatrixAnimation();
}

String generateConfigHtml() {
  String html = configHtml;
  html.replace("{pesan_pertama}", pesan[0]);
  html.replace("{pesan_kedua}", pesan[1]);
  html.replace("{pesan_ketiga}", pesan[2]);

  return html;
}

void simpan_pesan(String nama_file, String isi_file) {
  file = SPIFFS.open(nama_file, "w");
  if (file.println(isi_file)) {
    Serial.println("Berhasil Menyimpan Pesan");
  }
  file.close();
}

String baca_pesan(String nama_file) {
  String isi_file;
  file = SPIFFS.open(nama_file, "r");
  if (!file) {
    file.close();
    file = SPIFFS.open(nama_file, "w");
    Serial.println("File Tidak Ditemukan");
    isi_file = "Hai RobotikID";
    if (file.println(isi_file)) {
      Serial.println("Berhasil Membuat File");
    }
    file.close();
  } else {
    Serial.println("File Ditemukan");
    while (file.position() < file.size()) {
      isi_file = file.readStringUntil('\n');
      isi_file.trim();
      Serial.println(isi_file);
    }
    file.close();
  }
  return isi_file;
}


bool scrollText(bool bInit, const char* pmsg)
// Callback function for data that is required for scrolling into the display
{
  static char curMessage[BUF_SIZE];
  static char* p = curMessage;
  static uint8_t state = 0;
  static uint8_t curLen, showLen;
  static uint8_t cBuf[8];
  uint8_t colData;

  // are we initializing?
  if (bInit) {
    PRINTS("\n--- Initializing ScrollText");
    resetMatrix();
    strcpy(curMessage, pmsg);
    state = 0;
    p = curMessage;
    bInit = false;
  }

  // Is it time to scroll the text?
  if (millis() - prevTimeAnim < SCROLL_DELAY)
    return (bInit);

  // scroll the display
  mx.transform(MD_MAX72XX::TSL);  // scroll along
  prevTimeAnim = millis();        // starting point for next time

  // now run the finite state machine to control what we do
  PRINT("\nScroll FSM S:", state);
  switch (state) {
    case 0:  // Load the next character from the font table
      PRINTC("\nLoading ", *p);
      showLen = mx.getChar(*p++, sizeof(cBuf) / sizeof(cBuf[0]), cBuf);
      curLen = 0;
      state = 1;

      // !! deliberately fall through to next state to start displaying

    case 1:  // display the next part of the character
      colData = cBuf[curLen++];
      mx.setColumn(0, colData);
      if (curLen == showLen) {
        showLen = ((*p != '\0') ? CHAR_SPACING : mx.getColumnCount() - 1);
        curLen = 0;
        state = 2;
      }
      break;

    case 2:  // display inter-character spacing (blank column) or scroll off the display
      mx.setColumn(0, 0);
      if (++curLen == showLen) {
        state = 0;
        bInit = (*p == '\0');
      }
      break;

    default:
      state = 0;
  }

  return (bInit);
}

// ========== Graphic routines ===========
//
bool graphicMidline1(bool bInit) {
  // are we initializing?
  if (bInit) {
    PRINTS("\n--- Midline1 init");
    resetMatrix();
    bInit = false;
  } else {
    for (uint8_t j = 0; j < MAX_DEVICES; j++) {
      mx.setRow(j, 3, 0xff);
      mx.setRow(j, 4, 0xff);
    }
  }

  return (bInit);
}

bool graphicMidline2(bool bInit) {
  static uint8_t idx = 0;    // position
  static int8_t idOffs = 1;  // increment direction

  // are we initializing?
  if (bInit) {
    PRINTS("\n--- Midline2 init");
    resetMatrix();
    idx = 0;
    idOffs = 1;
    bInit = false;
  }

  // Is it time to animate?
  if (millis() - prevTimeAnim < MIDLINE_DELAY)
    return (bInit);
  prevTimeAnim = millis();  // starting point for next time

  PRINT("\nML2 R:", idx);
  PRINT(" D:", idOffs);

  // now run the animation
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  // turn off the old lines
  for (uint8_t j = 0; j < MAX_DEVICES; j++) {
    mx.setRow(j, idx, 0x00);
    mx.setRow(j, ROW_SIZE - 1 - idx, 0x00);
  }

  idx += idOffs;
  if ((idx == 0) || (idx == ROW_SIZE - 1))
    idOffs = -idOffs;

  // turn on the new lines
  for (uint8_t j = 0; j < MAX_DEVICES; j++) {
    mx.setRow(j, idx, 0xff);
    mx.setRow(j, ROW_SIZE - 1 - idx, 0xff);
  }

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

  return (bInit);
}

bool graphicScanner(bool bInit) {
  const uint8_t width = 3;   // scanning bar width
  static uint8_t idx = 0;    // position
  static int8_t idOffs = 1;  // increment direction

  // are we initializing?
  if (bInit) {
    PRINTS("\n--- Scanner init");
    resetMatrix();
    idx = 0;
    idOffs = 1;
    bInit = false;
  }

  // Is it time to animate?
  if (millis() - prevTimeAnim < SCANNER_DELAY)
    return (bInit);
  prevTimeAnim = millis();  // starting point for next time

  PRINT("\nS R:", idx);
  PRINT(" D:", idOffs);

  // now run the animation
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  // turn off the old lines
  for (uint8_t i = 0; i < width; i++)
    mx.setColumn(idx + i, 0);

  idx += idOffs;
  if ((idx == 0) || (idx + width == mx.getColumnCount()))
    idOffs = -idOffs;

  // turn on the new lines
  for (uint8_t i = 0; i < width; i++)
    mx.setColumn(idx + i, 0xff);

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

  return (bInit);
}

bool graphicRandom(bool bInit) {
  // are we initializing?
  if (bInit) {
    PRINTS("\n--- Random init");
    resetMatrix();
    bInit = false;
  }

  // Is it time to animate?
  if (millis() - prevTimeAnim < RANDOM_DELAY)
    return (bInit);
  prevTimeAnim = millis();  // starting point for next time

  // now run the animation
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  for (uint8_t i = 0; i < mx.getColumnCount(); i++)
    mx.setColumn(i, (uint8_t)random(255));
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

  return (bInit);
}

bool graphicScroller(bool bInit) {
  const uint8_t width = 3;  // width of the scroll bar
  const uint8_t offset = mx.getColumnCount() / 3;
  static uint8_t idx = 0;  // counter

  // are we initializing?
  if (bInit) {
    PRINTS("\n--- Scroller init");
    resetMatrix();
    idx = 0;
    bInit = false;
  }

  // Is it time to animate?
  if (millis() - prevTimeAnim < SCANNER_DELAY)
    return (bInit);
  prevTimeAnim = millis();  // starting point for next time

  PRINT("\nS I:", idx);

  // now run the animation
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  mx.transform(MD_MAX72XX::TSL);

  mx.setColumn(0, idx >= 0 && idx < width ? 0xff : 0);
  if (++idx == offset) idx = 0;

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

  return (bInit);
}

bool graphicSpectrum1(bool bInit) {
  // are we initializing?
  if (bInit) {
    PRINTS("\n--- Spectrum1 init");
    resetMatrix();
    bInit = false;
  }

  // Is it time to animate?
  if (millis() - prevTimeAnim < SPECTRUM_DELAY)
    return (bInit);
  prevTimeAnim = millis();  // starting point for next time

  // now run the animation
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  for (uint8_t i = 0; i < MAX_DEVICES; i++) {
    uint8_t r = random(ROW_SIZE);
    uint8_t cd = 0;

    for (uint8_t j = 0; j < r; j++)
      cd |= 1 << j;
    for (uint8_t j = 1; j < COL_SIZE - 1; j++)
      mx.setColumn(i, j, ~cd);
  }
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

  return (bInit);
}

bool graphicSpectrum2(bool bInit) {
  // are we initializing?
  if (bInit) {
    PRINTS("\n--- Spectrum2init");
    resetMatrix();
    bInit = false;
  }

  // Is it time to animate?
  if (millis() - prevTimeAnim < SPECTRUM_DELAY)
    return (bInit);
  prevTimeAnim = millis();  // starting point for next time

  // now run the animation
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  for (uint8_t i = 0; i < mx.getColumnCount(); i++) {
    uint8_t r = random(ROW_SIZE);
    uint8_t cd = 0;

    for (uint8_t j = 0; j < r; j++)
      cd |= 1 << j;

    mx.setColumn(i, ~cd);
  }
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

  return (bInit);
}

bool graphicHeartbeat(bool bInit) {
#define BASELINE_ROW 4

  static uint8_t state;
  static uint8_t r, c;
  static bool bPoint;

  // are we initializing?
  if (bInit) {
    PRINTS("\n--- Heartbeat init");
    resetMatrix();
    state = 0;
    r = BASELINE_ROW;
    c = mx.getColumnCount() - 1;
    bPoint = true;
    bInit = false;
  }

  // Is it time to animate?
  if (millis() - prevTimeAnim < HEARTBEAT_DELAY)
    return (bInit);
  prevTimeAnim = millis();  // starting point for next time

  // now run the animation
  PRINT("\nHB S:", state);
  PRINT(" R: ", r);
  PRINT(" C: ", c);
  PRINT(" P: ", bPoint);
  mx.setPoint(r, c, bPoint);

  switch (state) {
    case 0:  // straight line from the right side
      if (c == mx.getColumnCount() / 2 + COL_SIZE)
        state = 1;
      c--;
      break;

    case 1:  // first stroke
      if (r != 0) {
        r--;
        c--;
      } else state = 2;
      break;

    case 2:  // down stroke
      if (r != ROW_SIZE - 1) {
        r++;
        c--;
      } else state = 3;
      break;

    case 3:  // second up stroke
      if (r != BASELINE_ROW) {
        r--;
        c--;
      } else state = 4;
      break;

    case 4:  // straight line to the left
      if (c == 0) {
        c = mx.getColumnCount() - 1;
        bPoint = !bPoint;
        state = 0;
      } else c--;
      break;

    default:
      state = 0;
  }

  return (bInit);
}

bool graphicFade(bool bInit) {
  static uint8_t intensity = 0;
  static int8_t iOffs = 1;

  // are we initializing?
  if (bInit) {
    PRINTS("\n--- Fade init");
    resetMatrix();
    mx.control(MD_MAX72XX::INTENSITY, intensity);

    // Set all LEDS on
    mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
    for (uint8_t i = 0; i < mx.getColumnCount(); i++)
      mx.setColumn(i, 0xff);
    mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

    bInit = false;
  }

  // Is it time to animate?
  if (millis() - prevTimeAnim < FADE_DELAY)
    return (bInit);
  prevTimeAnim = millis();  // starting point for next time

  // now run the animation
  intensity += iOffs;
  PRINT("\nF I:", intensity);
  PRINT(" D:", iOffs);
  if ((intensity == 0) || (intensity == MAX_INTENSITY))
    iOffs = -iOffs;
  mx.control(MD_MAX72XX::INTENSITY, intensity);

  return (bInit);
}

bool graphicHearts(bool bInit) {
#define NUM_HEARTS ((MAX_DEVICES / 2) + 1)
  const uint8_t heartFull[] = { 0x1c, 0x3e, 0x7e, 0xfc };
  const uint8_t heartEmpty[] = { 0x1c, 0x22, 0x42, 0x84 };
  const uint8_t offset = mx.getColumnCount() / (NUM_HEARTS + 1);
  const uint8_t dataSize = (sizeof(heartFull) / sizeof(heartFull[0]));

  static bool bEmpty;

  // are we initializing?
  if (bInit) {
    PRINTS("\n--- Hearts init");
    resetMatrix();
    bEmpty = true;
    bInit = false;
  }

  // Is it time to animate?
  if (millis() - prevTimeAnim < HEARTS_DELAY)
    return (bInit);
  prevTimeAnim = millis();  // starting point for next time

  // now run the animation
  PRINT("\nH E:", bEmpty);

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  for (uint8_t h = 1; h <= NUM_HEARTS; h++) {
    for (uint8_t i = 0; i < dataSize; i++) {
      mx.setColumn((h * offset) - dataSize + i, bEmpty ? heartEmpty[i] : heartFull[i]);
      mx.setColumn((h * offset) + dataSize - i - 1, bEmpty ? heartEmpty[i] : heartFull[i]);
    }
  }
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  bEmpty = !bEmpty;

  return (bInit);
}

bool graphicEyes(bool bInit) {
#define NUM_EYES 2
  const uint8_t eyeOpen[] = { 0x18, 0x3c, 0x66, 0x66 };
  const uint8_t eyeClose[] = { 0x18, 0x3c, 0x3c, 0x3c };
  const uint8_t offset = mx.getColumnCount() / (NUM_EYES + 1);
  const uint8_t dataSize = (sizeof(eyeOpen) / sizeof(eyeOpen[0]));

  bool bOpen;

  // are we initializing?
  if (bInit) {
    PRINTS("\n--- Eyes init");
    resetMatrix();
    bInit = false;
  }

  // Is it time to animate?
  if (millis() - prevTimeAnim < EYES_DELAY)
    return (bInit);
  prevTimeAnim = millis();  // starting point for next time

  // now run the animation
  bOpen = (random(1000) > 100);
  PRINT("\nH E:", bOpen);

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  for (uint8_t e = 1; e <= NUM_EYES; e++) {
    for (uint8_t i = 0; i < dataSize; i++) {
      mx.setColumn((e * offset) - dataSize + i, bOpen ? eyeOpen[i] : eyeClose[i]);
      mx.setColumn((e * offset) + dataSize - i - 1, bOpen ? eyeOpen[i] : eyeClose[i]);
    }
  }
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

  return (bInit);
}

bool graphicBounceBall(bool bInit) {
  static uint8_t idx = 0;    // position
  static int8_t idOffs = 1;  // increment direction

  // are we initializing?
  if (bInit) {
    PRINTS("\n--- BounceBall init");
    resetMatrix();
    bInit = false;
  }

  // Is it time to animate?
  if (millis() - prevTimeAnim < SCANNER_DELAY)
    return (bInit);
  prevTimeAnim = millis();  // starting point for next time

  PRINT("\nBB R:", idx);
  PRINT(" D:", idOffs);

  // now run the animation
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  // turn off the old ball
  mx.setColumn(idx, 0);
  mx.setColumn(idx + 1, 0);

  idx += idOffs;
  if ((idx == 0) || (idx == mx.getColumnCount() - 2))
    idOffs = -idOffs;

  // turn on the new lines
  mx.setColumn(idx, 0x18);
  mx.setColumn(idx + 1, 0x18);

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

  return (bInit);
}

bool graphicArrowScroll(bool bInit) {
  const uint8_t arrow[] = { 0x3c, 0x66, 0xc3, 0x99 };
  const uint8_t dataSize = (sizeof(arrow) / sizeof(arrow[0]));

  static uint8_t idx = 0;

  // are we initializing?
  if (bInit) {
    PRINTS("\n--- ArrowScroll init");
    resetMatrix();
    bInit = false;
  }

  // Is it time to animate?
  if (millis() - prevTimeAnim < ARROWS_DELAY)
    return (bInit);
  prevTimeAnim = millis();  // starting point for next time

  // now run the animation
  PRINT("\nAR I:", idx);

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  mx.transform(MD_MAX72XX::TSL);
  mx.setColumn(0, arrow[idx++]);
  if (idx == dataSize) idx = 0;
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

  return (bInit);
}

bool graphicWiper(bool bInit) {
  static uint8_t idx = 0;    // position
  static int8_t idOffs = 1;  // increment direction

  // are we initializing?
  if (bInit) {
    PRINTS("\n--- Wiper init");
    resetMatrix();
    bInit = false;
  }

  // Is it time to animate?
  if (millis() - prevTimeAnim < WIPER_DELAY)
    return (bInit);
  prevTimeAnim = millis();  // starting point for next time

  PRINT("\nW R:", idx);
  PRINT(" D:", idOffs);

  // now run the animation
  mx.setColumn(idx, idOffs == 1 ? 0xff : 0);
  idx += idOffs;
  if ((idx == 0) || (idx == mx.getColumnCount()))
    idOffs = -idOffs;

  return (bInit);
}

bool graphicInvader(bool bInit) {
  const uint8_t invader1[] = { 0x0e, 0x98, 0x7d, 0x36, 0x3c };
  const uint8_t invader2[] = { 0x70, 0x18, 0x7d, 0xb6, 0x3c };
  const uint8_t dataSize = (sizeof(invader1) / sizeof(invader1[0]));

  static int8_t idx;
  static bool iType;

  // are we initializing?
  if (bInit) {
    PRINTS("\n--- Invader init");
    resetMatrix();
    bInit = false;
    idx = -dataSize;
    iType = false;
  }

  // Is it time to animate?
  if (millis() - prevTimeAnim < INVADER_DELAY)
    return (bInit);
  prevTimeAnim = millis();  // starting point for next time

  // now run the animation
  PRINT("\nINV I:", idx);

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  mx.clear();
  for (uint8_t i = 0; i < dataSize; i++) {
    mx.setColumn(idx - dataSize + i, iType ? invader1[i] : invader2[i]);
    mx.setColumn(idx + dataSize - i - 1, iType ? invader1[i] : invader2[i]);
  }
  idx++;
  if (idx == mx.getColumnCount() + (dataSize * 2)) bInit = true;
  iType = !iType;
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

  return (bInit);
}

bool graphicPacman(bool bInit) {
#define MAX_FRAMES 4  // number of animation frames
#define PM_DATA_WIDTH 18
  const uint8_t pacman[MAX_FRAMES][PM_DATA_WIDTH] =  // ghost pursued by a pacman
    {
      { 0x3c, 0x7e, 0x7e, 0xff, 0xe7, 0xc3, 0x81, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe },
      { 0x3c, 0x7e, 0xff, 0xff, 0xe7, 0xe7, 0x42, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe },
      { 0x3c, 0x7e, 0xff, 0xff, 0xff, 0xe7, 0x66, 0x24, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe },
      { 0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe },
    };

  static int16_t idx;         // display index (column)
  static uint8_t frame;       // current animation frame
  static uint8_t deltaFrame;  // the animation frame offset for the next frame

  // are we initializing?
  if (bInit) {
    PRINTS("\n--- Pacman init");
    resetMatrix();
    bInit = false;
    idx = -1;  //DATA_WIDTH;
    frame = 0;
    deltaFrame = 1;
  }

  // Is it time to animate?
  if (millis() - prevTimeAnim < PACMAN_DELAY)
    return (bInit);
  prevTimeAnim = millis();  // starting point for next time

  PRINT("\nPAC I:", idx);

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  mx.clear();

  // clear old graphic
  for (uint8_t i = 0; i < PM_DATA_WIDTH; i++)
    mx.setColumn(idx - PM_DATA_WIDTH + i, 0);
  // move reference column and draw new graphic
  idx++;
  for (uint8_t i = 0; i < PM_DATA_WIDTH; i++)
    mx.setColumn(idx - PM_DATA_WIDTH + i, pacman[frame][i]);

  // advance the animation frame
  frame += deltaFrame;
  if (frame == 0 || frame == MAX_FRAMES - 1)
    deltaFrame = -deltaFrame;

  // check if we are completed and set initialize for next time around
  if (idx == mx.getColumnCount() + PM_DATA_WIDTH) bInit = true;

  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

  return (bInit);
}

bool graphicArrowRotate(bool bInit) {
  static uint16_t idx;  // transformation index

  uint8_t arrow[COL_SIZE] = {
    0b00000000,
    0b00011000,
    0b00111100,
    0b01111110,
    0b00011000,
    0b00011000,
    0b00011000,
    0b00000000
  };

  MD_MAX72XX::transformType_t t[] = {
    MD_MAX72XX::TRC,
    MD_MAX72XX::TRC,
    MD_MAX72XX::TSR,
    MD_MAX72XX::TSR,
    MD_MAX72XX::TSR,
    MD_MAX72XX::TSR,
    MD_MAX72XX::TSR,
    MD_MAX72XX::TSR,
    MD_MAX72XX::TSR,
    MD_MAX72XX::TSR,
    MD_MAX72XX::TRC,
    MD_MAX72XX::TRC,
    MD_MAX72XX::TSL,
    MD_MAX72XX::TSL,
    MD_MAX72XX::TSL,
    MD_MAX72XX::TSL,
    MD_MAX72XX::TSL,
    MD_MAX72XX::TSL,
    MD_MAX72XX::TSL,
    MD_MAX72XX::TSL,
    MD_MAX72XX::TRC,
  };

  // are we initializing?
  if (bInit) {
    PRINTS("\n--- ArrowRotate init");
    resetMatrix();
    bInit = false;
    idx = 0;

    // use the arrow bitmap
    mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
    for (uint8_t j = 0; j < mx.getDeviceCount(); j++)
      mx.setBuffer(((j + 1) * COL_SIZE) - 1, COL_SIZE, arrow);
    mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  }

  // Is it time to animate?
  if (millis() - prevTimeAnim < ARROWR_DELAY)
    return (bInit);
  prevTimeAnim = millis();  // starting point for next time

  mx.control(MD_MAX72XX::WRAPAROUND, MD_MAX72XX::ON);
  mx.transform(t[idx++]);
  mx.control(MD_MAX72XX::WRAPAROUND, MD_MAX72XX::OFF);

  // check if we are completed and set initialize for next time around
  if (idx == (sizeof(t) / sizeof(t[0]))) bInit = true;

  return (bInit);
}

bool graphicSinewave(bool bInit) {
  static uint8_t curWave = 0;
  static uint8_t idx;

#define SW_DATA_WIDTH 11  // valid data count followed by up to 10 data points
  const uint8_t waves[][SW_DATA_WIDTH] = {
    { 9, 8, 6, 1, 6, 24, 96, 128, 96, 16, 0 },
    { 6, 12, 2, 12, 48, 64, 48, 0, 0, 0, 0 },
    { 10, 12, 2, 1, 2, 12, 48, 64, 128, 64, 48 },

  };
  const uint8_t WAVE_COUNT = sizeof(waves) / (SW_DATA_WIDTH * sizeof(uint8_t));

  // are we initializing?
  if (bInit) {
    PRINTS("\n--- Sinewave init");
    resetMatrix();
    bInit = false;
    idx = 1;
  }

  // Is it time to animate?
  if (millis() - prevTimeAnim < SINE_DELAY)
    return (bInit);
  prevTimeAnim = millis();  // starting point for next time

  mx.control(MD_MAX72XX::WRAPAROUND, MD_MAX72XX::ON);
  mx.transform(MD_MAX72XX::TSL);
  mx.setColumn(0, waves[curWave][idx++]);
  if (idx > waves[curWave][0]) {
    curWave = random(WAVE_COUNT);
    idx = 1;
  }
  mx.control(MD_MAX72XX::WRAPAROUND, MD_MAX72XX::OFF);

  return (bInit);
}

// ========== Control routines ===========
//
void resetMatrix(void) {
  mx.control(MD_MAX72XX::INTENSITY, MAX_INTENSITY / 2);
  mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
  mx.clear();
  prevTimeAnim = 0;
}

void runMatrixAnimation(void)
// Schedule the animations, switching to the next one when the
// the mode switch is pressed.
{
  static uint8_t state = 0;
  static uint8_t mesg = 0;
  static boolean bRestart = true;
  static boolean bInMessages = false;
  boolean changeState = false;

#if RUN_DEMO
  // check if one second has passed and then count down the demo timer. Once this
  // gets to zero, change the state.
  if (millis() - prevTimeDemo >= 1000) {
    prevTimeDemo = millis();
    if (--timeDemo == 0) {
      timeDemo = DEMO_DELAY;
      changeState = true;
    }
  }
#endif
  if (changeState) {
    if (bInMessages)  // the message display state
    {
      mesg++;
      if (mesg >= sizeof(pesan) / sizeof(pesan[0])) {
        mesg = 0;
        bInMessages = false;
        state++;
      }
    } else
      state++;

    bRestart = true;
  };

  // now do whatever we do in the current state
  switch (state) {
    case 0:
      bInMessages = true;
      bRestart = scrollText(bRestart, pesan[mesg].c_str());
      break;
    case 1: bRestart = graphicMidline1(bRestart); break;
    case 2: bRestart = graphicMidline2(bRestart); break;
    case 3: bRestart = graphicScanner(bRestart); break;
    case 4: bRestart = graphicRandom(bRestart); break;
    case 5: bRestart = graphicFade(bRestart); break;
    case 6: bRestart = graphicSpectrum1(bRestart); break;
    case 7: bRestart = graphicHeartbeat(bRestart); break;
    case 8: bRestart = graphicHearts(bRestart); break;
    case 9: bRestart = graphicEyes(bRestart); break;
    case 10: bRestart = graphicBounceBall(bRestart); break;
    case 11: bRestart = graphicArrowScroll(bRestart); break;
    case 12: bRestart = graphicScroller(bRestart); break;
    case 13: bRestart = graphicWiper(bRestart); break;
    case 14: bRestart = graphicInvader(bRestart); break;
    case 15: bRestart = graphicPacman(bRestart); break;
    case 16: bRestart = graphicArrowRotate(bRestart); break;
    case 17: bRestart = graphicSpectrum2(bRestart); break;
    case 18: bRestart = graphicSinewave(bRestart); break;

    default: state = 0;
  }
}
