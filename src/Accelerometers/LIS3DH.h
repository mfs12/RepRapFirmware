/*
 * LIS3DH.h
 *
 *  Created on: 14 Mar 2021
 *      Author: David
 */

#ifndef SRC_HARDWARE_LIS3DH_H_
#define SRC_HARDWARE_LIS3DH_H_

#include <RepRapFirmware.h>

#if SUPPORT_ACCELEROMETERS

#include <Hardware/SharedSpi/SharedSpiClient.h>

class LIS3DH : public SharedSpiClient
{
public:
	LIS3DH(SharedSpiDevice& dev, uint32_t freq, Pin p_csPin, Pin p_int1Pin) noexcept;

	// Do a quick test to check whether the accelerometer is present, returning true if it is
	bool CheckPresent() noexcept;

	// Configure the accelerometer to collect at or near the requested sampling rate and the requested resolution in bits.
	bool Configure(uint16_t& samplingRate, uint8_t& resolution) noexcept;

	// Start collecting data
	bool StartCollecting(uint8_t axes) noexcept;

	// Collect some data from the FIFO, suspending until the data is available
	unsigned int CollectData(const uint16_t **collectedData, uint16_t &dataRate, bool &overflowed) noexcept;

	// Stop collecting data
	void StopCollecting() noexcept;

	// Get a status byte
	uint8_t ReadStatus() noexcept;

	// Used by the ISR
	void Int1Isr() noexcept;

private:
	enum LisRegister : uint8_t
	{
		WhoAmI = 0x0f,
		Ctrl1 = 0x20,
		Status = 0x27,
		OutXL = 0x28,
		FifoControl = 0x2E,
		FifoSource = 0x2F
	};

	bool ReadRegisters(uint8_t reg, size_t numToRead) noexcept;
	bool WriteRegisters(LisRegister reg, size_t numToWrite) noexcept;
	bool ReadRegister(uint8_t reg, uint8_t& val) noexcept;
	bool WriteRegister(LisRegister reg, uint8_t val) noexcept;

	volatile TaskHandle taskWaiting;
	uint32_t firstInterruptTime;
	uint32_t lastInterruptTime;
	uint32_t totalNumRead;
	uint8_t currentAxis;
	uint8_t ctrlReg1;
	Pin int1Pin;
	alignas(2) uint8_t transferBuffer[2 + (6 * 32)];			// 1 dummy byte for alignment, one register address byte, 192 data bytes to read entire FIFO
	uint8_t* const dataBuffer = transferBuffer + 2;
};

#endif

#endif /* SRC_HARDWARE_LIS3DH_H_ */
