#include <steadyView.h>
#include <algorithm>

const uint16_t channelsTable[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
const char* bandsTable[6] = { "A", "B", "E", "F", "R", "L" };

SteadyView::SteadyView(byte mosiGpio, byte clkGpio, byte csGpio, MemoryValue* bandIndexMemory, MemoryValue* channelIndexMemory): bandIndexMemory(bandIndexMemory), channelIndexMemory(channelIndexMemory) {
    this->mosiGpio = mosiGpio;
    this->clkGpio = clkGpio;
    this->csGpio = csGpio;

    this->delayActionExecutionThresholdMicros = 1000 * 100 * 5; // 500ms
    this->delayedActionTimeMicros = 0;
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

    this->setFrequency(currentBandIndex, currentChannelIndex);
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
    uint16_t frequencyIndex = getFrequencyIndex(bandIndex, channelIndex);
    
    uint16_t frequency = frequenciesTable[frequencyIndex];

    // this->setFrequencyIndex

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
}

void SteadyView::increaseBandIndex() {
    uint16_t bandIndex = this->bandIndexMemory->readValue();
    uint16_t channelIndex = this->channelIndexMemory->readValue();

    size_t bandsSize = sizeof(bandsTable);

    uint16_t newBandIndex;
    if (bandIndex == bandsSize) {
        newBandIndex = 0;
    } else {
        newBandIndex++;
    }

    delayedActionCallback = [this, newBandIndex, channelIndex] {
        this->setFrequency(newBandIndex, channelIndex);
        this->saveBandAndChannel(newBandIndex, channelIndex);
    };

    delayedActionTimeMicros = micros();
}

void SteadyView::increaseChannelIndex() {
    uint16_t bandIndex = this->bandIndexMemory->readValue();
    uint16_t channelIndex = this->channelIndexMemory->readValue();

    size_t channelsSize = sizeof(channelsTable);

    uint16_t newChannelIndex;
    if (channelIndex == channelsSize) {
        newChannelIndex = 0;
    } else {
        newChannelIndex++;
    }

    delayedActionCallback = [this, bandIndex, newChannelIndex] {
        this->setFrequency(bandIndex, newChannelIndex);
        this->saveBandAndChannel(bandIndex, newChannelIndex);
    };

    delayedActionTimeMicros = micros();
}

tuple<const char*, uint16_t> SteadyView::getBandAndChannel() {
    uint16_t bandIndex = this->bandIndexMemory->readValue();
    uint16_t channelIndex = this->channelIndexMemory->readValue();

    return make_tuple(bandsTable[bandIndex], channelIndex);
}

void SteadyView::checkDelayedExecution() {
    
    // Kontynuuj:
    // delayedActionTimeMicros czy roznica wieksza od threshold
    // Jak wieksza, wykonuj i wyczysc pointer

    delayedActionCallback = nullptr;
}