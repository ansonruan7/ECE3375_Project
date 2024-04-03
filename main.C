#include <cmath>
#define HEX3_HEX0_BASE        0xFF200020
#define ADC_BASE 0xFF204000
#define Mask_12_bits 0x00000FFF
#define Mask_Update_Bit 0x00010000 //0x00008000 for in class

//pointers
volatile unsigned int *hex_ptr = (unsigned int *)HEX3_HEX0_BASE;
volatile int * ADC_BASE_ptr = (int *)ADC_BASE;
int mask12 = Mask_12_bits;
int update_mask = Mask_Update_Bit;
volatile int channel0, channel1, channel_update, temp_value, wind_speed_value;

// Convert ADC result to temperature
int convertADCToTemp(unsigned int adc_result) {
    // Convert ADC result to voltage
    float potentiometer_voltage = 3.3;
    float voltage = (adc_result / 4095.0) * potentiometer_voltage;

    // Convert voltage to temperature
    int max_temp = 50;
    int min_temp = -40;
    float temperature = (voltage / potentiometer_voltage) * max_temp - min_temp;

    // Convert temperature to integer
    int temp_int = (int) temperature;
    return temp_int;
}

// Convert ADC result to wind speed
int convertADCToWindSpeed(unsigned int adc_result) {
    // Convert ADC result to voltage
    float potentiometer_voltage = 3.3;
    float voltage = (adc_result / 4095.0) * potentiometer_voltage;

    // Convert voltage to wind speed
    int max_wind_speed = 200;
    float wind_speed = (voltage / potentiometer_voltage) * max_wind_speed;

    // Convert wind speed to integer
    int wind_speed_int = (int) wind_speed;
    return wind_speed_int;
}

// Calculate wind chill from temperature and wind speed 
int calculateWindChill(int temperature, int wind_speed) {
    // Calculate wind chill
    float wind_chill = 13.12 + 0.6215 * temperature - 11.37 * std::pow(wind_speed, 0.16) + 0.3965 * temperature * std::pow(wind_speed, 0.16);

    // Convert wind chill to integer
    int wind_chill_int = (int) wind_chill;
    return wind_chill_int;
}

//hex numbers
const int hexDigits[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

//display hex
void DisplayHex(int value){
	int tens = hexDigits[value/10];
	int ones = hexDigits[value%10];
	
	*hex_ptr = (tens << 24) | (ones << 16) | (0x63 << 8) | 0x39;
}

//main
int main(void){
    // Set all channels to auto-update
    *(ADC_BASE_ptr + 1) = 1;

    // Input from Channel 0 will represent the temperature
    channel0 = *(ADC_BASE_ptr);
    channel_update = channel0 & update_mask;
    if (channel_update) {
        temp_value = convertADCToTemp(channel0 & mask12);
    }

    // Input from Channel 1 will represent the wind speed
    channel1 = *(ADC_BASE_ptr + 1);
    channel_update = channel1 & update_mask;
    if (channel_update) {
        wind_speed_value = convertADCToWindSpeed(channel1 & mask12);
    }

    // Calculate wind chill and display
    int wind_chill = calculateWindChill(temp_value, wind_speed_value);
    DisplayHex(wind_chill);
}