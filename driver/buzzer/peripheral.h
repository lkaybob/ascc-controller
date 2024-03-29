#ifndef __ECUBEPERI_H__
#define __ECUBEPERI_H__


// Major setup
#define MAJOR_NUMBER		240
#define MAJOR_DEV_NAME		"CNPERI"

#define DEV_NAME_MATRIX_LED		"cnmled"
#define DEV_NAME_DIP_SW			"cndipsw"
#define DEV_NAME_BTN_SW			"cnbtsw"
#define DEV_NAME_KEYPD			"cnkey"
#define DEV_NAME_LED			"cnled"
#define DEV_NAME_FND			"cnfnd"
#define DEV_NAME_TEXT_LCD		"cntlcd"
#define DEV_NAME_STEP_MOTOR		"cnmotor"
#define DEV_NAME_COLOR_LED		"cncled"
#define DEV_NAME_OLED			"cnoled"
#define DEV_NAME_BUZZER			"cnbuzzer"


// Minor Number
#define MINOR_MATRIX_LED_NUMBER		1
#define MINOR_DIP_SW_NUMBER			2
#define MINOR_BTN_SW_NUMBER			3
#define MINOR_KEYPD_NUMBER			4
#define MINOR_LED_NUMBER			5
#define MINOR_FND_NUMBER			6
#define MINOR_TEXT_LED_NUMBER		7
#define MINOR_STEP_MOTOR_NUMBER		8
#define MINOR_COLOR_LED_NUMBER		9
#define MINOR_OLED_NUMBER			10
#define MINOR_BUZZER_NUMBER			11




// Phigical Address
// chip select 2 use , 
#define FPGA_BASE_ADDRESS	0x06000000
#define MATRIX_LED_BASE_ADDRESS		(FPGA_BASE_ADDRESS + 0x1000)
#define DIP_SW_BASE_ADDRESS			(FPGA_BASE_ADDRESS + 0x2000)
#define BTN_SW_BASE_ADDRESS			(FPGA_BASE_ADDRESS + 0x3000)
#define KEYPD_BASE_ADDRESS			(FPGA_BASE_ADDRESS + 0x4000)
#define LED_BASE_ADDRESS			(FPGA_BASE_ADDRESS + 0x5000)
#define FND_BASE_ADDRESS			(FPGA_BASE_ADDRESS + 0x6000)
#define TEXT_LED_BASE_ADDRESS		(FPGA_BASE_ADDRESS + 0x7000)
#define STEP_MOTOR_BASE_ADDRESS		(FPGA_BASE_ADDRESS + 0x8000)
#define COLOR_LED_BASE_ADDRESS		(FPGA_BASE_ADDRESS + 0x9000)
#define OLED_BASE_ADDRESS			(FPGA_BASE_ADDRESS + 0xA000)
#define BUZZER_BASE_ADDRESS			(FPGA_BASE_ADDRESS + 0xB000)



#define ADDRESS_MAP_SIZE	0x1000





#endif// __ECUBEPERI_H__
