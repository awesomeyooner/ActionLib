#ifndef ACTION_HPP
#define ACTION_HPP


#include <functional>
#include <cmath>

#include "EmbeddedLib/util/status.hpp"

using namespace status_utils;


class Action
{
    public:

        // Initialize the action
        StatusCode init(double timestamp);

        StatusCode update(double timestamp, double time_since_last);

        StatusCode finish(double timestamp, double time_since_last);

        bool is_active();

        bool is_finished();

        void kill();

        void restart(double timestamp);

    private:

        // How far the modulus can differ for the callback to be called (in seconds)
        const double TIME_WINDOW = 0.005; // 5 ms

        // When to call the callback (in seconds)
        // Ex: `update_every = 1.5` Means the callback gets called every `1.5` seconds
        double m_update_every;

        bool m_is_active = false;
        bool m_is_finished = false;

        // The function to run when the action is first started
        // Returns `true` if this action should terminate
        std::function<StatusedValue<bool>(double)> m_on_init;

        // The callback function whenever the time is right
        // Returns `true` if this action has finished
        std::function<StatusedValue<bool>(double, double)> m_on_callback;

        // The function to run when this action finishes
        std::function<StatusCode(double, double)> m_on_finish;



}; // struct Action


#endif // ACTION_HPP