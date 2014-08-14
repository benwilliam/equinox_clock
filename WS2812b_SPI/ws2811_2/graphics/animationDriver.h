#include <animation.h>

/** \brief Interface for a timer driven animation driver
 *
 *
 *
 */
class IAnimationDriver{

public:

    struct AnimationEntry{
        IAnimation *animation;
        uint8_t performSteps;

        AnimationEntry() : animation(nullptr), performSteps(0)
        {}
        AnimationEntry(IAnimation &ani) : animation(ani), performSteps(0)
        {}
    };
    /** \brief perform one step of all registered Animation
     *
     *  should be called in an endless loop in an own thread
     */
    void updateAnimations(void);

    /** \brief register an Animation
     *
     * \param animation to register
     *
     */
    void registerAnimation(IAnimation &animation);

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

    AnimationEntry animations[MAX_ANIMATION_COUNT];

};
