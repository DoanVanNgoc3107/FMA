# HƯỚNG DẪN TỔNG HỢP: KẾT NỐI NE555 VỚI ARDUINO UNO R3 VÀ GIẢI THÍCH CODE

## 📋 TỔNG QUAN HỆ THỐNG

Hệ thống này bao gồm:
1. **Mạch NE555** - Tạo tín hiệu PWM/xung vuông với tần số có thể điều chỉnh
2. **Arduino Uno R3** - Đo tần số từ NE555 và xử lý
3. **LCD I2C 16x2** - Hiển thị tần số đo được

---

## 🔌 SƠ ĐỒ KẾT NỐI CHI TIẾT

### 1. KẾT NỐI MẠCH NE555

#### Linh kiện cần thiết:
- **IC NE555** (1 cái)
- **R1** = 1kΩ (điện trở)
- **R2** = 1kΩ hoặc biến trở 10kΩ (để điều chỉnh tần số)
- **C1** = 1µF (tụ điện - tần số ~480 Hz) hoặc 100nF (tần số ~4.8 kHz)
- **C2** = 10nF (tụ nối Pin 5 xuống GND - tùy chọn nhưng nên có)
- **C3** = 100nF (tụ lọc nguồn - tùy chọn)

#### Sơ đồ mạch NE555:

```
                    +5V (Arduino)
                     │
                     ├───[R1: 1kΩ]───┐
                     │                 │
                     │                 ├───[R2: 1kΩ hoặc biến trở]───┐
                     │                 │                               │
                     │                 │                              ┌┴─┐
                     │                 │                              │C1│ (1µF)
                     │                 │                              └┬─┘
                     │                 │                               │
                     │                 │                               │
        ┌────────────┼─────────────────┼───────────────────────────────┼───┐
        │            │                 │                               │   │
        │         ┌──▼──┐              │                               │   │
        │      ┌──┤ 2   │              │                               │   │
        │      │  │     │              │                               │   │
        │      │  │NE555│              │                               │   │
        │      │  │     │              │                               │   │
        │      │  │  3  ├──────────────┼───────────────────────────────┼───┼───→ D3 (Arduino)
        │      │  │     │              │                               │   │
        │      │  └──┬──┘              │                               │   │
        │      │     │                 │                               │   │
        │      │     │ 7 (DISCHARGE)   │                               │   │
        │      │     └─────────────────┘                               │   │
        │      │                                                        │   │
        │      │ 4 (RESET) ─────────────────────────────────────────────┼───┘
        │      │                                                        │
        │      │ 5 (CONTROL) ───[C2: 10nF]─── GND                      │
        │      │                                                        │
        │      │ 6 (THRESHOLD) ────────────────────────────────────────┘
        │      │                                                        │
        │      │ 1 (GND) ───────────────────────────────────────────────┘
        │      │                                                        │
        │      └───────────────────────────────────────────────────────┘
        │                                                               │
        └───────────────────────────────────────────────────────────────┴─── GND (Arduino)
```

#### Kết nối NE555 với Arduino:

| Chân NE555 | Kết nối với | Mô tả |
|------------|-------------|-------|
| **Pin 8 (VCC)** | **Arduino 5V** | Nguồn cấp 5V |
| **Pin 1 (GND)** | **Arduino GND** | Mass chung (BẮT BUỘC) |
| **Pin 3 (OUTPUT)** | **Arduino Digital Pin 3** | Tín hiệu PWM đầu ra |
| **Pin 4 (RESET)** | **Arduino 5V** hoặc để trống | Cho phép NE555 hoạt động |
| **Pin 5 (CONTROL)** | **Tụ 10nF → GND** | Ổn định tần số (tùy chọn) |

**⚠️ QUAN TRỌNG:** Phải nối GND chung giữa NE555 và Arduino!

### 2. KẾT NỐI LCD I2C VỚI ARDUINO

| Chân LCD I2C | Chân Arduino Uno R3 | Mô tả |
|--------------|---------------------|-------|
| **VCC** | **5V** | Nguồn 5V |
| **GND** | **GND** | Mass chung |
| **SDA** | **A4** | Dữ liệu I2C |
| **SCL** | **A5** | Xung đồng bộ I2C |

### 3. SƠ ĐỒ KẾT NỐI TỔNG QUAN

```
┌─────────────────────────────────────────────────────────┐
│                  ARDUINO UNO R3                        │
│                                                          │
│  5V ────┬────────────────────────────────────────────┐  │
│  GND ───┼───┬──────────────────────────────────────┬─┼─┐│
│  A4 ────┼───┼──────────────────────────────────┐  │ │ ││
│  A5 ────┼───┼──────────────────────────────┐   │  │ │ ││
│  D3 ────┼───┼──────────────────────────┐   │   │  │ │ ││
│         │   │                          │   │   │  │ │ ││
└─────────┼───┼──────────────────────────┼───┼───┼──┼─┼─┘
          │   │                          │   │   │  │ │
          │   │                          │   │   │  │ │
    ┌─────▼───▼───┐              ┌───────▼───▼───▼──▼──▼──┐
    │   LCD I2C   │              │      MẠCH NE555         │
    │             │              │                         │
    │  VCC ───────┘              │  Pin 8 (VCC) ───────────┘
    │  GND ──────────────────────┼─ Pin 1 (GND) ────────────┘
    │  SDA ───────┘              │  Pin 3 (OUT) ────────────┘
    │  SCL ───────┘              │  Pin 4 (RESET) ───────────┘
    └────────────────────────────┘  Pin 5 (CONTROL) ────────┘
                                    Pin 6 (THRESHOLD) ───────┘
                                    Pin 7 (DISCHARGE) ───────┘
```

---

## 💻 GIẢI THÍCH CODE CHI TIẾT

### PHẦN 1: KHAI BÁO VÀ CẤU HÌNH

```cpp
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
```
- **Arduino.h**: Thư viện cơ bản của Arduino
- **Wire.h**: Thư viện giao tiếp I2C
- **LiquidCrystal_I2C.h**: Thư viện điều khiển LCD I2C

```cpp
#define LCD_ADDRESS 0x27
#define LCD_COLS 16
#define LCD_ROWS 2
#define FREQUENCY_PIN 3
#define SAMPLE_INTERVAL 1000
```
- **LCD_ADDRESS**: Địa chỉ I2C của LCD (có thể là 0x27 hoặc 0x3F)
- **LCD_COLS/ROWS**: Kích thước LCD (16 cột, 2 dòng)
- **FREQUENCY_PIN**: Chân đo tần số (D3 - Interrupt 1)
- **SAMPLE_INTERVAL**: Thời gian lấy mẫu (1000ms = 1 giây)

### PHẦN 2: BIẾN TOÀN CỤC

```cpp
volatile unsigned long pulseCount = 0;
volatile unsigned long lastPulseTime = 0;
```
- **volatile**: Báo cho compiler biết biến này có thể thay đổi trong ISR
- **pulseCount**: Đếm số xung từ NE555
- **lastPulseTime**: Thời gian xung cuối cùng (để phát hiện mất tín hiệu)

```cpp
unsigned long lastSampleTime = 0;
float frequency = 0.0;
bool signalDetected = false;
```
- **lastSampleTime**: Thời gian lấy mẫu cuối cùng
- **frequency**: Tần số tính được (Hz)
- **signalDetected**: Cờ phát hiện tín hiệu

### PHẦN 3: HÀM NGẮT (ISR - Interrupt Service Routine)

```cpp
void countPulseISR() {
  pulseCount++;
  lastPulseTime = millis();
}
```

**Giải thích:**
- Hàm này được gọi **TỰ ĐỘNG** mỗi khi có cạnh lên (RISING) trên chân D3
- **pulseCount++**: Tăng bộ đếm xung lên 1
- **lastPulseTime**: Lưu thời gian hiện tại để biết xung cuối cùng khi nào
- Hàm này phải **NGẮN GỌN** và **NHANH** để không bỏ sót xung

**Cách hoạt động:**
1. NE555 tạo xung vuông ở Pin 3
2. Mỗi khi xung chuyển từ LOW → HIGH, Arduino tự động gọi `countPulseISR()`
3. Bộ đếm `pulseCount` tăng lên

### PHẦN 4: HÀM SETUP

```cpp
void setup() {
  Serial.begin(9600);
  // ... khởi tạo Serial
```

**Khởi tạo Serial:**
- Bật Serial ở tốc độ 9600 baud để debug
- Đợi tối đa 3 giây để Serial sẵn sàng

```cpp
  lcd.init();
  lcd.backlight();
```

**Khởi tạo LCD:**
- `init()`: Khởi tạo LCD I2C
- `backlight()`: Bật đèn nền LCD

```cpp
  pinMode(FREQUENCY_PIN, INPUT_PULLUP);
```

**Cấu hình chân đo:**
- `INPUT_PULLUP`: Chân ở chế độ INPUT với điện trở kéo lên nội bộ
- Giúp chân luôn ở mức HIGH khi không có tín hiệu, tránh nhiễu

```cpp
  attachInterrupt(digitalPinToInterrupt(FREQUENCY_PIN), countPulseISR, RISING);
```

**Gắn hàm ngắt:**
- `digitalPinToInterrupt(3)`: Chuyển chân D3 thành số ngắt (Interrupt 1)
- `countPulseISR`: Hàm sẽ được gọi khi có ngắt
- `RISING`: Ngắt khi tín hiệu chuyển từ LOW → HIGH

### PHẦN 5: HÀM LOOP

```cpp
void loop() {
  unsigned long currentTime = millis();
```

**Lấy thời gian hiện tại:**
- `millis()`: Trả về số millisecond từ khi Arduino bật
- Dùng để tính toán thời gian trôi qua

```cpp
  if (currentTime - lastSampleTime >= SAMPLE_INTERVAL) {
```

**Kiểm tra thời gian lấy mẫu:**
- Nếu đã qua 1 giây (1000ms) kể từ lần lấy mẫu cuối
- Thì tiến hành tính toán tần số

```cpp
    noInterrupts();
    unsigned long count = pulseCount;
    unsigned long pulseTime = lastPulseTime;
    pulseCount = 0;
    interrupts();
```

**Đọc an toàn biến volatile:**
- `noInterrupts()`: Tắt tất cả ngắt tạm thời
- Đọc `pulseCount` và `lastPulseTime` vào biến local
- Reset `pulseCount = 0` để bắt đầu đếm lại
- `interrupts()`: Bật ngắt trở lại

**Tại sao cần tắt ngắt?**
- Nếu không tắt, ISR có thể thay đổi `pulseCount` trong lúc đang đọc
- Dẫn đến đọc sai giá trị

```cpp
    float timeElapsed = (currentTime - lastSampleTime) / 1000.0;
    frequency = count / timeElapsed;
```

**Tính toán tần số:**
- `timeElapsed`: Thời gian thực tế đã trôi qua (giây)
- `frequency = count / timeElapsed`: Tần số = số xung / thời gian

**Ví dụ:**
- Đếm được 480 xung trong 1 giây
- Tần số = 480 / 1.0 = 480 Hz

```cpp
    if (currentTime - pulseTime > 2000) {
      signalDetected = false;
      frequency = 0.0;
    } else {
      signalDetected = true;
    }
```

**Phát hiện mất tín hiệu:**
- Nếu không có xung nào trong 2 giây → mất tín hiệu
- Đặt `signalDetected = false` và `frequency = 0.0`

```cpp
  if (currentTime - displayUpdateTime >= DISPLAY_UPDATE_INTERVAL) {
    displayFrequency(frequency);
    displayUpdateTime = currentTime;
  }
```

**Cập nhật LCD:**
- Cập nhật LCD mỗi 100ms (không chặn việc đo)
- Gọi hàm `displayFrequency()` để hiển thị

### PHẦN 6: HÀM HIỂN THỊ

```cpp
void displayFrequency(float freq) {
  lcd.setCursor(0, 0);
  lcd.print("Frequency (Hz)");
```

**Hiển thị tiêu đề:**
- Dòng 1: "Frequency (Hz)"

```cpp
  if (!signalDetected) {
    lcd.print("No signal     ");
    return;
  }
```

**Kiểm tra tín hiệu:**
- Nếu không có tín hiệu → hiển thị "No signal"

```cpp
  if (freq < 10) {
    lcd.print("F: ");
    lcd.print(freq, 2);
    lcd.print(" Hz      ");
  }
```

**Định dạng hiển thị:**
- Tần số < 10 Hz: Hiển thị 2 chữ số thập phân (ví dụ: "F: 4.80 Hz")
- Tần số 10-99 Hz: Hiển thị 1 chữ số thập phân (ví dụ: "F: 48.0 Hz")
- Tần số 100-999 Hz: Hiển thị số nguyên (ví dụ: "F: 480 Hz")
- Tần số 1-9.9 kHz: Hiển thị với đơn vị kHz, 2 chữ số thập phân (ví dụ: "F: 4.80 kHz")
- Tần số >= 10 kHz: Hiển thị với đơn vị kHz, số nguyên (ví dụ: "F: 48 kHz")

---

## 🔄 LUỒNG HOẠT ĐỘNG CỦA HỆ THỐNG

```
1. NE555 tạo xung vuông
   └─> Pin 3 (OUTPUT) → Arduino D3

2. Mỗi cạnh lên (LOW → HIGH):
   └─> Arduino tự động gọi countPulseISR()
       └─> pulseCount++ (tăng bộ đếm)

3. Mỗi 1 giây:
   └─> Đọc pulseCount
       └─> Tính frequency = pulseCount / 1.0
           └─> Hiển thị lên LCD

4. Mỗi 100ms:
   └─> Cập nhật LCD với tần số mới nhất
```

---

## 📊 CÔNG THỨC TÍNH TẦN SỐ NE555

**Tần số NE555:**
```
f = 1.44 / ((R1 + 2×R2) × C1)
```

**Ví dụ với R1=1kΩ, R2=1kΩ, C1=1µF:**
```
f = 1.44 / ((1000 + 2×1000) × 0.000001)
f = 1.44 / (3000 × 0.000001)
f = 1.44 / 0.003
f = 480 Hz
```

**Duty Cycle (chu kỳ làm việc):**
```
Duty Cycle = (R1 + R2) / (R1 + 2×R2) × 100%
```

**Ví dụ:**
```
Duty Cycle = (1000 + 1000) / (1000 + 2×1000) × 100%
Duty Cycle = 2000 / 3000 × 100%
Duty Cycle = 66.67%
```

---

## ⚙️ CÁC CẤU HÌNH NE555 ĐỀ XUẤT

### Cấu hình 1: Tần số thấp (~4.8 Hz)
- R1 = 10kΩ
- R2 = 10kΩ
- C1 = 10µF

### Cấu hình 2: Tần số trung bình (~480 Hz) ⭐ Khuyến nghị
- R1 = 1kΩ
- R2 = 1kΩ
- C1 = 1µF

### Cấu hình 3: Tần số cao (~4.8 kHz)
- R1 = 1kΩ
- R2 = 1kΩ
- C1 = 100nF (0.1µF)

### Cấu hình 4: Điều chỉnh được
- R1 = 1kΩ (cố định)
- R2 = Biến trở 10kΩ
- C1 = 1µF
- Tần số: ~160 Hz đến ~480 Hz (điều chỉnh bằng biến trở)

---

## ⚠️ LƯU Ý QUAN TRỌNG

### 1. GND chung
- ⚠️ **BẮT BUỘC** phải nối GND chung giữa NE555 và Arduino
- Nếu không, tín hiệu sẽ không ổn định hoặc không đo được

### 2. Điện áp
- NE555 chạy tốt với 5V từ Arduino
- Không cần nguồn riêng

### 3. Chân ngắt
- Chỉ có **D2** (Interrupt 0) và **D3** (Interrupt 1) hỗ trợ ngắt ngoài
- Code hiện tại dùng **D3** (Interrupt 1)

### 4. Địa chỉ I2C LCD
- Nếu LCD không hiển thị, thử đổi địa chỉ:
  - Từ `0x27` → `0x3F` hoặc ngược lại
  - Sửa trong code: `#define LCD_ADDRESS 0x3F`

### 5. Nhiễu tín hiệu
- Nên đặt tụ 100nF gần Pin 8 (VCC) và Pin 1 (GND) của NE555
- Giúp lọc nhiễu nguồn, tín hiệu ổn định hơn

---

## 🔍 KIỂM TRA VÀ DEBUG

### 1. Kiểm tra NE555 hoạt động:
- Nối LED qua điện trở 220Ω vào Pin 3 (OUTPUT) của NE555
- LED sẽ nhấp nháy nếu NE555 hoạt động

### 2. Kiểm tra Serial Monitor:
- Mở Serial Monitor (9600 baud)
- Nên thấy: "=== He thong do tan so ==="
- Mỗi giây sẽ in: "Tan so: XXX Hz"

### 3. Kiểm tra LCD:
- LCD phải hiển thị "Frequency Meter" khi khởi động
- Sau đó hiển thị tần số hoặc "No signal"

### 4. Kiểm tra kết nối:
- Kiểm tra dây nối có lỏng không
- Kiểm tra GND chung
- Kiểm tra nguồn 5V

---

## 🆘 XỬ LÝ SỰ CỐ

### LCD không hiển thị:
1. Kiểm tra địa chỉ I2C (thử 0x27 hoặc 0x3F)
2. Kiểm tra dây SDA và SCL
3. Kiểm tra nguồn 5V

### Không đo được tần số:
1. Kiểm tra GND chung giữa NE555 và Arduino
2. Kiểm tra dây nối từ Pin 3 (NE555) đến D3 (Arduino)
3. Kiểm tra NE555 có hoạt động không (dùng LED test)

### Tần số hiển thị sai:
1. Kiểm tra giá trị R1, R2, C1
2. Kiểm tra tụ C1 có bị hỏng không
3. Kiểm tra nguồn cấp có ổn định không

### Tần số = 0 hoặc "No signal":
1. Kiểm tra NE555 có hoạt động không
2. Kiểm tra dây nối
3. Kiểm tra GND chung

---

## 📝 TÓM TẮT KẾT NỐI

```
LCD I2C:
  VCC → Arduino 5V
  GND → Arduino GND
  SDA → Arduino A4
  SCL → Arduino A5

NE555:
  Pin 8 (VCC) → Arduino 5V
  Pin 1 (GND) → Arduino GND
  Pin 3 (OUT) → Arduino D3
  Pin 4 (RESET) → Arduino 5V (hoặc để trống)
  Pin 5 (CONTROL) → [10nF] → GND
  Pin 6 (THRESHOLD) → Nối với Pin 2 (TRIGGER)
  Pin 7 (DISCHARGE) → Nối với Pin 6 qua R2
  Pin 2 (TRIGGER) → Nối với Pin 6 qua C1 xuống GND
```

---

## 🎯 KẾT LUẬN

Hệ thống này cho phép:
- ✅ Tạo tín hiệu PWM với tần số có thể điều chỉnh bằng NE555
- ✅ Đo tần số chính xác bằng Arduino
- ✅ Hiển thị tần số lên LCD với định dạng đẹp
- ✅ Phát hiện tự động khi mất tín hiệu
- ✅ Code ổn định, dễ hiểu và dễ bảo trì

**Chúc bạn thành công với dự án! 🎉**

