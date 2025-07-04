#include <Arduino.h>
#include <memoryValue.h>
#include <memoryData.h>

// MemoryValues

// Band
MemoryValue bandIndexMemory(BAND_INDEX_SET_ADDRESS, BAND_INDEX_VALUE_ADDRESS, 0);

// Channel
MemoryValue channelIndexMemory(CHANNEL_INDEX_SET_ADDRESS, CHANNEL_INDEX_VALUE_ADDRESS, 0);

// Mode
MemoryValue modeIndexMemory(MODE_INDEX_SET_ADDRESS, MODE_INDEX_VALUE_ADDRESS, 0);