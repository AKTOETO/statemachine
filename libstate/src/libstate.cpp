#include "libstate.hpp"

#include <algorithm>
#include <iostream>

namespace SM
{
    State::State(const std::string &name) : m_name(name)
    {
    }

    State::~State(){

    };

    void State::setSwitcher(std::function<void(const Event &)> switcher)
    {
        m_switcher = switcher;
    }

    void State::needToSwitch(const Event &event)
    {
        std::cout << "Need to switch '" << m_name << "' event: " << (uint)event.m_type << "\n";
        if (m_switcher)
            m_switcher(event);
    };

    std::optional<std::shared_ptr<State>> Scenario::findState(const std::string &name)
    {
        auto found = std::find_if(m_states.begin(), m_states.end(),
                                  [&name](const std::shared_ptr<State> &state) { return state->m_name == name; });
        return (found == m_states.end()) ? std::nullopt : std::make_optional(*found);
    }

    void Scenario::setStartState(const std::string &name)
    {
        auto state = findState(name);
        if (state)
        {
            m_cur_state = *state;
            m_cur_state->init({});
        }
        else
        {
            std::cout << "Unknown state: " << name << "\n";
        }
    }

    Scenario::Scenario()
    {
    }

    Scenario::~Scenario()
    {
        if (m_cur_state)
            m_cur_state->exit({});
    }

    void Scenario::addState(std::shared_ptr<State> state)
    {
        std::cout << "Adding state: " << state->m_name << '\n';
        for (auto &el : m_states)
        {
            if (el->m_name == state->m_name)
                return;
        }
        state->setSwitcher([this](const Event &event) { switcher(event); });
        m_states.push_back(state);
    }

    void Scenario::update(const State::callbackParams &params)
    {
        if (m_cur_state)
        {
            std::cout << "Updating state: " << m_cur_state->m_name << "\n";
            m_cur_state->update(params);
        }
    }
} // namespace SM