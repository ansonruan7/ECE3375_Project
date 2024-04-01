#define HEX3_HEX0_BASE        0xFF200020
	
//pointers
volatile unsigned int *hex_ptr = (unsigned int *)HEX3_HEX0_BASE;

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
	//DisplayHex(24);
	
	DisplayHex(6);
}