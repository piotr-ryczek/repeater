#include <steadyView.h>
#include <algorithm>

const uint16_t channelsTable[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
const char* bandsTable[6] = { "A", "B", "E", "F", "R", "L" };
const videoMode_t modesTable[2] = { ModeMix, ModeDiversity };

SteadyView::SteadyView(byte mosiGpio, byte clkGpio, byte csGpio, MemoryValue* bandIndexMemory, MemoryValue* channelIndexMemory, MemoryValue* modeIndexMemory): bandIndexMemory(bandIndexMemory), channelIndexMemory(channelIndexMemory), modeIndexMemory(modeIndexMemory) {
    this->mosiGpio = mosiGpio;
    this->clkGpio = clkGpio;
    this->csGpio = csGpio;

    this->delayActionExecutionThresholdMicros = 1000 * 1000; // 1s
    this->delayedActionSetTimeMicros = 0;
};

void SteadyView::initialize() {
    pinMode(this->mosiGpio, OUTPUT);
    pinMode(this->clkGpio, OUTPUT);
    pinMode(this->csGpio, OUTPUT);

    digitalWrite(this->clkGpio, LOW);
    digitalWrite(this->mosiGpio, HIGH);
    digitalWrite(this->csGpio, HIGH);

    uint16_t currentBandIndex = this->bandIndexMemory->readValue();
    uint16_t currentChannelIndex = this->channelIndexMemory->readValue();
    uint16_t currentModeIndex = this->modeIndexMemory->readValue();

    this->temporaryBandIndex = currentBandIndex;
    this->temporaryChannelIndex = currentChannelIndex;
    this->temporaryModeIndex = currentModeIndex;

    this->setFrequency(currentBandIndex, currentChannelIndex);
    this->setMode(modesTable[currentModeIndex]);
}

// Bands: A, B, E, F, R, L
uint16_t SteadyView::getBandIndex(char* band) {
    auto itBands = find(begin(bandsTable), end(bandsTable), band);
    int bandIndex;
    if (itBands != end(bandsTable)) {
        bandIndex = distance(begin(bandsTable), itBands);
    } else {
        Serial.println("Invalid band, available ones: A, B, E, F, R, L");
        throw std::runtime_error("Invalid band");
    }

    return bandIndex;
}

// Channels: 1, 2, 3, 4, 5, 6, 7, 8
uint16_t SteadyView::getChannelIndex(uint16_t channel) {
    auto itChannels = find(begin(channelsTable), end(channelsTable), channel);
    int channelIndex;
    if (itChannels != end(channelsTable)) {
        channelIndex = distance(begin(channelsTable), itChannels);
    } else {
        Serial.println("Invalid channel, available ones: 1-8");
        throw std::runtime_error("Invalid channel");
    }

    return channelIndex;
}

uint16_t SteadyView::fetchFrequencyIndex() {
    uint16_t bandIndex = this->bandIndexMemory->readValue();
    uint16_t channelIndex = this->channelIndexMemory->readValue();

    return this->getFrequencyIndex(bandIndex, channelIndex);
}

uint16_t SteadyView::getFrequencyIndex(uint16_t bandIndex, uint16_t channelIndex) {
    return bandIndex * 8 + channelIndex;
}

void SteadyView::setFrequency(uint16_t bandIndex, uint16_t channelIndex) {
    Serial.println("Frequency set. BandIndex: " + String(bandIndex) + " / ChannelIndex: " + String(channelIndex));

    uint16_t frequencyIndex = getFrequencyIndex(bandIndex, channelIndex);
    
    uint16_t frequency = frequenciesTable[frequencyIndex];

    uint32_t data = ((((frequency - 479) / 2) / 32) << 7) | (((frequency - 479) / 2) % 32);
    uint32_t newRegisterData = SYNTHESIZER_REG_B  | (RX5808_WRITE_CTRL_BIT << 4) | (data << 5);

    uint32_t currentRegisterData = SYNTHESIZER_REG_B | (RX5808_WRITE_CTRL_BIT << 4) | rtc6705readRegister(SYNTHESIZER_REG_B);

    if (newRegisterData != currentRegisterData) {
        rtc6705WriteRegister(SYNTHESIZER_REG_A  | (RX5808_WRITE_CTRL_BIT << 4) | (0x8 << 5));
        rtc6705WriteRegister(newRegisterData);
    }
}

void SteadyView::rtc6705WriteRegister(uint32_t buf) {
    uint32_t periodMicroSec = 1000000 / BIT_BANG_FREQ;

    digitalWrite(this->csGpio, LOW);
    delayMicroseconds(periodMicroSec);

    for (uint8_t i = 0; i < RX5808_PACKET_LENGTH; ++i)
    {
        digitalWrite(this->mosiGpio, buf & 0x01);
        delayMicroseconds(periodMicroSec / 4);
        digitalWrite(this->clkGpio, HIGH);
        delayMicroseconds(periodMicroSec / 4);
        digitalWrite(this->clkGpio, LOW);
        delayMicroseconds(periodMicroSec / 4);

        buf >>= 1;
    }

    delayMicroseconds(periodMicroSec);
    digitalWrite(this->mosiGpio, HIGH);
    digitalWrite(this->csGpio, HIGH);
}

uint32_t SteadyView::rtc6705readRegister(uint8_t readRegister) {
    uint32_t buf = readRegister | (RX5808_READ_CTRL_BIT << 4);
    uint32_t registerData = 0;

    uint32_t periodMicroSec = 1000000 / BIT_BANG_FREQ;

    digitalWrite(this->csGpio, LOW);
    delayMicroseconds(periodMicroSec);

    // Write register address and read bit
    for (uint8_t i = 0; i < RX5808_ADDRESS_R_W_LENGTH; ++i)
    {
        digitalWrite(this->mosiGpio, buf & 0x01);
        delayMicroseconds(periodMicroSec / 4);
        digitalWrite(this->clkGpio, HIGH);
        delayMicroseconds(periodMicroSec / 4);
        digitalWrite(this->clkGpio, LOW);
        delayMicroseconds(periodMicroSec / 4);

        buf >>= 1;
    }

    // Change pin from output to input
    pinMode(this->mosiGpio, INPUT);

    // Read data 20 bits
    for (uint8_t i = 0; i < RX5808_DATA_LENGTH; i++) {
        digitalWrite(this->clkGpio, HIGH);
        delayMicroseconds(periodMicroSec / 4);

        if (digitalRead(this->mosiGpio))
        {
            registerData = registerData | (1 << (5 + i));
        }

        delayMicroseconds(periodMicroSec / 4);
        digitalWrite(this->clkGpio, LOW);
        delayMicroseconds(periodMicroSec / 2);
    }

    // Change pin back to output
    pinMode(this->mosiGpio, OUTPUT);

    digitalWrite(this->mosiGpio, LOW);
    digitalWrite(this->csGpio, HIGH);

    return registerData;
}

void SteadyView::setMode(videoMode_t mode) {
    Serial.println("Mode set: " + String(mode));

    if (mode == ModeMix) {
        digitalWrite(this->clkGpio, HIGH);
        delay(100);
        digitalWrite(this->clkGpio, LOW);
        delay(500);
    }

    uint16_t frequencyIndex = this->fetchFrequencyIndex();

    uint16_t frequency = frequenciesTable[frequencyIndex];
    uint32_t data = ((((frequency - 479) / 2) / 32) << 7) | (((frequency - 479) / 2) % 32);
    uint32_t registerData = SYNTHESIZER_REG_B  | (RX5808_WRITE_CTRL_BIT << 4) | (data << 5);

    rtc6705WriteRegister(SYNTHESIZER_REG_A  | (RX5808_WRITE_CTRL_BIT << 4) | (0x8 << 5));
    delayMicroseconds(500);
    rtc6705WriteRegister(SYNTHESIZER_REG_A  | (RX5808_WRITE_CTRL_BIT << 4) | (0x8 << 5));
    rtc6705WriteRegister(registerData);
}

void SteadyView::saveBandAndChannel(uint16_t bandIndex, uint16_t channelIndex) {
    this->bandIndexMemory->setValue(bandIndex);
    this->channelIndexMemory->setValue(channelIndex);

    this->temporaryBandIndex = bandIndex;
    this->temporaryChannelIndex = channelIndex;
}

void SteadyView::saveMode(uint16_t modeIndex) {
    this->modeIndexMemory->setValue(modeIndex);

    this->temporaryModeIndex = modeIndex;
}

void SteadyView::increaseBandIndex() {
    // uint16_t bandIndex = this->bandIndexMemory->readValue();
    // uint16_t channelIndex = this->channelIndexMemory->readValue();

    uint16_t bandIndex = this->temporaryBandIndex;
    uint16_t channelIndex = this->temporaryChannelIndex;

    size_t bandsSize = sizeof(bandsTable) / sizeof(bandsTable[0]);

    uint16_t newBandIndex = bandIndex;
    if (bandIndex == bandsSize - 1) {
        newBandIndex = 0;
    } else {
        newBandIndex++;
    }

    Serial.println("Setting Delayed callback newBandIndex: " + String(newBandIndex));

    this->delayedActionCallback = [this, newBandIndex, channelIndex] {
        this->setFrequency(newBandIndex, channelIndex);
        this->saveBandAndChannel(newBandIndex, channelIndex);
    };

    this->temporaryBandIndex = newBandIndex;
    this->delayedActionSetTimeMicros = micros();
}

void SteadyView::increaseChannelIndex() {
    // uint16_t bandIndex = this->bandIndexMemory->readValue();
    // uint16_t channelIndex = this->channelIndexMemory->readValue();

    uint16_t bandIndex = this->temporaryBandIndex;
    uint16_t channelIndex = this->temporaryChannelIndex;

    size_t channelsSize = sizeof(channelsTable) / sizeof(channelsTable[0]);

    uint16_t newChannelIndex = channelIndex;
    if (channelIndex == channelsSize - 1) {
        newChannelIndex = 0;
    } else {
        newChannelIndex++;
    }

    this->delayedActionCallback = [this, bandIndex, newChannelIndex] {
        this->setFrequency(bandIndex, newChannelIndex);
        this->saveBandAndChannel(bandIndex, newChannelIndex);
    };

    this->temporaryChannelIndex = newChannelIndex;
    this->delayedActionSetTimeMicros = micros();
}

void SteadyView::increaseModeIndex() {
    uint16_t modeIndex = this->temporaryModeIndex;

    size_t modesSize = sizeof(modesTable) / sizeof(modesTable[0]);

    uint16_t newModeIndex = modeIndex;
    if (modeIndex == modesSize - 1) {
        newModeIndex = 0;
    } else {
        newModeIndex++;
    }

    this->delayedActionCallback = [this, newModeIndex] {
        this->setMode(modesTable[newModeIndex]);
        this->saveMode(newModeIndex);
    };

    this->temporaryModeIndex = newModeIndex;
    this->delayedActionSetTimeMicros = micros();
}

tuple<const char*, uint16_t, uint16_t> SteadyView::getBandAndChannelAndFrequency() {
    // uint16_t bandIndex = this->bandIndexMemory->readValue();
    // uint16_t channelIndex = this->channelIndexMemory->readValue();

    uint16_t bandIndex = this->temporaryBandIndex;
    uint16_t channelIndex = this->temporaryChannelIndex;

    uint16_t frequency = frequenciesTable[this->getFrequencyIndex(bandIndex, channelIndex)];

    return make_tuple(bandsTable[bandIndex], channelIndex + 1, frequency);
}

String SteadyView::getMode() {
    uint16_t modeIndex = this->temporaryModeIndex;

    return modeIndex == 0 ? "MIX" : "DIV";
}

void SteadyView::checkDelayedExecution() {
    unsigned long now = micros();

    if (
        this->delayedActionSetTimeMicros == 0  || 
        delayedActionCallback == nullptr || 
        now - this->delayedActionSetTimeMicros < this->delayActionExecutionThresholdMicros
    ) {
        return;
    }

    delayedActionCallback();

    delayedActionCallback = nullptr;
}