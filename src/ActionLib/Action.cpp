#include "ActionLib/Action.hpp"


StatusCode Action::init(double timestamp)
{
    m_is_active = true;
    m_is_finished = false;

    // If m_on_init is set, then call it
    if(m_on_init)
    {
        return m_on_init(timestamp).status;
    }

    // Just default to OK   
    return StatusCode::OK;

} // end of "init(double)"


StatusCode Action::update(double timestamp, double time_since_last)
{
    // If inactive
    // Then do nothing
    if(!m_is_active)
        return StatusCode::OK;

    double mod_remainder = std::remainder(timestamp, m_update_every);

    // If mod_remainder is within TIME_WINDOW (close enough to 0)
    // That means that the current timestamp is a multiple of `m_update_every`
    // Meaning it has elapsed the proper amount of time for the callback

    if(mod_remainder < TIME_WINDOW && m_on_callback)
    {
        StatusedValue<bool> callback_status = m_on_callback(timestamp, time_since_last);
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


void Action::restart(double timestamp)
{
    init(timestamp);

} // end of "restart()"


void Action::kill()
{
    m_is_active = false;

} // end of "kill()"


bool Action::is_active()
{
    return m_is_active;

} // end of "is_active()"


bool Action::is_finished()
{
    return m_is_finished;

} // end of "is_finished()"