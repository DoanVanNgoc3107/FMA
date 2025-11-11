# HƯỚNG DẪN KẾT NỐI MẠCH NE555 VỚI ARDUINO UNO R3

## 📋 LINH KIỆN CẦN THIẾT CHO MẠCH NE555

1. **IC NE555** (1 cái)
2. **Điện trở R1** - 1kΩ đến 10kΩ (điều chỉnh tần số)
3. **Điện trở R2** - 1kΩ đến 10kΩ (điều chỉnh tần số)
4. **Tụ điện C1** - 10nF đến 100µF (điều chỉnh tần số)
5. **Biến trở 10kΩ** (tùy chọn - để điều chỉnh tần số)
6. **Tụ lọc 100nF** (tùy chọn - để ổn định nguồn)
7. **Breadboard** và dây nối

---

## 🔌 SƠ ĐỒ MẠCH NE555 (ASTABLE MODE)

NE555 hoạt động ở chế độ Astable (dao động tự do) để tạo tín hiệu PWM/xung vuông.

### Sơ đồ chân NE555:
```
        ┌─────┐
   GND ─┤1   8├─ VCC (+5V)
  TRIG ─┤2   7├─ DISCHARGE
  OUT  ─┤3   6├─ THRESHOLD
  RESET─┤4   5├─ CONTROL (không dùng, nối tụ 10nF xuống GND)
        └─────┘
```

### Sơ đồ mạch đầy đủ:

```
                    +5V
                     │
                     ├───[R1]───┐
                     │           │
                     │           ├───[R2]───┐
                     │           │           │
                     │           │           │
                    ┌┴─┐        │          ┌┴─┐
                    │C1│        │          │C2│ (100nF - tùy chọn)
                    └┬─┘        │          └┬─┘
                     │          │           │
                     │          │           │
        ┌────────────┼──────────┼───────────┼───┐
        │            │          │           │   │
        │         ┌──▼──┐       │           │   │
        │      ┌──┤ 2   │       │           │   │
        │      │  │     │       │           │   │
        │      │  │NE555│       │           │   │
        │      │  │     │       │           │   │
        │      │  │  3  ├───────┼───────────┼───┼─── OUTPUT (→ Arduino D2)
        │      │  │     │       │           │   │
        │      │  └──┬──┘       │           │   │
        │      │     │          │           │   │
        │      │     │ 7        │           │   │
        │      │     └──────────┘           │   │
        │      │                            │   │
        │      │ 4 (RESET) ────────────────┼───┘
        │      │                            │
        │      │ 5 (CONTROL) ───[10nF]─── GND
        │      │                            │
        │      │ 1 (GND) ───────────────────┘
        │      │                            │
        │      └───────────────────────────┘
        │                                  │
        └──────────────────────────────────┴─── GND (→ Arduino GND)
```

### Sơ đồ đơn giản hơn (dễ làm):

```
VCC (+5V) ──[R1]──┬──[R2]──┬── GND
                   │        │
                   │       [C1]
                   │        │
              ┌────┴────┐   │
              │  NE555  │   │
              │         │   │
              │ Pin 2 ──┼───┘
              │ Pin 6 ──┼───┘
              │ Pin 7 ──┼───┘
              │ Pin 3 ──┼─────── OUTPUT (→ Arduino D2)
              │         │
              │ Pin 1 ──┴─── GND
              │ Pin 4 ────── VCC (hoặc để trống)
              │ Pin 5 ───[10nF]── GND
              └─────────┘
```

---

## 📐 CÔNG THỨC TÍNH TẦN SỐ NE555

Tần số được tính theo công thức:

**f = 1.44 / ((R1 + 2×R2) × C1)**

Trong đó:
- **f** = Tần số (Hz)
- **R1, R2** = Điện trở (Ω)
- **C1** = Tụ điện (F)

### Chu kỳ (Duty Cycle):
**Duty Cycle = (R1 + R2) / (R1 + 2×R2) × 100%**

---

## 🔧 CÁC GIÁ TRỊ LINH KIỆN ĐỀ XUẤT

### Cấu hình 1: Tần số thấp (1-100 Hz)
- **R1** = 10kΩ
- **R2** = 10kΩ  
- **C1** = 10µF
- **Tần số ≈ 4.8 Hz**

### Cấu hình 2: Tần số trung bình (100-1000 Hz)
- **R1** = 1kΩ
- **R2** = 1kΩ
- **C1** = 1µF
- **Tần số ≈ 480 Hz**

### Cấu hình 3: Tần số cao (1-10 kHz)
- **R1** = 1kΩ
- **R2** = 1kΩ
- **C1** = 100nF (0.1µF)
- **Tần số ≈ 4.8 kHz**

### Cấu hình 4: Tần số rất cao (10-100 kHz)
- **R1** = 1kΩ
- **R2** = 1kΩ
- **C1** = 10nF (0.01µF)
- **Tần số ≈ 48 kHz**

### Cấu hình 5: Điều chỉnh được (dùng biến trở)
- **R1** = 1kΩ (cố định)
- **R2** = Biến trở 10kΩ
- **C1** = 1µF
- **Tần số:** Điều chỉnh được từ ~160 Hz đến ~480 Hz

---

## 🔌 KẾT NỐI VỚI ARDUINO UNO R3

### Kết nối nguồn:
| NE555 | Arduino Uno R3 |
|-------|----------------|
| **VCC (Pin 8)** | **5V** |
| **GND (Pin 1)** | **GND** |

### Kết nối tín hiệu:
| NE555 | Arduino Uno R3 |
|-------|----------------|
| **OUTPUT (Pin 3)** | **Digital Pin 3** (hoặc D2) |
| **GND (Pin 1)** | **GND** (đã nối ở trên) |

### Kết nối RESET (tùy chọn):
| NE555 | Arduino Uno R3 |
|-------|----------------|
| **RESET (Pin 4)** | **5V** hoặc để trống (nếu để trống, mạch luôn chạy) |

---

## 📋 HƯỚNG DẪN LẮP MẠCH TỪNG BƯỚC

### Bước 1: Lắp NE555 lên breadboard
1. Đặt IC NE555 lên breadboard (chú ý hướng chân)
2. Chân 1 (GND) ở góc dưới bên trái

### Bước 2: Kết nối nguồn
1. Nối **Pin 8 (VCC)** của NE555 vào **5V** của Arduino
2. Nối **Pin 1 (GND)** của NE555 vào **GND** của Arduino

### Bước 3: Lắp mạch dao động
1. Nối **Pin 8 (VCC)** qua **R1** đến **Pin 7 (DISCHARGE)**
2. Nối **Pin 7 (DISCHARGE)** qua **R2** đến **Pin 6 (THRESHOLD)**
3. Nối **Pin 6 (THRESHOLD)** với **Pin 2 (TRIGGER)**
4. Nối **Pin 2 (TRIGGER)** và **Pin 6 (THRESHOLD)** qua **C1** xuống **GND**

### Bước 4: Kết nối tụ CONTROL (tùy chọn nhưng nên có)
1. Nối **Pin 5 (CONTROL)** qua tụ **10nF** xuống **GND**

### Bước 5: Kết nối RESET
1. Nối **Pin 4 (RESET)** vào **5V** (hoặc để trống)

### Bước 6: Kết nối OUTPUT
1. Nối **Pin 3 (OUTPUT)** của NE555 vào **Digital Pin 3** của Arduino (code hiện tại dùng D3)
2. Đảm bảo **GND chung** giữa NE555 và Arduino
3. **Lưu ý:** Có thể dùng Digital Pin 2 nếu muốn, nhưng nhớ sửa code

---

## 🎛️ ĐIỀU CHỈNH TẦN SỐ

### Cách 1: Thay đổi tụ C1
- Tăng C1 → Giảm tần số
- Giảm C1 → Tăng tần số

### Cách 2: Thay đổi điện trở R1, R2
- Tăng R1, R2 → Giảm tần số
- Giảm R1, R2 → Tăng tần số

### Cách 3: Dùng biến trở (khuyến nghị)
- Thay **R2** bằng biến trở 10kΩ
- Xoay biến trở để điều chỉnh tần số
- Tần số sẽ thay đổi liên tục

---

## ⚠️ LƯU Ý QUAN TRỌNG

1. **Điện áp:**
   - NE555 chạy tốt với 5V từ Arduino
   - Không cần nguồn riêng

2. **GND chung:**
   - ⚠️ **BẮT BUỘC** phải nối GND chung giữa NE555 và Arduino
   - Nếu không, tín hiệu sẽ không ổn định

3. **Tụ CONTROL (Pin 5):**
   - Nên nối tụ 10nF từ Pin 5 xuống GND
   - Giúp ổn định tần số, giảm nhiễu

4. **RESET (Pin 4):**
   - Nếu nối vào 5V: Mạch luôn hoạt động
   - Nếu để trống: Mạch vẫn hoạt động (có điện trở kéo lên nội bộ)
   - Có thể dùng chân Arduino để điều khiển bật/tắt

5. **Dòng ra:**
   - NE555 có thể cung cấp dòng ra tối đa ~200mA
   - Đủ để kích hoạt ngắt của Arduino

6. **Nhiễu:**
   - Nên đặt tụ 100nF gần Pin 8 (VCC) và Pin 1 (GND) để lọc nhiễu nguồn
   - Giúp tín hiệu ổn định hơn

---

## 📊 BẢNG TRA CỨU NHANH TẦN SỐ

| R1 | R2 | C1 | Tần số (Hz) |
|----|----|----|-------------|
| 10kΩ | 10kΩ | 10µF | ~4.8 |
| 10kΩ | 10kΩ | 1µF | ~48 |
| 1kΩ | 1kΩ | 1µF | ~480 |
| 1kΩ | 1kΩ | 100nF | ~4,800 |
| 1kΩ | 1kΩ | 10nF | ~48,000 |
| 10kΩ | Biến trở 10kΩ | 1µF | 160-480 (điều chỉnh) |

---

## 🔍 KIỂM TRA MẠCH NE555

### Kiểm tra bằng LED:
1. Nối LED qua điện trở 220Ω vào **Pin 3 (OUTPUT)** của NE555
2. LED sẽ nhấp nháy với tần số của mạch
3. Nếu LED không nhấp nháy: Kiểm tra lại mạch

### Kiểm tra bằng Arduino:
1. Upload code đo tần số vào Arduino
2. Kết nối **Pin 3 (OUTPUT)** của NE555 vào **Digital Pin 3** của Arduino
3. Mở Serial Monitor hoặc xem LCD
4. Tần số hiển thị phải khớp với tính toán

### Kiểm tra bằng Oscilloscope (nếu có):
1. Nối đầu dò vào **Pin 3 (OUTPUT)** của NE555
2. Quan sát dạng sóng xung vuông
3. Đo tần số và so sánh với tính toán

---

## 🆘 XỬ LÝ SỰ CỐ

### NE555 không hoạt động:
- Kiểm tra nguồn 5V và GND
- Kiểm tra hướng chân IC (có thể lắp ngược)
- Kiểm tra RESET (Pin 4) có nối đúng không

### Tần số không đúng:
- Kiểm tra giá trị R1, R2, C1
- Kiểm tra tụ C1 có bị hỏng không
- Thử thay tụ mới

### Tín hiệu không ổn định:
- Thêm tụ 100nF lọc nhiễu nguồn
- Kiểm tra GND chung
- Kiểm tra dây nối có lỏng không

### Arduino không đo được:
- Kiểm tra dây nối từ Pin 3 (NE555) đến D2 (Arduino)
- Kiểm tra GND chung
- Kiểm tra tín hiệu có > 5V không (không nên)

---

## 📝 TÓM TẮT KẾT NỐI NHANH

```
NE555:
  Pin 8 (VCC) → Arduino 5V
  Pin 1 (GND) → Arduino GND
  Pin 3 (OUT) → Arduino D2
  Pin 4 (RESET) → Arduino 5V (hoặc để trống)
  Pin 5 (CONTROL) → [10nF] → GND

Mạch dao động:
  VCC → [R1] → Pin 7 → [R2] → Pin 6
  Pin 6 ──┬── Pin 2
          │
         [C1]
          │
         GND
```

---

## 🎯 ỨNG DỤNG

Mạch NE555 này có thể dùng để:
- ✅ Tạo tín hiệu PWM để test module đo tần số
- ✅ Tạo xung clock cho các mạch số
- ✅ Tạo tín hiệu điều khiển LED nhấp nháy
- ✅ Tạo tín hiệu test cho các mạch khác
- ✅ Học về dao động và tần số

---

**Chúc bạn thành công với mạch NE555! 🎉**

