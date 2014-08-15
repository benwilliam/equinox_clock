
class IAnimation{
  public:
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
};
