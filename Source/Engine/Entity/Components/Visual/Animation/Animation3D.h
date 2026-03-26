/*
/
// filename: Animation3D.h
// author: Callen Betts
// brief: Controls the speed, frame, and other properties at which a 3D model animates
//
// description: 3D Animations need to be dynamic. We want to be able to trigger certain events when an animation
//  ends or starts, or if a specific frame is hit. To do this, we can either derive animations from this base class
//  or create a new animation and set function callbacks.
//  This lets us create fully custom animations for each of our entities with custom triggers on start/end. 
/
*/

#pragma once
#include "Engine/Entity/Components/Visual/Animation/Bone.h"

namespace Components
{

  class Animation3D : public Component
  {

  public:

    // initialize the animation's base values
    Animation3D();

    // update an animation's logic
    void update();

    // play an animation with default parameters
    void play();

    // play an animation with custom parameters
    void play(int startFrame, float animationSpeed, bool isLooping);

    // reset an animation's values
    void end();

    // called when an animation starts from frame 0
    virtual void start() {};
    // called when an animation ends
    virtual void stop() {};

    // load the 3D animation data from an FBX file
    void loadFBXData();

  private:

    unsigned int frame; // the current frame of animation
    unsigned int totalFrames; // how many frames are in this animation

    float frameSpeed; // how fast we animate 
    float frameCounter; // incremented each frame by speed, used in logic updates

    bool running; // if the animation is running
    bool loop; // if the animation should loop

    // the vector of keyframes and bones
    std::vector<Bone> bones;


  };

}