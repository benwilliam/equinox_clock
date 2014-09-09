#ifndef DCF77_H_INCLUDED
#define DCF77_H_INCLUDED

#include <stdint.h>

#define MEASURED_SECOND_SIZE 40
#define BITS 60

extern "C" {
void TIM8_UP_TIM13_IRQHandler();
}

class CDCF77Decoder {
public:
    CDCF77Decoder();
    void init();

    friend void TIM8_UP_TIM13_IRQHandler();
private:
    enum EBitKind {
        BIT_0 = 0, //Zero bit
        BIT_1 = 1, // High Bit
        BIT_M, //minute mark (bit 59.)
        BIT_INVALID, //no valid bit found
    };

    bool SyncedToBit = false;
    bool SyncedToMinute = false;
    uint8_t BitPos = 0;
    uint8_t lastMeasured = 0;

    uint8_t measuredSecond[MEASURED_SECOND_SIZE]; //40 measurements per second -> every 25ms
    uint8_t measurePos = 0;

    uint8_t receivedTime[BITS];

    void handleBitISR(uint8_t bitValue);
    void handleMeassuredSecond(uint8_t measured[]);
    static CDCF77Decoder *hadnleBitISRInstance;

    EBitKind parseBit(uint8_t measured[]);
};

#endif /* DCF77_H_INCLUDED */
