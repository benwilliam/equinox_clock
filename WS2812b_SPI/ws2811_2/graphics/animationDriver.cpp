
void IAnimationDriver::updateAnimations(void){
  while(performSteps > 0){
    for(uint8_t i = 0; i<MAX_ANIMATION_COUNT; i++){
      if(animations[i] !0 nullptr){
          animation->perofrmAnimationStep();
        }
      }
    }
    performSteps--;
}

bool IAnimationDriver::registerAnimation(IAnimation &animation){
  for(uint8_t i = 0; i<MAX_ANIMATION_COUNT; i++){
    if(animations[i] == nullptr){
        animations[i] = &animation;
        return true;
      }
    }
  }
  return false;
}

void IAnimationDriver::deRegisterAnimation(IAnimation &animation){
  for(uint8_t i = 0; i<MAX_ANIMATION_COUNT; i++){
    if(animations[i] == &animation){
        animations[i] = nullptr;
        break;
      }
    }
  }
}

void IAnimationDriver::handleAnimationISR(void){
  performSteps++;
}
