//--------------------------------------------------------------
void set_motor_a2(uint8_t val)
{
	if(0 == val)
	{
		GPIO_PinOutClear(m_motor_pin_list[0].port, m_motor_pin_list[0].pin);
	}
	else
	{

		GPIO_PinOutSet(m_motor_pin_list[0].port, m_motor_pin_list[0].pin);
	}
}
void set_motor_a1(uint8_t val)
{
	if(0 == val)
	{

		GPIO_PinOutClear(m_motor_pin_list[1].port, m_motor_pin_list[1].pin);
	}
	else
	{
		GPIO_PinOutSet(m_motor_pin_list[1].port, m_motor_pin_list[1].pin);
	}
}

void set_motor_b2(uint8_t val)
{
	if(0 == val)
	{
		GPIO_PinOutClear(m_motor_pin_list[2].port, m_motor_pin_list[2].pin);
	}
	else
	{

		GPIO_PinOutSet(m_motor_pin_list[2].port, m_motor_pin_list[2].pin);
	}
}
void set_motor_b1(uint8_t val)
{
	if(0 == val)
	{

		GPIO_PinOutClear(m_motor_pin_list[3].port, m_motor_pin_list[3].pin);
	}
	else
	{
		GPIO_PinOutSet(m_motor_pin_list[3].port, m_motor_pin_list[3].pin);
	}
}


void step_motor_io_init(void)
{
	set_motor_a2(0);
	set_motor_a1(0);
	set_motor_b2(0);
	set_motor_b1(0);
	GPIO_PinOutSet(gpioPortD, 4);
}

void Delay(uint16_t us)
{
	uint16_t n = us*1250;
	sl_udelay_wait(n);
	//delay_ms(n);
}

void Setp1()
{
    set_motor_a2(1);
    set_motor_a1(0);
    set_motor_b2(1);
    set_motor_b1(0);
}

void Setp2()
{
    set_motor_a2(0);
    set_motor_a1(1);
    set_motor_b2(1);
    set_motor_b1(0);
}
void Setp3()
{
    set_motor_a2(0);
   	set_motor_a1(1);
   	set_motor_b2(0);
   	set_motor_b1(1);
}
void Setp4()
{
    set_motor_a2(1);
    set_motor_a1(0);
    set_motor_b2(0);
    set_motor_b1(1);
}

//-------------------------------------------
void Step_motor_running(int8_t dir)
{
	static uint8_t k = 0;
   if(1 == dir) //ccw
	{
	   LOGD("CCW\n");
	   switch(k)
	   {
	   case 0:
		   Setp1();
		   k = 1;
		   break;


	   case 1:
		   Setp2();
		   k = 2;
		   break;

	   case 2:
		   k = 3;
		   Setp3();
		   break;

	   case 3:
		   k = 0;
		   Setp4();
		   break;
	   }
	}
	else if(-1 == dir) //cw
	{
		 switch(k)
		 {
			   case 0:
				   Setp4();
				   k = 1;
				   break;


			   case 1:
				   Setp3();
				   k = 2;
				   break;

			   case 2:
				   k = 3;
				   Setp2();
				   break;

			   case 3:
				   k = 0;
				   Setp1();
				   break;
		 }
		 LOGD("CW\n");
	}
	else //static
	{
		Setp_Motor_Stop();
		LOGD("STOP\n");
	}
}

volatile uint32_t msTickCount;
uint32_t UTIL_init( void )
{

	uint32_t stat;
	uint32_t ticks;

	/* Setup SysTick Timer for 1 msec interrupts  */
	ticks = CMU_ClockFreqGet( cmuClock_CORE ) / 1000;
	stat = SysTick_Config( ticks );

	return stat;
}
//--------------------------------------------------------------------------
void SysTick_Handler( void )
{

   msTickCount++;
   //LOGD("systick_handler=%d\n", msTickCount);
   Step_motor_running(app_system_param.m_dir);
   return;

}
//---------------------------------
//@zk 20230620