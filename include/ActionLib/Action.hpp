#ifndef ACTION_HPP
#define ACTION_HPP


#include <functional>
#include <cmath>

#include "EmbeddedLib/util/status.hpp"

using namespace status_utils;


class Action
{
    public:

        Action(double update_every = 0, int repeat_n_times = -1);

        static Action run_once();

        void link_init(std::function<StatusedValue<bool>(double, double)> func);

        void link_callback(std::function<StatusedValue<bool>(double, double)> func);

        void link_finish(std::function<StatusCode(double, double)> func);

        // Initialize the action
        StatusCode init(double timestamp, double time_since_last);

        StatusCode update(double timestamp, double time_since_last);

        StatusCode finish(double timestamp, double time_since_last);

        bool is_active();

        bool is_finished();

        void kill();

        void restart(double timestamp, double time_since_last);

    private:

        // How far the modulus can differ for the callback to be called (in seconds)
        static constexpr double TIME_WINDOW = 0.0001; // 5 ms

        // The number of times this function should repeat before finishing
        // If this number is -1, then that means indefinite
        int m_repeat = -1;

        // The numebr of times the callback has been called
        int m_num_calls = 0;

        // When to call the callback (in seconds)
        // Ex: `update_every = 1.5` Means the callback gets called every `1.5` seconds
        double m_update_every;

        // The time since the last callback was called (in seconds)
        // This is NOT `time_since_last` as that refers to the last iteration
        // of ActionManager
        double m_time_since_last_call;

        // The time when the action initialized / started
        double m_time_when_started;

        // Flag for if this action should be updating or not
        bool m_is_active = false;

        // Flag for if this action has FINISHED, signalling to remove this action
        bool m_is_finished = false;

        // The function to run when the action is first started
        // Returns `true` if this action should terminate
        std::function<StatusedValue<bool>(double, double)> m_on_init;

        // The callback function whenever the time is right
        // Returns `true` if this action has finished
        std::function<StatusedValue<bool>(double, double)> m_on_callback;

        // The function to run when this action finishes
        std::function<StatusCode(double, double)> m_on_finish;



}; // struct Action


#endif // ACTION_HPP