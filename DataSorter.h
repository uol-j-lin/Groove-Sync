/*
  ==============================================================================

    DataSorter.h
    Created: 18 Jul 2022 9:36:05pm
    Author:  Jonathan

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

using namespace juce;

class DataSorter
{
public:
    /**
    * Constructor that initializes data members, determining ascending or descending sort order and track meta data attribute to sort by
    *
    * @param _player                 Audio player
    * @param formatManagerToUse      Format manager that records available formats
    *
    * @return                        None
    */
    DataSorter(String& attributeToSortBy, bool forwards);

    /**
    * Sort two XML elements based on a sorting attribute comparator
    *
    * @param first                   XML element representing an audio track
    * @param second                  XML element representing an audio track
    *
    * @return                        Value of < 0 if first comes before second, 0 is two objects are equivalent, > 0 if the second comes before the first
    */
    int compareElements(XmlElement* first, XmlElement* second) const;

private:
    // Track meta data attribute to sort by
    String attributeToSort;

    // Sort ascending or descending
    int direction;
};
