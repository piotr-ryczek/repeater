#ifndef MEMORY_DATA_H
#define MEMORY_DATA_H

#include <Arduino.h>
#include <memoryValue.h>

// Memory Addresses

const int BAND_INDEX_SET_ADDRESS = 0;
const int BAND_INDEX_VALUE_ADDRESS = 4;

const int CHANNEL_INDEX_SET_ADDRESS = 0;
const int CHANNEL_INDEX_VALUE_ADDRESS = 4;

// MemoryValues

extern MemoryValue bandIndexMemory;

extern MemoryValue channelIndexMemory;

#endif
