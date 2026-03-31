#include "ActionLib/ActionManager.hpp"


double ActionManager::m_prev_timestamp = 0;

std::vector<Action> ActionManager::m_actions;


void ActionManager::init()
{
    m_prev_timestamp = System::get_seconds();

} // end of "init()"


StatusCode ActionManager::update()
{
    // Get the current timestamp (in seconds)
    double timestamp = System::get_seconds();
    double time_since_last = timestamp - m_prev_timestamp;

    // Assume all actions return OK
    // If one isn't OK, then update this variable
    bool all_actions_OK = true;

    for(int i = 0; i < m_actions.size(); i++)
    {
        // Get the current action
        Action& current_action = m_actions.at(i);
        
        // Update the action
        // Note that all callback calculations are done within the Action's update method,
        // not this method
        StatusCode update_status = current_action.update(timestamp, time_since_last);

        // If this update wasn't successful
        // Then `all_actions_OK` is false
        if(update_status != StatusCode::OK)
            all_actions_OK = false;

        // If the action is done
        // Then run the actions finished routine and remove it from the action list
        if(current_action.is_finished())
        {
            // Call the finish routine
            StatusCode finish_status = current_action.finish(timestamp, time_since_last);

            // If the action didn't finish properly
            // Then `all_actions_OK` is false
            if(finish_status != StatusCode::OK)
                all_actions_OK = false;

            // Remove this action from the list
            m_actions.erase(m_actions.begin() + i);
        }
    }

    // Refresh the previous timestamp with the latest one
    m_prev_timestamp = timestamp;

    // If OK, return OK
    // Else return FAILED
    return all_actions_OK ? StatusCode::OK : StatusCode::FAILED;

} // end of "update()"


void ActionManager::add(Action action)
{
    m_actions.push_back(action);

} // end of "add(Action)"