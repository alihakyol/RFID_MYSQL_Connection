import serial
import requests

# Arduino ile seri bağlantı oluştur
ser = serial.Serial('COM3', 9600)    # Arduino'nun bağlı olduğu seri portu ve baud oranını belirtin

# API'nin URL'si
url = "https://...."

# Abonelik anahtarı
subscription_key = "...."

# Başlıklar (headers) sözlüğü oluşturun
headers = {
    "Ocp-Apim-Subscription-Key": subscription_key,
    "Content-Type": "application/json"  # İstek gövdesinin JSON formatında olduğunu belirtin
}

while True:
    try:
        # Arduino'dan gelen veriyi oku
        data = ser.readline().strip().decode("utf-8") # Arduino'dan gelen veriyi oku ve gereksiz boşlukları temizle
        if "KART ID:" in data:
            data = data.replace("KART ID: ", "")  # "KART ID:" metnini kaldır
            data = data.replace(" ","")
        print("Received data from Arduino:", data)
        
        # RFID karttan gelen veriyi ile "cardNumber" değerini karşılaştır
        if "4130130106" in data:
            # Güncellenecek veri
            data_payload = {
                "cardNumber": "4130130106"
            }  
            # PUT isteği gönderin
            response = requests.put(url, headers=headers, json=data_payload)
            
            # Yanıtı kontrol edin
            if response.status_code == 200:
                print("İstek başarılı, yanıt:", response.text)
            else:
                print("İstek başarısız, hata kodu:", response.status_code)
                
    except KeyboardInterrupt:
        print("Program terminated by user.")
        break
    except Exception as e:
        print("ERROR:", e)
