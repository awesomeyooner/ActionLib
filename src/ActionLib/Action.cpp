#include "ActionLib/Action.hpp"

#include "EmbeddedLib/communication/Serial.hpp"

Action::Action(double update_every, int repeat_n_times)
{
    m_update_every = update_every;
    m_repeat = repeat_n_times;

} // end of "Action(double, double)"


Action Action::run_once()
{
    return Action(0, 1);   

} // end of "run_once()"


Action Action::run_once(std::function<void()> func)
{
    Action action{0, 1};

    // Create the proper callback with the correct params and arguments
    // but all it does is just call the function
    std::function<StatusedValue<bool>(double, double)> callback = [func](double timestamp, double time_since_last) -> StatusedValue<bool>
    {
        func();

        return StatusedValue<bool>{false, StatusCode::OK};
    };

    action.link_callback(callback);

    return action;

} // end of "run_once(std::function<void()>)"


Action Action::run_once(std::function<void(double)> func)
{
    Action action{0, 1};

    // Create the proper callback with the correct params and arguments
    // but all it does is just call the function
    std::function<StatusedValue<bool>(double, double)> callback = [func](double timestamp, double time_since_last) -> StatusedValue<bool>
    {
        func(timestamp);

        return StatusedValue<bool>{false, StatusCode::OK};
    };

    action.link_callback(callback);
} // end of "run_once(std::function<void(double)>)"


void Action::link_init(std::function<StatusedValue<bool>(double, double)> func)
{
    m_on_init = func;

} // end of "link_init(std::function<StatusedValue<bool>(double, double)>)"


void Action::link_callback(std::function<StatusedValue<bool>(double, double)> func)
{
    m_on_callback = func;

} // end of "link_callback(std::function<StatusedValue<bool>(double, double)>)"


void Action::link_finish(std::function<StatusCode(double, double)> func)
{
    m_on_finish = func;

} // end of "link_finish(std::function<StatusedValue<bool>(double, double)>)"


StatusCode Action::init(double timestamp, double time_since_last)
{
    m_is_active = true;
    m_is_finished = false;

    m_time_when_started = timestamp;

    m_num_calls = 0;
    
    // If m_on_init is set, then call it
    if(m_on_init)
    {
        return m_on_init(timestamp, time_since_last).status;
    }

    // Just default to OK   
    return StatusCode::OK;

} // end of "init(double)"


StatusCode Action::update(double timestamp, double time_since_last)
{

    // If inactive and finished
    // Then that means there's nothing to do
    // So this action is waiting to be restarted to deleted
    if(!m_is_active && m_is_finished)
        return StatusCode::OK;
    // If not active nor finished
    // That means this action is uninitialized
    // So initialize it
    else if(!m_is_active && !m_is_finished)
        init(timestamp, time_since_last);
    // If the difference between the last call and now is less than the update every,
    // Then don't call the update function since we'd be double dipping on calls
    else if(m_time_since_last_call != 0 && timestamp - m_time_since_last_call < m_update_every)
        return StatusCode::OK;

    if(m_update_every == 0)
    {
        StatusedValue<bool> callback_status = m_on_callback(timestamp, time_since_last);

        m_time_since_last_call = timestamp;

        // If the callback signaled finished
        // Then kill this action
        if(callback_status.value == true)
        {
            kill();
            return callback_status.status;
        }

        if(m_repeat != -1)
        {
            // Increment the number of calls that have been... called
            m_num_calls++;

            // If the number of calls equals or exceeds the number of calls until finished
            // Then kill this action
            if(m_num_calls >= m_repeat)
            {
                kill();
                return callback_status.status;
            }
        }

        return callback_status.status;
    }

    double mod_remainder = std::fmod(timestamp - m_time_since_last_call, m_update_every);

    // If mod_remainder is within TIME_WINDOW (close enough to 0)
    // That means that the current timestamp is a multiple of `m_update_every`
    // Meaning it has elapsed the proper amount of time for the callback
    if(mod_remainder < TIME_WINDOW && m_on_callback)
    {
        StatusedValue<bool> callback_status = m_on_callback(timestamp, time_since_last);

        m_time_since_last_call = timestamp;

        // If the callback signaled finished
        // Then kill this action
        if(callback_status.value == true)
        {
            kill();
            return callback_status.status;
        }

        if(m_repeat != -1)
        {
            // Increment the number of calls that have been... called
            m_num_calls++;

            // If the number of calls equals or exceeds the number of calls until finished
            // Then kill this action
            if(m_num_calls >= m_repeat)
            {
                kill();
                return callback_status.status;
            }
        }
    }

} // end of "update(double, double)"


StatusCode Action::finish(double timestamp, double time_since_last)
{
    m_is_active = false;
    m_is_finished = true;

    if(m_on_finish)
        return m_on_finish(timestamp, time_since_last);

    return StatusCode::OK;

} // end of "finish(double, double)"


void Action::restart(double timestamp, double time_since_last)
{
    m_is_active = false;
    m_is_finished = false;

    update(timestamp, time_since_last);

} // end of "restart()"


void Action::kill()
{
    m_is_active = false;
    m_is_finished = true;

} // end of "kill()"


bool Action::is_active()
{
    return m_is_active;

} // end of "is_active()"


bool Action::is_finished()
{
    return m_is_finished;

} // end of "is_finished()"


bool Action::has_initialized()
{
    // Not active and not finished means it hasn't started, aka not init
    return !m_is_active && !m_is_finished;

} // end of "has_initialized()"