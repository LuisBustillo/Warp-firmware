/*
	Authored 2016-2018. Phillip Stanley-Marbell.

	Additional contributions, 2018 onwards: See git blame.

	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:

	*	Redistributions of source code must retain the above
		copyright notice, this list of conditions and the following
		disclaimer.

	*	Redistributions in binary form must reproduce the above
		copyright notice, this list of conditions and the following
		disclaimer in the documentation and/or other materials
		provided with the distribution.

	*	Neither the name of the author nor the names of its
		contributors may be used to endorse or promote products
		derived from this software without specific prior written
		permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
	FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
	COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
	INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
	BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
	CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
	LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
	ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	POSSIBILITY OF SUCH DAMAGE.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/*
 *	config.h needs to come first
 */
#include "config.h"

#include "fsl_misc_utilities.h"
#include "fsl_device_registers.h"
#include "fsl_i2c_master_driver.h"
#include "fsl_spi_master_driver.h"
#include "fsl_rtc_driver.h"
#include "fsl_clock_manager.h"
#include "fsl_power_manager.h"
#include "fsl_mcglite_hal.h"
#include "fsl_port_hal.h"
#include "fsl_lpuart_driver.h"
#include "glaux.h"
#include "warp.h"
#include "errstrs.h"
#include "gpio_pins.h"
#include "SEGGER_RTT.h"

#include "devSSD1331.h"
#include "pedometer.h"


#define							kWarpConstantStringI2cFailure		"\rI2C failed, reg 0x%02x, code %d\n"
#define							kWarpConstantStringErrorInvalidVoltage	"\rInvalid supply voltage [%d] mV!"
#define							kWarpConstantStringErrorSanity		"\rSanity check failed!"


#if (WARP_BUILD_ENABLE_DEVADXL362)
	#include "devADXL362.h"
	volatile WarpSPIDeviceState			deviceADXL362State;
#endif

#if (WARP_BUILD_ENABLE_DEVIS25xP)
	#include "devIS25xP.h"
	volatile WarpSPIDeviceState			deviceIS25xPState;
#endif

#if (WARP_BUILD_ENABLE_DEVISL23415)
	#include "devISL23415.h"
	volatile WarpSPIDeviceState			deviceISL23415State;
#endif

#if (WARP_BUILD_ENABLE_DEVAT45DB)
	#include "devAT45DB.h"
	volatile WarpSPIDeviceState			deviceAT45DBState;
#endif

#if (WARP_BUILD_ENABLE_DEVICE40)
	#include "devICE40.h"
	volatile WarpSPIDeviceState			deviceICE40State;
#endif

#if (WARP_BUILD_ENABLE_DEVBMX055)
	#include "devBMX055.h"
	volatile WarpI2CDeviceState			deviceBMX055accelState;
	volatile WarpI2CDeviceState			deviceBMX055gyroState;
	volatile WarpI2CDeviceState			deviceBMX055magState;
#endif

#if (WARP_BUILD_ENABLE_DEVMMA8451Q)
	#include "devMMA8451Q.h"
	volatile WarpI2CDeviceState			deviceMMA8451QState;
#endif

#if (WARP_BUILD_ENABLE_DEVINA219)
	#include "devINA219.h"
	volatile WarpI2CDeviceState			deviceINA219State;
#endif

#if (WARP_BUILD_ENABLE_DEVLPS25H)
	#include "devLPS25H.h"
	volatile WarpI2CDeviceState			deviceLPS25HState;
#endif

#if (WARP_BUILD_ENABLE_DEVHDC1000)
	#include "devHDC1000.h"
	volatile WarpI2CDeviceState			deviceHDC1000State;
#endif

#if (WARP_BUILD_ENABLE_DEVMAG3110)
	#include "devMAG3110.h"
	volatile WarpI2CDeviceState			deviceMAG3110State;
#endif

#if (WARP_BUILD_ENABLE_DEVSI7021)
	#include "devSI7021.h"
	volatile WarpI2CDeviceState			deviceSI7021State;
#endif

#if (WARP_BUILD_ENABLE_DEVL3GD20H)
	#include "devL3GD20H.h"
	volatile WarpI2CDeviceState			deviceL3GD20HState;
#endif

#if (WARP_BUILD_ENABLE_DEVBME680)
	#include "devBME680.h"
	volatile WarpI2CDeviceState			deviceBME680State;
	volatile uint8_t				deviceBME680CalibrationValues[kWarpSizesBME680CalibrationValuesCount];
#endif

#if (WARP_BUILD_ENABLE_DEVTCS34725)
	#include "devTCS34725.h"
	volatile WarpI2CDeviceState			deviceTCS34725State;
#endif

#if (WARP_BUILD_ENABLE_DEVSI4705)
	#include "devSI4705.h"
	volatile WarpI2CDeviceState			deviceSI4705State;
#endif

#if (WARP_BUILD_ENABLE_DEVCCS811)
	#include "devCCS811.h"
	olatile WarpI2CDeviceState			deviceCCS811State;
#endif

#if (WARP_BUILD_ENABLE_DEVAMG8834)
	#include "devAMG8834.h"
	volatile WarpI2CDeviceState			deviceAMG8834State;
#endif

#if (WARP_BUILD_ENABLE_DEVAS7262)
	#include "devAS7262.h"
	volatile WarpI2CDeviceState			deviceAS7262State;
#endif

#if (WARP_BUILD_ENABLE_DEVAS7263)
	#include "devAS7263.h"
	volatile WarpI2CDeviceState			deviceAS7263State;
#endif

#if (WARP_BUILD_ENABLE_DEVRV8803C7)
	#include "devRV8803C7.h"
	volatile WarpI2CDeviceState			deviceRV8803C7State;
#endif

#if (WARP_BUILD_ENABLE_DEVBGX)
	#include "devBGX.h"
	volatile WarpUARTDeviceState			deviceBGXState;
#endif


volatile i2c_master_state_t				i2cMasterState;
volatile spi_master_state_t				spiMasterState;
volatile spi_master_user_config_t			spiUserConfig;
volatile lpuart_user_config_t				lpuartUserConfig;
volatile lpuart_state_t					lpuartState;


volatile bool						gWarpBooted				= false;
volatile uint32_t					gWarpI2cBaudRateKbps			= kWarpDefaultI2cBaudRateKbps;
volatile uint32_t					gWarpUartBaudRateBps			= kWarpDefaultUartBaudRateBps;
volatile uint32_t					gWarpSpiBaudRateKbps			= kWarpDefaultSpiBaudRateKbps;
volatile uint32_t					gWarpSleeptimeSeconds			= kWarpDefaultSleeptimeSeconds;
volatile WarpModeMask					gWarpMode				= kWarpModeDisableAdcOnSleep;
volatile uint32_t					gWarpI2cTimeoutMilliseconds		= kWarpDefaultI2cTimeoutMilliseconds;
volatile uint32_t					gWarpSpiTimeoutMicroseconds		= kWarpDefaultSpiTimeoutMicroseconds;
volatile uint32_t					gWarpUartTimeoutMilliseconds		= kWarpDefaultUartTimeoutMilliseconds;
volatile uint32_t					gWarpMenuPrintDelayMilliseconds		= kWarpDefaultMenuPrintDelayMilliseconds;
volatile uint32_t					gWarpSupplySettlingDelayMilliseconds	= kWarpDefaultSupplySettlingDelayMilliseconds;
volatile uint16_t					gWarpCurrentSupplyVoltage		= kWarpDefaultSupplyVoltageMillivolts;
char							gWarpPrintBuffer[kWarpDefaultPrintBufferSizeBytes];

/*
 *	Since only one SPI transaction is ongoing at a time in our implementation
 */
uint8_t							gWarpSpiCommonSourceBuffer[kWarpMemoryCommonSpiBufferBytes];
uint8_t							gWarpSpiCommonSinkBuffer[kWarpMemoryCommonSpiBufferBytes];

static void						sleepUntilReset(void);
static void						lowPowerPinStates(void);
static void						disableTPS62740(void);
static void						enableTPS62740(uint16_t voltageMillivolts);
static void						setTPS62740CommonControlLines(uint16_t voltageMillivolts);
static void						dumpProcessorState(void);
static void						repeatRegisterReadForDeviceAndAddress(WarpSensorDevice warpSensorDevice, uint8_t baseAddress,
								bool autoIncrement, int chunkReadsPerAddress, bool chatty,
								int spinDelay, int repetitionsPerAddress, uint16_t sssupplyMillivolts,
								uint16_t adaptiveSssupplyMaxMillivolts, uint8_t referenceByte);
static int						char2int(int character);
static void						activateAllLowPowerSensorModes(bool verbose);
static void						powerupAllSensors(void);
static uint8_t						readHexByte(void);
static int						read4digits(void);
static void						printAllSensors(bool printHeadersAndCalibration, bool hexModeFlag, int menuDelayBetweenEachRun, bool loopForever);

/*
 *	TODO: change the following to take byte arrays
 */
WarpStatus						writeByteToI2cDeviceRegister(uint8_t i2cAddress, bool sendCommandByte, uint8_t commandByte, bool sendPayloadByte, uint8_t payloadByte);
WarpStatus						writeBytesToSpi(uint8_t *  payloadBytes, int payloadLength);


void							warpLowPowerSecondsSleep(uint32_t sleepSeconds, bool forceAllPinsIntoLowPowerState);



/*
 *	Derived from KSDK power_manager_demo.c BEGIN>>>
 */
clock_manager_error_code_t clockManagerCallbackRoutine(clock_notify_struct_t *  notify, void *  callbackData);

/*
 *	static clock callback table.
 */
clock_manager_callback_user_config_t		clockManagerCallbackUserlevelStructure =
									{
										.callback	= clockManagerCallbackRoutine,
										.callbackType	= kClockManagerCallbackBeforeAfter,
										.callbackData	= NULL
									};

static clock_manager_callback_user_config_t *	clockCallbackTable[] =
									{
										&clockManagerCallbackUserlevelStructure
									};

clock_manager_error_code_t
clockManagerCallbackRoutine(clock_notify_struct_t *  notify, void *  callbackData)
{
	clock_manager_error_code_t result = kClockManagerSuccess;

	switch (notify->notifyType)
	{
		case kClockManagerNotifyBefore:
			break;
		case kClockManagerNotifyRecover:
		case kClockManagerNotifyAfter:
			break;
		default:
			result = kClockManagerError;
		break;
	}

	return result;
}


/*
 *	Override the RTC IRQ handler
 */
void
RTC_IRQHandler(void)
{
	if (RTC_DRV_IsAlarmPending(0))
	{
		RTC_DRV_SetAlarmIntCmd(0, false);
	}
}

/*
 *	Override the RTC Second IRQ handler
 */
void
RTC_Seconds_IRQHandler(void)
{
	gWarpSleeptimeSeconds++;
}

/*
 *	LLW_IRQHandler override. Since FRDM_KL03Z48M is not defined,
 *	according to power_manager_demo.c, what we need is LLW_IRQHandler.
 *	However, elsewhere in the power_manager_demo.c, the code assumes
 *	FRDM_KL03Z48M _is_ defined (e.g., we need to use LLWU_IRQn, not
 *	LLW_IRQn). Looking through the code base, we see in
 *
 *		ksdk1.1.0/platform/startup/MKL03Z4/gcc/startup_MKL03Z4.S
 *
 *	that the startup initialization assembly requires LLWU_IRQHandler,
 *	not LLW_IRQHandler. See power_manager_demo.c, circa line 216, if
 *	you want to find out more about this dicsussion.
 */
void
LLWU_IRQHandler(void)
{
	/*
	 *	BOARD_* defines are defined in warp.h
	 */
	LLWU_HAL_ClearExternalPinWakeupFlag(LLWU_BASE, (llwu_wakeup_pin_t)BOARD_SW_LLWU_EXT_PIN);
}

/*
 *	IRQ handler for the interrupt from RTC, which we wire up
 *	to PTA0/IRQ0/LLWU_P7 in Glaux. BOARD_SW_LLWU_IRQ_HANDLER
 *	is a synonym for PORTA_IRQHandler.
 */
void
BOARD_SW_LLWU_IRQ_HANDLER(void)
{
	/*
	 *	BOARD_* defines are defined in warp.h
	 */
	PORT_HAL_ClearPortIntFlag(BOARD_SW_LLWU_BASE);
}

/*
 *	Power manager user callback
 */
power_manager_error_code_t
callback0(power_manager_notify_struct_t *  notify, power_manager_callback_data_t *  dataPtr)
{
	WarpPowerManagerCallbackStructure *		callbackUserData = (WarpPowerManagerCallbackStructure *) dataPtr;
	power_manager_error_code_t			status = kPowerManagerError;

	switch (notify->notifyType)
	{
		case kPowerManagerNotifyBefore:
			status = kPowerManagerSuccess;
			break;
		case kPowerManagerNotifyAfter:
			status = kPowerManagerSuccess;
			break;
		default:
			callbackUserData->errorCount++;
			break;
	}

	return status;
}
/*
 *	Derived from KSDK power_manager_demo.c <<END
 */



void
sleepUntilReset(void)
{
	while (1)
	{
		#if (WARP_BUILD_ENABLE_DEVSI4705)
			GPIO_DRV_SetPinOutput(kWarpPinSI4705_nRST);
		#endif

		warpLowPowerSecondsSleep(1, false /* forceAllPinsIntoLowPowerState */);

		#if (WARP_BUILD_ENABLE_DEVSI4705)
			GPIO_DRV_ClearPinOutput(kWarpPinSI4705_nRST);
		#endif

		warpLowPowerSecondsSleep(60, true /* forceAllPinsIntoLowPowerState */);
	}
}


void
enableLPUARTpins(void)
{
	/*
	 *	Enable UART CLOCK
	 */
	CLOCK_SYS_EnableLpuartClock(0);

	/*
	 *	Set UART pin association. See, e.g., page 99 in
	 *
	 *		https://www.nxp.com/docs/en/reference-manual/KL03P24M48SF0RM.pdf
	 *
	 *	Setup:
	 *		PTB3/kWarpPinI2C0_SCL_UART_TX for UART TX
	 *		PTB4/kWarpPinI2C0_SCL_UART_RX for UART RX

//TODO: we don't use hw flow control so don't need RTS/CTS
 *		PTA6/kWarpPinSPI_MISO_UART_RTS for UART RTS
 *		PTA7/kWarpPinSPI_MOSI_UART_CTS for UART CTS
	 */
	PORT_HAL_SetMuxMode(PORTB_BASE, 3, kPortMuxAlt3);
	PORT_HAL_SetMuxMode(PORTB_BASE, 4, kPortMuxAlt3);

//TODO: we don't use hw flow control so don't need RTS/CTS
//	PORT_HAL_SetMuxMode(PORTA_BASE, 6, kPortMuxAsGpio);
//	PORT_HAL_SetMuxMode(PORTA_BASE, 7, kPortMuxAsGpio);
//	GPIO_DRV_SetPinOutput(kWarpPinSPI_MISO_UART_RTS);
//	GPIO_DRV_SetPinOutput(kWarpPinSPI_MOSI_UART_CTS);

	/*
	 *	Initialize LPUART0. See KSDK13APIRM.pdf section 40.4.3, page 1353
	 */
	lpuartUserConfig.baudRate = gWarpUartBaudRateBps;
	lpuartUserConfig.parityMode = kLpuartParityDisabled;
	lpuartUserConfig.stopBitCount = kLpuartOneStopBit;
	lpuartUserConfig.bitCountPerChar = kLpuart8BitsPerChar;
	lpuartUserConfig.clockSource = kClockLpuartSrcMcgIrClk;

	LPUART_DRV_Init(0,(lpuart_state_t *)&lpuartState,(lpuart_user_config_t *)&lpuartUserConfig);
}


void
disableLPUARTpins(void)
{
	/*
	 *	LPUART deinit
	 */
	LPUART_DRV_Deinit(0);

	/*
	 *	Set UART pin association. See, e.g., page 99 in
	 *
	 *		https://www.nxp.com/docs/en/reference-manual/KL03P24M48SF0RM.pdf
	 *
	 *	Setup:
	 *		PTB3/kWarpPinI2C0_SCL_UART_TX for UART TX
	 *		PTB4/kWarpPinI2C0_SCL_UART_RX for UART RX

//TODO: we don't use the HW flow control and that messes with the SPI any way
 *		PTA6/kWarpPinSPI_MISO_UART_RTS for UART RTS
 *		PTA7/kWarpPinSPI_MOSI_UART_CTS for UART CTS
	 */
	PORT_HAL_SetMuxMode(PORTB_BASE, 3, kPortPinDisabled);
	PORT_HAL_SetMuxMode(PORTB_BASE, 4, kPortPinDisabled);

//TODO: we don't use flow-control
	PORT_HAL_SetMuxMode(PORTA_BASE, 6, kPortMuxAsGpio);
	PORT_HAL_SetMuxMode(PORTA_BASE, 7, kPortMuxAsGpio);

	GPIO_DRV_ClearPinOutput(kWarpPinSPI_MISO_UART_RTS);
	GPIO_DRV_ClearPinOutput(kWarpPinSPI_MOSI_UART_CTS);

	/*
	 *	Disable LPUART CLOCK
	*/
	CLOCK_SYS_DisableLpuartClock(0);
}



WarpStatus
sendBytesToUART(uint8_t *  bytes, size_t nbytes)
{
	lpuart_status_t	status;

	status = LPUART_DRV_SendDataBlocking(0, bytes, nbytes, gWarpUartTimeoutMilliseconds);
	if (status != 0)
	{
		return kWarpStatusDeviceCommunicationFailed;
	}

	return kWarpStatusOK;
}



void
warpEnableSPIpins(void)
{
	CLOCK_SYS_EnableSpiClock(0);

	/*	kWarpPinSPI_MISO_UART_RTS_UART_RTS --> PTA6 (ALT3)	*/
	PORT_HAL_SetMuxMode(PORTA_BASE, 6, kPortMuxAlt3);

	/*	kWarpPinSPI_MOSI_UART_CTS --> PTA7 (ALT3)	*/
	PORT_HAL_SetMuxMode(PORTA_BASE, 7, kPortMuxAlt3);

	#if (WARP_BUILD_ENABLE_GLAUX_VARIANT)
		/*	kWarpPinSPI_SCK	--> PTA9	(ALT3)		*/
		PORT_HAL_SetMuxMode(PORTA_BASE, 9, kPortMuxAlt3);
	#else
		/*	kWarpPinSPI_SCK	--> PTB0	(ALT3)		*/
		PORT_HAL_SetMuxMode(PORTB_BASE, 0, kPortMuxAlt3);
	#endif

	/*
	 *	Initialize SPI master. See KSDK13APIRM.pdf Section 70.4
	 */
	uint32_t			calculatedBaudRate;
	spiUserConfig.polarity		= kSpiClockPolarity_ActiveHigh;
	spiUserConfig.phase		= kSpiClockPhase_FirstEdge;
	spiUserConfig.direction		= kSpiMsbFirst;
	spiUserConfig.bitsPerSec	= gWarpSpiBaudRateKbps * 1000;
	SPI_DRV_MasterInit(0 /* SPI master instance */, (spi_master_state_t *)&spiMasterState);
	SPI_DRV_MasterConfigureBus(0 /* SPI master instance */, (spi_master_user_config_t *)&spiUserConfig, &calculatedBaudRate);
}



void
warpDisableSPIpins(void)
{
	SPI_DRV_MasterDeinit(0);

	/*	kWarpPinSPI_MISO_UART_RTS	--> PTA6	(GPI)		*/
	PORT_HAL_SetMuxMode(PORTA_BASE, 6, kPortMuxAsGpio);

	/*	kWarpPinSPI_MOSI_UART_CTS	--> PTA7	(GPIO)		*/
	PORT_HAL_SetMuxMode(PORTA_BASE, 7, kPortMuxAsGpio);

	#if (WARP_BUILD_ENABLE_GLAUX_VARIANT)
		/*	kWarpPinSPI_SCK	--> PTA9	(GPIO)			*/
		PORT_HAL_SetMuxMode(PORTA_BASE, 9, kPortMuxAsGpio);
	#else
		/*	kWarpPinSPI_SCK	--> PTB0	(GPIO)			*/
		PORT_HAL_SetMuxMode(PORTB_BASE, 0, kPortMuxAsGpio);
	#endif

//TODO: we don't use HW flow control so can remove these since we don't use the RTS/CTS
	GPIO_DRV_ClearPinOutput(kWarpPinSPI_MOSI_UART_CTS);
	GPIO_DRV_ClearPinOutput(kWarpPinSPI_MISO_UART_RTS);
	GPIO_DRV_ClearPinOutput(kWarpPinSPI_SCK);

	CLOCK_SYS_DisableSpiClock(0);
}



void
warpDeasserAllSPIchipSelects(void)
{
	/*
	 *	By default, assusme pins are currently disabled (e.g., by a recent lowPowerPinStates())
	 *
	 *	Drive all chip selects high to disable them. Individual drivers call this routine before
	 *	appropriately asserting their respective chip selects.
	 *
	 *	Setup:
	 *		PTA12/kWarpPinISL23415_SPI_nCS	for GPIO
	 *		PTA9/kWarpPinAT45DB_SPI_nCS	for GPIO
	 *		PTA8/kWarpPinADXL362_SPI_nCS	for GPIO
	 *		PTB1/kWarpPinFPGA_nCS		for GPIO
	 *
	 *		On Glaux
	 		PTB2/kGlauxPinFlash_SPI_nCS for GPIO
	 */
	PORT_HAL_SetMuxMode(PORTA_BASE, 12, kPortMuxAsGpio);
	PORT_HAL_SetMuxMode(PORTA_BASE, 9, kPortMuxAsGpio);
	PORT_HAL_SetMuxMode(PORTA_BASE, 8, kPortMuxAsGpio);
	PORT_HAL_SetMuxMode(PORTB_BASE, 1, kPortMuxAsGpio);
	#if (WARP_BUILD_ENABLE_GLAUX_VARIANT)
		PORT_HAL_SetMuxMode(PORTB_BASE, 2, kPortMuxAsGpio);
	#endif

	#if (WARP_BUILD_ENABLE_DEVISL23415)
		GPIO_DRV_SetPinOutput(kWarpPinISL23415_SPI_nCS);
	#endif

	#if (WARP_BUILD_ENABLE_DEVAT45DB)
		GPIO_DRV_SetPinOutput(kWarpPinAT45DB_SPI_nCS);
	#endif

	#if (WARP_BUILD_ENABLE_DEVADXL362)
		GPIO_DRV_SetPinOutput(kWarpPinADXL362_SPI_nCS);
	#endif

	#if (WARP_BUILD_ENABLE_DEVICE40)
		GPIO_DRV_SetPinOutput(kWarpPinFPGA_nCS);
	#endif

	#if (WARP_BUILD_ENABLE_GLAUX_VARIANT)
		GPIO_DRV_SetPinOutput(kGlauxPinFlash_SPI_nCS);
	#endif
}



void
debugPrintSPIsinkBuffer(void)
{
	for (int i = 0; i < kWarpMemoryCommonSpiBufferBytes; i++)
	{
		warpPrint("\tgWarpSpiCommonSinkBuffer[%d] = [0x%02X]\n", i, gWarpSpiCommonSinkBuffer[i]);
	}
	warpPrint("\n");
}



void
warpEnableI2Cpins(void)
{
	CLOCK_SYS_EnableI2cClock(0);

	/*
	 *	Setup:
	 *
	 *		PTB3/kWarpPinI2C0_SCL_UART_TX	-->	(ALT2 == I2C)
	 *		PTB4/kWarpPinI2C0_SDA_UART_RX	-->	(ALT2 == I2C)
	 */
	PORT_HAL_SetMuxMode(PORTB_BASE, 3, kPortMuxAlt2);
	PORT_HAL_SetMuxMode(PORTB_BASE, 4, kPortMuxAlt2);

	I2C_DRV_MasterInit(0 /* I2C instance */, (i2c_master_state_t *)&i2cMasterState);
}



void
warpDisableI2Cpins(void)
{
	I2C_DRV_MasterDeinit(0 /* I2C instance */);

	/*
	 *	Setup:
	 *
	 *		PTB3/kWarpPinI2C0_SCL_UART_TX	-->	disabled
	 *		PTB4/kWarpPinI2C0_SDA_UART_RX	-->	disabled
	 */
	PORT_HAL_SetMuxMode(PORTB_BASE, 3, kPortPinDisabled);
	PORT_HAL_SetMuxMode(PORTB_BASE, 4, kPortPinDisabled);

	CLOCK_SYS_DisableI2cClock(0);
}


#if (WARP_BUILD_ENABLE_GLAUX_VARIANT)
	void
	lowPowerPinStates(void)
	{
		/*
		 *	Following Section 5 of "Power Management for Kinetis L Family" (AN5088.pdf),
		 *	we configure all pins as output and set them to a known state, except for the
		 *	sacrificial pins (WLCSP package, Glaux) where we set them to disabled. We choose
		 *	to set non-disabled pins to '0'.
		 *
		 *	NOTE: Pin state "disabled" means default functionality is active.
		 */

		/*
		 *			PORT A
		 */
		/*
		 *	Leave PTA0/1/2 SWD pins in their default state (i.e., as SWD / Alt3).
		 *
		 *	See GitHub issue https://github.com/physical-computation/Warp-firmware/issues/54
		 */
		PORT_HAL_SetMuxMode(PORTA_BASE, 0, kPortMuxAlt3);
		PORT_HAL_SetMuxMode(PORTA_BASE, 1, kPortMuxAlt3);
		PORT_HAL_SetMuxMode(PORTA_BASE, 2, kPortMuxAlt3);

		/*
		 *	PTA3 and PTA4 are the EXTAL0/XTAL0. They are also connected to the clock output
		 *	of the RV8803 (and PTA4 is a sacrificial pin for PTA3), so do not want to drive them.
		 *	We however have to configure PTA3 to Alt0 (kPortPinDisabled) to get the EXTAL0
		 *	functionality.
		 *
		 *	NOTE:	kPortPinDisabled is the equivalent of `Alt0`
		 */
		PORT_HAL_SetMuxMode(PORTA_BASE, 3, kPortPinDisabled);
		PORT_HAL_SetMuxMode(PORTA_BASE, 4, kPortPinDisabled);

		/*
		 *	Disable PTA5
		 *
		 *	NOTE: Enabling this significantly increases current draw
		 *	(from ~180uA to ~4mA) and we don't need the RTC on Glaux.
		 *
		 */
		PORT_HAL_SetMuxMode(PORTA_BASE, 5, kPortPinDisabled);

		/*
		 *	PTA6, PTA7, PTA8, and PTA9 on Glaux are SPI and sacrificial SPI.
		 *
		 *	Section 2.6 of Kinetis Energy Savings – Tips and Tricks says
		 *
		 *		"Unused pins should be configured in the disabled state, mux(0),
		 *		to prevent unwanted leakage (potentially caused by floating inputs)."
		 *
		 *	However, other documents advice to place pin as GPIO and drive low or high.
		 *	For now, leave disabled. Filed issue #54 low-power pin states to investigate.
		 */
		PORT_HAL_SetMuxMode(PORTA_BASE, 6, kPortPinDisabled);
		PORT_HAL_SetMuxMode(PORTA_BASE, 7, kPortPinDisabled);
		PORT_HAL_SetMuxMode(PORTA_BASE, 8, kPortPinDisabled);
		PORT_HAL_SetMuxMode(PORTA_BASE, 9, kPortPinDisabled);

		/*
		 *	NOTE: The KL03 has no PTA10 or PTA11
		 */

		/*
		 *	In Glaux, PTA12 is a sacrificial pin for SWD_RESET, so careful not to drive it.
		 */
		PORT_HAL_SetMuxMode(PORTA_BASE, 12, kPortPinDisabled);



		/*
		 *			PORT B
		 *
		 *	PTB0 is LED on Glaux. PTB1 is unused, and PTB2 is FLASH_!CS
		 */
		PORT_HAL_SetMuxMode(PORTB_BASE, 0, kPortMuxAsGpio);
		PORT_HAL_SetMuxMode(PORTB_BASE, 1, kPortPinDisabled);
		PORT_HAL_SetMuxMode(PORTB_BASE, 2, kPortMuxAsGpio);

		/*
		 *	PTB3 and PTB4 (I2C pins) are true open-drain and we
		 *	purposefully leave them disabled since they have pull-ups.
		 *	PTB5 is sacrificial for I2C_SDA, so disable.
		 */
		PORT_HAL_SetMuxMode(PORTB_BASE, 3, kPortPinDisabled);
		PORT_HAL_SetMuxMode(PORTB_BASE, 4, kPortPinDisabled);
		PORT_HAL_SetMuxMode(PORTB_BASE, 5, kPortPinDisabled);

		/*
		 *	NOTE:
		 *
		 *	The KL03 has no PTB8, PTB9, or PTB12.  Additionally, the WLCSP package
		 *	we in Glaux has no PTB6, PTB7, PTB10, or PTB11.
		 */

		/*
		 *	In Glaux, PTB13 is a sacrificial pin for SWD_RESET, so careful not to drive it.
		 */
		PORT_HAL_SetMuxMode(PORTB_BASE, 13, kPortPinDisabled);

		GPIO_DRV_SetPinOutput(kGlauxPinFlash_SPI_nCS);
		GPIO_DRV_ClearPinOutput(kGlauxPinLED);

		return;
	}
#else
	void
	lowPowerPinStates(void)
	{
		/*
		 *	Following Section 5 of "Power Management for Kinetis L Family" (AN5088.pdf),
		 *	we configure all pins as output and set them to a known state. We choose
		 *	to set them all to '0' since it happens that the devices we want to keep
		 *	deactivated (SI4705) also need '0'.
		 */

		/*
		 *			PORT A
		 */
		/*
		 *	For now, don't touch the PTA0/1/2 SWD pins. Revisit in the future.
		 */
		PORT_HAL_SetMuxMode(PORTA_BASE, 0, kPortMuxAlt3);
		PORT_HAL_SetMuxMode(PORTA_BASE, 1, kPortMuxAlt3);
		PORT_HAL_SetMuxMode(PORTA_BASE, 2, kPortMuxAlt3);

		/*
		 *	PTA3 and PTA4 are the EXTAL0/XTAL0. They are also connected to the clock output
		 *	of the RV8803 (and PTA4 is a sacrificial pin for PTA3), so do not want to drive them.
		 *	We however have to configure PTA3 to Alt0 (kPortPinDisabled) to get the EXTAL0
		 *	functionality.
		 *
		 *	NOTE:	kPortPinDisabled is the equivalent of `Alt0`
		 */
		PORT_HAL_SetMuxMode(PORTA_BASE, 3, kPortPinDisabled);
		PORT_HAL_SetMuxMode(PORTA_BASE, 4, kPortPinDisabled);

		/*
		 *	Disable PTA5
		 *
		 *	NOTE: Enabling this significantly increases current draw
		 *	(from ~180uA to ~4mA) and we don't need the RTC on revC.
		 *
		 */
		PORT_HAL_SetMuxMode(PORTA_BASE, 5, kPortPinDisabled);

		/*
		 *	Section 2.6 of Kinetis Energy Savings – Tips and Tricks says
		 *
		 *		"Unused pins should be configured in the disabled state, mux(0),
		 *		to prevent unwanted leakage (potentially caused by floating inputs)."
		 *
		 *	However, other documents advice to place pin as GPIO and drive low or high.
		 *	For now, leave disabled. Filed issue #54 low-power pin states to investigate.
		 */
		PORT_HAL_SetMuxMode(PORTA_BASE, 6, kPortPinDisabled);
		PORT_HAL_SetMuxMode(PORTA_BASE, 7, kPortPinDisabled);
		PORT_HAL_SetMuxMode(PORTA_BASE, 8, kPortPinDisabled);
		PORT_HAL_SetMuxMode(PORTA_BASE, 9, kPortPinDisabled);

		/*
		 *	NOTE: The KL03 has no PTA10 or PTA11
		 */
		PORT_HAL_SetMuxMode(PORTA_BASE, 12, kPortPinDisabled);


		/*
		 *			PORT B
		 */
		PORT_HAL_SetMuxMode(PORTB_BASE, 0, kPortPinDisabled);
		PORT_HAL_SetMuxMode(PORTB_BASE, 1, kPortPinDisabled);
		PORT_HAL_SetMuxMode(PORTB_BASE, 2, kPortPinDisabled);
		PORT_HAL_SetMuxMode(PORTB_BASE, 3, kPortPinDisabled);
		PORT_HAL_SetMuxMode(PORTB_BASE, 4, kPortPinDisabled);
		PORT_HAL_SetMuxMode(PORTB_BASE, 5, kPortPinDisabled);
		PORT_HAL_SetMuxMode(PORTB_BASE, 6, kPortPinDisabled);
		PORT_HAL_SetMuxMode(PORTB_BASE, 7, kPortPinDisabled);
		PORT_HAL_SetMuxMode(PORTB_BASE, 10, kPortPinDisabled);
		PORT_HAL_SetMuxMode(PORTB_BASE, 11, kPortPinDisabled);
		PORT_HAL_SetMuxMode(PORTB_BASE, 13, kPortPinDisabled);
	}
#endif


void
disableTPS62740(void)
{
	#if (!WARP_BUILD_ENABLE_GLAUX_VARIANT)
		GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_REGCTRL);
	#endif
}

void
enableTPS62740(uint16_t voltageMillivolts)
{
	#if (!WARP_BUILD_ENABLE_GLAUX_VARIANT)
		/*
		 *	By default, assusme pins are currently disabled (e.g., by a recent lowPowerPinStates())
		 *
		 *	Setup:
		 *		PTB5/kWarpPinTPS62740_REGCTRL for GPIO
		 *		PTB6/kWarpPinTPS62740_VSEL4 for GPIO
		 *		PTB7/kWarpPinTPS62740_VSEL3 for GPIO
		 *		PTB10/kWarpPinTPS62740_VSEL2 for GPIO
		 *		PTB11/kWarpPinTPS62740_VSEL1 for GPIO
		 */
		PORT_HAL_SetMuxMode(PORTB_BASE, 5, kPortMuxAsGpio);
		PORT_HAL_SetMuxMode(PORTB_BASE, 6, kPortMuxAsGpio);
		PORT_HAL_SetMuxMode(PORTB_BASE, 7, kPortMuxAsGpio);
		PORT_HAL_SetMuxMode(PORTB_BASE, 10, kPortMuxAsGpio);
		PORT_HAL_SetMuxMode(PORTB_BASE, 11, kPortMuxAsGpio);

		setTPS62740CommonControlLines(voltageMillivolts);
		GPIO_DRV_SetPinOutput(kWarpPinTPS62740_REGCTRL);
	#endif
}

void
setTPS62740CommonControlLines(uint16_t voltageMillivolts)
{
	#if (!WARP_BUILD_ENABLE_GLAUX_VARIANT)
		switch(voltageMillivolts)
		{
			case 1800:
			{
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL1);
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL2);
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL3);
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL4);

				break;
			}

			case 1900:
			{
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL1);
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL2);
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL3);
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL4);

				break;
			}

			case 2000:
			{
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL1);
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL2);
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL3);
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL4);

				break;
			}

			case 2100:
			{
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL1);
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL2);
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL3);
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL4);

				break;
			}

			case 2200:
			{
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL1);
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL2);
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL3);
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL4);

				break;
			}

			case 2300:
			{
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL1);
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL2);
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL3);
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL4);

				break;
			}

			case 2400:
			{
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL1);
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL2);
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL3);
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL4);

				break;
			}

			case 2500:
			{
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL1);
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL2);
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL3);
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL4);

				break;
			}

			case 2600:
			{
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL1);
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL2);
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL3);
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL4);

				break;
			}

			case 2700:
			{
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL1);
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL2);
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL3);
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL4);

				break;
			}

			case 2800:
			{
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL1);
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL2);
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL3);
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL4);

				break;
			}

			case 2900:
			{
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL1);
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL2);
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL3);
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL4);

				break;
			}

			case 3000:
			{
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL1);
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL2);
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL3);
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL4);

				break;
			}

			case 3100:
			{
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL1);
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL2);
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL3);
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL4);

				break;
			}

			case 3200:
			{
				GPIO_DRV_ClearPinOutput(kWarpPinTPS62740_VSEL1);
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL2);
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL3);
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL4);

				break;
			}

			case 3300:
			{
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL1);
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL2);
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL3);
				GPIO_DRV_SetPinOutput(kWarpPinTPS62740_VSEL4);

				break;
			}

			/*
			 *	Should never happen, due to previous check in warpScaleSupplyVoltage()
			 */
			default:
			{
				warpPrint(RTT_CTRL_RESET RTT_CTRL_BG_BRIGHT_YELLOW RTT_CTRL_TEXT_BRIGHT_WHITE kWarpConstantStringErrorSanity RTT_CTRL_RESET "\n");
			}
		}

		/*
		 *	Vload ramp time of the TPS62740 is 800us max (datasheet, Table 8.5 / page 6)
		 */
		OSA_TimeDelay(gWarpSupplySettlingDelayMilliseconds);
	#endif
}



void
warpScaleSupplyVoltage(uint16_t voltageMillivolts)
{
	if (voltageMillivolts == gWarpCurrentSupplyVoltage)
	{
		return;
	}

	#if (!WARP_BUILD_ENABLE_GLAUX_VARIANT)
		if (voltageMillivolts >= 1800 && voltageMillivolts <= 3300)
		{
			enableTPS62740(voltageMillivolts);
			gWarpCurrentSupplyVoltage = voltageMillivolts;
		}
		else
		{
			warpPrint(RTT_CTRL_RESET RTT_CTRL_BG_BRIGHT_RED RTT_CTRL_TEXT_BRIGHT_WHITE kWarpConstantStringErrorInvalidVoltage RTT_CTRL_RESET "\n", voltageMillivolts);
		}
	#endif
}



void
warpDisableSupplyVoltage(void)
{
	#if (!WARP_BUILD_ENABLE_GLAUX_VARIANT)
		disableTPS62740();

		/*
		 *	Vload ramp time of the TPS62740 is 800us max (datasheet, Table 8.5 / page 6)
		 */
		OSA_TimeDelay(gWarpSupplySettlingDelayMilliseconds);
	#endif
}


void
warpLowPowerSecondsSleep(uint32_t sleepSeconds, bool forceAllPinsIntoLowPowerState)
{
	WarpStatus	status = kWarpStatusOK;

	/*
	 *	Set all pins into low-power states. We don't just disable all pins,
	 *	as the various devices hanging off will be left in higher power draw
	 *	state. And manuals say set pins to output to reduce power.
	 */
	if (forceAllPinsIntoLowPowerState)
	{
		lowPowerPinStates();
	}

	warpSetLowPowerMode(kWarpPowerModeVLPR, 0 /* Sleep Seconds */);
	if ((status != kWarpStatusOK) && (status != kWarpStatusPowerTransitionErrorVlpr2Vlpr))
	{
		warpPrint("warpSetLowPowerMode(kWarpPowerModeVLPR, 0 /* sleep seconds : irrelevant here */)() failed...\n");
	}

	status = warpSetLowPowerMode(kWarpPowerModeVLPS, sleepSeconds);
	if (status != kWarpStatusOK)
	{
		warpPrint("warpSetLowPowerMode(kWarpPowerModeVLPS, 0 /* sleep seconds : irrelevant here */)() failed...\n");
	}
}


/*
void
printPinDirections(void)
{
	warpPrint("I2C0_SDA:%d\n", GPIO_DRV_GetPinDir(kWarpPinI2C0_SDA_UART_RX));
	OSA_TimeDelay(100);
	warpPrint("I2C0_SCL:%d\n", GPIO_DRV_GetPinDir(kWarpPinI2C0_SCL_UART_TX));
	OSA_TimeDelay(100);
	warpPrint("SPI_MOSI:%d\n", GPIO_DRV_GetPinDir(kWarpPinSPI_MOSI_UART_CTS));
	OSA_TimeDelay(100);
	warpPrint("SPI_MISO:%d\n", GPIO_DRV_GetPinDir(kWarpPinSPI_MISO_UART_RTS));
	OSA_TimeDelay(100);
	warpPrint("SPI_SCK_I2C_PULLUP_EN:%d\n", GPIO_DRV_GetPinDir(kWarpPinSPI_SCK_I2C_PULLUP_EN));
	OSA_TimeDelay(100);
	warpPrint("ADXL362_CS:%d\n", GPIO_DRV_GetPinDir(kWarpPinADXL362_CS));
	OSA_TimeDelay(100);
}
*/



void
dumpProcessorState(void)
{
	uint32_t	cpuClockFrequency;

	CLOCK_SYS_GetFreq(kCoreClock, &cpuClockFrequency);
	warpPrint("\r\n\n\tCPU @ %u KHz\n", (cpuClockFrequency / 1000));
	warpPrint("\r\tCPU power mode: %u\n", POWER_SYS_GetCurrentMode());
	warpPrint("\r\tCPU clock manager configuration: %u\n", CLOCK_SYS_GetCurrentConfiguration());
	warpPrint("\r\tRTC clock: %d\n", CLOCK_SYS_GetRtcGateCmd(0));
	warpPrint("\r\tSPI clock: %d\n", CLOCK_SYS_GetSpiGateCmd(0));
	warpPrint("\r\tI2C clock: %d\n", CLOCK_SYS_GetI2cGateCmd(0));
	warpPrint("\r\tLPUART clock: %d\n", CLOCK_SYS_GetLpuartGateCmd(0));
	warpPrint("\r\tPORT A clock: %d\n", CLOCK_SYS_GetPortGateCmd(0));
	warpPrint("\r\tPORT B clock: %d\n", CLOCK_SYS_GetPortGateCmd(1));
	warpPrint("\r\tFTF clock: %d\n", CLOCK_SYS_GetFtfGateCmd(0));
	warpPrint("\r\tADC clock: %d\n", CLOCK_SYS_GetAdcGateCmd(0));
	warpPrint("\r\tCMP clock: %d\n", CLOCK_SYS_GetCmpGateCmd(0));
	warpPrint("\r\tVREF clock: %d\n", CLOCK_SYS_GetVrefGateCmd(0));
	warpPrint("\r\tTPM clock: %d\n", CLOCK_SYS_GetTpmGateCmd(0));
}


void
printBootSplash(uint16_t gWarpCurrentSupplyVoltage, uint8_t menuRegisterAddress, WarpPowerManagerCallbackStructure *  powerManagerCallbackStructure)
{
	/*
	 *	We break up the prints with small delays to allow us to use small RTT print
	 *	buffers without overrunning them when at max CPU speed.
	 */
	warpPrint("\r\n\n\n\n[ *\t\t\t\tWarp (HW revision C) / Glaux (HW revision B)\t\t\t* ]\n");
	warpPrint("\r[  \t\t\t\t      Cambridge / Physcomplab   \t\t\t\t  ]\n\n");
	warpPrint("\r\tSupply=%dmV,\tDefault Target Read Register=0x%02x\n",
			gWarpCurrentSupplyVoltage, menuRegisterAddress);
	warpPrint("\r\tI2C=%dkb/s,\tSPI=%dkb/s,\tUART=%db/s,\tI2C Pull-Up=%d\n\n",
			gWarpI2cBaudRateKbps, gWarpSpiBaudRateKbps, gWarpUartBaudRateBps);
	warpPrint("\r\tSIM->SCGC6=0x%02x\t\tRTC->SR=0x%02x\t\tRTC->TSR=0x%02x\n", SIM->SCGC6, RTC->SR, RTC->TSR);
	warpPrint("\r\tMCG_C1=0x%02x\t\t\tMCG_C2=0x%02x\t\tMCG_S=0x%02x\n", MCG_C1, MCG_C2, MCG_S);
	warpPrint("\r\tMCG_SC=0x%02x\t\t\tMCG_MC=0x%02x\t\tOSC_CR=0x%02x\n", MCG_SC, MCG_MC, OSC_CR);
	warpPrint("\r\tSMC_PMPROT=0x%02x\t\t\tSMC_PMCTRL=0x%02x\t\tSCB->SCR=0x%02x\n", SMC_PMPROT, SMC_PMCTRL, SCB->SCR);
	warpPrint("\r\tPMC_REGSC=0x%02x\t\t\tSIM_SCGC4=0x%02x\tRTC->TPR=0x%02x\n\n", PMC_REGSC, SIM_SCGC4, RTC->TPR);
	warpPrint("\r\t%ds in RTC Handler to-date,\t%d Pmgr Errors\n", gWarpSleeptimeSeconds, powerManagerCallbackStructure->errorCount);
}

void
blinkLED(int pin)
{
	GPIO_DRV_SetPinOutput(pin);
	OSA_TimeDelay(200);
	GPIO_DRV_ClearPinOutput(pin);
	OSA_TimeDelay(200);

	return;
}

void
warpPrint(const char *fmt, ...)
{
	int	fmtlen;
	va_list	arg;

	/*
	 *	We use an ifdef rather than a C if to allow us to compile-out
	 *	all references to SEGGER_RTT_*printf if we don't want them.
	 *
	 *	NOTE: SEGGER_RTT_vprintf takes a va_list* rather than a va_list
	 *	like usual vprintf. We modify the SEGGER_RTT_vprintf so that it
	 *	also takes our print buffer which we will eventually send over
	 *	BLE. Using SEGGER_RTT_vprintf() versus the libc vsnprintf saves
	 *	2kB flash and removes the use of malloc so we can keep heap
	 *	allocation to zero.
	 */
	#if (WARP_BUILD_ENABLE_SEGGER_RTT_PRINTF)
		/*
		 *	We can't use SEGGER_RTT_vprintf to format into a buffer
		 *	since SEGGER_RTT_vprintf formats directly into the special
		 *	RTT memory region to be picked up by the RTT / SWD mechanism...
		 */
		va_start(arg, fmt);
		fmtlen = SEGGER_RTT_vprintf(0, fmt, &arg, gWarpPrintBuffer, kWarpDefaultPrintBufferSizeBytes);
		va_end(arg);

		if (fmtlen < 0)
		{
			SEGGER_RTT_WriteString(0, gWarpEfmt);

			#if (WARP_BUILD_ENABLE_DEVBGX)
				if (gWarpBooted)
				{
					WarpStatus	status;

					enableLPUARTpins();
					initBGX(kWarpDefaultSupplyVoltageMillivoltsBGX);
					status = sendBytesToUART((uint8_t *)gWarpEfmt, strlen(gWarpEfmt)+1);
					if (status != kWarpStatusOK)
					{
						SEGGER_RTT_WriteString(0, gWarpEuartSendChars);
					}
					disableLPUARTpins();

					/*
					 *	We don't want to deInit() the BGX since that would drop
					 *	any remote terminal connected to it.
					 */
					//deinitBGX();
				}
			#endif

			return;
		}

		/*
		 *	If WARP_BUILD_ENABLE_DEVBGX, also send the fmt to the UART / BLE.
		 */
		#if (WARP_BUILD_ENABLE_DEVBGX)
			if (gWarpBooted)
			{
				WarpStatus	status;

				enableLPUARTpins();
				initBGX(kWarpDefaultSupplyVoltageMillivoltsBGX);

				status = sendBytesToUART((uint8_t *)gWarpPrintBuffer, max(fmtlen, kWarpDefaultPrintBufferSizeBytes));
				if (status != kWarpStatusOK)
				{
					SEGGER_RTT_WriteString(0, gWarpEuartSendChars);
				}
				disableLPUARTpins();

				/*
				 *	We don't want to deInit() the BGX since that would drop
				 *	any remote terminal connected to it.
				 */
				//deinitBGX();
			}
		#endif
	#else
		/*
		 *	If we are not compiling in the SEGGER_RTT_printf,
		 *	we just send the format string of warpPrint()
		 */
		SEGGER_RTT_WriteString(0, fmt);

		/*
		 *	If WARP_BUILD_ENABLE_DEVBGX, also send the fmt to the UART / BLE.
		 */
		#if (WARP_BUILD_ENABLE_DEVBGX)
			if (gWarpBooted)
			{
				WarpStatus	status;

				enableLPUARTpins();
				initBGX(kWarpDefaultSupplyVoltageMillivoltsBGX);
				status = sendBytesToUART(fmt, strlen(fmt));
				if (status != kWarpStatusOK)
				{
					SEGGER_RTT_WriteString(0, gWarpEuartSendChars);
				}
				disableLPUARTpins();

				/*
				 *	We don't want to deInit() the BGX since that would drop
				 *	any remote terminal connected to it.
				 */
				//deinitBGX();
			}
		#endif
	#endif

	return;
}

int
warpWaitKey(void)
{
	/*
	 *	SEGGER'S implementation assumes the result of result of
	 *	SEGGER_RTT_GetKey() is an int, so we play along.
	 */
	int		rttKey, bleChar = kWarpMiscMarkerForAbsentByte;

	/*
	 *	Set the UART buffer to 0xFF and then wait until either the
	 *	UART RX buffer changes or the RTT icoming key changes.
	 *
	 *	The check below on rttKey is exactly what SEGGER_RTT_WaitKey()
	 *	does in SEGGER_RTT.c.
	 */
	#if (WARP_BUILD_ENABLE_DEVBGX)
		deviceBGXState.uartRXBuffer[0] = kWarpMiscMarkerForAbsentByte;
		enableLPUARTpins();
		initBGX(kWarpDefaultSupplyVoltageMillivoltsBGX);
	#endif

	do
	{
		rttKey	= SEGGER_RTT_GetKey();

		#if (WARP_BUILD_ENABLE_DEVBGX)
			bleChar	= deviceBGXState.uartRXBuffer[0];
		#endif

		/*
		 *	NOTE: We ignore all chars on BLE except '0'-'9', 'a'-'z'/'A'-Z'
		 */
		if (!(bleChar > 'a' && bleChar < 'z') && !(bleChar > 'A' && bleChar < 'Z') && !(bleChar > '0' && bleChar < '9'))
		{
			bleChar = kWarpMiscMarkerForAbsentByte;
		}
	} while ((rttKey < 0) && (bleChar == kWarpMiscMarkerForAbsentByte));

	#if (WARP_BUILD_ENABLE_DEVBGX)
		if (bleChar != kWarpMiscMarkerForAbsentByte)
		{
			/*
			 *	Send a copy of incoming BLE chars to RTT
			 */
			SEGGER_RTT_PutChar(0, bleChar);
			disableLPUARTpins();

			/*
			 *	We don't want to deInit() the BGX since that would drop
			 *	any remote terminal connected to it.
			 */
			//deinitBGX();

			return (int)bleChar;
		}

		/*
		 *	Send a copy of incoming RTT chars to BLE
		 */
		WarpStatus status = sendBytesToUART((uint8_t *)&rttKey, 1);
		if (status != kWarpStatusOK)
		{
			SEGGER_RTT_WriteString(0, gWarpEuartSendChars);
		}

		disableLPUARTpins();

		/*
		 *	We don't want to deInit() the BGX since that would drop
		 *	any remote terminal connected to it.
		 */
		//deinitBGX();
	#endif

	return rttKey;
}

int
main(void)
{
	WarpStatus				status;
	uint8_t					key;
	WarpSensorDevice			menuTargetSensor		= kWarpSensorBMX055accel;
	volatile WarpI2CDeviceState *		menuI2cDevice			= NULL;
	uint8_t					menuRegisterAddress		= 0x00;
	rtc_datetime_t				warpBootDate;
	power_manager_user_config_t		warpPowerModeWaitConfig;
	power_manager_user_config_t		warpPowerModeStopConfig;
	power_manager_user_config_t		warpPowerModeVlpwConfig;
	power_manager_user_config_t		warpPowerModeVlpsConfig;
	power_manager_user_config_t		warpPowerModeVlls0Config;
	power_manager_user_config_t		warpPowerModeVlls1Config;
	power_manager_user_config_t		warpPowerModeVlls3Config;
	power_manager_user_config_t		warpPowerModeRunConfig;

	/*
	 *	We use this as a template later below and change the .mode fields for the different other modes.
	 */
	const power_manager_user_config_t	warpPowerModeVlprConfig = {
							.mode			= kPowerManagerVlpr,
							.sleepOnExitValue	= false,
							.sleepOnExitOption	= false
						};

	power_manager_user_config_t const *	powerConfigs[] = {
							/*
							 *	NOTE: POWER_SYS_SetMode() depends on this order
							 *
							 *	See KSDK13APIRM.pdf Section 55.5.3
							 */
							&warpPowerModeWaitConfig,
							&warpPowerModeStopConfig,
							&warpPowerModeVlprConfig,
							&warpPowerModeVlpwConfig,
							&warpPowerModeVlpsConfig,
							&warpPowerModeVlls0Config,
							&warpPowerModeVlls1Config,
							&warpPowerModeVlls3Config,
							&warpPowerModeRunConfig,
						};

	WarpPowerManagerCallbackStructure		powerManagerCallbackStructure;

	/*
	 *	Callback configuration structure for power manager
	 */
	const power_manager_callback_user_config_t callbackCfg0 = {
							callback0,
							kPowerManagerCallbackBeforeAfter,
							(power_manager_callback_data_t *) &powerManagerCallbackStructure};

	/*
	 *	Pointers to power manager callbacks.
	 */
	power_manager_callback_user_config_t const *	callbacks[] = {
								&callbackCfg0
						};

	/*
	 *	Enable clock for I/O PORT A and PORT B
	 */
	CLOCK_SYS_EnablePortClock(0);
	CLOCK_SYS_EnablePortClock(1);

	/*
	 *	Set board crystal value (Warp revB and earlier).
	 */
	g_xtal0ClkFreq = 32768U;

	/*
	 *	Initialize KSDK Operating System Abstraction layer (OSA) layer.
	 */
	OSA_Init();

	/*
	 *	Setup SEGGER RTT to output as much as fits in buffers.
	 *
	 *	Using SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL can lead to deadlock, since
	 *	we might have SWD disabled at time of blockage.
	 */
	SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_TRIM);

	/*
	 *	When booting to CSV stream, we wait to be up and running as soon as possible after
	 *	a reset (e.g., a reset due to waking from VLLS0)
	 */
	if (!WARP_BUILD_BOOT_TO_CSVSTREAM)
	{
		warpPrint("\n\n\n\rBooting Warp, in 3... ");
		OSA_TimeDelay(1000);
		warpPrint("2... ");
		OSA_TimeDelay(1000);
		warpPrint("1...\n\n\n\r");
		OSA_TimeDelay(1000);
	}

	/*
	 *	Configure Clock Manager to default, and set callback for Clock Manager mode transition.
	 *
	 *	See "Clocks and Low Power modes with KSDK and Processor Expert" document (Low_Power_KSDK_PEx.pdf)
	 */
	CLOCK_SYS_Init(	g_defaultClockConfigurations,
			CLOCK_CONFIG_NUM, /* The default value of this is defined in fsl_clock_MKL03Z4.h as 2 */
			&clockCallbackTable,
			ARRAY_SIZE(clockCallbackTable)
			);
	CLOCK_SYS_UpdateConfiguration(CLOCK_CONFIG_INDEX_FOR_RUN, kClockManagerPolicyForcible);

	/*
	 *	Initialize RTC Driver (not needed on Glaux, but we enable it anyway for now
	 *	as that lets us use the current sleep routines). NOTE: We also don't seem to
	 *	be able to go to VLPR mode unless we enable the RTC.
	 */
	RTC_DRV_Init(0);

	/*
	 *	Set initial date to 1st January 2016 00:00, and set date via RTC driver
	 */
	warpBootDate.year	= 2016U;
	warpBootDate.month	= 1U;
	warpBootDate.day	= 1U;
	warpBootDate.hour	= 0U;
	warpBootDate.minute	= 0U;
	warpBootDate.second	= 0U;
	RTC_DRV_SetDatetime(0, &warpBootDate);

	/*
	 *	Setup Power Manager Driver
	 */
	memset(&powerManagerCallbackStructure, 0, sizeof(WarpPowerManagerCallbackStructure));

	warpPowerModeVlpwConfig = warpPowerModeVlprConfig;
	warpPowerModeVlpwConfig.mode = kPowerManagerVlpw;

	warpPowerModeVlpsConfig = warpPowerModeVlprConfig;
	warpPowerModeVlpsConfig.mode = kPowerManagerVlps;

	warpPowerModeWaitConfig = warpPowerModeVlprConfig;
	warpPowerModeWaitConfig.mode = kPowerManagerWait;

	warpPowerModeStopConfig = warpPowerModeVlprConfig;
	warpPowerModeStopConfig.mode = kPowerManagerStop;

	warpPowerModeVlls0Config = warpPowerModeVlprConfig;
	warpPowerModeVlls0Config.mode = kPowerManagerVlls0;

	warpPowerModeVlls1Config = warpPowerModeVlprConfig;
	warpPowerModeVlls1Config.mode = kPowerManagerVlls1;

	warpPowerModeVlls3Config = warpPowerModeVlprConfig;
	warpPowerModeVlls3Config.mode = kPowerManagerVlls3;

	warpPowerModeRunConfig.mode = kPowerManagerRun;

	POWER_SYS_Init(	&powerConfigs,
			sizeof(powerConfigs)/sizeof(power_manager_user_config_t *),
			&callbacks,
			sizeof(callbacks)/sizeof(power_manager_callback_user_config_t *)
			);

	/*
	 *	Switch CPU to Very Low Power Run (VLPR) mode
	 */
	if (WARP_BUILD_BOOT_TO_VLPR)
	{
		warpPrint("About to switch CPU to VLPR mode... ");
		status = warpSetLowPowerMode(kWarpPowerModeVLPR, 0 /* Sleep Seconds */);
		if ((status != kWarpStatusOK) && (status != kWarpStatusPowerTransitionErrorVlpr2Vlpr))
		{
			warpPrint("warpSetLowPowerMode(kWarpPowerModeVLPR() failed...\n");
		}
		warpPrint("done.\n\r");
	}

	/*
	 *	Initialize the GPIO pins with the appropriate pull-up, etc.,
	 *	defined in the inputPins and outputPins arrays (gpio_pins.c).
	 *
	 *	See also Section 30.3.3 GPIO Initialization of KSDK13APIRM.pdf
	 */
	warpPrint("About to GPIO_DRV_Init()... ");
	GPIO_DRV_Init(inputPins  /* input pins */, outputPins  /* output pins */);
	warpPrint("done.\n");

	/*
	 *	Make sure the SWD pins, PTA0/1/2 SWD pins in their ALT3 state (i.e., as SWD).
	 *
	 *	See GitHub issue https://github.com/physical-computation/Warp-firmware/issues/54
	 */
	PORT_HAL_SetMuxMode(PORTA_BASE, 0, kPortMuxAlt3);
	PORT_HAL_SetMuxMode(PORTA_BASE, 1, kPortMuxAlt3);
	PORT_HAL_SetMuxMode(PORTA_BASE, 2, kPortMuxAlt3);

	/*
	 *	Note that it is lowPowerPinStates() that sets the pin mux mode,
	 *	so until we call it pins are in their default state.
	 */
	warpPrint("About to lowPowerPinStates()... ");
	lowPowerPinStates();
	warpPrint("done.\n");

	/*
	 *	Toggle LED3 (kWarpPinSI4705_nRST on Warp revB, kGlauxPinLED on Glaux)
	 */
	#if (WARP_BUILD_ENABLE_GLAUX_VARIANT)
		blinkLED(kGlauxPinLED);
		blinkLED(kGlauxPinLED);
		blinkLED(kGlauxPinLED);

		USED(disableTPS62740);
		USED(enableTPS62740);
		USED(setTPS62740CommonControlLines);
	#endif

	/*
	 *	Initialize all the sensors
	 */
	#if (WARP_BUILD_ENABLE_DEVBMX055)
		initBMX055accel(0x18	/* i2cAddress */,	&deviceBMX055accelState,	kWarpDefaultSupplyVoltageMillivoltsBMX055accel	);
		initBMX055gyro(	0x68	/* i2cAddress */,	&deviceBMX055gyroState,		kWarpDefaultSupplyVoltageMillivoltsBMX055gyro	);
		initBMX055mag(	0x10	/* i2cAddress */,	&deviceBMX055magState,		kWarpDefaultSupplyVoltageMillivoltsBMX055mag	);
	#endif

	#if (WARP_BUILD_ENABLE_DEVINA219)
		initINA219(	0x40	/* i2cAddress */,		kWarpDefaultSupplyVoltageMillivoltsINA219	);
	#endif

	#if (WARP_BUILD_ENABLE_DEVMMA8451Q)
		initMMA8451Q(	0x1D	/* i2cAddress */,		kWarpDefaultSupplyVoltageMillivoltsMMA8451Q	);
	#endif

	#if (WARP_BUILD_ENABLE_DEVLPS25H)
		initLPS25H(	0x5C	/* i2cAddress */,	&deviceLPS25HState,		kWarpDefaultSupplyVoltageMillivoltsLPS25H	);
	#endif

	#if (WARP_BUILD_ENABLE_DEVHDC1000)
		initHDC1000(	0x43	/* i2cAddress */,	&deviceHDC1000State,		kWarpDefaultSupplyVoltageMillivoltsHDC1000	);
	#endif

	#if (WARP_BUILD_ENABLE_DEVMAG3110)
		initMAG3110(	0x0E	/* i2cAddress */,	&deviceMAG3110State,		kWarpDefaultSupplyVoltageMillivoltsMAG3110	);
	#endif

	#if (WARP_BUILD_ENABLE_DEVSI7021)
		initSI7021(	0x40	/* i2cAddress */,	&deviceSI7021State,		kWarpDefaultSupplyVoltageMillivoltsSI7021	);
	#endif

	#if (WARP_BUILD_ENABLE_DEVL3GD20H)
		initL3GD20H(	0x6A	/* i2cAddress */,	&deviceL3GD20HState,		kWarpDefaultSupplyVoltageMillivoltsL3GD20H	);
	#endif

	#if (WARP_BUILD_ENABLE_DEVBME680)
//		initBME680(	0x77	/* i2cAddress */,	&deviceBME680State,		kWarpDefaultSupplyVoltageMillivoltsBME680	);
		initBME680(	0x77	/* i2cAddress */,		kWarpDefaultSupplyVoltageMillivoltsBME680	);
	#endif

	#if (WARP_BUILD_ENABLE_DEVTCS34725)
		initTCS34725(	0x29	/* i2cAddress */,	&deviceTCS34725State,		kWarpDefaultSupplyVoltageMillivoltsTCS34725	);
	#endif

	#if (WARP_BUILD_ENABLE_DEVSI4705)
		initSI4705(	0x11	/* i2cAddress */,	&deviceSI4705State,		kWarpDefaultSupplyVoltageMillivoltsSI4705	);
	#endif

	#if (WARP_BUILD_ENABLE_DEVCCS811)
		initCCS811(	0x5A	/* i2cAddress */,	&deviceCCS811State,		kWarpDefaultSupplyVoltageMillivoltsCCS811	);
	#endif

	#if (WARP_BUILD_ENABLE_DEVAMG8834)
		initAMG8834(	0x68	/* i2cAddress */,	&deviceAMG8834State,		kWarpDefaultSupplyVoltageMillivoltsAMG8834	);
	#endif

	#if (WARP_BUILD_ENABLE_DEVAS7262)
		initAS7262(	0x49	/* i2cAddress */,	&deviceAS7262State,		kWarpDefaultSupplyVoltageMillivoltsAS7262	);
	#endif

	#if (WARP_BUILD_ENABLE_DEVAS7263)
		initAS7263(	0x49	/* i2cAddress */,	&deviceAS7263State,		kWarpDefaultSupplyVoltageMillivoltsAS7263	);
	#endif

	#if (WARP_BUILD_ENABLE_DEVRV8803C7)
		initRV8803C7(	0x32	/* i2cAddress */,					kWarpDefaultSupplyVoltageMillivoltsRV8803C7	);
		status = setRTCCountdownRV8803C7(0 /* countdown */, kWarpRV8803ExtTD_1HZ /* frequency */, false /* interupt_enable */);
		if (status != kWarpStatusOK)
		{
			warpPrint("setRTCCountdownRV8803C7() failed...\n");
		}
		else
		{
			warpPrint("setRTCCountdownRV8803C7() succeeded.\n");
		}

		/*
		 *	Set the CLKOUT frequency to 1Hz, to reduce CV^2 power on the CLKOUT pin.
		 *	See RV-8803-C7_App-Manual.pdf section 3.6 (register is 0Dh)
		 */
		uint8_t	extReg;
		status = readRTCRegisterRV8803C7(kWarpRV8803RegExt, &extReg);
		if (status != kWarpStatusOK)
		{
			warpPrint("readRTCRegisterRV8803C7() failed...\n");
		}
		else
		{
			warpPrint("readRTCRegisterRV8803C7() succeeded.\n");
		}

		/*
		 *	Set bits 3:2 (FD) to 10 (1Hz CLKOUT)
		 */
		extReg &= 0b11110011;
		extReg |= 0b00001000;
		status = writeRTCRegisterRV8803C7(kWarpRV8803RegExt, extReg);
		if (status != kWarpStatusOK)
		{
			warpPrint("writeRTCRegisterRV8803C7() failed...\n");
		}
		else
		{
			warpPrint("writeRTCRegisterRV8803C7() succeeded.\n");
		}
	#endif

	/*
	 *	Initialization: Devices hanging off SPI
	 */

	#if (WARP_BUILD_ENABLE_DEVADXL362)
		/*
		 *	Only supported in main Warp variant.
		 */
		initADXL362(kWarpPinADXL362_SPI_nCS,						kWarpDefaultSupplyVoltageMillivoltsADXL362	);

		status = readSensorRegisterADXL362(kWarpSensorConfigurationRegisterADXL362DEVID_AD, 1);
		if (status != kWarpStatusOK)
		{
			warpPrint("ADXL362: SPI transaction to read DEVID_AD failed...\n");
		}
		else
		{
			warpPrint("ADXL362: DEVID_AD = [0x%02X].\n", deviceADXL362State.spiSinkBuffer[2]);
		}

		status = readSensorRegisterADXL362(kWarpSensorConfigurationRegisterADXL362DEVID_MST, 1);
		if (status != kWarpStatusOK)
		{
			warpPrint("ADXL362: SPI transaction to read DEVID_MST failed...\n");
		}
		else
		{
			warpPrint("ADXL362: DEVID_MST = [0x%02X].\n", deviceADXL362State.spiSinkBuffer[2]);
		}
	#endif

	#if (WARP_BUILD_ENABLE_DEVIS25xP && WARP_BUILD_ENABLE_GLAUX_VARIANT)
		/*
		 *	Only supported in Glaux.
		 */
		initIS25xP(kGlauxPinFlash_SPI_nCS,						kWarpDefaultSupplyVoltageMillivoltsIS25xP	);

		uint8_t	ops1[] = {0x9F /* op0 */,  0x00 /* op1 */,  0x00 /* op2 */, 0x00 /* op3 */, 0x00 /* op4 */};
		status = spiTransactionIS25xP(ops1, sizeof(ops1)/sizeof(uint8_t) /* opCount */);
		if (status != kWarpStatusOK)
		{
			warpPrint("IS25xP: SPI transaction to read JEDEC ID failed...\n");
		}
		else
		{
			warpPrint("IS25xP JEDEC ID = [0x%X] [0x%X] [0x%X]\n", deviceIS25xPState.spiSinkBuffer[1], deviceIS25xPState.spiSinkBuffer[2], deviceIS25xPState.spiSinkBuffer[3]);
		}

		uint8_t	ops2[] = {0x90 /* op0 */,  0x00 /* op1 */,  0x00 /* op2 */, 0x00 /* op3 */, 0x00 /* op4 */};
		status = spiTransactionIS25xP(ops2, sizeof(ops2)/sizeof(uint8_t) /* opCount */);
		if (status != kWarpStatusOK)
		{
			warpPrint("IS25xP: SPI transaction to read Manufacturer ID failed...\n");
		}
		else
		{
			warpPrint("IS25xP Manufacturer ID = [0x%X] [0x%X] [0x%X]\n", deviceIS25xPState.spiSinkBuffer[3], deviceIS25xPState.spiSinkBuffer[4], deviceIS25xPState.spiSinkBuffer[5]);
		}

		uint8_t	ops3[] = {0xAB /* op0 */,  0x00 /* op1 */,  0x00 /* op2 */, 0x00 /* op3 */, 0x00 /* op4 */};
		status = spiTransactionIS25xP(ops3, sizeof(ops3)/sizeof(uint8_t) /* opCount */);
				if (status != kWarpStatusOK)
		{
			warpPrint("IS25xP: SPI transaction to read Flash ID failed...\n");
		}
		else
		{
			warpPrint("IS25xP Flash ID = [0x%X]\n", deviceIS25xPState.spiSinkBuffer[4]);
		}
		
	#endif

	#if (WARP_BUILD_ENABLE_DEVISL23415)
		/*
		 *	Only supported in main Warp variant.
		 */
		initISL23415(kWarpPinISL23415_SPI_nCS, kWarpDefaultSupplyVoltageMillivoltsISL23415);

		/*
		 *	Take the DCPs out of shutdown by setting the SHDN bit in the ACR register
		 */
		status = writeDeviceRegisterISL23415(kWarpSensorConfigurationRegisterISL23415ACRwriteInstruction, 0x40);
		if (status != kWarpStatusOK)
		{
			warpPrint("ISL23415: SPI transaction to write ACR failed...\n");
		}

		status = readDeviceRegisterISL23415(kWarpSensorConfigurationRegisterISL23415ACRreadInstruction);
		if (status != kWarpStatusOK)
		{
			warpPrint("ISL23415: SPI transaction to read ACR failed...\n");
		}
		else
		{
			warpPrint("ISL23415 ACR=[0x%02X], ", deviceISL23415State.spiSinkBuffer[3]);
		}

		status = readDeviceRegisterISL23415(kWarpSensorConfigurationRegisterISL23415WRreadInstruction);
		if (status != kWarpStatusOK)
		{
			warpPrint("ISL23415: SPI transaction to read WR failed...\n");
		}
		else
		{
			warpPrint("WR=[0x%02X]\n", deviceISL23415State.spiSinkBuffer[3]);
		}
	#endif

	#if (WARP_BUILD_ENABLE_DEVAT45DB)
		/*
		 *	Only supported in main Warp variant.
		 */
		initAT45DB(kWarpPinAT45DB_SPI_nCS,						kWarpDefaultSupplyVoltageMillivoltsAT45DB	);

		status = spiTransactionAT45DB(&deviceAT45DBState, (uint8_t *)"\x9F\x00\x00\x00\x00\x00", 6 /* opCount */);
		if (status != kWarpStatusOK)
		{
			warpPrint("AT45DB: SPI transaction to read Manufacturer ID failed...\n");
		}
		else
		{
			warpPrint("AT45DB Manufacturer ID=[0x%02X], Device ID=[0x%02X 0x%02X], Extended Device Information=[0x%02X 0x%02X]\n",
						deviceAT45DBState.spiSinkBuffer[1],
						deviceAT45DBState.spiSinkBuffer[2], deviceAT45DBState.spiSinkBuffer[3],
						deviceAT45DBState.spiSinkBuffer[4], deviceAT45DBState.spiSinkBuffer[5]);
		}
	#endif

	#if (WARP_BUILD_ENABLE_DEVICE40)
		/*
		 *	Only supported in main Warp variant.
		 */
		initICE40(kWarpPinFPGA_nCS,							kWarpDefaultSupplyVoltageMillivoltsICE40	);
	#endif

	#if (WARP_BUILD_ENABLE_DEVBGX)
		warpPrint("Configuring BGX Bluetooth.\n");
		warpPrint("Enabling UART... ");
		enableLPUARTpins();
		warpPrint("done.\n");
		warpPrint("initBGX()... ");
		initBGX(kWarpDefaultSupplyVoltageMillivoltsBGX);
		warpPrint("done.\n");
	#endif

	/*
	 *	If WARP_BUILD_DISABLE_SUPPLIES_BY_DEFAULT, will turn of the supplies
	 *	below which also means that the console via BLE will be disabled as
	 *	the BLE module will be turned off by default.
	 */
	#if (WARP_BUILD_DISABLE_SUPPLIES_BY_DEFAULT)
		/*
		*	Make sure sensor supplies are off.
		*
		*	(There's no point in calling activateAllLowPowerSensorModes())
		*/
		warpPrint("Disabling sensor supply... \n");
		warpDisableSupplyVoltage();
		warpPrint("done.\n");
	#endif

	/*
	 *	At this point, we consider the system "booted" and, e.g., warpPrint()s
	 *	will also be sent to the BLE if that is compiled in.
	 */
	gWarpBooted = true;
	warpPrint("Boot done.\n");

	#if (WARP_BUILD_BOOT_TO_CSVSTREAM)
		printBootSplash(gWarpCurrentSupplyVoltage, menuRegisterAddress, &powerManagerCallbackStructure);

		/*
		 *	Force to printAllSensors
		 */
		gWarpI2cBaudRateKbps = 300;

		if (!WARP_BUILD_BOOT_TO_VLPR)
		{
			status = warpSetLowPowerMode(kWarpPowerModeRUN, 0 /* sleep seconds : irrelevant here */);
			if (status != kWarpStatusOK)
			{
				warpPrint("warpSetLowPowerMode(kWarpPowerModeRUN, 0 /* sleep seconds : irrelevant here */)() failed...\n");
			}
		}

		warpScaleSupplyVoltage(3300);
		printAllSensors(true /* printHeadersAndCalibration */, true /* hexModeFlag */, 0 /* menuDelayBetweenEachRun */, true /* loopForever */);
		/*
		 *	Notreached
		 */
	#endif

	#if (WARP_BUILD_ENABLE_GLAUX_VARIANT)
		printBootSplash(gWarpCurrentSupplyVoltage, menuRegisterAddress, &powerManagerCallbackStructure);

		#if (WARP_BUILD_ENABLE_DEVIS25xP)
			warpPrint("About to read IS25xP JEDEC ID...\n");
			//spiTransactionIS25xP({0x9F /* op0 */,  0x00 /* op1 */,  0x00 /* op2 */, 0x00 /* op3 */, 0x00 /* op4 */, 0x00 /* op5 */, 0x00 /* op6 */}, 5 /* opCount */);
			warpPrint("IS25xP JEDEC ID = [0x%X] [0x%X] [0x%X]\n", deviceIS25xPState.spiSinkBuffer[1], deviceIS25xPState.spiSinkBuffer[2], deviceIS25xPState.spiSinkBuffer[3]);

			warpPrint("About to read IS25xP Manufacturer ID...\n");
			//spiTransactionIS25xP({0x90 /* op0 */,  0x00 /* op1 */,  0x00 /* op2 */, 0x00 /* op3 */, 0x00 /* op4 */, 0x00 /* op5 */, 0x00 /* op6 */}, 5 /* opCount */);
			warpPrint("IS25xP Manufacturer ID = [0x%X] [0x%X] [0x%X]\n", deviceIS25xPState.spiSinkBuffer[3], deviceIS25xPState.spiSinkBuffer[4], deviceIS25xPState.spiSinkBuffer[5]);

			warpPrint("About to read IS25xP Flash ID (also releases low-power mode)...\n");
			//spiTransactionIS25xP({0xAB /* op0 */,  0x00 /* op1 */,  0x00 /* op2 */, 0x00 /* op3 */, 0x00 /* op4 */, 0x00 /* op5 */, 0x00 /* op6 */}, 5 /* opCount */);
			warpPrint("IS25xP Flash ID = [0x%X]\n", deviceIS25xPState.spiSinkBuffer[4]);
		#endif

		warpPrint("About to activate low-power modes (including IS25xP Flash)...\n");
		activateAllLowPowerSensorModes(true /* verbose */);

		uint8_t	tmpRV8803RegisterByte;
		status = readRTCRegisterRV8803C7(kWarpRV8803RegSec, &tmpRV8803RegisterByte);
		if (status != kWarpStatusOK)
		{
			warpPrint("readRTCRegisterRV8803C7(kWarpRV8803RegSec, &tmpRV8803RegisterByte) failed\n");
		}
		else
		{
			warpPrint("kWarpRV8803RegSec = [0x%X]\n", tmpRV8803RegisterByte);
		}

		status = readRTCRegisterRV8803C7(kWarpRV8803RegMin, &tmpRV8803RegisterByte);
		if (status != kWarpStatusOK)
		{
			warpPrint("readRTCRegisterRV8803C7(kWarpRV8803RegMin, &tmpRV8803RegisterByte) failed\n");
		}
		else
		{
			warpPrint("kWarpRV8803RegMin = [0x%X]\n", tmpRV8803RegisterByte);
		}

		status = readRTCRegisterRV8803C7(kWarpRV8803RegHour, &tmpRV8803RegisterByte);
		if (status != kWarpStatusOK)
		{
			warpPrint("readRTCRegisterRV8803C7(kWarpRV8803RegHour, &tmpRV8803RegisterByte) failed\n");
		}
		else
		{
			warpPrint("kWarpRV8803RegHour = [0x%X]\n", tmpRV8803RegisterByte);
		}

		status = readRTCRegisterRV8803C7(kWarpRV8803RegExt, &tmpRV8803RegisterByte);
		if (status != kWarpStatusOK)
		{
			warpPrint("readRTCRegisterRV8803C7(kWarpRV8803RegExt, &tmpRV8803RegisterByte) failed\n");
		}
		else
		{
			warpPrint("kWarpRV8803RegExt = [0x%X]\n", tmpRV8803RegisterByte);
		}

		status = readRTCRegisterRV8803C7(kWarpRV8803RegFlag, &tmpRV8803RegisterByte);
		if (status != kWarpStatusOK)
		{
			warpPrint("readRTCRegisterRV8803C7(kWarpRV8803RegFlag, &tmpRV8803RegisterByte) failed\n");
		}
		else
		{
			warpPrint("kWarpRV8803RegFlag = [0x%X]\n", tmpRV8803RegisterByte);
		}

		status = readRTCRegisterRV8803C7(kWarpRV8803RegCtrl, &tmpRV8803RegisterByte);
		if (status != kWarpStatusOK)
		{
			warpPrint("readRTCRegisterRV8803C7(kWarpRV8803RegCtrl, &tmpRV8803RegisterByte) failed\n");
		}
		else
		{
			warpPrint("kWarpRV8803RegCtrl = [0x%X]\n", tmpRV8803RegisterByte);
		}

		warpPrint("About to configureSensorBME680() for measurement...\n");
		status = configureSensorBME680(	0b00000001,	/*	payloadCtrl_Hum: Humidity oversampling (OSRS) to 1x				*/
						0b00100100,	/*	payloadCtrl_Meas: Temperature oversample 1x, pressure overdsample 1x, mode 00	*/
						0b00001000	/*	payloadGas_0: Turn off heater							*/
						);
		if (status != kWarpStatusOK)
		{
			warpPrint("configureSensorBME680() failed...\n");
		}

		warpDisableI2Cpins();

		warpPrint("About to loop with printSensorDataBME680()...\n");
		while (1)
		{
			blinkLED(kGlauxPinLED);
			for (int i = 0; i < kGlauxSensorRepetitionsPerSleepIteration; i++)
			{
				printAllSensors(true /* printHeadersAndCalibration */, true /* hexModeFlag */, 0 /* menuDelayBetweenEachRun */, true /* loopForever */);
			}

			warpPrint("About to configureSensorBME680() for sleep...\n");
			status = configureSensorBME680(	0b00000000,	/*	payloadCtrl_Hum: Sleep							*/
							0b00000000,	/*	payloadCtrl_Meas: No temperature samples, no pressure samples, sleep	*/
							0b00001000	/*	payloadGas_0: Turn off heater						*/
						);
			if (status != kWarpStatusOK)
			{
				warpPrint("configureSensorBME680() failed...\n");
			}
			warpDisableI2Cpins();
			blinkLED(kGlauxPinLED);

			warpPrint("About to go into VLLS0 for 30 (was 60*60) seconds (will reset afterwords)...\n");
			status = warpSetLowPowerMode(kWarpPowerModeVLLS0, kGlauxSleepSecondsBetweenSensorRepetitions /* sleep seconds */);
			if (status != kWarpStatusOK)
			{
				warpPrint("warpSetLowPowerMode(kWarpPowerModeVLLS0, 10)() failed...\n");
			}
			warpPrint("Should not get here...");
		}
	#endif

    
    devSSD1331init();


    // Configure accelerometer - set high pass filter to remove gravity offset
    configureSensorMMA8451Q(0x00,/* Payload: Disable FIFO */
                            0x00,/* Normal read 14bit, 800Hz, normal, standby mode to write HP */
                            0x11,/* 4G Scale, HPF data enabled */
                            0x00,/* 16Hz cutoff */
                            0x01/* Normal read 14bit, 800Hz, normal, active mode*/);

    
    uint32_t    step_count          = 0;            // Tracks step count
    uint32_t    last_step_count     = 0;            // Tracks last step count
    uint32_t    cal_count           = 0;            // Tracks calories (Kcal / 1000)
    uint8_t     mode                = 0;            // Tracks mode
    uint8_t     last_mode           = 0;            // Tracks last mode
    uint32_t    start_time          = 0;            // Time at start of cycle
    uint8_t     run_time            = 0;            // Time for one cycle to run
    uint32_t    last_step_time      = 0;            // Last step time
    uint8_t     ticks               = 0;            // Tracks seconds as measured by 50 cycles

    
    
    // Initialise display information
    displayBackground(mode);
    displayMode(mode);
    drawSteps(step_count,mode);
    drawCals(cal_count,mode);

    warpPrint("\nRunning...\n\r");
    
    while(1){
                // Measure start time
                start_time = OSA_TimeGetMsec();
                ticks ++;

                // Reset steps when 100000 is reached to avoid overflow
                if(step_count >= 100000)
                {
                    step_count = 0;
                    cal_count = 0;
                }
                
        
                // Count steps
                step_count = countSteps(step_count);
                mode = modeSelector(mode, last_step_time);
                
                // Count and update cals every second (every 50 cycles)
                if(ticks >= 50)
                {
                    cal_count = countCals(cal_count, HEIGHT, WEIGHT);
                    drawCals(cal_count, mode);
                    ticks = 0;
                }
        

                // Update steps and reset step timer
                if(step_count != last_step_count)
                {
                    drawSteps(step_count, mode);
                    
                    last_step_count = step_count;
                    last_step_time = OSA_TimeGetMsec();
                }
                
                // Update mode
                if(last_mode != mode)
                {
                    drawCals(cal_count, mode);
                    displayBackground(mode);
                    displayMode(mode);
                    drawSteps(step_count, mode);
                    
                    last_mode = mode;
                }
            
        
                // Run at 50Hz so delay for 20ms minus amount of time for code to run - loop rarely runs for >20ms
                run_time = OSA_TimeGetMsec() - start_time;
                if(run_time < 20)
                {
                    OSA_TimeDelay(20-run_time);
                }
}
}


