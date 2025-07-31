#ifndef LIBSTATE_HPP
#define LIBSTATE_HPP

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <algorithm>
#include <iostream>

// Пространство имен библиотеки состояний
namespace SM
{
    // параметры функции обратного вызова для состояния
    using callbackParams = std::map<std::string, std::string>;
    // тип функции обратного вызова
    using callbackType = std::function<void(callbackParams)>;

    template <typename CustomData>
    class State;

    namespace Events
    {
        // Типы возможных событий
        enum class Type
        {
            None,
            Request,
            Switch,
            TryAgain
        };

        // Структура события, передаваемая при желании переключиться
        template <typename CustomData = void>
        struct Base
        {
            Type m_type;
            State<CustomData> *m_sender_state = nullptr;
            std::optional<CustomData> m_custom_data;
            callbackParams m_data;

            // Base();
            Base(Type type, State<CustomData> *state, const callbackParams &data = {})
                : m_type(type)
                , m_sender_state(state)
                , m_custom_data(std::nullopt)
                , m_data(data)
            {
            }

            Base(Type type, State<CustomData> *state, const CustomData &custom_data, const callbackParams &data = {})
                : m_type(type)
                , m_sender_state(state)
                , m_custom_data(custom_data)
                , m_data(data)
            {
            }
        };

        // Тип переключения на другое состояние
        template <typename CustomData = void>
        struct Switch : public Base<CustomData>
        {
            Switch(State<CustomData> *state, const callbackParams &data = {})
                : Base<CustomData>(Type::Switch, state, data)
            {
            }

            Switch(State<CustomData> *state, const CustomData &custom_data, const callbackParams &data = {})
                : Base<CustomData>(Type::Switch, state, data, custom_data)
            {
            }
        };

        // Тип запроса, который отправляется куда-то
        template <typename CustomData = void>
        struct Request : public Base<CustomData>
        {
            Request(State<CustomData> *state, const callbackParams &data = {})
                : Base<CustomData>(Type::Request, state, data)
            {
            }

            Request(State<CustomData> *state, const CustomData &custom_data, const callbackParams &data = {})
                : Base<CustomData>(Type::Request, state, data, custom_data)
            {
            }
        };

        // Перезапуск текущего состояния
        template <typename CustomData = void>
        struct TryAgain : public Base<CustomData>
        {
            TryAgain(State<CustomData> *state, const callbackParams &data = {})
                : Base<CustomData>(Type::TryAgain, state, data)
            {
            }

            TryAgain(State<CustomData> *state, const CustomData &custom_data, const callbackParams &data = {})
                : Base<CustomData>(Type::TryAgain, state, data, custom_data)
            {
            }
        };

        // Никакого события не произошло
        template <typename CustomData = void>
        struct None : public Base<CustomData>
        {
            None(State<CustomData> *state, const callbackParams &data = {})
                : Base<CustomData>(Type::None, state, data)
            {
            }

            None(State<CustomData> *state, const CustomData &custom_data, const callbackParams &data = {})
                : Base<CustomData>(Type::None, state, data, custom_data)
            {
            }
        };

    } // namespace Events

    // Структура описывает состояние в текущем сценарии
    template <typename CustomData = void>
    class State
    {
      public:
        State(const std::string &name)
            : m_name(name)
        {
        }

        virtual ~State()
        {
        }

        /**
         * @brief Функция запуска состояния. Вызывается при переходе в это
         * состояние.
         *
         * @param params Параметры для функции инициализации текущего состояния
         */
        virtual Events::Base<CustomData> init(const callbackParams &params)
        {
            return Events::Base<CustomData>{Events::Type::None, this};
        };

        /// @brief Обновление состояния с учетом переданных данных
        /// @param params данные
        virtual Events::Base<CustomData> update(const callbackParams &params)
        {
            return Events::Base<CustomData>{Events::Type::None, this};
        };

        /// @brief Выход из текущего состояния
        /// @param params параметры для выхода
        virtual Events::Base<CustomData> exit(const callbackParams &params)
        {
            return Events::Base<CustomData>{Events::Type::None, this};
        };

        /// @brief Получить имя состояния
        /// @return имя состояния
        const std::string &getName() const
        {
            return m_name;
        }

      protected:
        std::string m_name;
    };

    // Сценарий взаимодействия состояний
    template <typename CustomData = void>
    class Scenario
    {
      protected:
        std::shared_ptr<State<CustomData>> m_cur_state = nullptr;
        std::list<std::shared_ptr<State<CustomData>>> m_states;
        std::function<void(const callbackParams &params)> m_requester;

        /**
         * @brief Функция переключения состояния. Реализуется в каждом сценарии
         * своя. Он должен вызывать функции exit() и init() у состояний.
         *
         * @param event событие переключения
         */
        virtual void switcher(const Events::Base<CustomData> &event) = 0;

        /// @brief Найти загруженное состояние
        /// @param name имя состояния
        /// @return Если состояние есть `std::shared_ptr<State>`, иначе `std::nullopt`
        std::optional<std::shared_ptr<State<CustomData>>> findState(const std::string &name)
        {
            auto found = std::find_if(m_states.begin(), m_states.end(),
                                      [&name](const std::shared_ptr<State<CustomData>> &state)
                                      { return state->getName() == name; });
            return (found == m_states.end()) ? std::nullopt : std::make_optional(*found);
        }

        /// @brief Установить загруженное состояние
        /// @param name имя состояния
        void setStartState(const std::string &name)
        {
            auto state = findState(name);
            if (state)
            {
                m_cur_state = *state;
                transfer(m_cur_state->init({}));
            }
            else
                std::cout << "Unknown state: " << name << "\n";
        }

        // /// @brief Отправить запрос
        // /// @param params данные запроса
        // void request(const callbackParams &params);

        /// @brief Функция обработки переходов из одного состояния в другое
        /// @param event Событие перехода
        virtual void transfer(const Events::Base<CustomData> &event){};

      public:
        Scenario()
            : m_requester(nullptr)
            , m_cur_state(nullptr)
        {
        }
        virtual ~Scenario()
        {
            if (m_cur_state)
                transfer(m_cur_state->exit({}));
        }

        /// @brief Добавить состояние в сценарий
        /// @param state само состояние
        void addState(std::shared_ptr<State<CustomData>> state)
        {
            std::cout << "Adding state: " << state->getName() << '\n';
            for (auto &el : m_states)
            {
                if (el->getName() == state->getName())
                    return;
            }
            //state->setRequester([this](const callbackParams &params) { request(params); });
            m_states.push_back(state);
        }

        /// @brief Передать какие-то данные в текущее состояние
        /// @param params Данные для передачи в состояние
        void update(const callbackParams &params)
        {
            if (m_cur_state)
            {
                std::cout << "Updating state: " << m_cur_state->getName() << "\n";
                transfer(m_cur_state->update(params));
            }
        }
    };
} // namespace SM

#endif // !LIBSTATE_HPP