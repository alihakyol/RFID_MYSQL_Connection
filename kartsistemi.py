import serial
import mysql.connector

# Arduino ile seri bağlantı oluştur
ser = serial.Serial('COM3', 9600) # Arduino'nun bağlı olduğu seri portu ve baud oranını belirtin

# MySQL veritabanına bağlan
db = mysql.connector.connect(host="localhost", user="root", passwd="Asd12345@", database="cart_value")
cursor = db.cursor()

while True:
    try:
        # Arduino'dan gelen veriyi oku
        data = ser.readline().strip().decode("utf-8") # Arduino'dan gelen veriyi oku ve gereksiz boşlukları temizle
        print(data)
        # Ekstra satır kontrolü yap
        if "KART ID:" in data:
            data = data.replace("KART ID: ", "")  # "KART ID:" metnini kaldır
            data = data.replace(" ","")
            print("Received data from Arduino:", data)
            
            # Kart ID'sini MySQL veritabanına ekle
            cursor.execute("INSERT INTO kartlar (kart_id) VALUES (%s)", (data,))
            db.commit()
            print("Kart ID added to MySQL database.")
    except KeyboardInterrupt:
        print("Program terminated by user.")
        break
    except Exception as e:
        print("ERROR:", e)

# MySQL bağlantısını kapat
cursor.close()
db.close()
