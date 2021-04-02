#pragma once

#include <Arduino.h>

class LedSequence
{
public:
    LedSequence(const char *sequence) : _index(0), _sequence(sequence), _length(strlen(sequence))
    {
    }

    inline bool getNextState() volatile
    {
        bool result = _sequence[_index]=='1';
        _index = (_index + 1) % _length;
        return result;
    }

    inline void reset() volatile
    {
        _index = 0;
    }

private:
    uint8_t _index;
    const char *_sequence;

    uint8_t _length;
};