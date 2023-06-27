#define MP8859_CHIP_ID (0x58)
#define MP8859_READ_ID_OK  (0)
#define MP8859_READ_ID_FAIL ((int8_t)-1)


//-------------------------------------------------------------------------------


typedef struct _mp8859_t
{
	uint8_t chip_id;
	uint8_t dev_addr;
	uint32_t (*pFuncRead)(uint8_t, uint8_t, uint8_t *, uint32_t);
	uint32_t (*pFuncWrite)(uint8_t, uint8_t, uint8_t *, uint32_t);
	void (*delay_msec_func)(uint32_t); //��ʱ����
}MP8859_T;

enum MP8859_REG_VOUT_GO_bits {
    MP8859_GO_BIT      = 1 << 0 , // Set bit to 1 to engage voltage change after updating VOUT_L, VOUT_H registers. Resets to 0 when new setpoint is applied.
    MP8859_PG_DELAY_EN = 1 << 1 , // When PG_DELAY_EN D[1] is 1, PG experiences a 100μs rising delay. (default 0)
};


enum MP8859_REG_CTL1_bits {
    MP8859_FREQ_00_500kHz   = 0x0 << 2, // switching frequency (default)
    // MP8859_FREQ bits 01, 10, 11 : reserved
    MP8859_MODE             = 1 << 4 , // switching mode: 0: Enables auto PFM/PWM mode, 1: Sets forced PWM mode (default)
    MP8859_DISCH_EN         = 1 << 5 , // 1: Output discharge function during EN or VIN shutdown (default)
    MP8859_HICCUP_OCP_OVP   = 1 << 6 , // Over-current and over-voltage protection mode: 0: Latch-off, 1: Hiccup (default)
    MP8859_EN               = 1 << 7 , // I2C on/off control (EN is high, the EN bit takes over), soft EN : I2C register does not reset,
                                     // HW EN: reset to defaults, delay to reset depending on DISCH_EN. )
    MP8859_CTL1_DEFAULT_OUTPUT_OFF = 0x70,
    MP8859_CTL1_DEFAULT_OUTPUT_ON  = 0xF0,
};

enum MP8859_REG_CTL2_bits {
    // MP8859_REG_CTL2[3:0] : reserved
    MP8859_SS_00_300us           = 0x0 << 4 , // output start-up soft-start timer
    MP8859_SS_01_500us           = 0x1 << 4 ,
    MP8859_SS_10_700us           = 0x2 << 4 ,
    MP8859_SS_11_900us           = 0x3 << 4 , // (SS default 900 µs)
    MP8859_LINE_COMP_00_OFF      = 0x0 << 6 , // output voltage compensation vs. load current (default OFF)
    MP8859_LINE_COMP_01_2A_100mV = 0x1 << 6 ,
    MP8859_LINE_COMP_10_2A_200mV = 0x2 << 6 ,
    MP8859_LINE_COMP_11_2A_400mV = 0x3 << 6 ,

};

enum MP8859_REG_STATUS_bits {
    MP8859_STATUS_CC_CV = 1 << 4 , // CC/!CV : constant-current output mode (1) or constant voltage output mode (0)
    MP8859_STATUS_OTW   = 1 << 5 , // Over-temperature warning indication.
    MP8859_STATUS_OTP   = 1 << 6 , // Over-temperature protection indication.
    MP8859_STATUS_PG    = 1 << 7 , // Output power good indication.
};

enum MP8859_REG_INTERRUPT_bits{
    MP8859_PG_RISING        = 1 << 0 , // Output power good rising edge.
    MP8859_OT_WARNING_EXIT  = 1 << 1 , // Die temperature early warning exit bit.
    MP8859_OTEMPP_EXIT      = 1 << 2 , // Over-temperature protection exit
    MP8859_UVP_FALLING      = 1 << 3 , // Output voltage is in under-voltage protection.
    MP8859_OC_RECOVER       = 1 << 4 , // Recovery from CC current-limit mode
    MP8859_OC_ENTER         = 1 << 5 , // Entry of OC or CC current-limit mode.
    MP8859_OT_WARNING_ENTER = 1 << 6 , // Die temperature early warning entry bit
    MP8859_OTEMPP_ENTER     = 1 << 7 , // Over-temperature protection entry indication.
};

enum MP8859_REG_INT_MASK{
    MP8859_PG_MSK  = 1 << 0 , // Masks off the PG indication function on ALT. Setting  MSK to 1 disables this interrupt source.
    MP8859_UVP_MSK = 1 << 1 , // Masks off the output UVP interrupt. Setting  MSK to 1 disables this interrupt source.
    MP8859_OC_MSK  = 1 << 2 , // Masks off both OC/CC entry and recovery. Setting  MSK to 1 disables this interrupt source.
    MP8859_OTW_MSK = 1 << 3 , // Masks off the over-temperature warning. Setting  MSK to 1 disables this interrupt source.
    MP8859_OTP_MSK = 1 << 4 , // Masks off the OTP alert. Setting  MSK to 1 disables this interrupt source.
};

enum MP8859_register {
    MP8859_REG_VOUT_L    = 0x00, // R[2:0] : VOUT[2:0] - default 0x04
    MP8859_REG_VOUT_H    = 0x01, // R[7:0] : VOUT[10:3] - default 0x3E ( 0x3E << 3 | 0x04 = 500 decimal = 5.00V )
    MP8859_REG_VOUT_GO   = 0x02, // R[1] : PG_DELAY_EN , R[0] : GO_BIT
    MP8859_REG_IOUT_LIM  = 0x03, // R[6:0] : output current limit threshold (0 .. 4 A (ROC 21.5k : 50 mA steps) - defaut 0x3C = 3.0 A
    MP8859_REG_CTL1      = 0x04, // R[7] : EN, R[6]: HICCUP_OCP_OVP, R[5] : DISCHG_EN, R[4] : MODE, R[3:2]: FREQ - default 0xF0
    MP8859_REG_CTL2      = 0x05, // R[7:6] : LINE_DROP_COMP, R[5:4] : SS - default 0x30
    // 0x07 .. 0x08 : Reserved
    MP8859_UNK_0x06      = 0x06, // 0x06 [7:1] is denoted "Reserved, ALL “0”" , [0] is denoted "Reserved", structurally an odd remnant. (variable)
    MP8859_UNK_0x07      = 0x07, // undocumented (variable)
    MP8859_UNK_0x08      = 0x08, // undocumented (variable)
    MP8859_REG_STATUS    = 0x09, // R[7] : PG, R[6] : OTP, R[5] : OTW, R[4] : CC_CV (instantaneous values)
    MP8859_REG_INTERRUPT = 0x0A, // R[7] : OTEMPP_ENTER, R[6] : OT_WARNING_ENTER, R[5] : OC_ENTER, R[4] : OC_RECOVER, R[3]: UVP_FALLING, R[2] : OTEMP_EXIT, R[1] : OT_WARNING_EXIT, R[0] : PG_RISING
                                 // Write 0xFF to MP8859_REG_INTERRUPT to reset the interrupt (ALT) pin state.
    MP8859_REG_INT_MASK  = 0x0B, // R[4] : OTPMSK, R[3] : OTWMSK, R[2] : OC_MSK, R[1] : UVP_MSK, R[0] : PG_MSK
    MP8859_REG_ID1       = 0x0C, // 0x00: standard MP8859-0000Q, 0x01: MP8859-0001 part number etc.
    MP8859_UNK_0x0D      = 0x0D, // undocumented (0x30)
    // 0x0E .. 0x26 : unused / undocumented
    MP8859_REG_MFR_ID    = 0x27, // Manufacturer ID (0x09)
    MP8859_REG_DEV_ID    = 0x28, // Device ID       (0x58)
    MP8859_REG_IC_REV    = 0x29, // IC revision     (0x01)
    // 0x40 .. 0x4F : ROM defaults? / unused
    MP8859_UNK_0x40      = 0x40, // undocumented (0x04)
    MP8859_UNK_0x41      = 0x41, // undocumented (0x3E)
    MP8859_UNK_0x42      = 0x42, // undocumented (0x00)
    MP8859_UNK_0x43      = 0x43, // undocumented (0x3C)
    MP8859_UNK_0x44      = 0x44, // undocumented (0xF0)
    MP8859_UNK_0x45      = 0x45, // undocumented (0x30)
    // 0x50 .. 0x5F : calibration coefficients? / unused
    MP8859_UNK_0x50      = 0x50, // undocumented (variable)
    MP8859_UNK_0x51      = 0x51, // undocumented (variable)
    MP8859_UNK_0x52      = 0x52, // undocumented (variable)
    // 0x60 .. 0xFF : unused / undocumented
};

#define VOL_MIN_IDX			0x00
#define VOL_MAX_IDX			0x7ff

/* Register definitions */
#define MP8859_VOUT_L_REG		0    //3 lo Bits
#define MP8859_VOUT_H_REG		1    //8 hi Bits
#define MP8859_VOUT_GO_REG		2
#define MP8859_IOUT_LIM_REG		3
#define MP8859_CTL1_REG			4
#define MP8859_CTL2_REG			5
#define MP8859_RESERVED1_REG		6
#define MP8859_RESERVED2_REG		7
#define MP8859_RESERVED3_REG		8
#define MP8859_STATUS_REG		9
#define MP8859_INTERRUPT_REG		0x0A
#define MP8859_MASK_REG			0x0B
#define MP8859_ID1_REG			0x0C
#define MP8859_MFR_ID_REG		0x27
#define MP8859_DEV_ID_REG		0x28
#define MP8859_IC_REV_REG		0x29

#define MP8859_MAX_REG			0x29

#define MP8859_GO_BIT			0x01

int8_t init_mp8859(MP8859_T *pChip);
void set_voltage(uint16_t vol);