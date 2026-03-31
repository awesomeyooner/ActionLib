#ifndef ACTION_MANAGER_HPP
#define ACTION_MANAGER_HPP


#include <vector>

#include "EmbeddedLib/System.hpp"

#include "ActionLib/Action.hpp"


class ActionManager
{
    public:
        
        static std::vector<Action> m_actions;

        static void init();
        
        static StatusCode update();

        /**
         * @brief Add an action to the list of actions. This function is
         * NON BLOCKING (should be), since all of the function callbacks are handled
         * in `update`
         * 
         * @param action 
         */
        static void add(Action action);

    private:
        
        static double m_prev_timestamp;


}; // class ActionManager


#endif // ACTION_MANAGER_HPP