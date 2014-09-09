#ifndef RTC_H_INCLUDED
#define RTC_H_INCLUDED

#include <stm32f10x.h>

/**
  * @brief  RTC Time structure definition
  */
typedef struct
{
  uint8_t RTC_Hours;    /*!< Specifies the RTC Time Hour.
                        This parameter must be set to a value in the 0-12 range
                        if the RTC_HourFormat_12 is selected or 0-23 range if
                        the RTC_HourFormat_24 is selected. */

  uint8_t RTC_Minutes;  /*!< Specifies the RTC Time Minutes.
                        This parameter must be set to a value in the 0-59 range. */

  uint8_t RTC_Seconds;  /*!< Specifies the RTC Time Seconds.
                        This parameter must be set to a value in the 0-59 range. */

  uint8_t RTC_H12;      /*!< Specifies the RTC AM/PM Time.
                        This parameter can be a value of @ref RTC_AM_PM_Definitions */
}RTC_TimeTypeDef;

/**
  * @brief  RTC Date structure definition
  */
typedef struct
{
  uint8_t RTC_WeekDay; /*!< Specifies the RTC Date WeekDay.
                        This parameter can be a value of @ref RTC_WeekDay_Definitions */

  uint8_t RTC_Month;   /*!< Specifies the RTC Date Month (in BCD format).
                        This parameter can be a value of @ref RTC_Month_Date_Definitions */

  uint8_t RTC_Date;     /*!< Specifies the RTC Date.
                        This parameter must be set to a value in the 1-31 range. */

  uint8_t RTC_Year;     /*!< Specifies the RTC Date Year.
                        This parameter must be set to a value in the 0-99 range. */
}RTC_DateTypeDef;

class RTC_Driver {
public:
    RTC_Driver();

    /** \brief read out the actual time in the RTC
     * \param time RTC_TimeTypeDef struct to hold the time
     * \return true if time is valid, false else
     */
    static bool rtc_getTime(RTC_TimeTypeDef &time);

    /** \brief read out the actual date in the RTC
     * \param date RTC_TimeTypeDef struct to hold the date
     * \return true if date is valid, false else
     */
    static bool rtc_getDate(RTC_DateTypeDef &date);

    static void init();

    static volatile bool IsSecondPast;

private:




};



#endif /* RTC_H_INCLUDED */
