/**
	******************************************************************************
	* File Name          : CCS811.C
	* Description        : This file contains the common defines of the application
	******************************************************************************
	*
	* COPYRIGHT(c) 2017 Akash kapashia
       * Created by Akash kapashia
	******************************************************************************
*/
 
	/* Includes ------------------------------------------------------------------*/
	 #include "CCS811_Basic.h"
         #include "HDC_1080_TH.h"
	 #define I2CXC I2C1
	 I2C_HandleTypeDef  hi2cxc;
         uint8_t BurnIn_Time_Complete=0;
	 uint8_t RunIn_Time_Complete=0;
	 uint8_t Baseline_Time_Complete=0;
         uint8_t EBaseline_Time_Complete=0;
  	//These are the air quality values obtained from the sensor
	 unsigned int tVOC = 0;
	 unsigned int CO2 = 0;
	 uint16_t adc_raw=0;
	 uint8_t current_value=0;
	 uint8_t dummyread=0;
	 uint8_t appvalue=0;
	 uint8_t errvalue=0;
	 uint8_t mosetting=0;
	 uint8_t dtvalue =0;	 
	 uint8_t appStart=0;
         uint32_t ELBaseline_period=0;
	 uint32_t ALBaseline_period=0;
	 uint8_t  Mode_CCS811=1;
         float relativeHumidity=65.0,temperature=25.0;

/**
  * @brief  Init_I2C_CCS811.
  * @param  NONE.
  * @retval None.
 */	
 void Init_I2C_CCS811(void)
{
 
  hi2cxc.Instance = I2CXC;
  hi2cxc.Init.Timing = 0x2000090E;
  hi2cxc.Init.OwnAddress1 = 0;
  hi2cxc.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2cxc.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2cxc.Init.OwnAddress2 = 0;
  hi2cxc.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2cxc.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2cxc.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2cxc) != HAL_OK)
  {
 while(1);
  }

    /**Configure Analogue filter 
    */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2cxc, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
 while(1); 
  }

    /**Configure Digital filter 
    */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2cxc, 0) != HAL_OK)
  {
 while(1); 
  }

}	
		
/*
  * @brief  Updates the total voltatile organic compounds (TVOC) in parts per billion (PPB) and the CO2 value.
  * @param  NONE.
  * @retval None.
 */	
void readAlgorithmResults()
{
	 
		uint8_t data_rq[4];
		uint8_t status = HAL_I2C_Mem_Read( &hi2cxc, CCS811_ADDRD, ( uint8_t )CSS811_ALG_RESULT_DATA, I2C_MEMADD_SIZE_8BIT, data_rq, 4,
																100 );
		
		uint8_t co2MSB = data_rq[0];
		uint8_t co2LSB = data_rq[1];
		uint8_t tvocMSB = data_rq[2];
		uint8_t tvocLSB = data_rq[3];
	 
	/*	TVOC value, in parts per billion (ppb)

		eC02 value, in parts per million (ppm) */
		CO2 = ((unsigned int)co2MSB << 8) | co2LSB;
		tVOC = ((unsigned int)tvocMSB << 8) | tvocLSB;
}

/*
  * @brief  configureCCS811.
  * @param  NONE.
  * @retval None.
 */
void configureCCS811()
{
		
	        HAL_Delay(69);
		//Verify the hardware ID is what we expect
		
		uint8_t hwID = readRegister(0x20); //Hardware ID should be 0x81
	if (hwID != 0x81)
		{
			//Serial.println("CCS811 not found. Please check wiring.");
			while (1); //Freeze!
		}
	 
	
		
	       uint8_t    lodata[1];
	       lodata[0]= CSS811_APP_START;
		
	       HAL_I2C_Master_Transmit(&hi2cxc, 0xB6, lodata, 1, 100);
	 
		HAL_Delay(20);
		setDriveMode(Mode_CCS811); //Read every second
                HAL_Delay(10);	
		
	 if (checkForError() == 1)
		{
	        while (1); //Freeze!
		}
		//Set Drive Mode
	
		//Check for errors
		if (checkForError() == 1)
		{
		while (1); //Freeze!
		}
		readHumidity();
                readTemperature();
		setEnvironmentalData(  HDC1080_Humidity/1000,  HDC1080_Temp);
                //RunIn_Time_Complete= BURN_IN_TIME_ADDRESS;
                BurnIn_Time_Complete=BURN_IN_TIME_ADDRESS_VAL ;
		Baseline_Time_Complete=BASELINE_EARLYLIFE_PERIOD_ADDRESS_VAL;
		
}

	//Checks to see if error bit is set
	FlagStatus checkForError()
	{
		errvalue=readRegister(CSS811_ERROR_ID);
	        errvalue = readRegister(CSS811_STATUS);
		return (errvalue & 1 << 0);
	}

	 

	//Checks to see if DATA_READ flag is set in the status register
	FlagStatus dataAvailable()
	{   // dtvalue = readRegister(CSS811_ERROR_ID);
                 //HAL_Delay(00);
	        dtvalue = readRegister(CSS811_STATUS);
		return (dtvalue & 1 << 3);
	}

	//Checks to see if APP_VALID flag is set in the status register
	FlagStatus appValid()
	{
		appvalue = readRegister(CSS811_STATUS);
		return (appvalue & (1 << 4));
	}

	//Enable the nINT signal
	void enableInterrupts(void)
	{
		uint8_t setting = readRegister(CSS811_MEAS_MODE); //Read what's currently there
		setting |= (1 << 3); //Set INTERRUPT bit
		writeRegister(CSS811_MEAS_MODE, setting);
	}

	//Disable the nINT signal
	void disableInterrupts(void)
	{
		uint8_t setting = readRegister(CSS811_MEAS_MODE); //Read what's currently there
		setting &= ~(1 << 3); //Clear INTERRUPT bit
		writeRegister(CSS811_MEAS_MODE, setting);
	}

/*
  * @brief  //Mode 0 = Idle
	    //Mode 1 = read every 1s
	    //Mode 2 = every 10s
	    //Mode 3 = every 60s
	    //Mode 4 = RAW mode.
  * @param  MODE.
  * @retval None.
 */
void setDriveMode(uint8_t mode)
{
		if (mode > 4) mode = 4; //Error correction
	 
		mosetting = readRegister(CSS811_MEAS_MODE); //Read what's currently there
	 
		mosetting &=~(7<<4); //Clear DRIVE_MODE bits
		mosetting |= (mode << 4); //Mask in mode
	 
		writeRegister(CSS811_MEAS_MODE, mosetting);
		mosetting = readRegister(CSS811_MEAS_MODE); //Read what's currently there
	 
}

 
 /*
  * @brief //Given a temp and humidity, write this data to the CSS811 for better compensation
	 //This function expects the humidity and temp to come in as floats
  * @param  relativeHumidity HUMIDITY.
  * @param  temperature TEMPERATURE.
  * @retval None.
 */
void setEnvironmentalData(float relativeHumidity, float temperature)
{
		int rH = relativeHumidity * 1000; //42.348 becomes 42348
		int temp = temperature * 1000; //23.2 becomes 23200

		uint8_t envData[4];

		//Split value into 7-bit integer and 9-bit fractional
		envData[0] = ((rH % 1000) / 100) > 7 ? (rH / 1000 + 1) << 1 : (rH / 1000) << 1;
		envData[1] = 0; //CCS811 only supports increments of 0.5 so bits 7-0 will always be zero
		if (((rH % 1000) / 100) > 2 && (((rH % 1000) / 100) < 8))
		{
			envData[0] |= 1; //Set 9th bit of fractional to indicate 0.5%
		}

		temp += 25000; //Add the 25C offset
		//Split value into 7-bit integer and 9-bit fractional
		envData[2] = ((temp % 1000) / 100) > 7 ? (temp / 1000 + 1) << 1 : (temp / 1000) << 1;
		envData[3] = 0;
		if (((temp % 1000) / 100) > 2 && (((temp % 1000) / 100) < 8))
		{
			envData[2] |= 1;  //Set 9th bit of fractional to indicate 0.5C
		}

		uint8_t env[6];
		env[0]=CSS811_ENV_DATA;
		env[1]=envData[0];
		env[2]=envData[1];
		env[3]=envData[2];
		env[5]=envData[3];
		HAL_I2C_Mem_Write( &hi2cxc, CCS811_ADDWR, ( uint8_t )CSS811_ENV_DATA, I2C_MEMADD_SIZE_8BIT, envData, 4,
																100 );
		 while (HAL_I2C_GetState(&hi2cxc) != HAL_I2C_STATE_READY)
			{
			} 
			while (HAL_I2C_IsDeviceReady(&hi2cxc, CCS811_ADDRD, 10, 300) == HAL_TIMEOUT);
			while(HAL_I2C_GetState(&hi2cxc) != HAL_I2C_STATE_READY)
			{
			}
		 
	}


 /*
  * @brief  reading_Multiple_Register
  * @param  addr ADDRESS.
  * @param  val VALUE.
  * @param  size SIZE.
  * @retval None.
 */
void read_Mul_Register(uint8_t addr, uint8_t * val,uint8_t size)
	{
			HAL_I2C_Mem_Read( &hi2cxc, CCS811_ADDRD, ( uint8_t )addr, I2C_MEMADD_SIZE_8BIT, val, size,100 );
		
	}

 /*
  * @brief  softRest
  * @param  NONE.
  * @retval None.
 */
void softRest() {
     
               uint8_t rstCMD[5] = {CSS811_SW_RESET, 0x11,0xE5,0x72,0x8A};
 
         	HAL_I2C_Mem_Write( &hi2cxc, CCS811_ADDWR, CSS811_SW_RESET, I2C_MEMADD_SIZE_8BIT, rstCMD, 5,300);
		while (HAL_I2C_GetState(&hi2cxc) != HAL_I2C_STATE_READY);
	      
}	


 /*
  * @brief  get_Sensor_Resistance
  * @param  NONE.
  * @retval RESISTANCE OF SENSOR.
 */
uint32_t get_Sensor_Resistance(){
	uint8_t sensor_Resistance_raw[2];

	uint32_t sensor_Resistance;
	
	HAL_I2C_Mem_Read( &hi2cxc, CCS811_ADDRD, CSS811_RAW_DATA , I2C_MEMADD_SIZE_8BIT, sensor_Resistance_raw, 2,100 );
        current_value=sensor_Resistance_raw[0]>>2;
	sensor_Resistance_raw[0]=sensor_Resistance_raw[0]&0x03;
	
	adc_raw=(sensor_Resistance_raw[0]<<8)|sensor_Resistance_raw[1];
	
	sensor_Resistance=((165*adc_raw)*10000)/(current_value*1023);
 
	return sensor_Resistance;
}

 /*
  * @brief  sleep
  * @param  NONE.
  * @retval NONE.
 */
void sleep()
{
  // sets sensor to idle; measurements are disabled; lowest power mode
 writeRegister(CSS811_MEAS_MODE, 0);
 }



 /*
  * @brief  Reads from a give location from the CSS811
  * @param  addr  ADDRESS.
  * @retval VALUE AT THE ADDRESS.
 */	 
uint8_t readRegister(uint8_t addr)
{
			uint8_t dt;
					
			 HAL_I2C_Mem_Read( &hi2cxc, CCS811_ADDRD, ( uint8_t )addr,1, &dt, 1,
																	300 );
				 while (HAL_I2C_GetState(&hi2cxc) != HAL_I2C_STATE_READY);
		 
		 
			return dt;
 }
		
		 
		 
uint32_t  restore_Baselines;
		 
/*
  * @brief  	 //restore the baseline value
	       //Used for telling sensor what 'clean' air is
	      //You must put the sensor in clean air and record this value
  * @param  NONE.
  * @retval NONE.
 */
void restore_Baseline()
  {   
                 uint32_t  restore_Baseline;
		 restore_Baseline= * ((  uint32_t *)DATA_EEPROM_BASE);
		
		restore_Baselines=restore_Baseline ;
		uint8_t res_bs[2];
		res_bs[0]=restore_Baseline>>8;
		res_bs[1]=restore_Baseline&0x000000FF;
    	        HAL_I2C_Mem_Write( &hi2cxc, CCS811_ADDWR, CSS811_BASELINE, I2C_MEMADD_SIZE_8BIT, res_bs,2,300);
		 while (HAL_I2C_GetState(&hi2cxc) != HAL_I2C_STATE_READY)
			{
			} 
    }

 
/*
  * @brief  	//Returns the baseline value and saves into EEPROM
	       //Used for telling sensor what 'clean' air is
	      //You must put the sensor in clean air and record this value
  * @param  NONE.
  * @retval BASELINE VALUE.
 */
unsigned int getBaseline()
{
		 
  	        uint8_t ada[2];
		HAL_StatusTypeDef status = HAL_OK;
	 
		  status = HAL_I2C_Mem_Read( &hi2cxc, CCS811_ADDRD, ( uint8_t )CSS811_BASELINE,1,  ada, 2,100 );
			 while (HAL_I2C_GetState(&hi2cxc) != HAL_I2C_STATE_READY)
			{
			} 
			while (HAL_I2C_IsDeviceReady(&hi2cxc, CCS811_ADDRD, 10, 300) == HAL_TIMEOUT);
			while(HAL_I2C_GetState(&hi2cxc) != HAL_I2C_STATE_READY)
			{
			}

		uint8_t baselineMSB = ada[0];
		uint8_t baselineLSB = ada[1];

		unsigned int baseline = ((unsigned int)baselineMSB << 8) | baselineLSB;
		HAL_FLASHEx_DATAEEPROM_Unlock();
	        HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, DATA_EEPROM_BASE, baseline);
			
		return (baseline);
}



	//Write a value to a spot in the CCS811
/*
  * @brief  Write a value to a spot in the CCS811
  * @param  addr ADDRESS.
  * @param  val  VALUE.
  * @retval NONE.
 */
void writeRegister(uint8_t addr, uint8_t val)
{
			

			HAL_I2C_Mem_Write( &hi2cxc, CCS811_ADDWR, ( uint8_t )addr, I2C_MEMADD_SIZE_8BIT, &val, 1,300);
		 while (HAL_I2C_GetState(&hi2cxc) != HAL_I2C_STATE_READY)
			{
			} 
			while (HAL_I2C_IsDeviceReady(&hi2cxc, CCS811_ADDRD, 10, 300) == HAL_TIMEOUT);
			while(HAL_I2C_GetState(&hi2cxc) != HAL_I2C_STATE_READY)
			{
			}
}
	 


