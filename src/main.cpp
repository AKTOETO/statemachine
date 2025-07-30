#include <algorithm>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <string>

namespace SM {

class State;
// Структура события, передаваемая при желании переключиться
struct Event
{
    enum class Type { TryAgain, GotPassword };
    Type m_type;
    State *m_sender_state = nullptr;
};

class Scenario;
// Структура описывает состояние в текущем сценарии
class State
{
    friend Scenario;

protected:
    std::function<void(const Event &)> m_switcher = nullptr;
    std::string m_name;

public:
    State(const std::string &name)
        : m_name(name) {};
    virtual ~State() {};

    // параметры функции обратного вызова для состояния
    using callbackParams = std::map<std::string, std::string>;
    // тип функции обратного вызова
    using callbackType = std::function<void(callbackParams)>;

    /**
     * @brief Функция запуска состояния. Вызывается при переходе в это состояние.
     * 
     * @param prams Параметры для функции инициализации текущего состояния
     */
    virtual void init(const callbackParams &prams) {};
    virtual void update(const callbackParams &prams) {};
    virtual void exit(const callbackParams &prams) {};

    /**
     * @brief Установка функции уведомления о переключении
     * 
     * @param switcher функция - переключатель
     */
    void setSwitcher(std::function<void(const Event &)> switcher) { m_switcher = switcher; }

protected:
    /**
     * @brief Переключение с событием. Нужно для наследованных состояний
     * 
     * @param event Событие переключения
     */
    void needToSwitch(const Event &event)
    {
        std::cout << "Need to switch '" << m_name << "' event: " << (uint) event.m_type << "\n";
        if (m_switcher)
            m_switcher(event);
    };
};

// Сценарий взаимодействия состояний
class Scenario
{
protected:
    std::shared_ptr<State> m_cur_state = nullptr;
    std::list<std::shared_ptr<State>> m_states;

    /**
     * @brief Функция переключения состояния. Реализуется в каждом сценарии своя. Он должен вызывать функции exit() и init() у состояний.
     * 
     * @param event событие переключения
     */
    virtual void switcher(const Event &event) = 0;

    std::optional<std::shared_ptr<State>> findState(const std::string &name)
    {
        auto found = std::find_if(m_states.begin(),
                                  m_states.end(),
                                  [&name](const std::shared_ptr<State> &state) {
                                      return state->m_name == name;
                                  });
        return (found == m_states.end()) ? std::nullopt : std::make_optional(*found);
    }

    void setStartState(const std::string &name)
    {
        auto state = findState(name);
        if (state) {
            m_cur_state = *state;
            m_cur_state->init({});
        } else {
            std::cout << "Unknown state: " << name << "\n";
        }
    }

public:
    Scenario() {}
    virtual ~Scenario()
    {
        if (m_cur_state)
            m_cur_state->exit({});
    };

    void addState(std::shared_ptr<State> state)
    {
        std::cout << "Adding state: " << state->m_name << '\n';
        for (auto &el : m_states) {
            if (el->m_name == state->m_name)
                return;
        }
        state->setSwitcher([this](const Event &event) { switcher(event); });
        m_states.push_back(state);
    }

    void update(const State::callbackParams &params)
    {
        if (m_cur_state) {
            std::cout << "Updating state: " << m_cur_state->m_name << "\n";
            m_cur_state->update(params);
        }
    }
};

} // namespace SM

////// EXAMPLE 1

class StateTryAgain : public SM::State
{
public:
    StateTryAgain()
        : SM::State("TryAgain") {};

    virtual void init(const callbackParams &prams) override
    {
        std::cout << "Initing...." << m_name << "\n";
    };
    virtual void update(const callbackParams &prams) override
    {
        std::cout << "Updating" << m_name << "\n";
        SM::Event ev = {.m_type = SM::Event::Type::GotPassword, .m_sender_state = this};

        needToSwitch(ev);
    };
    virtual void exit(const callbackParams &prams) override
    {
        std::cout << "exiting..." << m_name << "\n";
    };
};

class StateNewLogin : public SM::State
{
public:
    StateNewLogin()
        : SM::State("NewLogin") {};

    virtual void init(const callbackParams &prams) override
    {
        std::cout << "Initing...." << m_name << "\n";
    };
    virtual void update(const callbackParams &prams) override
    {
        std::cout << "Updating" << m_name << "\n";
        SM::Event ev = {.m_type = SM::Event::Type::TryAgain, .m_sender_state = this};

        needToSwitch(ev);
    };
    virtual void exit(const callbackParams &prams) override
    {
        std::cout << "exiting..." << m_name << "\n";
    };
};

class Scen : public SM::Scenario
{
public:
    Scen()
    {
        addState(std::make_shared<StateNewLogin>());
        addState(std::make_shared<StateTryAgain>());
        setStartState("NewLogin");
    }

private:
    virtual void switcher(const SM::Event &event) override
    {
        auto &sender = event.m_sender_state;
        auto &type = event.m_type;

        sender->exit({});
        std::optional<std::shared_ptr<SM::State>> state;
        // Логика переключения состояний
        switch (type) {
        case SM::Event::Type::TryAgain:
            state = findState("TryAgain");
            if (state) {
                m_cur_state = *state;
                m_cur_state->init({});
            } else
                std::cout << "Unknown state: TryAgain\n";
            break;
        case SM::Event::Type::GotPassword:
            state = findState("NewLogin");
            if (state) {
                m_cur_state = *state;
                m_cur_state->init({});
            } else
                std::cout << "Unknown state: NewLogin\n";
            break;
        default:
            std::cout << "Unknown type: " << (uint) type << '\n';
        }
    }
};

int main()
{
    Scen sc;
    std::cout << "=== Hello" << "\n";
    sc.update({});
    sc.update({});
    sc.update({});
    sc.update({});
    std::cout << "=== Hello" << "\n";
    sc.update({});
    std::cout << "=== Hello" << "\n";
    sc.update({});
    sc.update({});
    return 0;
}