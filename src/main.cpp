// #include <algorithm>
// #include <functional>
// #include <iostream>
// #include <list>
// #include <map>
// #include <memory>
// #include <optional>
// #include <string>

#include <libstate.hpp>

#include <iostream>

////// EXAMPLE 1

// namespace States{
//     class TryAgain : public SM::State
//     {
//       public:
//         TryAgain()
//             : SM::State("TryAgain"){};

//         virtual void update(const callbackParams &prams) override
//         {
//             std::cout << "Updating" << m_name << "\n";
//             SM::Base ev {SM::Type::GotPassword, this};

//             needToSwitch(ev);
//         };
//     };

//     class NewLogin : public SM::State
//     {
//       public:
//         NewLogin()
//             : SM::State("NewLogin"){};

//         virtual void update(const callbackParams &prams) override
//         {
//             std::cout << "Updating" << m_name << "\n";
//             SM::Base ev {SM::Type::TryAgain, this};

//             SM::State::callbackParams param = {{"password", "true"}};
//             request(param);

//             needToSwitch(ev);
//         };
//     };

//     class RequestPassword : public SM::State
//     {
//       public:
//         RequestPassword()
//             : SM::State("RequestPassword"){};

//         virtual void update(const callbackParams &prams) override
//         {
//             //request()
//         }
//     };
// }
// class Scen : public SM::Scenario
// {
//   public:
//     Scen()
//     {
//         addState(std::make_shared<States::NewLogin>());
//         addState(std::make_shared<States::TryAgain>());
//         setStartState("NewLogin");
//     }

//   private:
//     virtual void switcher(const SM::Base &event) override
//     {
//         auto &sender = event.m_sender_state;
//         auto &type = event.m_type;

//         sender->exit({});
//         std::optional<std::shared_ptr<SM::State>> state;
//         // Логика переключения состояний
//         switch (type)
//         {
//         case SM::Type::TryAgain:
//             state = findState("TryAgain");
//             if (state)
//             {
//                 m_cur_state = *state;
//                 m_cur_state->init({});
//             }
//             else
//                 std::cout << "Unknown state: TryAgain\n";
//             break;
//         case SM::Type::GotPassword:
//             state = findState("NewLogin");
//             if (state)
//             {
//                 m_cur_state = *state;
//                 m_cur_state->init({});
//             }
//             else
//                 std::cout << "Unknown state: NewLogin\n";
//             break;
//         default:
//             std::cout << "Unknown type: " << (uint)type << '\n';
//         }
//     }
// };

int main()
{
    // Scen sc;
    // std::cout << "=== Hello"
    //           << "\n";
    // sc.update({});
    // sc.update({});
    // sc.update({});
    // sc.update({});
    // std::cout << "=== Hello"
    //           << "\n";
    // sc.update({});
    // std::cout << "=== Hello"
    //           << "\n";
    // sc.update({});
    // sc.update({});
    return 0;
}