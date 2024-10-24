import requests

# API'nin URL'si
url = "api adresi "

# Abonelik anahtarı
subscription_key = "..."

# Başlıklar (headers) sözlüğü oluşturun
headers = {
    "Ocp-Apim-Subscription-Key": subscription_key,
    "Content-Type": "application/json"  # İstek gövdesinin JSON formatında olduğunu belirtin
}

# Güncellenecek veri
data = {
    "cardNumber":'4130130106'
}

# PUT isteği gönderin
response = requests.put(url, headers=headers, json=data)

# Yanıtı kontrol edin
if response.status_code == 200:
    print("İstek başarılı, yanıt:", response.text)
else:
    print("İstek başarısız, hata kodu:", response.status_code)