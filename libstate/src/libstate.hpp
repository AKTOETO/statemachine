#ifndef LIBSTATE_HPP
#define LIBSTATE_HPP

#include <algorithm>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <unordered_map>

// Пространство имен библиотеки состояний
namespace SM
{
    // параметры полученные извне
    using outsideParams = std::map<std::string, std::string>;

    template <typename CustomEvents>
    class State;

    namespace Events
    {
        // Стандартные события
        enum class Type
        {
            None,    // Ничего не произошло
            Request, // Передача данных "наружу"
            Switch,  // Переключение состояния
            TryAgain, // Перезапуск текущего состояния,
            Finish, // Конец цепочки переключения состояний
        };

        // Структура события, передаваемая при желании переключиться
        template <typename CustomEvents = void>
        struct Base
        {
            Type m_type;
            std::optional<CustomEvents> m_custom_data;
            State<CustomEvents> *m_sender_state = nullptr;
            outsideParams m_data;

            // Base();
            Base(Type type, State<CustomEvents> *state = nullptr,
                 const outsideParams &data = {})
                : m_type(type)
                , m_sender_state(state)
                , m_custom_data(std::nullopt)
                , m_data(data)
            {
            }

            Base(Type type, State<CustomEvents> *state,
                 const CustomEvents &custom_event,
                 const outsideParams &data = {})
                : m_type(type)
                , m_sender_state(state)
                , m_custom_data(custom_event)
                , m_data(data)
            {
            }
            bool operator==(const Base &other) const
            {
                return m_type == other.m_type &&
                       m_custom_data == other.m_custom_data &&
                       m_sender_state->getName() ==
                           other.m_sender_state->getName();
            }
        };

        // Тип переключения на другое состояние
        template <typename CustomEvents = void>
        struct Switch : public Base<CustomEvents>
        {
            Switch(State<CustomEvents> *state,
                   const outsideParams &data = {})
                : Base<CustomEvents>(Type::Switch, state, data)
            {
            }

            Switch(State<CustomEvents> *state,
                   const CustomEvents &custom_event,
                   const outsideParams &data = {})
                : Base<CustomEvents>(Type::Switch, state, custom_event,
                                     data)
            {
            }
        };

        // Тип запроса, который отправляется куда-то
        template <typename CustomEvents = void>
        struct Request : public Base<CustomEvents>
        {
            Request(State<CustomEvents> *state,
                    const outsideParams &data = {})
                : Base<CustomEvents>(Type::Request, state, data)
            {
            }

            Request(State<CustomEvents> *state,
                    const CustomEvents &custom_event,
                    const outsideParams &data = {})
                : Base<CustomEvents>(Type::Request, state, custom_event,
                                     data)
            {
            }
        };

        // Перезапуск текущего состояния
        template <typename CustomEvents = void>
        struct TryAgain : public Base<CustomEvents>
        {
            TryAgain(State<CustomEvents> *state,
                     const outsideParams &data = {})
                : Base<CustomEvents>(Type::TryAgain, state, data)
            {
            }

            TryAgain(State<CustomEvents> *state,
                     const CustomEvents &custom_event,
                     const outsideParams &data = {})
                : Base<CustomEvents>(Type::TryAgain, state, custom_event,
                                     data)
            {
            }
        };

        // Конец вветки переключения состояний
        template <typename CustomEvents = void>
        struct Finish : public Base<CustomEvents>
        {
            Finish(State<CustomEvents> *state,
                   const outsideParams &data = {})
                : Base<CustomEvents>(Type::Finish, state, data)
            {
            }

            Finish(State<CustomEvents> *state,
                   const CustomEvents &custom_event,
                   const outsideParams &data = {})
                : Base<CustomEvents>(Type::Finish, state, custom_event,
                                     data)
            {
            }
        };

        // Никакого события не произошло
        template <typename CustomEvents = void>
        struct None : public Base<CustomEvents>
        {
            None(State<CustomEvents> *state,
                 const outsideParams &data = {})
                : Base<CustomEvents>(Type::None, state, data)
            {
            }

            None(State<CustomEvents> *state,
                 const CustomEvents &custom_event,
                 const outsideParams &data = {})
                : Base<CustomEvents>(Type::None, state, custom_event, data)
            {
            }
        };

    } // namespace Events

    // Структура описывает состояние в текущем сценарии
    template <typename CustomEvents = void>
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
         * @brief Функция запуска состояния. Вызывается при переходе в
         * это состояние.
         *
         * @param params Параметры для функции инициализации текущего
         * состояния
         */
        virtual Events::Base<CustomEvents> init(
            const outsideParams &params)
        {
            return Events::Base<CustomEvents>{Events::Type::None, this};
        };

        /// @brief Обновление состояния с учетом переданных данных
        /// @param params данные
        virtual Events::Base<CustomEvents> update(
            const outsideParams &params)
        {
            return Events::Base<CustomEvents>{Events::Type::None, this};
        };

        /// @brief Выход из текущего состояния
        /// @param params параметры для выхода
        virtual Events::Base<CustomEvents> exit(
            const outsideParams &params)
        {
            return Events::Base<CustomEvents>{Events::Type::None, this};
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
    template <typename CustomEvents = void>
    class Scenario
    {
      protected:
        // Список зарегистрированных состояний
        std::map<std::string, std::unique_ptr<State<CustomEvents>>>
            m_states;

        // таблица переходов
        // std::map<Events::Base<CustomEvents>, State<CustomEvents> *>
        //     m_transfers;
        std::map<std::pair<State<CustomEvents> *, CustomEvents>,
                 State<CustomEvents> *>
            m_transfers;

        // Текущее состояние
        State<CustomEvents> *m_cur_state;

        /// @brief Установить загруженное состояние
        /// @param name имя состояния
        void setStartState(const std::string &name)
        {
            auto it = m_states.find(name);
            if (it != m_states.end())
            {
                m_cur_state = it->second.get();
                // transfer(m_cur_state->init({}));
            }
            else
                std::cout << "Unknown state: " << name << "\n";
        }

        void setStartState(State<CustomEvents> *state)
        {
            if (!state)
                return;
            auto it = m_states.find(state->getName());
            if (it != m_states.end())
            {
                m_cur_state = it->second.get();
                // transfer(m_cur_state->init({}));
            }
            else
                std::cout << "Unknown state: " << state->getName() << "\n";
        }

        // /// @brief Отправить запрос
        // /// @param params данные запроса
        // void request(const outsideParams &params);

        /// @brief Функция обработки переходов из одного состояния в
        /// другое. Он должен вызывать функции exit() и init() у
        /// состояний.
        /// @param event Событие перехода
        virtual void transfer(const Events::Base<CustomEvents> &event){};

        /// @brief Добавить в сценарий новое состояние
        /// @tparam DerivedState Тип нового состояния, унаследованного
        /// от State
        /// @tparam ...Args Типы параметров конструктора
        /// @param ...args Параметры конструктора нового состояния
        template <typename DerivedState, typename... Args>
        DerivedState *addState(Args &&...args)
        {
            auto state = std::make_unique<DerivedState>(
                std::forward<Args>(args)...);
            const std::string &name = state->getName();

            if (m_states.count(name) > 0)
            {
                std::cout << "Cannot add state (" << state->getName()
                          << "): state already exists\n";
                return nullptr;
            }

            m_states[state->getName()] = std::move(state);
            std::cout << "State (" << state->getName() << ") added\n";
            return state.get();
        }

        /// @brief Добавить переход first_state -> second_state :
        /// custom_event
        /// @tparam FirstDerivedState Тип первого состояния
        /// @tparam SecondDerivedState Тип второго состояния
        /// @param first_state Первое состояние
        /// @param second_state Второе состояние
        /// @param custom_event Условие перехода
        /// @return Удалось ли добавить состояние
        template <typename FirstDerivedState = State<CustomEvents>,
                  typename SecondDerivedState = State<CustomEvents>>
        bool addTransfer(FirstDerivedState *first_state,
                         SecondDerivedState *second_state,
                         const CustomEvents &custom_event)
        {
            m_transfers[std::make_pair(first_state, custom_event)] =
                second_state;
            std::cout << "Added state transfer (" << first_state->getName()
                      << ") -" << (short)custom_event << "-> ("
                      << second_state->getName() << ")\n";
            return true;
        }

        State<CustomEvents> *getState(const std::string &name)
        {
            auto it = m_states.find(name);
            if (it != m_states.end())
                return it->second.get();
            return nullptr;
        }

      public:
        Scenario()
            : m_cur_state(nullptr)
        {
        }

        virtual ~Scenario()
        {
            // if (m_cur_state)
            //     transfer(m_cur_state->exit({}));
        }

        virtual Events::Base<CustomEvents> init(
            const outsideParams &params)
        {
            return Events::Base<CustomEvents>(Events::Type::None);
        };

        /// @brief Передать какие-то данные в текущее состояние
        /// @param params Данные для передачи в состояние
        virtual Events::Base<CustomEvents> update(
            const outsideParams &params)
        {
            // if (m_cur_state)
            // {
            //     std::cout << "Updating state: " <<
            //     m_cur_state->getName()
            //               << "\n";
            //     transfer(m_cur_state->update(params));
            // }
            return Events::Base<CustomEvents>(Events::Type::None);
        }
    };
} // namespace SM

#endif // !LIBSTATE_HPP