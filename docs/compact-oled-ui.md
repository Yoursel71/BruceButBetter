# ESP-General 128×64 OLED arayüzü

ESP32-S3 N16R8 üzerindeki SH1106 ekran, `UI_COMPACT=1` ile kendi yerleşimini kullanır.
Hedef PlatformIO ortamı `esp32-s3-devkitc-1-psram`dır.

GPIO40 üzerindeki OK tuşu kısa basılıp bırakılınca seçer. 800 ms basılı tutulunca
bir kez ESC / geri gönderir; bırakırken ayrıca seçim yapılmaz. Fiziksel BACK
tuşu GPIO41 üzerinde kullanılmaya devam eder. Ekranı uyandıran ilk OK hareketi
tüketilir; yanlışlıkla bir menü açmaz. OK girişine 25 ms debounce uygulanır.

İki NRF24 aynı SPI hattındadır. Açılışta her iki CSN HIGH, her iki CE LOW
tutulur; birinci radyo başlatılırken ikinci radyo tekrar seçimsiz bırakılır.
GPIO7'deki ikinci CSN'nin boşta LOW kalması, ilk NRF24 başlatmasını bozabiliyordu.

Derin uykudan UP (GPIO15) ile uyanılır; OK (GPIO40) RTC GPIO olmadığı için
uyandırma kaynağı olamaz. OK+BACK üç saniye tutulunca cihaz uyur. Yalnız beş
tuş bağlıysa ayrı BACK gerektiren bu kısayol kullanılamaz. Uykudan sonra UP
yeniden dijital girişe alınır. OLED dimmer ve buzzer davranışı değiştirilmemiştir.

Uzun durum mesajları iki satıra bölünür. Menü gizleme listesi öğeleri açıp
kapatır, BACK ile kapanır ve Config menüsünü gizlemez. Navigator kayıtları
halka tampon sırasıyla gönderilir; kayan metin alt satırların kaydını silmez.
Görsel yuvası dolduğunda kayıt atlanır, günlük tamponları yeniden kullanılır.
Bozuk/açılamayan görseller, bellek ayırma hataları ve düğmesiz mesajlar
korumalıdır. Web UI dosya açmayı sınırlı sayıda dener; şifreli yüklemede
yalnız tek parça kabul edilir ve gelen veri uzunluğu kadar kopyalanır.

- Ana menü 4×2 ikon ızgarasıdır: sayfa başına 8 menü, 13 menü iki sayfaya yayılır.
  SOL/SAĞ hücreleri okuma sırasıyla dolaşır (satır ve sayfa sonunda devam eder),
  YUKARI/AŞAĞI satır değiştirir; üst/alt kenarda önceki/sonraki sayfaya geçer.
  Seçili hücre 1 px yuvarlak çerçeveyle sarılır. Ana menüde BACK bir şey yapmaz.
- İkonlar 24×24 tasarım biriminde tanımlıdır ve ızgarada 16 px ölçekle çizilir.
  Her menü sınıfı `compactIcon()` ile kendi simgesini seçer; isim değişiklikleri simgeyi etkilemez.
- Alt şerit (y=54…63) seçili menünün tam adını, sayfa noktalarını (dolu = bu sayfa) ve
  `n/toplam` sayacını gösterir. Sığmayan ad listelerdeki gibi kayar. Sayaç, gizlenen
  menüler çıkarıldıktan sonraki görünür menü sayısını kullanır.
- Durum çubuğu tek satırdır. `S`: SD, `W`: Wi-Fi, `@`: Web UI, `B`: Bluetooth,
  `G`: GPS (GPS kapalı derlemelerde yuvası yoktur), `V`: WireGuard. Her harfin sabit bir
  yuvası vardır; biri açılıp kapanınca diğerleri kaymaz. Sağ kenar her zaman pil yüzdesi
  veya `USB` ile biter. Ana menüde solda saat (ayarlı değilse `BRUCE`) durur; bir kategori
  açılınca onun adı (ör. `WiFi`) o kategorinin altındaki tüm ekranlarda solda kalır.
- Alt menüler, bağlam menüleri ve dosya tarayıcısı dört satır gösterir.
  Seçili satır ters renklidir; uzun adlar kısa bir beklemeden sonra kayar.
  `*` seçili ayarı, `-` kullanılamayan seçeneği, `/` klasörü belirtir.
  Alt şeritte ince ayırıcının altında tuş ipucu ve `mevcut/toplam` sayacı bulunur:
  `OK:Select BACK:Back`, ayar listelerinde `OK:Change`, dosya tarayıcısında
  `OK:Open BACK:Up`. Cihazdaki tüm yazılar İngilizcedir.
- İletişim pencerelerinin metni en fazla üç satıra bölünür. Düğmeler (x=2/44/86, y=50)
  ve ilerleme çubuğu içerikten ayrılmıştır; toplam boyutu sıfır olan ilerleme çağrıları da
  geçerlidir. BACK tek düğmeli pencereyi kapatır, varsa `Cancel` düğmesini seçer.
- `displayLoading(mesaj)` süresi belli olmayan bekleyişler içindir: tekrar tekrar çağrıldıkça
  çubuğun içinde bir blok gidip gelir.
- Web UI arka planda açılır. Cihazın menüsü kullanımda kalır ve Navigator görüntüsü
  menünün kendi çizim döngüsü üzerinden yenilenir.

128×64 yatay görünümde durum çubuğu y=0…13 (ayırıcı y=11), ızgara satırlarının
çerçeveleri y=14…33 ve y=34…53, ikon merkezleri x=16/48/80/112, y=24/44, alt şerit
y=54…63 aralığındadır. Listeler y=14…53 aralığına sığar.

İkonlar `src/core/menu_items/compact_icons.cpp` içindedir. Standart çizim çağrıları kullanılır;
bu nedenle Web UI Navigator da simgeleri gösterebilir. Bir ikon en fazla 12 çizim çağrısı kullanır.
Izgaranın bir sayfası 65 civarı çizim çağrısıdır; Navigator günlüğü bu yüzden kompakt
arayüzde 128 kayıttır. Bir ekranın çizimleri tek seferde panele gönderilir
(`startWrite`/`endWrite`); amaç imleç hareketindeki titremeyi azaltmaktır.

Kompakt ana menü kendi yerleşik ikonlarını kullanır. Renkli ekranların mevcut ikonları,
tema/GIF çizimleri ve düzeni `UI_COMPACT` kapalı dalında korunur.

Derleme:

```powershell
platformio run -e esp32-s3-devkitc-1-psram
```

Cihaz kontrolünde ızgaranın dört yönde dolaşılması, sayfa geçişleri ve kısa son satır,
uzun bir adın alt şeritte kayması, durum çubuğu yuvaları, dosya listesi, onay düğmelerinin
sınırları ve BACK ile iptal kontrol edilmelidir.
