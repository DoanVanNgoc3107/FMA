#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

// ==================== CẤU HÌNH HỆ THỐNG ====================
// Địa chỉ I2C của LCD (thường là 0x27 hoặc 0x3F)
#define LCD_ADDRESS 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

// Chân đo tần số (chân 2 hoặc 3 hỗ trợ ngắt ngoài trên Arduino Uno)
#define FREQUENCY_PIN 2

#define BAUD_RATE 9600

// Thời gian lấy mẫu (ms) - mặc định 1 giây
#define SAMPLE_INTERVAL 1000

// Thời gian debounce: NE555 tạo xung vuông sạch,
// nên giảm về 0 để đo được tần số cao (>20kHz).
// Nếu để 50us, tần số tối đa chỉ đo được 20kHz.
#define DEBOUNCE_TIME 0

// Số mẫu để làm mượt kết quả
#define SMOOTHING_SAMPLES 5

// ==================== KHAI BÁO ĐỐI TƯỢNG ====================
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);

// ==================== BIẾN TOÀN CỤC ====================
volatile unsigned long pulseCount = 0; // Đếm xung trong ISR
volatile unsigned long lastPulseTime = 0; // Thời gian xung cuối cùng
volatile unsigned long lastInterruptTime = 0; // Thời gian ngắt cuối cùng
unsigned long lastSampleTime = 0; // Thời gian lấy mẫu cuối cùng
float frequency = 0.0; // Tần số tính được (Hz)
float frequencyHistory[SMOOTHING_SAMPLES]; // Lịch sử tần số
int historyIndex = 0; // Chỉ số hiện tại
bool signalDetected = false; // Cờ phát hiện tín hiệu
unsigned long displayUpdateTime = 0; // Thời gian cập nhật LCD
constexpr unsigned long DISPLAY_UPDATE_INTERVAL = 100; // Cập nhật LCD mỗi 100ms

// [THÊM] Biến nhận lệnh từ máy tính
String inputString = "";
bool stringComplete = false;

// ==================== HÀM NGẮT (ISR) VỚI LỌC NHIỄU ====================
void countPulseISR() {
    unsigned long currentInterruptTime = micros();

    // NE555 phát xung tần số cao, ta bỏ qua debounce mềm
    // Nếu vẫn muốn lọc nhiễu cho nút nhấn cơ, hãy tăng DEBOUNCE_TIME > 0
#if DEBOUNCE_TIME > 0
    if (currentInterruptTime - lastInterruptTime >= DEBOUNCE_TIME) {
        pulseCount++;
        lastPulseTime = millis();
        lastInterruptTime = currentInterruptTime;
    }
#else
    // Logic đếm trực tiếp cho tần số cao
    pulseCount++;
    lastPulseTime = millis();
#endif
}

// ==================== HÀM TÍNH TẦN SỐ TRUNG BÌNH (LÀM MỊN) ====================
float calculateSmoothedFrequency(float newFreq) {
    // Thêm tần số mới vào lịch sử
    frequencyHistory[historyIndex] = newFreq;
    historyIndex = (historyIndex + 1) % SMOOTHING_SAMPLES;

    // Tính trung bình
    float sum = 0.0;
    int validSamples = 0;
    for (int i = 0; i < SMOOTHING_SAMPLES; i++) {
        sum += frequencyHistory[i];
        validSamples++;
    }

    if (validSamples > 0) {
        return sum / SMOOTHING_SAMPLES; // Chia đều cho số mẫu cố định để ổn định hơn
    }
    return newFreq;
}

// ==================== HÀM KHỞI TẠO LCD ====================
bool initLCD() {
    lcd.init(); // Khoi tao LCD
    lcd.backlight(); // NEN DEN

    // Kiểm tra LCD có hoạt động không
    lcd.setCursor(0, 0);
    lcd.print("System Init...");
    delay(500);
    lcd.clear();

    return true;
}

// ==================== HÀM HIỂN THỊ TẦN SỐ LÊN LCD ====================
void displayFrequency(const float freq) {
    lcd.setCursor(0, 0);
    lcd.print("TAN SO: "); // Hiển thị tên module

    lcd.setCursor(0, 1);

    if (!signalDetected) {
        lcd.print("No Signal       ");
        return;
    }

    // Hiển thị tần số với định dạng đẹp
    if (freq < 10) {
        lcd.print("F: ");
        lcd.print(freq, 2);
        lcd.print(" Hz      ");
    } else if (freq < 100) {
        lcd.print("F: ");
        lcd.print(freq, 1);
        lcd.print(" Hz      ");
    } else if (freq < 1000) {
        lcd.print("F: ");
        lcd.print(static_cast<unsigned long>(freq));
        lcd.print(" Hz      ");
    } else if (freq < 1000000) {
        lcd.print("F: ");
        lcd.print(freq / 1000.0, 2);
        lcd.print(" kHz     ");
    } else {
        lcd.print("F: ");
        lcd.print(freq / 1000000.0, 3);
        lcd.print(" MHz     ");
    }
}

// ==================== [THÊM] XỬ LÝ LỆNH TỪ MÁY TÍNH ====================
void handleSerialCommands() {
    if (stringComplete) {
        inputString.trim(); // Loại bỏ khoảng trắng thừa
        inputString.toUpperCase(); // Chuyển về chữ hoa

        if (inputString == "RESET") {
            pulseCount = 0;
            for (int i = 0; i < SMOOTHING_SAMPLES; i++) frequencyHistory[i] = 0;
            Serial.println("MSG: System Reset Done");
        } else if (inputString == "GET") {
            // Gửi dữ liệu ngay lập tức khi máy tính yêu cầu
            Serial.print("DATA,");
            Serial.println(frequency, 2);
        }

        // Reset chuỗi nhận
        inputString = "";
        stringComplete = false;
    }

    // Đọc dữ liệu Serial đến
    while (Serial.available()) {
        char inChar = (char) Serial.read();
        if (inChar == '\n') {
            stringComplete = true;
        } else {
            inputString += inChar;
        }
    }
}

// ==================== HÀM SETUP ====================
void setup() {
    // Khởi tạo Serial
    Serial.begin(BAUD_RATE);

    // [THÊM] Header cho Serial Plotter (Công cụ vẽ đồ thị của Arduino)
    Serial.println("Label:Raw Freq,Smoothed Freq");

    Serial.println("=== ĐO TAN SO ===");

    if (initLCD()) {
        Serial.println("LCD: OK");
    } else {
        Serial.println("LCD: ERROR");
    }

    // [SỬA ĐỔI] Dùng INPUT_PULLUP để tránh nhiễu khi chưa cắm dây tín hiệu
    pinMode(FREQUENCY_PIN, INPUT_PULLUP);

    // Khởi tạo mảng lịch sử tần số
    for (int i = 0; i < SMOOTHING_SAMPLES; i++) {
        frequencyHistory[i] = 0.0;
    }

    attachInterrupt(digitalPinToInterrupt(FREQUENCY_PIN), countPulseISR, RISING);

    Serial.println("He thong san sang!");
    lcd.setCursor(0, 0);
    lcd.print("Freq Meter Ready");
    lcd.setCursor(0, 1);
    lcd.print("Waiting signal..");
    lcd.clear();

    lastSampleTime = millis();
    lastPulseTime = millis();
    lastInterruptTime = micros();
    displayUpdateTime = millis();
}


// ==================== HÀM LOOP ====================
void loop() {
    // [THÊM] Luôn lắng nghe lệnh từ máy tính
    handleSerialCommands();

    unsigned long currentTime = millis();

    // Kiểm tra xem đã đến thời gian lấy mẫu chưa
    if (currentTime - lastSampleTime >= SAMPLE_INTERVAL) {
        // Tắt ngắt để đọc an toàn
        noInterrupts();
        unsigned long count = pulseCount;
        unsigned long pulseTime = lastPulseTime;
        pulseCount = 0; // Reset bộ đếm
        interrupts();

        // Tính toán tần số
        float timeElapsed = (currentTime - lastSampleTime) / 1000.0;
        float rawFrequency = count / timeElapsed;

        // Làm mượt tần số
        frequency = calculateSmoothedFrequency(rawFrequency);

        // Kiểm tra tín hiệu (Timeout 2s)
        if (currentTime - pulseTime > 2000) {
            signalDetected = false;
            frequency = 0.0;
            rawFrequency = 0.0; // Reset cả tần số thô
            // Reset lịch sử khi mất tín hiệu
            for (int i = 0; i < SMOOTHING_SAMPLES; i++) {
                frequencyHistory[i] = 0.0;
            }
        } else {
            signalDetected = true;
        }

        lastSampleTime = currentTime;

        // In kết quả ra Serial theo định dạng chuẩn để máy tính dễ đọc
        // Định dạng 1: Dành cho người đọc (Verbose)
        Serial.print("Info: Xung=");
        Serial.print(count);
        Serial.print(" | Raw=");
        Serial.print(rawFrequency, 1);
        Serial.print("Hz | Filtered=");
        Serial.print(frequency, 1);
        Serial.println("Hz");

        // Định dạng 2: Dành cho máy tính vẽ đồ thị (Serial Plotter)
        // Format: RawValue, FilteredValue
        Serial.print(rawFrequency);
        Serial.print(",");
        Serial.println(frequency);
    }

    // Cập nhật LCD định kỳ
    if (currentTime - displayUpdateTime >= DISPLAY_UPDATE_INTERVAL) {
        displayFrequency(frequency);
        displayUpdateTime = currentTime;
    }
}
