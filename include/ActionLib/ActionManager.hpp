#ifndef ACTION_MANAGER_HPP
#define ACTION_MANAGER_HPP


#include <vector>

#include "EmbeddedLib/system.hpp"

#include "ActionLib/Action.hpp"


class ActionManager
{
    public:
        
        // All of the actions to execute
        static std::vector<Action> m_actions;

        /**
         * @brief Sets the current timestamp
         * 
         */
        static void init();
        
        /**
         * @brief Calls all of the actions' update functions. This method is BLOCKING, and
         * is what actually handles all of the expensive calls
         * 
         * @return `StatusCode` OK if all actions were OK 
         */
        static status_utils::StatusCode update();

        /**
         * @brief Add an action to the list of actions. This function is
         * NON BLOCKING (should be), since all of the function callbacks are handled
         * in `update`
         * 
         * @param action 
         */
        static void add(Action action);

        static int get_size();

    private:

        // Max number of actions in the list
        static constexpr int MAX_ACTIONS = 32;
        
        // The timestamp of the previous loop in seconds
        static double m_prev_timestamp;

        static void erase_actions(const std::vector<int>& indexes);


}; // class ActionManager


#endif // ACTION_MANAGER_HPP