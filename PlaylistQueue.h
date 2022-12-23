/*
  ==============================================================================

    PlaylistQueue.h
    Created: 4 Aug 2022 1:05:29am
    Author:  Jonathan

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

using namespace juce;

class PlaylistQueue  : public juce::Component,
    public TableListBoxModel
{
public:
    /**
     * Constructor that initializes the playlist queue component
     *
     * Set the model and add columns to the queue table
     *
     * @param                         None
     *
     * @return                        None
     */
    PlaylistQueue();

    /**
     * Destructor that cleans up after object is deallocated
     *
     * @param                         None
     *
     * @return                        None
     */
    ~PlaylistQueue() override;

    /**
    * Redraw a region of a component due to a change in the screen
    *
    * @param                         Graphics context for drawing a component or image
    *
    * @return                        None
    */
    void paint (juce::Graphics&) override;

    /**
    * Determine what should happen to the component when resized
    *
    * @param                         None
    *
    * @return                        None
    */
    void resized() override;

    /**
     * Returns the number of rows in the playlist queue
     *
     * @param                         None
     *
     * @return                        Number of rows in table
     */
    int getNumRows() override;

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
    void paintRowBackground(Graphics&, int rowNumber, int width, int height, bool rowIsSelected) override;

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
    void paintCell(Graphics&, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

    /**
    * Enqueue the file meta data into a track title queue and a file queue
    *
    * @param fileMetaData            Track meta data consisting of the absolute file path and track title 
    *
    * @return                        None
    */
    void enqueueTrack(std::vector<std::string> trackMetaData);

    /**
    * Dequeue the track meta data from the file and title queues
    *
    * @param                         None
    *
    * @return                        None
    */
    File dequeueTrack();

    /**
    * Determine if there are any tracks queued up
    *
    * @param                         None
    *
    * @return                        True if there are no queued tracks, false otherwise
    */
    bool isEmpty();

private:
    TableListBox tableQueueComponent;

    std::queue<File> audioTrackQueue;
    std::queue<std::string> audioNameQueue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistQueue)
};
