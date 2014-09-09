#ifndef ANIMATION
#define ANIMATION

#include "PixelDisplay.h"

class IAnimation
{
public:
    /** \brief constructor
     *
     * \param display sets the display to animate on
     *
     */

    IAnimation(IPixelDisplay *display);


    /** \brief perform one step of Animation
     *
     *  will be called periodically
     */
    void perofrmAnimationStep(void);

protected:
    /** \brief implement here your animation steps
     *
     */
    virtual void doAnimationStep(void) = 0;

    /** \brief holds the display to write on
     *
     */
    IPixelDisplay *display;
};
#endif //ANIMATION
