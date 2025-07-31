#include <iostream>
#include <libstate.hpp>

namespace States
{
    enum class CustomEvents
    {
        GotPassword
    };

    using MyState = SM::State<CustomEvents>;
    using MyBase = SM::Events::Base<CustomEvents>;

    // class TryAgain : public SM::State
    // {
    //   public:
    //     TryAgain()
    //         : SM::State("TryAgain"){};

    //     virtual SM::Base update(const SM::callbackParams &prams) override
    //     {
    //         std::cout << "Updating" << m_name << "\n";
    //         return {SM::Type::GotPassword, this};

    //         //needToSwitch(ev);
    //     };
    // };

    // class NewLogin : public SM::State
    // {
    //   public:
    //     NewLogin()
    //         : SM::State("NewLogin"){};

    //     virtual SM::Base update(const SM::callbackParams &prams) override
    //     {
    //         std::cout << "Updating" << m_name << "\n";
    //         // SM::Base ev = {.m_type = SM::Type::TryAgain, .m_sender_state = this};

    //         SM::callbackParams param = {{"password", "true"}};
    //         request(param);

    //         // needToSwitch(ev);
    //         return {SM::Type::None, this};
    //     };
    // };

    class RequestOldPassword : public MyState
    {
      public:
        RequestOldPassword()
            : MyState("RequestOldPassword"){};

        virtual MyBase init(const SM::callbackParams &prams) override
        {
            return SM::Events::Request{this, {{"password", ""}}};
        };

        virtual MyBase update(const SM::callbackParams &prams) override
        {
            // если пароля нет
            if (prams.at("password").length() == 0)
                return SM::Events::TryAgain{this};

            // если пароль есть, нужно его проверить
            // needToSwitch({SM::Type::GotPassword, this, prams});
            // return {SM::Events::Type::None, this};
            return MyBase{SM::Events::Type::Switch, this, CustomEvents::GotPassword};
        }
    };

    // class CheckPassword : public SM::State
    // {
    //   public:
    //     CheckPassword()
    //         : SM::State("CheckPassword"){};

    //     virtual SM::Events::Base update(const SM::callbackParams &prams) override
    //     {
    //         request({{"password", ""}});
    //         return {SM::Events::Type::None, this};
    //     }
    // };

} // namespace States

int main()
{
    return 0;
}