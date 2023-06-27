MP8859_T g_chip_mp8859;
//------------------------------------------
void set_mp8859_api(MP8859_T *pChip)
{
	pChip->pFuncRead = platform_i2c0_read;
	pChip->pFuncWrite = platform_i2c0_write;
	pChip->dev_addr  = (0x60<<1);//(0x69<<1);
	pChip->delay_msec_func = delay_ms;
}
//=============================================================================
int8_t init_one_mp8859(MP8859_T *pChip)
{
	int8_t com_rslt = ERROR;
	uint8_t v_data_u8 = 0; //0
	uint8_t v_chip_id_read_count = 10; //5

	while (v_chip_id_read_count > 0)
	{
			/* read chip id */
			com_rslt = pChip->pFuncRead(pChip->dev_addr, 0x28, &v_data_u8, 1);
			/* Check for the correct chip id */
			LOGD("devaddr=0x%2X,  chip_id=0x%2X\n", pChip->dev_addr, v_data_u8);
			if(v_data_u8 == MP8859_CHIP_ID)
				break;
			v_chip_id_read_count--;
			/* Delay added concerning the low speed of power up system to
			facilitate the proper reading of the chip ID */
			pChip->delay_msec_func(30);
	}
	pChip->chip_id = v_data_u8;
	com_rslt = (v_chip_id_read_count == 0) ?MP8859_READ_ID_FAIL : MP8859_READ_ID_OK;
	if (com_rslt == MP8859_READ_ID_OK)
	{
		LOGD("read chip ok\n");
	}
	return com_rslt;
}

int8_t init_mp8859(MP8859_T *pChip)
{
	int8_t retValue = 0;
	set_mp8859_api(pChip);
	retValue = init_one_mp8859(pChip);
	return retValue;
}

//---------------------------------------------------------------
bool setCurrentLimit_mA(MP8859_T *pChip, uint16_t current_mA)
{

	if(current_mA > 3000)
	{
		return false;
	}
	if(current_mA > 4025)
	{
		current_mA = 4025;
	}
	uint32_t limit_raw = (current_mA*1311+(24*1311))>>16;
	uint8_t tmp = limit_raw & 0x7F;
	pChip->pFuncWrite(pChip->dev_addr, MP8859_REG_IOUT_LIM, &tmp, 1);
	return true;
}
//---------------------------------------------------------------
int8_t setVoltageSetpoint_mV(MP8859_T *pChip, uint16_t  voltage_mV )
{
	int8_t a = 0;
	uint8_t tmp[3];
	uint16_t voltage_raw=0;
	uint8_t reg = MP8859_REG_VOUT_L;

	if( voltage_mV  > 6500 )
	{
	        return false;
	}
	if( voltage_mV > 20480 )// highest permitted value producing int part 2047
	{
	        voltage_mV = 20480; // 20.47 V max (VOUT = 2047)
	}
	voltage_raw = voltage_mV/10;
	//LOGD("set vol_raw=%d\n", voltage_raw);

	tmp[0] = voltage_raw & 0x07;                 // VOUT_L value = voltage_raw[ 2:0]
	tmp[1] = (voltage_raw >> 3) & 0xFF;          // VOUT_H value = voltage_raw[10:3]
	tmp[2] = MP8859_GO_BIT | MP8859_PG_DELAY_EN; // Apply VOUT changes by setting the GO bit right after updating VOUT.
	pChip->pFuncWrite(pChip->dev_addr, reg, tmp, 3);

	return a;
}


//---------------------------------------------------------------
uint16_t readVoltageSetpoint_mV(MP8859_T *pChip, uint16_t *vol_mv)
{

	uint8_t tmp[2];
	uint16_t tmpV = 0;
	pChip->pFuncRead(pChip->dev_addr, MP8859_REG_VOUT_L, tmp, 2);

	vol_mv = (tmp[1] << 3 | (tmp[0] & 0x07)) * 10; // scale by 10 mV / LSB
	LOGD("read voltage set point:%d\n", vol_mv);
	tmpV = vol_mv;
	return tmpV;
}
//----------------------------------------------------------------
uint16_t GetSetPointVoltage_mV(MP8859_T *pChip)
{

	uint8_t tmp[2];
	uint16_t tmpV = 0;
	pChip->pFuncRead(pChip->dev_addr, MP8859_REG_VOUT_L, tmp, 2);

	tmpV = (tmp[1] << 3 | (tmp[0] & 0x07)) * 10; // scale by 10 mV / LSB
	LOGD("read voltage set point:%d\n", tmpV);
	return tmpV;
}

void set_voltage(uint16_t vol)
{
	LOGD("set new voltage=%d(mv)\n", vol);
	setVoltageSetpoint_mV(&g_chip_mp8859, vol);
}


/***********************************************************
usage:

init_mp8859(& g_chip_mp8859);
set_voltage(3000);




************************************************************/