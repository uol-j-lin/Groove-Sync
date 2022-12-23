/*
  ==============================================================================

    PlaylistQueue.cpp
    Created: 4 Aug 2022 1:05:29am
    Author:  Jonathan

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PlaylistQueue.h"

/**
 * Constructor that initializes the playlist queue component
 *
 * Set the model and add columns to the queue table
 *
 * @param                         None
 *
 * @return                        None
 */
PlaylistQueue::PlaylistQueue()
{
    // Add a column to the table
    tableQueueComponent.getHeader().addColumn("Queued Tracks", 1, 130);

    // Set model for table queue component
    tableQueueComponent.setModel(this);

    // Add the table queue as a child component
    Component::addAndMakeVisible(tableQueueComponent);
}

/**
 * Destructor that cleans up after object is deallocated
 *
 * @param                         None
 *
 * @return                        None
 */
PlaylistQueue::~PlaylistQueue()
{
}

/**
 * Redraw a region of a component due to a change in the screen
 *
 * @param                         Graphics context for drawing a component or image
 *
 * @return                        None
 */
void PlaylistQueue::paint(juce::Graphics& g)
{
    // Clear the background
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::grey);

    // Draw outline around the component
    g.drawRect(getLocalBounds(), 1);

    g.setColour(juce::Colours::white);
    g.setFont(14.0f);
    g.drawText("PlaylistQueue", getLocalBounds(),
        // Draw some placeholder text
        juce::Justification::centred, true);
}

/**
 * Determine what should happen to the component when resized
 *
 * @param                         None
 *
 * @return                        None
 */
void PlaylistQueue::resized()
{
    tableQueueComponent.setBounds(0, 0, Component::getWidth(), Component::getHeight());
}

/**
 * Returns the number of rows in the playlist queue
 *
 * @param                         None
 *
 * @return                        Number of rows in table
 */
int PlaylistQueue::getNumRows()
{
    return audioTrackQueue.size();
}

/**
 * Draw the background behind one of the rows in the table of queued tracks
 *
 * @param rowNumber               Row background to draw
 * @param width                   Width of background
 * @param height                  Height of background
 * @param rowIsSelected           True if current row is selected, false otherwise
 *
 * @return                        None
 */
void PlaylistQueue::paintRowBackground(Graphics& g,
    int rowNumber,
    int width,
    int height,
    bool rowIsSelected)
{
    if (rowIsSelected)
    {
        // Make selected row distinct from unselected rows
        g.fillAll(Colours::cornflowerblue);
    }
    else
    {
        // Fill unselected row with default color
        g.fillAll(Colour(93, 118, 147));
    }
}

/**
 * Draw one of the cells in the table of queued tracks
 * @param rowNumber               Row background to draw
 * @param columnId                Unique ID to identify column
 * @param width                   Width of background
 * @param height                  Height of background
 * @param rowIsSelected           True if current row is selected, false otherwise
 *
 * @return                        None
 */
void PlaylistQueue::paintCell(Graphics& g,
    int rowNumber,
    int columnId,
    int width,
    int height,
    bool rowIsSelected)
{
    // Copy queue of audio track titles
    std::queue<std::string> audioNameQueueClone = audioNameQueue;
    std::vector<std::string> audioTrackNames;

    // Flatten queue into a vector so that track titles can be accessed by index/row number
    while (!audioNameQueueClone.empty())
    {
        audioTrackNames.push_back(audioNameQueueClone.front());
        audioNameQueueClone.pop();
    }

    if (rowNumber < getNumRows())
    {
        // Display track title in each row of the table in the order that they were enqueued
        if (columnId == 1)
        {
            g.drawText(audioTrackNames[rowNumber],
                1, 0,
                width - 4, height,
                Justification::centred,
                true);
        }
    }
}

/**
 * Enqueue the file meta data into a track title queue and a file queue
 *
 * @param fileMetaData            Track meta data consisting of the absolute file path and track title
 *
 * @return                        None
 */
void PlaylistQueue::enqueueTrack(std::vector<std::string> trackMetaData)
{
    // Parse the audio track file and its title
    File fileTrack = File{ trackMetaData[0] };
    std::string fileName = trackMetaData[1];

    // Enqueue the file and title into their respective queues
    audioTrackQueue.push(fileTrack);
    audioNameQueue.push(fileName);

    // Refresh content of table to display added track
    tableQueueComponent.updateContent();
    repaint();
}

/**
 * Dequeue the track meta data from the file and title queues
 *
 * @param                         None
 *
 * @return                        None
 */
File PlaylistQueue::dequeueTrack()
{
    // Extract next track in queue
    File nextTrackToPlay = audioTrackQueue.front();

    // Remove track from file and title queues
    audioTrackQueue.pop();
    audioNameQueue.pop();

    // Refresh content of table to remove track from display
    tableQueueComponent.updateContent();
    repaint();

    return nextTrackToPlay;
}

/**
 * Determine if there are any tracks queued up
 *
 * @param                         None
 *
 * @return                        True if there are no queued tracks, false otherwise
 */
bool PlaylistQueue::isEmpty()
{
    return audioTrackQueue.empty();
}