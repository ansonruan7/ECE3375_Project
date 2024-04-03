#define HEX3_HEX0_BASE        0xFF200020
#define HEX5_HEX4_BASE        0xFF200030
#define ADC_BASE 0xFF204000
#define Mask_12_bits 0x00000FFF
#define Mask_Update_Bit 0x00010000 //0x00008000 for in class

//pointers
volatile unsigned int *hex_ptr = (unsigned int *)HEX3_HEX0_BASE;
volatile unsigned int *hex_ptr2 = (unsigned int *)HEX5_HEX4_BASE;
volatile int * ADC_BASE_ptr = (int *)ADC_BASE;
int mask12 = Mask_12_bits;
int update_mask = Mask_Update_Bit;
volatile int channel0, channel1, channel_update, temp_value, wind_speed_value;

// Convert ADC result to temperature
int convertADCToTemp(unsigned int adc_result) {
    // Convert ADC result to voltage
    int potentiometer_voltage = 3300;
    float voltage_float = (adc_result / 4095.0) * potentiometer_voltage;
    int voltage = (int) voltage_float;

    // Convert voltage to temperature
    int max_temp = 50;
    int min_temp = -40;

    int voltage_over_potentiometer_voltage = voltage * 100 / potentiometer_voltage;
    int range = max_temp - min_temp;

    int temperature = (voltage_over_potentiometer_voltage * range / 100) + min_temp;

    return temperature;
}

// Convert ADC result to wind speed
int convertADCToWindSpeed(unsigned int adc_result) {
    // Convert ADC result to voltage
    int potentiometer_voltage = 3300;
    float voltage_float = (adc_result / 4095.0) * potentiometer_voltage;
    int voltage = (int) voltage_float;

    // Convert voltage to wind speed
    int max_wind_speed = 200;

    int voltage_over_potentiometer_voltage = voltage * 100 / potentiometer_voltage;

    int wind_speed = voltage_over_potentiometer_voltage * max_wind_speed / 100;

    return wind_speed;
}

// Calculate wind chill from temperature and wind speed 
int calculateWindChill(int temperature, int wind_speed) {
    // Calculate wind chill
    float wind_chill = 13.12 + 0.6215 * temperature - 11.37 * pow(wind_speed, 0.16) + 0.3965 * temperature * pow(wind_speed, 0.16);

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
void DisplayHex(int value) {
    int sign = 0x00;
    if (value < 0) {
        sign = 0x40;    // Display a negative sign
        value = -value;
    }

    int tens = hexDigits[value / 10];
    int ones = hexDigits[value % 10];
    
    *hex_ptr = (tens << 24) | (ones << 16) | (0x63 << 8) | 0x39;
    *hex_ptr2 = sign;
}

//main
int main(void){
    // Set all channels to auto-update
    *(ADC_BASE_ptr + 1) = 1;

    channel0 = 0x00010141;
    channel1 = 0x00010000;

    while(1) {
        // Input from Channel 0 will represent the temperature
        channel0++;
        if (channel0 > 0x00010FFF) {
            channel0 = 0x00010000;
        }
        channel_update = channel0 & update_mask;
        if (channel_update) {
            temp_value = convertADCToTemp(channel0 & mask12);
        }

        // Input from Channel 1 will represent the wind speed
        channel1++;
        if (channel1 > 0x00010FFF) {
            channel1 = 0x00010000;
        }
        channel_update = channel1 & update_mask;
        if (channel_update) {
            wind_speed_value = convertADCToWindSpeed(channel1 & mask12);
        }

        // Calculate wind chill and display
        int wind_chill = calculateWindChill(temp_value, wind_speed_value);
        DisplayHex(wind_chill);

        // Delay
        for(int i = 0; i < 10000; i++) {}
    }
}