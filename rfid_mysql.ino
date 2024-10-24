#include <SPI.h>
#include <MFRC522.h>
#include <Ethernet.h>
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

#define RED D14      // Kırmızı LED D14 pinine bağlı
#define BLUE D13     // Mavi LED D13 pinine bağlı
#define GREEN D12    // Yeşil LED D12 pinine bağlı
#define RST_PIN D0   // RC522 modülü reset pini D0'a bağlı
#define SDA_PIN D1   // RC522 modülü chip select pini D1'e bağlı

MFRC522 rfid(SDA_PIN, RST_PIN);  //RC522 modülü ayarlarını yapılır.
byte ID[4] = {121,127,254,179};  //Yetkili kartın ID'si

char host[] = "MYSQL_SERVER_IP";      // MySQL sunucunuzun IP adresi
char user[] = "MYSQL_USERNAME";        // MySQL kullanıcı adı
char password[] = "MYSQL_PASSWORD";    // MySQL şifre
char database[] = "MYSQL_DATABASE";    // Kullanılacak veritabanı adı

EthernetClient client;
MySQL_Connection conn((Client *)&client);
MySQL_Cursor *cur_mem;

void setup() {
  Serial.begin(115200);        // Seri haberleşme başlatılır
  SPI.begin();                 // SPI iletişimi başlatılır.
  rfid.PCD_Init();             // RC522 modülü başlatılır.
  pinMode(RED, OUTPUT);        // LED'ler çıkış olarak ayarlanır
  pinMode(BLUE, OUTPUT);
  pinMode(GREEN, OUTPUT);
  Ethernet.begin(mac);         // Ethernet bağlantısını başlatır
  delay(1000);
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent())  // Yeni kartın okunması beklenir
  {
    BEKLE(); // Kart okutulmazsa mavi LED yanar
    delay(1000);
    return;
  }

  if (!rfid.PICC_ReadCardSerial())     // Kart okunmadığı zaman beklenilir
  {
    BEKLE(); // Kart okutulmazsa mavi LED yanar
    delay(1000);
    return;
  }

  if (rfid.uid.uidByte[0] == ID[0] &&  // Okunan kart ID'si ile ID değişkeni karşılaştırılır
      rfid.uid.uidByte[1] == ID[1] &&
      rfid.uid.uidByte[2] == ID[2] &&
      rfid.uid.uidByte[3] == ID[3]) {
    DOGRU();       // Okutulan kart doğruysa yeşil LED yanar
    delay(1000);
  } else {       // Okutulan kart yanlışsa bu blok çalışır
    YANLIS();     // Okutulan kart yanlışsa mavi LED yanar
    ID_OGREN();   // Seri port ekranında kartın ID'si yazılır.
    
    // MySQL bağlantısı
    if (conn.connect(host, 3306, user, password)) {
      // Bağlantı başarılı
      cur_mem = new MySQL_Cursor(&conn);
      String query = "SELECT * FROM yetkili_kartlar WHERE kart_id = '";
      query += String(rfid.uid.uidByte[0]) + String(rfid.uid.uidByte[1]) + String(rfid.uid.uidByte[2]) + String(rfid.uid.uidByte[3]) + "'";
      cur_mem->execute(query);
      
      MySQL_Row row;
      row = cur_mem->get_next_row();
      if (row.length() > 0) {
        // Yetkili kart bulundu
        Serial.println("Yetkili kart bulundu");
      } else {
        // Yetkili kart bulunamadı
        Serial.println("Yetkili kart bulunamadı");
      }
      
      delete cur_mem;
      conn.close();
    } else {
      // Bağlantı başarısız
      Serial.println("MySQL bağlantı hatası");
    }
    
    delay(1000);
  }
  rfid.PICC_HaltA();
}

void BEKLE() {
  digitalWrite(BLUE, HIGH);
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);
}

void DOGRU() {
  digitalWrite(BLUE, LOW);
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, HIGH);
}

void YANLIS() {
  digitalWrite(BLUE, LOW);
  digitalWrite(RED, HIGH);
  digitalWrite(GREEN, LOW);
}

void ID_OGREN() {
  Serial.print("ID Numarasi: ");
  for (int sayac = 0; sayac < 4; sayac++) {
    Serial.print(rfid.uid.uidByte[sayac]);
    Serial.print(" ");
  }
  Serial.println("");
}
