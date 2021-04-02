#pragma once

typedef void (*TouchButtonCallback)(const bool state);

class TouchButton
{
public:
    TouchButton() : min_count(0), max_count(5), current_count(0), current_state(false), callback(0)
    {
    }

    void onStateChange(TouchButtonCallback cb)
    {
        callback = cb;
    }

    void update(bool button_state)
    {
        if (button_state)
            ++current_count;
        else
            --current_count;

        bool oldState = current_state;
        if (current_count < min_count)
        {
            current_state = false;
            current_count = min_count;
        }
        if (current_count > max_count)
        {
            current_state = true;
            current_count = max_count;
        }
        if ((oldState ^ current_state) && callback)
        {
            callback(current_state);
        }
    }

private:
    int min_count;
    int max_count;
    int current_count;
    bool current_state;
    TouchButtonCallback callback;
};