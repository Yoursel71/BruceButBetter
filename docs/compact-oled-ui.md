# ESP-General 128×64 OLED arayüzü

ESP32-S3 N16R8 üzerindeki SH1106 ekran, `UI_COMPACT=1` ile kendi yerleşimini kullanır.
Hedef PlatformIO ortamı `esp32-s3-devkitc-1-psram`dır.

GPIO40 üzerindeki OK tuşu kısa basılıp bırakılınca seçer. 800 ms basılı tutulunca
bir kez ESC / geri gönderir; bırakırken ayrıca seçim yapılmaz. Fiziksel BACK
tuşu GPIO41 üzerinde kullanılmaya devam eder. Ekranı uyandıran ilk OK hareketi
tüketilir; yanlışlıkla bir menü açmaz. OK girişine 25 ms debounce uygulanır.

- Ana menüde 18 menü türü için doğrudan 24×24 piksel hücreye çizilen ikonlar bulunur.
  Her menü sınıfı `compactIcon()` ile kendi simgesini seçer; isim değişiklikleri simgeyi etkilemez.
- Durum çubuğu tek satırdır. `S`: SD, `W`: Wi-Fi, `@`: Web UI, `B`: Bluetooth,
  `G`: GPS, `V`: WireGuard. Sağda pil yüzdesi veya USB göstergesi bulunur.
- Ana menü başlığı ve sayfa göstergesi ikonun altında ayrı satırlardadır.
  Sayaç, gizlenen menüler çıkarıldıktan sonraki görünür menü sayısını kullanır.
- Alt menüler, bağlam menüleri ve dosya tarayıcısı dört satır gösterir.
  Seçili satır ters renklidir; uzun adlar kısa bir beklemeden sonra kayar.
  `*` seçili ayarı, `-` kullanılamayan seçeneği, `/` klasörü belirtir.
- İletişim pencerelerinin metni en fazla üç satıra bölünür. Düğmeler ve ilerleme çubuğu
  içerikten ayrılmıştır; toplam boyutu sıfır olan ilerleme çağrıları da geçerlidir.
- Web UI arka planda açılır. Cihazın menüsü kullanımda kalır ve Navigator görüntüsü
  menünün kendi çizim döngüsü üzerinden yenilenir.

128×64 yatay görünümde durum çubuğu y=0…13, ana menü ikonu x=52…75/y=17…40,
başlık y=46…53 ve sayfa göstergesi y=56…63 aralığındadır. Listeler y=14…53 aralığına sığar.

İkonlar `src/core/menu_items/compact_icons.cpp` içindedir. Standart çizim çağrıları kullanılır;
bu nedenle Web UI Navigator da simgeleri gösterebilir. Bir ikon en fazla 12 çizim çağrısı kullanır;
ana ekranın tamamı 64 kayıtlık Navigator günlüğüne sığmalıdır.

Kompakt ana menü kendi yerleşik ikonlarını kullanır. Renkli ekranların mevcut ikonları,
tema/GIF çizimleri ve düzeni `UI_COMPACT` kapalı dalında korunur.

Derleme:

```powershell
platformio run -e esp32-s3-devkitc-1-psram
```

Cihaz kontrolünde ana menünün iki yönde dolaşılması, ilk/son öğeye geçilmesi, uzun bir
etiketin kayması, dosya listesi ve onay düğmelerinin sınırları kontrol edilmelidir.
