	/**
		******************************************************************************
		* File Name          : CCS811.h
		* Description        : This file contains the common defines of the application
		******************************************************************************
		*
		* COPYRIGHT(c) 2017 STMicroelectronics
		*
		* Redistribution and use in source and binary forms, with or without modification,
		* are permitted provided that the following conditions are met:
		*   1. Redistributions of source code must retain the above copyright notice,
		*      this list of conditions and the following disclaimer.
		*   2. Redistributions in binary form must reproduce the above copyright notice,
		*      this list of conditions and the following disclaimer in the documentation
		*      and/or other materials provided with the distribution.
		*   3. Neither the name of STMicroelectronics nor the names of its contributors
		*      may be used to endorse or promote products derived from this software
		*      without specific prior written permission.
		*
		* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
		* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
		* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
		* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
		* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
		* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
		* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
		* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
		* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
		* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
		*
		******************************************************************************
		*/
	/* Define to prevent recursive inclusion -------------------------------------*/
	#ifndef __CCS811_BASIC_H
	#define __CCS811_BASIC_H
		#ifdef __cplusplus
	 extern "C" {
	#endif
		#ifdef STM32L073xx
		 #include "stm32l0xx_hal.h"
	 #endif
			 #ifdef STM32F072xB
			#include "stm32f0xx_hal.h"
	#endif
	
	 //#define CCS811_ADDR 0x5B //7-bit unshifted default I2C Address
	#define CCS811_ADDRD 0xB6 //7-bit unshifted default I2C Address
	#define CCS811_ADDWR 0XB6
	#define CSS811_STATUS 0x00
	#define CSS811_MEAS_MODE 0x01
	#define CSS811_ALG_RESULT_DATA 0x02
	#define CSS811_RAW_DATA 0x03
	#define CSS811_ENV_DATA 0x05
	#define CSS811_NTC 0x06
	#define CSS811_THRESHOLDS 0x10
	#define CSS811_BASELINE 0x11
	#define CSS811_HW_ID 0x20
	#define CSS811_HW_VERSION 0x21
	#define CSS811_FW_BOOT_VERSION 0x23
	#define CSS811_FW_APP_VERSION 0x24
	#define CSS811_ERROR_ID 0xE0
	#define CSS811_APP_START 0xF4
	#define CSS811_SW_RESET 0xFF
	
	//Register addresses
	#define FIRST_BASELINE_ADDRESS_VAL              * ((  uint32_t *)(DATA_EEPROM_BASE+2))
	#define BURN_IN_TIME_ADDRESS_VAL                * ((  uint32_t *)(DATA_EEPROM_BASE+3))
	#define BASELINE_EARLYLIFE_PERIOD_ADDRESS_VAL   * ((  uint32_t *)(DATA_EEPROM_BASE+4))
	
	#define FIRST_BASELINE_ADDRESS             (DATA_EEPROM_BASE+2)
	#define BURN_IN_TIME_ADDRESS               (DATA_EEPROM_BASE+3)
	#define BASELINE_EARLYLIFE_PERIOD_ADDRESS  (DATA_EEPROM_BASE+4)
	
	#define BURN_IN_TIME              ((48*60*60)/APPLICATION_RUN_CYCLE)//48 Hours
        #define RUN_IN_TIME               ((20*60)/APPLICATION_RUN_CYCLE)//20 Minutes
	#define NEW_MODE_RUN_IN_TIME      ((10*60)/APPLICATION_RUN_CYCLE)//10 Minutes
	#define BASELINE_EARLYLIFE_PERIOD ((500*60*60)/APPLICATION_RUN_CYCLE)//500 Hours
	#define BASELINE_EL_STORE_PERIOD  ((24*60*60)/APPLICATION_RUN_CYCLE)//24 Hours
	#define BASELINE_AEL_STORE_PERIOD ((7*BASELINE_EL_STORE_PERIOD)/APPLICATION_RUN_CYCLE)
	#define CALIB_TEMP_HUM            ((20*60)/APPLICATION_RUN_CYCLE)//20 Minutes
	
	void readAlgorithmResults(void);
	void configureCCS811(void);
	FlagStatus checkForError(void);
	unsigned int getBaseline(void);
	FlagStatus dataAvailable(void);
	FlagStatus appValid(void);
	void enableInterrupts(void);
	void disableInterrupts(void);
	void setDriveMode(uint8_t mode);
	void setEnvironmentalData(float relativeHumidity, float temperature);
	uint8_t readRegister(uint8_t addr);
	void writeRegister(uint8_t addr, uint8_t val);
	void Init_I2C_CCS811(void);
  void softRest(void);
  void sleep(void);
	uint32_t get_Sensor_Resistance(void);
	void restore_Baseline(void);
	#ifdef __cplusplus
	}
	#endif

	#endif  