#include "libstate.hpp"

#include <algorithm>
#include <iostream>

namespace SM
{
    State::State(const std::string &name)
        : m_name(name)
        , m_requester(nullptr)
        , m_switcher(nullptr)
    {
    }

    State::~State()
    {
    }

    void State::setSwitcher(std::function<void(const Event &)> switcher)
    {
        m_switcher = switcher;
    }

    void State::setRequester(std::function<void(const callbackParams &params)> requester)
    {
        m_requester = requester;
    }

    const std::string &State::getName() const
    {
        return m_name;
    }

    void State::needToSwitch(const Event &event)
    {
        std::cout << "Need to switch '" << m_name << "' event: " << (uint)event.m_type << "\n";
        if (m_switcher)
            m_switcher(event);
    }
    void State::request(const callbackParams &params)
    {
        std::cout << "Requesting smth" << '\n';
        if (m_requester)
            m_requester(params);
    };

    std::optional<std::shared_ptr<State>> Scenario::findState(const std::string &name)
    {
        auto found = std::find_if(m_states.begin(), m_states.end(),
                                  [&name](const std::shared_ptr<State> &state) { return state->getName() == name; });
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
        : m_requester(nullptr)
        , m_cur_state(nullptr)
    {
    }

    Scenario::~Scenario()
    {
        if (m_cur_state)
            m_cur_state->exit({});
    }

    void Scenario::request(const State::callbackParams &params)
    {
        std::cout << "Scenario::Reqesting\n";
        if (m_requester)
            m_requester(params);
    }

    void Scenario::addState(std::shared_ptr<State> state)
    {
        std::cout << "Adding state: " << state->getName() << '\n';
        for (auto &el : m_states)
        {
            if (el->getName() == state->getName())
                return;
        }
        state->setSwitcher([this](const Event &event) { switcher(event); });
        state->setRequester([this](const State::callbackParams &params) { request(params); });
        m_states.push_back(state);
    }

    void Scenario::update(const State::callbackParams &params)
    {
        if (m_cur_state)
        {
            std::cout << "Updating state: " << m_cur_state->getName() << "\n";
            m_cur_state->update(params);
        }
    }

    Event::Event(Event::Type type, State *state, const State::callbackParams &data)
        : m_type(type)
        , m_sender_state(state)
        , m_data(data)
    {
    }
} // namespace SM