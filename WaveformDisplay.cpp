/*
  ==============================================================================

    WaveformDisplay.cpp
    Created: 26 Jul 2022 4:05:16pm
    Author:  Jonathan

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "WaveformDisplay.h"

/**
 * Constructor that initializes the waveform component
 *
 * @param formatManageToUse       Manage available audio formats
 * @param cacheToUse              Cache runs background thread for AudioThumbnails
 *
 * @return                        None
 */
WaveformDisplay::WaveformDisplay(AudioFormatManager& formatManagerToUse,
    AudioThumbnailCache& cacheToUse) : audioThumb(1000, formatManagerToUse, cacheToUse),
    fileLoaded(false),
    positionRelative(0)
{
    audioThumb.addChangeListener(this);

    waveformHotCues[0] = 0.500;
}

/**
 * Destructor that performs clean up about object deallocation
 *
 * @param formatManageToUse       None
 *
 * @return                        None
 */
WaveformDisplay::~WaveformDisplay()
{
}

/**
 * Redraw a region of a component due to a change in the screen
 *
 * Draw different coloured waveforms before and after the custom playhead
 *
 * Add hot cue markers to frame the upper portion of the waveform
 *
 * @param                         Graphics context for drawing a component or image
 *
 * @return                        None
 */
void WaveformDisplay::paint(Graphics& g)
{
    // Paint the background
    g.fillAll(Colour(69, 81, 189));

    // Set outline colour of component
    g.setColour(Colours::lightblue);

    // Draw an outline around the component
    g.drawRect(getLocalBounds(), 3);

    // Set colour of waveform spanning from beginning to playhead
    g.setColour(Colour(147, 155, 220));

    // User has loaded file from library into deck
    if (fileLoaded)
    {
        // Generate a rectangle that indicates the region of audio that has not been played
        const Rectangle<int> waveformRectangleAfterPlayhead = createWaveformAfterPlayhead(audioThumb);

        // Draw a light blue waveform that spans from the playhead to end of the audio track
        audioThumb.drawChannel(g,
            waveformRectangleAfterPlayhead,
            positionRelative * audioThumb.getTotalLength(),
            audioThumb.getTotalLength(),
            0,
            1.0f);

        // Generate a rectangle that indicates the region of audio that has been played
        const Rectangle<int> waveformRectangle = createWaveformBeforePlayhead(audioThumb);

        // Set colour of waveform spanning from playhead to end
        g.setColour(Colour(3, 12, 99));

        // Draw a dark blue waveform that spans from the beginning of the audio track to the playhead position
        audioThumb.drawChannel(g,
            waveformRectangle,
            0,
            positionRelative * audioThumb.getTotalLength(),
            0,
            1.0f);

        // Draw center line of a white playhead
        g.setColour(Colours::white);
        g.drawRect(positionRelative * getWidth(), 0, getWidth() / 300, getHeight());

        // Create an upright triangle to frame the lower portion of the playhead
        Path lowerUprightTriangle;
        lowerUprightTriangle.addTriangle(positionRelative * getWidth() - 6, getHeight(), positionRelative * getWidth(), getHeight() - 18, positionRelative * getWidth() + 6, getHeight());
        g.fillPath(lowerUprightTriangle);

        // Create an inverted triangle to frame the upper portion of the playhead
        Path upperInvertedTriangle;
        upperInvertedTriangle.addTriangle(positionRelative * getWidth() - 6, 0, positionRelative * getWidth(), 18, positionRelative * getWidth() + 6, 0);
        g.fillPath(upperInvertedTriangle);

        // Set color for the hot cue indicators
        g.setColour(Colour(255, 67, 67));

        // Determine if first hot cue has been set by user
        if (waveformHotCues.count(1))
        {
            // Generate an inverted triangle to indicate the position of the first hot cue
            Path upperInvertedTriangle;
            upperInvertedTriangle.addTriangle(waveformHotCues[1] * getWidth() - 8, 0, waveformHotCues[1] * getWidth(), 15, waveformHotCues[1] * getWidth() + 8, 0);
            g.fillPath(upperInvertedTriangle);
        }

        // Determine if second hot cue has been set by user
        if (waveformHotCues.count(2))
        {
            // Generate an inverted triangle to indicate the position of the second hot cue
            Path upperInvertedTriangle;
            upperInvertedTriangle.addTriangle(waveformHotCues[2] * getWidth() - 8, 0, waveformHotCues[2] * getWidth(), 15, waveformHotCues[2] * getWidth() + 8, 0);
            g.fillPath(upperInvertedTriangle);
        }

        // Determine if third hot cue has been set by user
        if (waveformHotCues.count(3))
        {
            // Generate an inverted triangle to indicate the position of the third hot cue
            Path upperInvertedTriangle;
            upperInvertedTriangle.addTriangle(waveformHotCues[3] * getWidth() - 8, 0, waveformHotCues[3] * getWidth(), 15, waveformHotCues[3] * getWidth() + 8, 0);
            g.fillPath(upperInvertedTriangle);
        }
    }
    else
    {
        // No audio file loaded
        g.setColour(Colours::floralwhite);
        g.setFont(14.0f);
        g.drawText("Load a File", getLocalBounds(),
            Justification::centred, true);
    }
}

/**
 * Create a rectangle in the waveform that determines the region before the playhead whose audio has been played
 *
 * @param audioThumb              Enables drawing of scaled views of the waveform shape of an audio file
 *
 * @return                        Rectangle which contains region of played audio
 */
const Rectangle<int> WaveformDisplay::createWaveformBeforePlayhead(AudioThumbnail& audioThumb)
{
    // Create a region representing the waveform from start to playhead
    return Rectangle<int>(positionRelative * getWidth(), getHeight());
}

/**
 * Create a rectangle in the waveform that determines the region after the playhead whose audio has not been played
 *
 * @param audioThumb              Enables drawing of scaled views of the waveform shape of an audio file
 *
 * @return                        Rectangle which contains region of audio that has not been played
 */
const Rectangle<int> WaveformDisplay::createWaveformAfterPlayhead(AudioThumbnail& audioThumb)
{
    // Create a region representing the waveform from playhead to end
    return Rectangle<int>(positionRelative * getWidth(), 0, (1 - positionRelative) * getWidth(), getHeight());
}

/**
 * Determine what should happen to the component when resized
 *
 * @param                         None
 *
 * @return                        None
 */
void WaveformDisplay::resized()
{
}

/**
 * Specify the file or stream that contains the audio file
 *
 * @param audioURL                URL used as an input source to the drawing of the waveform
 *
 * @return                        None
 */
void WaveformDisplay::loadURL(URL audioURL)
{
    audioThumb.clear();
    fileLoaded = audioThumb.setSource(new URLInputSource(audioURL));
    if (fileLoaded)
    {
        repaint();
    }
}

/**
 * Receive change event callback due to changes in a ChangeBroadcaster
 *
 * @param source                  ChangeBroadcaster that triggered the callback
 *
 * @return                        None
 */
void WaveformDisplay::changeListenerCallback(ChangeBroadcaster* source)
{
    repaint();
}

/**
 * Getter that retrieves the relative position of the playhead
 *
 * @param                         None
 *
 * @return                        Relative position of playhead
 */
double WaveformDisplay::getPositionRelative()
{
    return positionRelative;
}

/**
 * Setter that sets the relative position of the playhead
 *
 * @param posRelative             Relative position to update
 *
 * @return                        None
 */
void WaveformDisplay::setPositionRelative(double posRelative)
{
    if (posRelative != positionRelative && posRelative >= 0)
    {
        positionRelative = posRelative;
        repaint();
    }
}

/**
 * Called when a mouse button is pressed
 *
 * This broadcasts the event to the audio player to update the playhead and audio track position
 *
 * @param event                   Object that details the position and status of the mouse event, and the source component in which it occurred
 *
 * @return                        None
 */
void WaveformDisplay::mouseDown(const MouseEvent& event)
{
    // Relative horizontal position of mouse event within waveform
    positionRelative = (double)event.getPosition().getX() / getWidth();

    // Send change message to registered listeners
    sendChangeMessage();
}

/**
 * Called when a mouse is moved while a button is held down
 *
 * This broadcasts the event to the audio player to update the playhead and audio track position
 *
 * @param event                   Object that details the position and status of the mouse event, and the source component in which it occurred
 *
 * @return                        None
 */
void WaveformDisplay::mouseDrag(const MouseEvent& event)
{
    // Relative horizontal position of mouse event within waveform
    positionRelative = (double)event.getPosition().getX() / getWidth();

    // Send change message to registered listeners
    sendChangeMessage();
}

/**
 * Called when a mouse button is released
 *
 * This broadcasts the event to the audio player to update the playhead and audio track position
 *
 * @param event                   Object that details the position and status of the mouse event, and the source component in which it occurred
 *
 * @return                        None
 */
void WaveformDisplay::mouseUp(const MouseEvent& event)
{
    // Relative horizontal position of mouse event within waveform
    positionRelative = (double)event.getPosition().getX() / getWidth();

    // Send change message to registered listeners
    sendChangeMessage();
}

/**
 * Store the relative position of a cue marker
 *
 * @param cueNumber               A number from one to three to identify the marker
 * @param relativePosition        Relative position of marker
 *
 * @return                        None
 */
void WaveformDisplay::insertCueRelativePosition(int cueNumber, double relativePosition)
{
    // Determine if a key value pair exists for a given cue number
    auto it = waveformHotCues.find(cueNumber);

    // Update relative position for a cue number
    if (it != waveformHotCues.end())
    {
        it->second = relativePosition;
    }
    else
    {
        /* Key-value pair does not exist
        Create a pair mapping the cue number to its relative position */
        waveformHotCues.insert(std::make_pair(cueNumber, relativePosition));
    }
}

/**
 * Play the audio track from the cue marker, given that it exists
 *
 * @param cueNumber               A number from one to three to identify the cue marker
 *
 * @return                        None
 */
void WaveformDisplay::playTrackFromCueMarker(int cueNumber)
{
    // Set audio track relative position given that the cue marker has been set by the user
    if (waveformHotCues.find(cueNumber) != waveformHotCues.end())
    {
        positionRelative = waveformHotCues[cueNumber];

        // Broadcast asynchronous change message to update audio track position
        sendChangeMessage();
    }
}