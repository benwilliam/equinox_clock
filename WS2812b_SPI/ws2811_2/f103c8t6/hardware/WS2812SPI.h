#ifndef WS2812SPI_H
#define WS2812SPI_H

#include "PixelDisplay.h"

extern "C" {
    void DMA1_Channel5_IRQHandler(void);
}

/** \brief SPI implementation of WS2812 Interface
 *
 * It uses the SPI3 of STm32F407 to generate the right signal
 * and than the DMA to send the whole framebuffer to the WS2812 Pixel
 *
 */
class CWS2812SPI : public IPixelDisplay
{
public:
    /** \brief Init the WS2812 driver hardware and framebuffer
    *
    */
    virtual void init(void);

    /** \brief do one update of the whole stripe
     *
     * this method will start the transforamtion from the Pixel buffer to the framebuffer
     * and sending the data to the WS2812 controller
     */
    virtual bool update(void);

    /** \brief Constructor, also calling parent class constructor an reserving RAM for framebuffer
     *
     */
    CWS2812SPI(uint16_t _nr_pixel);

    /** \brief Destructor, also freeingr framebuffer
     *
     */
    ~CWS2812SPI();

    friend void DMA1_Channel5_IRQHandler(void);

private:
    // ----------------------------- USER DEFINED CONSTANTS -----------------------------
    /** \brief how many bits are used to represent one WS2812B bit
     */
    static const uint8_t BIT_FACTOR;

    /** \brief send this amount of extra byte to trigger the WS2811 latch
     * this bytes are set to 0 to hold MOSI pin to low
     * normally the WS2812 needs at least 12 µS for the latch, but the whoe program between two frame updates needs more  than 12 µS
     * so only one byte is used.
     */
    const uint8_t WS2812_RESET_LEN  = 1;

    /** \brief length of the framebuffer with valid color data
     */
    const uint16_t WS2812_FRAMEBUF_LED_LEN;

    /** \brief whole length of the framebuffer including dummy reset bytes
     */
    const uint16_t WS2812_FRAMEBUF_LEN;


    /** \brief SPI representation of one LOW Bit of WS2812
     *
     */
    static const uint8_t WS2812_PWM_ZERO = 0x1; //0b0001

    /** \brief SPI representation of one HIGH Bit of WS2812
     *
     */
    static const uint8_t WS2812_PWM_ONE = 0x7; //0b0111


    /** \brief the real framebuffer which is send to WS2812
     *
     */
    uint8_t *framebuffer;

    /** \brief just a pointer to the framebuffer for easier access
     *
     */
    uint8_t *pPixel;


    /** \brief shows if DMA is jsut transfering the data
     *
     */
    static bool DMA_BUSY;

    /** \brief start of transferung frame buffer to WS2812
     *
     */
    void start_dma(void);

    /** \brief converts a bit to the WS2812 representation of a bit
     *
     * \param ledBit 1 or 0
     * \return @WS2812_PWM_ZERO or @WS2812_PWM_ONE
     *
     */
    uint8_t ledBitToOut(uint8_t ledBit);

    /** \brief transforms one LED byte to the framebuffer
     *
     * \param i LED number to transform. (Attention here is realy the LED number not the pixel number used)
     *
     */

    void LED_TO_PWM(uint8_t i);

    /** \brief gets called by the DMA readdy interrupt handler
     *
     */
    static void DMAReadyInterrupt(void);

    static const uint8_t lookUp[];

};

#endif //WS2812SPI_H
