/*
  ==============================================================================

    CustomDial.h
    Created: 11 Jul 2022 3:14:37pm
    Author:  Jonathan

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

using namespace juce;

class CustomDial : public LookAndFeel_V4
{
    /**
     * Draw a custom rotary slider for band, low, and high pass filters
     *
     * @param g                  Reference to graphics object
     * @param x                  Starting x position
     * @param y                  Starting y position
     * @param width              Width of rotary slider
     * @param height             Height of rotary slider
     * @param sliderPos          Slider's proportional position
     * @param rotaryStartAngle   Angle for rotation start
     * @param rotaryEndAngle     Angle for rotation end
     * @param slider             Reference to slider control
     *
     * @return None
     */
    void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider);
};
