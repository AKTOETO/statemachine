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
    // параметры функции обратного вызова для состояния
    using callbackParams = std::map<std::string, std::string>;
    // тип функции обратного вызова
    using callbackType = std::function<void(callbackParams)>;

    class State;

    namespace Events
    {
        // Типы возможных событий
        enum class Type
        {
            None,
            Request,
            Switch,

            TryAgain,
            GotPassword
        };

        // Структура события, передаваемая при желании переключиться
        struct Base
        {
            Type m_type;
            State *m_sender_state = nullptr;
            callbackParams m_data;

            Base(){};
            Base(Type type, State *state, const callbackParams &data = {});
        };

        // Тип переключения на другое состояние
        struct Switch : public Base
        {
            Switch(State *state, const callbackParams &data = {});
        };

        struct Request : public Base
        {
            Request(State *state, const callbackParams &data = {});
        };

        // Никакого события не произошло
        struct None : public Base
        {
            None(State *state, const callbackParams &data = {});
        };

    } // namespace Events

    // Структура описывает состояние в текущем сценарии
    class State
    {
      public:
        State(const std::string &name);
        virtual ~State();

        /**
         * @brief Функция запуска состояния. Вызывается при переходе в это
         * состояние.
         *
         * @param params Параметры для функции инициализации текущего состояния
         */
        virtual Events::Base init(const callbackParams &params)
        {
            return Events::Base{Events::Type::None, this};
        };

        /// @brief Обновление состояния с учетом переданных данных
        /// @param params данные
        virtual Events::Base update(const callbackParams &params)
        {
            return Events::Base{Events::Type::None, this};
        };

        /// @brief Выход из текущего состояния
        /// @param params параметры для выхода
        virtual Events::Base exit(const callbackParams &params)
        {
            return Events::Base{Events::Type::None, this};
        };

        /// @brief Установить callback для выполнения запросов из состояния
        /// @param requester callback
        void setRequester(std::function<void(const callbackParams &params)> requester);

        /// @brief Получить имя состояния
        /// @return имя состояния
        const std::string &getName() const;

      protected:
        /// @brief Отправить запрос
        /// @param params данные запроса
        void request(const callbackParams &params);

      protected:
        std::string m_name;
        std::function<void(const callbackParams &params)> m_requester;
    };

    // Сценарий взаимодействия состояний
    class Scenario
    {
      protected:
        std::shared_ptr<State> m_cur_state = nullptr;
        std::list<std::shared_ptr<State>> m_states;
        std::function<void(const callbackParams &params)> m_requester;

        /**
         * @brief Функция переключения состояния. Реализуется в каждом сценарии
         * своя. Он должен вызывать функции exit() и init() у состояний.
         *
         * @param event событие переключения
         */
        virtual void switcher(const Events::Base &event) = 0;

        /// @brief Найти загруженное состояние
        /// @param name имя состояния
        /// @return Если состояние есть `std::shared_ptr<State>`, иначе `std::nullopt`
        std::optional<std::shared_ptr<State>> findState(const std::string &name);

        /// @brief Установить загруженное состояние
        /// @param name имя состояния
        void setStartState(const std::string &name);

        /// @brief Отправить запрос
        /// @param params данные запроса
        void request(const callbackParams &params);

        /// @brief Функция обработки переходов из одного состояния в другое
        /// @param event Событие перехода
        virtual void transfer(const Events::Base &event){};

      public:
        Scenario();
        virtual ~Scenario();

        /// @brief Добавить состояние в сценарий
        /// @param state само состояние
        void addState(std::shared_ptr<State> state);

        void update(const callbackParams &params);
    };
} // namespace SM

#endif // !LIBSTATE_HPP