#ifndef LIBSTATE_HPP
#define LIBSTATE_HPP

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <string>

// Пространство имен библиотеки состояний
namespace SM
{
    class State;
    // Структура события, передаваемая при желании переключиться
    struct Event
    {
        enum class Type
        {
            TryAgain,
            GotPassword
        };
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
        State(const std::string &name);
        virtual ~State();

        // параметры функции обратного вызова для состояния
        using callbackParams = std::map<std::string, std::string>;
        // тип функции обратного вызова
        using callbackType = std::function<void(callbackParams)>;

        /**
         * @brief Функция запуска состояния. Вызывается при переходе в это
         * состояние.
         *
         * @param prams Параметры для функции инициализации текущего состояния
         */
        virtual void init(const callbackParams &prams){};
        virtual void update(const callbackParams &prams){};
        virtual void exit(const callbackParams &prams){};

        /**
         * @brief Установка функции уведомления о переключении
         *
         * @param switcher функция - переключатель
         */
        void setSwitcher(std::function<void(const Event &)> switcher);

      protected:
        /**
         * @brief Переключение с событием. Нужно для наследованных состояний
         *
         * @param event Событие переключения
         */
        void needToSwitch(const Event &event);
    };

    // Сценарий взаимодействия состояний
    class Scenario
    {
      protected:
        std::shared_ptr<State> m_cur_state = nullptr;
        std::list<std::shared_ptr<State>> m_states;

        /**
         * @brief Функция переключения состояния. Реализуется в каждом сценарии
         * своя. Он должен вызывать функции exit() и init() у состояний.
         *
         * @param event событие переключения
         */
        virtual void switcher(const Event &event) = 0;

        std::optional<std::shared_ptr<State>> findState(const std::string &name);

        void setStartState(const std::string &name);

      public:
        Scenario();
        virtual ~Scenario();

        /// @brief Добавить состояние в сценарий
        /// @param state само состояние
        void addState(std::shared_ptr<State> state);

        void update(const State::callbackParams &params);
    };
} // namespace SM

#endif // !LIBSTATE_HPP