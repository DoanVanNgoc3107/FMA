/**
* ============================================================================
 *  Project     : FREQUENCY MEASUREMENT MODULE DESIGN
 *  Author      : Doan Van Ngoc – Student ID: 104317
 *  Version     : 3.3.17
 *
 *  Team Members:
 *      - Nguyen Ba Khiem – Student ID: …
 *      - Nguyen Truong Giang – Student ID: …
 *      - Tran Nam – Student ID: …
 *
 *  Instructor  : M.Sc. Vu Thi Thu
 *
 *  Notice      : © Doan Van Ngoc.
 *                Do not copy or use without permission.
 * ============================================================================
 */


#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ==================== CẤU HÌNH HỆ THỐNG ====================
// Địa chỉ I2C của LCD (thường là 0x27 hoặc 0x3F)
#define LCD_ADDRESS 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

// Chân đo tần số (chân 2 hoặc 3 hỗ trợ ngắt ngoài trên Arduino Uno)
#define FREQUENCY_PIN 2

// Thời gian lấy mẫu (ms) - mặc định 1 giây
#define SAMPLE_INTERVAL 1000

// Thời gian debounce để lọc nhiễu (microseconds)
#define DEBOUNCE_TIME 50

// Số mẫu để làm mượt kết quả
#define SMOOTHING_SAMPLES 5

// ==================== KHAI BÁO ĐỐI TƯỢNG ====================
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);

// ==================== BIẾN TOÀN CỤC ====================
volatile unsigned long pulseCount = 0; // Đếm xung trong ISR
volatile unsigned long lastPulseTime = 0; // Thời gian xung cuối cùng (để phát hiện mất tín hiệu)
volatile unsigned long lastInterruptTime = 0; // Thời gian ngắt cuối cùng (để debounce)
unsigned long lastSampleTime = 0; // Thời gian lấy mẫu cuối cùng
float frequency = 0.0; // Tần số tính được (Hz)
float frequencyHistory[SMOOTHING_SAMPLES]; // Lịch sử tần số để làm mượt
int historyIndex = 0; // Chỉ số hiện tại trong lịch sử
bool signalDetected = false; // Cờ phát hiện tín hiệu
unsigned long displayUpdateTime = 0; // Thời gian cập nhật LCD
constexpr unsigned long DISPLAY_UPDATE_INTERVAL = 100; // Cập nhật LCD mỗi 100ms

// ==================== HÀM NGẮT (ISR) VỚI LỌC NHIỄU ====================
void countPulseISR()
{
    unsigned long currentInterruptTime = micros();

    // Lọc nhiễu: chỉ đếm nếu thời gian từ lần ngắt trước >= DEBOUNCE_TIME
    if (currentInterruptTime - lastInterruptTime >= DEBOUNCE_TIME)
    {
        pulseCount++;
        lastPulseTime = millis();
        lastInterruptTime = currentInterruptTime;
    }
}

// ==================== HÀM TÍNH TẦN SỐ TRUNG BÌNH (LÀM MỊN) ====================
float calculateSmoothedFrequency(float newFreq)
{
    // Thêm tần số mới vào lịch sử
    frequencyHistory[historyIndex] = newFreq;
    historyIndex = (historyIndex + 1) % SMOOTHING_SAMPLES;

    // Tính trung bình
    float sum = 0.0;
    int validSamples = 0;
    for (int i = 0; i < SMOOTHING_SAMPLES; i++)
    {
        if (frequencyHistory[i] > 0)
        {
            sum += frequencyHistory[i];
            validSamples++;
        }
    }

    if (validSamples > 0)
    {
        return sum / validSamples;
    }
    return newFreq;
}

// ==================== HÀM KHỞI TẠO LCD ====================
/**
 * @brief Ham nay su dung de khoi tao lcd (neu co loi se tra ve false)
 * @return bool - true neu khoi tao thanh cong, false neu loi
 */
bool initLCD()
{
    lcd.init();
    lcd.backlight();

    // Kiểm tra LCD có hoạt động không
    lcd.setCursor(0, 0);
    lcd.print("Initializing...");
    delay(500);
    lcd.clear();

    return true;
}

// ==================== HÀM HIỂN THỊ TẦN SỐ LÊN LCD ====================
/**
 * @brief 
 * @param freq
 */
void displayFrequency(const float freq)
{
    lcd.setCursor(0, 0);
    lcd.print("Frequency (Hz)");

    lcd.setCursor(0, 1);

    if (!signalDetected)
    {
        lcd.print("No signal     ");
        return;
    }

    // Hiển thị tần số với định dạng đẹp
    if (freq < 10)
    {
        // Tần số < 10 Hz: hiển thị 2 chữ số thập phân
        lcd.print("F: ");
        lcd.print(freq, 2);
        lcd.print(" Hz      ");
    }
    else if (freq < 100)
    {
        // Tần số 10-99 Hz: hiển thị 1 chữ số thập phân
        lcd.print("F: ");
        lcd.print(freq, 1);
        lcd.print(" Hz      ");
    }
    else if (freq < 1000)
    {
        // Tần số 100-999 Hz: hiển thị số nguyên
        lcd.print("F: ");
        lcd.print(static_cast<unsigned long>(freq));
        lcd.print(" Hz      ");
    }
    else if (freq < 10000)
    {
        // Tần số 1-9.9 kHz: hiển thị với đơn vị kHz
        lcd.print("F: ");
        lcd.print(freq / 1000.0, 2);
        lcd.print(" kHz     ");
    }
    else
    {
        // Tần số >= 10 kHz: hiển thị với đơn vị kHz
        lcd.print("F: ");
        lcd.print(static_cast<unsigned long>(freq / 1000));
        lcd.print(" kHz     ");
    }
}

// ==================== HÀM SETUP ====================
void setup()
{
    // Khởi tạo Serial để debug
    Serial.begin(9600);
    while (!Serial && millis() < 3000)
    {
        ; // Đợi Serial sẵn sàng (tối đa 3 giây)
    }

    Serial.println("=== He thong do tan so ===");
    Serial.println("Khoi tao he thong...");

    // Khởi tạo LCD
    if (initLCD())
    {
        Serial.println("LCD da khoi tao thanh cong");
    }
    else
    {
        Serial.println("Loi: Khong the khoi tao LCD!");
    }

    // Cấu hình chân đo tần số - dùng INPUT thay vì INPUT_PULLUP
    // Nếu tín hiệu từ NE555 đã có mức logic rõ ràng (0V/5V)
    pinMode(FREQUENCY_PIN, INPUT);

    // Khởi tạo mảng lịch sử tần số
    for (int i = 0; i < SMOOTHING_SAMPLES; i++)
    {
        frequencyHistory[i] = 0.0;
    }

    // Gắn hàm ngắt cho chân đo tần số
    // RISING: đếm khi tín hiệu chuyển từ LOW -> HIGH
    attachInterrupt(digitalPinToInterrupt(FREQUENCY_PIN), countPulseISR, RISING);

    Serial.println("He thong san sang!");
    Serial.println("Dang do tan so...");
    Serial.println();

    // Hiển thị màn hình chào mừng
    lcd.setCursor(0, 0);
    lcd.print("Frequency Meter");
    lcd.setCursor(0, 1);
    lcd.print("NE555 Ready...");

    // Khởi tạo biến thời gian
    lastSampleTime = millis();
    lastPulseTime = millis();
    lastInterruptTime = micros();
    displayUpdateTime = millis();
}

// ==================== HÀM LOOP ====================
void loop()
{
    unsigned long currentTime = millis();

    // Kiểm tra xem đã đến thời gian lấy mẫu chưa
    if (currentTime - lastSampleTime >= SAMPLE_INTERVAL)
    {
        // Tắt ngắt để đọc an toàn
        noInterrupts();
        unsigned long count = pulseCount;
        unsigned long pulseTime = lastPulseTime;
        pulseCount = 0; // Reset bộ đếm
        interrupts();

        // Tính toán tần số
        // Tần số = số xung / thời gian (giây)
        float timeElapsed = (currentTime - lastSampleTime) / 1000.0; // Chuyển sang giây
        float rawFrequency = count / timeElapsed;

        // Làm mượt tần số bằng cách tính trung bình
        frequency = calculateSmoothedFrequency(rawFrequency);

        // Kiểm tra xem có tín hiệu không
        // Nếu không có xung nào trong 2 giây, coi như mất tín hiệu
        if (currentTime - pulseTime > 2000)
        {
            signalDetected = false;
            frequency = 0.0;
            // Reset lịch sử khi mất tín hiệu
            for (int i = 0; i < SMOOTHING_SAMPLES; i++)
            {
                frequencyHistory[i] = 0.0;
            }
            historyIndex = 0;
        }
        else
        {
            signalDetected = true;
        }

        // Cập nhật thời gian lấy mẫu
        lastSampleTime = currentTime;

        // In kết quả ra Serial
        Serial.print("Xung dem duoc: ");
        Serial.print(count);
        Serial.print(" | Tan so thô: ");
        Serial.print(rawFrequency, 2);
        Serial.print(" Hz | Tan so da loc: ");
        if (signalDetected)
        {
            Serial.print(frequency, 2);
            Serial.println(" Hz");
        }
        else
        {
            Serial.println("Khong co tin hieu");
        }
    }

    // Cập nhật LCD định kỳ (không chặn việc đo)
    if (currentTime - displayUpdateTime >= DISPLAY_UPDATE_INTERVAL)
    {
        displayFrequency(frequency);
        displayUpdateTime = currentTime;
    }
}
