/*
  ==============================================================================

    CustomDial.cpp
    Created: 11 Jul 2022 3:14:37pm
    Author:  Jonathan

  ==============================================================================
*/

#include "CustomDial.h"

/**
 * Draw a custom rotary slider for band, low, and high pass filters
 *
 * This is re-factored JUCE library code from the LookAndFeel_V2 class
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
void CustomDial::drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
    const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
    const float radius = jmin((float)width * 0.5f, (float)height * 0.5f) - 2.0f;
    const float centreX = (float)x + (float)width * 0.5f;
    const float centreY = (float)y + (float)height * 0.5f;
    const float rx = centreX - radius;
    const float ry = centreY - radius;
    const float rw = radius * 2.0f;
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    const bool isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();

    if (radius > 12.0f)
    {
        slider.isEnabled() ? g.setColour(slider.findColour(Slider::rotarySliderFillColourId).withAlpha(isMouseOver ? 1.0f : 0.7f)) : g.setColour(Colour(0x80808080));

        const float thickness = 0.7f;

        {
            Path filledArc;
            filledArc.addPieSegment(rx, ry, rw, rw, rotaryStartAngle, angle, thickness);
            g.fillPath(filledArc);
        }

        {
            const float innerRadius = radius * 0.2f;
            Path p;
            p.addTriangle(-innerRadius, 0.0f,
                0.0f, -radius * thickness * 1.1f,
                innerRadius, 0.0f);

            p.addEllipse(-innerRadius, -innerRadius, innerRadius * 2.0f, innerRadius * 2.0f);

            g.fillPath(p, AffineTransform::rotation(angle).translated(centreX, centreY));
        }

        slider.isEnabled() ? g.setColour(slider.findColour(Slider::rotarySliderOutlineColourId)) : g.setColour(Colour(0x80808080));

        Path outlineArc;
        outlineArc.addPieSegment(rx, ry, rw, rw, rotaryStartAngle, rotaryEndAngle, thickness);
        outlineArc.closeSubPath();

        g.strokePath(outlineArc, PathStrokeType(slider.isEnabled() ? (isMouseOver ? 2.0f : 1.2f) : 0.3f));
    }
    else
    {
        slider.isEnabled() ? g.setColour(slider.findColour(Slider::rotarySliderFillColourId).withAlpha(isMouseOver ? 1.0f : 0.7f)) : g.setColour(Colour(0x80808080));

        Path p;
        p.addEllipse(-0.4f * rw, -0.4f * rw, rw * 0.8f, rw * 0.8f);
        PathStrokeType(rw * 0.1f).createStrokedPath(p, p);

        p.addLineSegment(Line<float>(0.0f, 0.0f, 0.0f, -radius), rw * 0.2f);

        g.fillPath(p, AffineTransform::rotation(angle).translated(centreX, centreY));
    }
}