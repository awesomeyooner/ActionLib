#ifndef ACTION_MANAGER_HPP
#define ACTION_MANAGER_HPP


#include <vector>

#include "EmbeddedLib/System.hpp"

#include "ActionLib/Action.hpp"


class ActionManager
{
    public:
        
        std::vector<Action> m_actions;

        StatusCode update();

    private:
        
        double m_prev_timestamp = 0;


}; // class ActionManager


#endif // ACTION_MANAGER_HPP