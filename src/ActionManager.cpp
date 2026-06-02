#include "ActionLib/ActionManager.hpp"


using namespace status_utils;


double ActionManager::m_prev_timestamp = 0;

std::vector<Action> ActionManager::m_actions;


void ActionManager::init()
{
    m_prev_timestamp = System::get_seconds();

    m_actions.reserve(MAX_ACTIONS);

} // end of "init()"


StatusCode ActionManager::update()
{
    // Get the current timestamp (in seconds)
    double timestamp = System::get_seconds();
    double time_since_last = timestamp - m_prev_timestamp;

    // OK for now
    // Will change if something comes up
    StatusCode accumulated_status = StatusCode::OK;

    std::vector<int> indexes_to_erase;

    /*
    For every Action

    - Call the update function
    - If it failed, change the value of `accumulated_status`
    - If the Action finished, queue it to be removed from the list
    */
    for(int i = 0; i < m_actions.size(); i++)
    {
        // Get the current action
        Action& current_action = m_actions.at(i);
        
        // Update the action
        // Note that all callback calculations are done within the Action's update method,
        // not this method
        StatusCode update_status = current_action.update(timestamp, time_since_last);

        accumulated_status = combine_statuses({accumulated_status, update_status});

        // If the action is done
        // Then remove it from the action list
        if(current_action.is_finished())
        {
            // Prepare to remove this action from the list
            indexes_to_erase.push_back(i);
        }

    }

    // Erase all the actions that are finished
    erase_actions(indexes_to_erase);

    // Refresh the previous timestamp with the latest one
    m_prev_timestamp = timestamp;

    // If OK, return OK
    // Else return FAILED
    return accumulated_status;

} // end of "update()"


void ActionManager::add(Action action)
{
    if(m_actions.size() < MAX_ACTIONS)
        m_actions.push_back(action);

} // end of "add(Action)"


int ActionManager::get_size()
{
    return m_actions.size();

} // end of "get_size()"


void ActionManager::erase_actions(const std::vector<int>& indexes)
{
    for(int i = 0; i < indexes.size(); i++)
    {
        int current_index = indexes.at(i);

        m_actions.erase(m_actions.begin() + current_index);
    }
    
} // end of "erase_actions(const std::vector<int>& indexes)"