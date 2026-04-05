#ifndef ACTION_HPP
#define ACTION_HPP


#include <functional>
#include <cmath>

#include "EmbeddedLib/util/status.hpp"

using namespace status_utils;


class Action
{
    public:

        /**
         * @brief Creates an Action with the given update frequency and repetitions
         * 
         * @param update_every `double` How often to call the callback in seconds.
         * If this value is 0 then it will update every single loop.
         * Ex: `update_every = 1.0` means update every second
         * @param repeat_n_times `int` How many times to call the callback until this Action is finished.
         * If this value is `-1` then this Action will repeat indefinetly until it is finished
         */
        Action(double update_every = 0, int repeat_n_times = -1);

        /**
         * @brief Creates a new action that only runs once
         * 
         * @return `Action` The newly created Action 
         */
        static Action run_once();

        /**
         * @brief Link the initialize function.
         * 
         * @param func `std::function<StatusedValue<bool>(double, double)>` The init function.
         * Takes in two doubles, the first is the current timestamp in seconds and the second is
         * the time since the last main loop (NOT the last call of the callback but of the actual while loop).
         * This should return `StatusedValue<bool>`, the `bool` part doesn't do anything
         */
        void link_init(std::function<StatusedValue<bool>(double, double)> func);

        /**
         * @brief Link the callback function.
         * 
         * @param func `std::function<StatusedValue<bool>(double, double)>` The callback function.
         * Takes in two doubles, the first is the current timestamp in seconds and the second is
         * the time since the last main loop (NOT the last call of the callback but of the actual while loop).
         * This should return `StatusedValue<bool>`, returns true if the Action has finished, false otherwise.
         * The StatusCode part is just for notifying ActionManager.
         */
        void link_callback(std::function<StatusedValue<bool>(double, double)> func);

        /**
         * @brief Link the finish function / cleanup function.
         * 
         * @param func `std::function<StatusCode<bool>(double, double)>` The function to call when finished.
         * Takes in two doubles, the first is the current timestamp in seconds and the second is
         * the time since the last main loop (NOT the last call of the callback but of the actual while loop).
         * This should return `StatusCode`.
         */
        void link_finish(std::function<StatusCode(double, double)> func);

        /**
         * @brief Initialize the timestamps. This function is called internally in `update`
         * 
         * @param timestamp `double` The current timestamp in seconds
         * @param time_since_last `double` The time since the last loop of the main while loop
         * @return `StatusCode` The status of how this method ended 
         */
        StatusCode init(double timestamp, double time_since_last);

        /**
         * @brief Checks if this Action should be initialized and checks if the time is right to
         * call the callback function.
         * 
         * @param timestamp `double` The current timestamp in seconds
         * @param time_since_last `double` The time since the last loop of the main while loop
         * @return `StatusCode` The status of how this method ended 
         */
        StatusCode update(double timestamp, double time_since_last);

        /**
         * @brief Sets the states of this Action that it's finished and calls the finish callback
         * if it's set.
         * 
         * @param timestamp `double` The current timestamp in seconds
         * @param time_since_last `double` The time since the last loop of the main while loop
         * @return `StatusCode` The status of how this method ended 
         */
        StatusCode finish(double timestamp, double time_since_last);

        /**
         * @brief Gets if this Action is actively updating or not
         * 
         * @return `true` If this Action is actively updating 
         * @return `false` Otherwise 
         */
        bool is_active();

        /**
         * @brief Gets if this Action has finished or not
         * 
         * @return `true` If this Action is finished 
         * @return `false` Otherwise 
         */
        bool is_finished();

        /**
         * @brief Gets if this Action has initialized or not
         * 
         * @return `true` If yes 
         * @return `false` Otherwise 
         */
        bool has_initialized();

        /**
         * @brief Sets the state of this Action that this Action is done. The actual
         * finished callback is called through ActionManager
         * 
         */
        void kill();

        /**
         * @brief Resets the state of this Action notifying ActionManager to keep this alive and
         * re-call init
         * 
         * @param timestamp `double` The current timestamp in seconds
         * @param time_since_last `double` The time since the last loop of the main while loop
         */
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