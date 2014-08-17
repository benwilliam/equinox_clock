#ifndef RTC_ANIMATIONDRIVER_H
#define RTC_ANIMATIONDRIVER_H

#include "animationDriver.h"
#include "stm32f4xx_rtc.h"
#include <vector>

extern "C" {
    void RTC_WKUP_IRQHandler(void);
}

class RTC_AnimationDriver : public IAnimationDriver
{
public:
    /** \brief constructor
    *
    * \param display the PixelDisplay driver to show the animation on
    *
    */
    RTC_AnimationDriver(IPixelDisplay *display);

    /** \brief init RTC hardware
     *
     */
    virtual void init();

    /** \brief read out the actual time in the RTC
     * \param time RTC_TimeTypeDef struct to hold the time
     * \return true if time is valid, false else
     */
    bool rtc_getTime(RTC_TimeTypeDef &time);

    /** \brief read out the actual date in the RTC
     * \param date RTC_TimeTypeDef struct to hold the date
     * \return true if date is valid, false else
     */
    bool rtc_getDate(RTC_DateTypeDef &date);


    friend void RTC_WKUP_IRQHandler(void);
private:
    /** \brief init wake up interrupt for AnimationDriver
     * wakeupCounter is init with a clock of 2048 Hz
     *
     * \param wakeUpCounter sets the value to which the RTC wakeup have to count until the interrupt ocurs
     * default is 16 which means 128 Interrupts per second. 2048 / 16 = 128
      */
    void rtc_InitWakeUpInterrupt(uint8_t wakeUpCounter = 16);

    static std::vector<RTC_AnimationDriver *> rtcAnimationDrivers;


};
#endif //RTC_ANIMATIONDRIVER_H


