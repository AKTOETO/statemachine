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

    // Структура описывает состояние в текущем сценарии
    class State
    {
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
         * @param params Параметры для функции инициализации текущего состояния
         */
        virtual void init(const callbackParams &params){};

        /// @brief Обновление состояния с учетом переданных данных
        /// @param params данные
        virtual void update(const callbackParams &params){};

        /// @brief Выход из текущего состояния
        /// @param params параметры для выхода
        virtual void exit(const callbackParams &params){};

        /**
         * @brief Установка функции уведомления о переключении
         *
         * @param switcher функция - переключатель
         */
        void setSwitcher(std::function<void(const Event &)> switcher);

        /// @brief Установить callback для выполнения запросов из состояния
        /// @param requester callback
        void setRequester(std::function<void(const callbackParams &params)> requester);

        /// @brief Получить имя состояния
        /// @return имя состояния
        const std::string &getName() const;

      protected:
        /**
         * @brief Переключение с событием. Нужно для наследованных состояний
         *
         * @param event Событие переключения
         */
        void needToSwitch(const Event &event);

        /// @brief Отправить запрос
        /// @param params данные запроса
        void request(const callbackParams &params);

      protected:
        std::string m_name;
        std::function<void(const Event &)> m_switcher;
        std::function<void(const callbackParams &params)> m_requester;
    };

    // Сценарий взаимодействия состояний
    class Scenario
    {
      protected:
        std::shared_ptr<State> m_cur_state = nullptr;
        std::list<std::shared_ptr<State>> m_states;
        std::function<void(const State::callbackParams &params)> m_requester;

        /**
         * @brief Функция переключения состояния. Реализуется в каждом сценарии
         * своя. Он должен вызывать функции exit() и init() у состояний.
         *
         * @param event событие переключения
         */
        virtual void switcher(const Event &event) = 0;

        /// @brief Найти загруженное состояние
        /// @param name имя состояния
        /// @return Если состояние есть `std::shared_ptr<State>`, иначе `std::nullopt`
        std::optional<std::shared_ptr<State>> findState(const std::string &name);

        /// @brief Установить загруженное состояние
        /// @param name имя состояния
        void setStartState(const std::string &name);

        /// @brief Отправить запрос
        /// @param params данные запроса
        void request(const State::callbackParams &params);

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