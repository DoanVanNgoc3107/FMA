# HƯỚNG DẪN KẾT NỐI MODULE ĐO TẦN SỐ VỚI ARDUINO UNO R3

## 📋 DANH SÁCH LINH KIỆN CẦN THIẾT

1. **Arduino Uno R3** (1 cái)
2. **LCD I2C 16x2** (1 cái) - Module LCD có module I2C tích hợp
3. **Mạch NE555** - Tạo tín hiệu PWM/tần số (xem file `HUONG_DAN_NE555.md` để biết cách lắp)
4. **Dây nối (jumper wires)** - Để kết nối các linh kiện
5. **Nguồn cấp cho Arduino** - USB cable hoặc adapter 7-12V
6. **Tín hiệu cần đo tần số** - Từ mạch NE555 hoặc nguồn tín hiệu khác

---

## 🔌 SƠ ĐỒ KẾT NỐI CHI TIẾT

### 1. KẾT NỐI LCD I2C VỚI ARDUINO UNO R3

LCD I2C có 4 chân chính cần kết nối:

| Chân LCD I2C | Chân Arduino Uno R3 | Mô tả |
|--------------|---------------------|-------|
| **VCC** | **5V** | Nguồn dương 5V |
| **GND** | **GND** | Mass (đất chung) |
| **SDA** | **A4** | Dữ liệu I2C (Serial Data) |
| **SCL** | **A5** | Xung đồng bộ I2C (Serial Clock) |

**Lưu ý quan trọng:**
- Trên Arduino Uno R3, chân I2C cố định là:
  - **SDA = A4** (chân analog 4)
  - **SCL = A5** (chân analog 5)
- Không thể thay đổi sang chân khác!

### 2. KẾT NỐI TÍN HIỆU TẦN SỐ VỚI ARDUINO UNO R3

#### A. Kết nối từ mạch NE555 (khuyến nghị):

| NE555 | Chân Arduino Uno R3 | Mô tả |
|-------|---------------------|-------|
| **Pin 3 (OUTPUT)** | **Digital Pin 3** | Tín hiệu PWM từ NE555 (hoặc D2) |
| **Pin 1 (GND)** | **GND** | Mass chung (bắt buộc phải nối) |
| **Pin 8 (VCC)** | **5V** | Nguồn cấp cho NE555 |

**Xem file `HUONG_DAN_NE555.md` để biết cách lắp mạch NE555 chi tiết!**

#### B. Kết nối từ nguồn tín hiệu khác:

| Tín hiệu đầu vào | Chân Arduino Uno R3 | Mô tả |
|------------------|---------------------|-------|
| **Tín hiệu (+) (Signal)** | **Digital Pin 3** | Tín hiệu cần đo tần số (hoặc D2) |
| **GND (Mass)** | **GND** | Mass chung (bắt buộc phải nối) |

**Lưu ý quan trọng:**
- Code hiện tại sử dụng **Digital Pin 3** (Interrupt 1)
- Có thể đổi sang **Digital Pin 2** (Interrupt 0) nếu muốn
- Nếu đổi chân, nhớ sửa code: `#define FREQUENCY_PIN 2`
- Chỉ có 2 chân này hỗ trợ ngắt ngoài trên Arduino Uno
- **BẮT BUỘC phải nối GND chung** giữa nguồn tín hiệu và Arduino

---

## 📐 SƠ ĐỒ KẾT NỐI TỔNG QUAN

```
                    ARDUINO UNO R3
    ┌─────────────────────────────────────────┐
    │                                         │
    │  [5V] ────────────┬─────────────────┐   │
    │  [GND] ───────┬───┼───┬─────────────┼─┐ │
    │  [A4] ────────┼───┼───┼───┐         │ │ │
    │  [A5] ────────┼───┼───┼───┼───┐     │ │ │
    │  [D3] ────────┼───┼───┼───┼───┼───┐ │ │ │
    │               │   │   │   │   │   │ │ │ │
    └───────────────┼───┼───┼───┼───┼───┼─┼─┼─┘
                    │   │   │   │   │   │ │ │
                    │   │   │   │   │   │ │ │
         ┌──────────┘   │   │   │   │   │ │ │
         │              │   │   │   │   │ │ │
    ┌────▼────┐    ┌────▼───▼───▼───▼───▼─▼─▼───┐
    │  LCD    │    │      MẠCH NE555            │
    │  I2C    │    │                            │
    │         │    │  Pin 8 (VCC) ──────────────┘
    │  VCC ───┘    │  Pin 1 (GND) ──────────────┘
    │  GND ────────┘  Pin 3 (OUT) ──────────────┘
    │  SDA ────────┘
    │  SCL ────────┘
    └─────────────┘
```

---

## 🔧 HƯỚNG DẪN KẾT NỐI TỪNG BƯỚC

### Bước 1: Kết nối LCD I2C
1. Lấy dây jumper, nối **VCC** của LCD I2C vào **5V** của Arduino
2. Nối **GND** của LCD I2C vào **GND** của Arduino
3. Nối **SDA** của LCD I2C vào **A4** của Arduino
4. Nối **SCL** của LCD I2C vào **A5** của Arduino

### Bước 2: Kết nối mạch NE555
**Xem file `HUONG_DAN_NE555.md` để biết cách lắp mạch NE555 chi tiết!**

Sau khi lắp xong mạch NE555:
1. Nối **Pin 8 (VCC)** của NE555 vào **5V** của Arduino
2. Nối **Pin 1 (GND)** của NE555 vào **GND** của Arduino
3. Nối **Pin 3 (OUTPUT)** của NE555 vào **Digital Pin 3** của Arduino
   - ⚠️ **QUAN TRỌNG**: Phải nối GND chung, nếu không sẽ không đo được!

**Hoặc nếu dùng nguồn tín hiệu khác:**
1. Nối **tín hiệu dương (+)** của nguồn tín hiệu vào **Digital Pin 3** của Arduino
2. Nối **GND (mass)** của nguồn tín hiệu vào **GND** của Arduino

### Bước 3: Cấp nguồn cho Arduino
1. Cắm cáp USB vào Arduino và máy tính, HOẶC
2. Cắm adapter nguồn 7-12V vào jack DC của Arduino

---

## ⚙️ CẤU HÌNH ĐỊA CHỈ I2C CỦA LCD

Nếu LCD không hiển thị, có thể địa chỉ I2C không đúng. Thử các bước sau:

1. **Kiểm tra địa chỉ I2C hiện tại:**
   - Mở Serial Monitor (115200 baud)
   - Upload code I2C Scanner để tìm địa chỉ

2. **Các địa chỉ I2C phổ biến:**
   - `0x27` (phổ biến nhất)
   - `0x3F` (cũng rất phổ biến)
   - `0x20`, `0x38`, `0x21` (ít gặp hơn)

3. **Thay đổi địa chỉ trong code:**
   - Mở file `src/main.cpp`
   - Tìm dòng: `#define LCD_ADDRESS 0x27`
   - Đổi thành địa chỉ phù hợp (ví dụ: `0x3F`)

---

## 📊 THÔNG SỐ KỸ THUẬT

### Tín hiệu đầu vào:
- **Điện áp:** 0-5V (Arduino Uno chỉ chịu được tối đa 5V!)
- **Tần số đo được:** Từ vài Hz đến khoảng 50-100 kHz (tùy thuộc vào code)
- **Dạng sóng:** Square wave (xung vuông) hoặc sine wave (sóng sin) đều được

### LCD I2C:
- **Kích thước:** 16x2 (16 ký tự, 2 dòng)
- **Giao tiếp:** I2C (chỉ cần 4 dây)
- **Điện áp:** 5V

---

## ⚠️ LƯU Ý QUAN TRỌNG

1. **Điện áp tín hiệu:**
   - ⚠️ **KHÔNG được vượt quá 5V!** Arduino sẽ bị hỏng!
   - Nếu tín hiệu > 5V, cần dùng mạch phân áp hoặc optocoupler

2. **Nối GND chung:**
   - ⚠️ **BẮT BUỘC** phải nối GND của nguồn tín hiệu với GND của Arduino
   - Nếu không nối GND chung, tín hiệu sẽ không ổn định hoặc không đo được

3. **Chân I2C:**
   - Trên Arduino Uno R3, I2C cố định ở A4 (SDA) và A5 (SCL)
   - Không thể thay đổi sang chân khác

4. **Chân ngắt:**
   - Chỉ có chân **D2** (Interrupt 0) và **D3** (Interrupt 1) hỗ trợ ngắt ngoài
   - Nếu muốn đổi chân, chỉ có thể chọn D2 hoặc D3

5. **Nhiễu tín hiệu:**
   - Nếu tín hiệu bị nhiễu, có thể cần thêm tụ lọc hoặc mạch debounce
   - Code đã sử dụng INPUT_PULLUP để giảm nhiễu

---

## 🔍 KIỂM TRA KẾT NỐI

Sau khi kết nối xong, kiểm tra:

1. **LCD có sáng không?**
   - Nếu không sáng: Kiểm tra nguồn 5V và GND
   - Nếu sáng nhưng không hiển thị: Kiểm tra địa chỉ I2C

2. **Serial Monitor có thông báo không?**
   - Mở Serial Monitor (9600 baud)
   - Nên thấy: "=== He thong do tan so ==="

3. **LCD có hiển thị "Frequency Meter" không?**
   - Nếu có: Kết nối LCD đúng
   - Nếu không: Kiểm tra lại địa chỉ I2C

4. **Có đo được tần số không?**
   - Kết nối tín hiệu vào D3
   - LCD sẽ hiển thị tần số hoặc "No signal"

---

## 📝 TÓM TẮT KẾT NỐI NHANH

```
LCD I2C:
  VCC → 5V
  GND → GND
  SDA → A4
  SCL → A5

Mạch NE555:
  Pin 8 (VCC) → 5V
  Pin 1 (GND) → GND
  Pin 3 (OUT) → D3
  Pin 4 (RESET) → 5V (hoặc để trống)

Hoặc tín hiệu khác:
  Signal (+) → D3
  GND (-) → GND
```

**Xem file `HUONG_DAN_NE555.md` để biết cách lắp mạch NE555!**

---

## 🆘 XỬ LÝ SỰ CỐ

### LCD không hiển thị:
- Kiểm tra địa chỉ I2C (thử 0x27 hoặc 0x3F)
- Kiểm tra dây nối SDA và SCL
- Kiểm tra nguồn 5V

### Không đo được tần số:
- Kiểm tra đã nối GND chung chưa
- Kiểm tra tín hiệu có > 5V không (sẽ hỏng Arduino!)
- Kiểm tra dây nối vào chân D3

### Tần số hiển thị sai:
- Kiểm tra tín hiệu có ổn định không
- Có thể cần thêm mạch lọc nhiễu
- Kiểm tra nguồn cấp có ổn định không

---

**Chúc bạn thành công! 🎉**

