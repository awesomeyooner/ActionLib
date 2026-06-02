#include "ActionLib/Action.hpp"


using namespace status_utils;


Action::Action(double update_every, int repeat_n_times)
{
    m_update_every = update_every;
    m_repeat = repeat_n_times;

} // end of "Action(double, double)"


Action::Action(const Action& other)
{
    m_repeat = other.m_repeat;
    m_num_calls = other.m_num_calls;
    m_update_every = other.m_update_every;

    m_time_since_last_call = other.m_time_since_last_call;
    m_time_when_started = other.m_time_when_started;

    m_state = other.m_state;

    m_on_init = other.m_on_init;
    m_on_callback = other.m_on_callback;
    m_on_finish = other.m_on_finish;

} // end of Copy Constructor


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
    std::function<StatusedValue<bool>(double, double)> callback = [func = std::move(func)](double timestamp, double time_since_last) -> StatusedValue<bool>
    {
        func(timestamp);

        return StatusedValue<bool>{false, StatusCode::OK};
    };

    action.link_callback(callback);

    return action;

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
    m_state = ActionState::ACTIVE;

    m_time_when_started = timestamp;

    m_num_calls = 0;
    
    // If m_on_init is set, then call it
    if(m_on_init)
        return m_on_init(timestamp, time_since_last).status;

    // Just default to OK   
    return StatusCode::OK;

} // end of "init(double)"


StatusCode Action::update(double timestamp, double time_since_last)
{
    StatusCode status = StatusCode::OK;
    
    if(!is_initialized())
        if(init(timestamp, time_since_last) != StatusCode::OK)
            status == StatusCode::FAILED;

    // If the difference between the last call and now is less than the update every,
    // Then don't call the update function since we'd be double dipping on calls
    if(timestamp - m_time_since_last_call < m_update_every)
        return StatusCode::OK;

    // If the timestamp is a multiple of m_update_every (close to 0)
    // Then it should be called
    // Remember that x % m == 0 means x is a multiple of m
    double mod_remainder = m_update_every == 0 ? 0 : std::fmod(timestamp, m_update_every);

    // If mod_remainder is within TIME_WINDOW (close enough to 0)
    // That means that the current timestamp is a multiple of `m_update_every`
    // Meaning it has elapsed the proper amount of time for the callback
    if(mod_remainder < TIME_WINDOW && m_on_callback)
    {        
        StatusedValue<bool> callback_status = m_on_callback(timestamp, time_since_last);

        // Increment the number of calls since we just called it
        m_num_calls++;
        
        // Save the time of calling the callback since we just called it
        m_time_since_last_call = timestamp;

        // If the callback signaled finished
        // OR
        // If this Action has a set number of calls and it has been exceeded
        // Then kill this action
        if(callback_status.value == true || (m_repeat != -1 && m_num_calls >= m_repeat))
        {
            StatusCode finish_status = finish(timestamp, time_since_last);

            return combine_statuses({callback_status.status, finish_status});
        }
    }

    return StatusCode::OK;

} // end of "update(double, double)"


StatusCode Action::finish(double timestamp, double time_since_last)
{
    m_state = ActionState::FINISHED;

    if(m_on_finish)
        return m_on_finish(timestamp, time_since_last);

    return StatusCode::OK;

} // end of "finish(double, double)"


void Action::restart(double timestamp, double time_since_last)
{
    m_state = ActionState::UNINITIALIZED;

    update(timestamp, time_since_last);

} // end of "restart()"


void Action::kill()
{
    m_state = ActionState::FINISHED;

} // end of "kill()"


ActionState Action::get_state()
{
    return m_state;

} // end of "get_state()"


bool Action::is_active()
{
    return m_state == ActionState::ACTIVE;

} // end of "is_active()"


bool Action::is_finished()
{
    return m_state == ActionState::FINISHED;

} // end of "is_finished()"


bool Action::is_initialized()
{
    return m_state != ActionState::UNINITIALIZED;

} // end of "has_initialized()"