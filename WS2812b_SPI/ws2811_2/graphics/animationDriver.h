#include <animation.h>

/** \brief Interface for a timer driven animation driver
 *
 *
 *
 */
class IAnimationDriver{

public:

    /** \brief perform one step of all registered Animation
     *
     *  should be called in an endless loop in an own thread
     *  all aniamtion with flagged performSteps > 0 will executed
     */
    void updateAnimations(void);

    /** \brief register an Animation
     *  ATTENTION! will register the aniamtion on the next free slot
     *  so if the order of animations is important you have to register them in the right order 
     *  and take care of deRegisterAnimation
     * 
     *  it will NOT check if it is already registered
     * 
     * \param animation to register
     * \return true if success, false when @MAX_ANIMATION_COUNT exceeded
     *
     */
    bool registerAnimation(IAnimation &animation);

    /** \brief deregister an Animation
     *
     * \param animation to deregister
     *
     */
    void deRegisterAnimation(IAnimation &animation);

    /** \brief initialize hardware e.g. Timer or RTC
     *
     *
     */
    virtual void init() = 0;

protected:

    /** \brief maximal amount of animations perform at the same time
     *
     */
    static const uint8_t MAX_ANIMATION_COUNT = 16;

    /** \brief all registered Animations
     *
     */
    IAnimation *animations[MAX_ANIMATION_COUNT];
    
    /** \brief amount of steps to be performed
     * will be increased by 1 from @handleAnimationISR;
     * and decreased to 0 by @updateAnimations
     */
    uint8_t performSteps = 0;
    
    /** \brief should be called by a timer interrupt at regular intervals
     *  e.g. 128 times per second
     */
    void handleAnimationISR(void);

};
