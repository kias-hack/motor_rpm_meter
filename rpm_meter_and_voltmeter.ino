#include <avr/io.h>
#include <avr/interrupt.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define IDLING_RPM 800
#define ONE_MINUTE_IN_SECONDS 60
#define TIM1_OCR_VALUE (8000000 / 1024) / ((IDLING_RPM / ONE_MINUTE_IN_SECONDS) * 2)
#define TEST_IDLING_OUTPUT 4

#define VOLTMETER_PIN 1
#define VOLTMETER_RESISTORS_COEFF (9910 / 728) * (1.22 / 128)
#define VOLTMETER_INTERVAL_MS 500

#define FREQ_PIN 0
#define DISPLAY_INTERVAL_MS 100

long voltmeter_interval = 0;
long display_interval = 0;
volatile long rpm = 0;
float voltage = 0;

LiquidCrystal_I2C LCD(0x27,16,2);

volatile int counter = 0;
volatile long prev = 0;

void freq_handler() {
  if(counter == 0) {
    prev = micros();
    counter = 1;
  } else if(counter == 1){
    rpm = (1000000 * ONE_MINUTE_IN_SECONDS/2) / (micros() -  prev);
    counter = 0;
  }
}

ISR(TIMER1_COMPA_vect)
{
    digitalWrite(TEST_IDLING_OUTPUT, !digitalRead(TEST_IDLING_OUTPUT));
}

void setup() {
  Serial.begin(115200);
  Serial.println(TIM1_OCR_VALUE);

  LCD.init();
  LCD.backlight();

  pinMode(VOLTMETER_PIN, INPUT);
  pinMode(FREQ_PIN, INPUT);
  pinMode(TEST_IDLING_OUTPUT, OUTPUT);

  attachInterrupt(FREQ_PIN, freq_handler, RISING);

  cli();

  TCCR1A = 0;   // установить регистры в 0
  TCCR1B = 0;

  TCCR1B |= (1 << WGM12);  // включить CTC режим 
  TCCR1B |= (1 << CS10); // Установить биты на коэффициент деления 1024
  TCCR1B |= (1 << CS12);

  TIMSK1 |= (1 << OCIE1A); // прерывание по совпадению
  OCR1A = TIM1_OCR_VALUE;

  sei();

  analogReference(INTERNAL);
}

void loop() {
  if(micros() -  prev > 1000000) {
    rpm = 0;
  }

  if(millis() - voltmeter_interval >= VOLTMETER_INTERVAL_MS) {
    voltmeter_interval = millis();

    voltage = (float)(analogRead(VOLTMETER_PIN) >> 3) * VOLTMETER_RESISTORS_COEFF;
  }

  if(millis() - display_interval >= DISPLAY_INTERVAL_MS) {
    display_interval = millis();

    LCD.setCursor(0, 0); // первая строка
    LCD.print("rpm: "); 
    LCD.print(rpm); 
    LCD.print("                ");

    LCD.setCursor(0, 1); // вторая строка
    LCD.print("volt: "); 
    LCD.print(voltage);
    LCD.print("                ");
  }
}
