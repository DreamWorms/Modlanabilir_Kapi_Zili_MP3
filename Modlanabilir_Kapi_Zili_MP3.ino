
#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include "Audio.h"

// MODLANABILIR KAPI ZILI - MP3 V1

#define BUTON 4
#define I2S_BCLK 16
#define I2S_LRC 17
#define I2S_DOUT 18

Audio audio;

bool sonOkuma = HIGH;
bool sabitDurum = HIGH;
unsigned long sonDegisim = 0;

void setup() {
  Serial.begin(115200);
  pinMode(BUTON, INPUT_PULLUP);

  delay(1000);
  Serial.println("MP3 KAPI ZILI BASLATILIYOR...");

  // false: Bellekteki dosyalari silme!
  if (!SPIFFS.begin(false)) {
    Serial.println("HATA: SPIFFS ACILAMADI!");
    return;
  }

  Serial.println("SPIFFS HAZIR!");

  if (!SPIFFS.exists("/zil")) {
    Serial.println("HATA: zil BULUNAMADI!");
    return;
  }

  File dosya = SPIFFS.open("/zil", "r");
  Serial.printf("MP3 bulundu: %u byte\n",
                (unsigned int)dosya.size());
  dosya.close();

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);

  // Ilk denemede dusuk ses seviyesi (0-21)
  audio.setVolume(4);

  Serial.println("KAPI ZILI HAZIR!");
  Serial.println("Butona basarak MP3 calabilirsin.");
}

void loop() {
  // Ses oynaticisinin surekli calismasi gerekir
  audio.loop();

  bool okuma = digitalRead(BUTON);

  if (okuma != sonOkuma) {
    sonDegisim = millis();
  }

  if (millis() - sonDegisim > 40) {
    if (okuma != sabitDurum) {
      sabitDurum = okuma;

      if (sabitDurum == LOW) {
        Serial.println("ZILE BASILDI!");

        if (!audio.isRunning()) {
          if (audio.connecttoFS(SPIFFS, "/zil")) {
            Serial.println("MP3 CALIYOR!");
          } else {
            Serial.println("MP3 ACILAMADI!");
          }
        }
      }
    }
  }

  sonOkuma = okuma;
  delay(1);
}
