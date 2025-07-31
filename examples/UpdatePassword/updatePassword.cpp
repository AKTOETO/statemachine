#include <iostream>
#include <libstate.hpp>

namespace States
{
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

    class RequestOldPassword : public SM::State
    {
      public:
        RequestOldPassword()
            : SM::State("RequestOldPassword"){};

        virtual SM::Events::Base init(const SM::callbackParams &prams) override
        {
            // request({{"password", ""}});
            // return {SM::Events::Type::Request, this, {{"password", ""}}};
            return SM::Events::Request{this, {{"password", ""}}};
        };

        virtual SM::Events::Base update(const SM::callbackParams &prams) override
        {
            // если пароля нет
            if (prams.at("password").length() == 0)
            {
                // needToSwitch({SM::Type::TryAgain, this});
                return SM::Events::None{this};
            }

            // если пароль есть, нужно его проверить
            // needToSwitch({SM::Type::GotPassword, this, prams});
            return {SM::Events::Type::None, this};
        }
    };

    class CheckPassword : public SM::State
    {
      public:
        CheckPassword()
            : SM::State("CheckPassword"){};

        virtual SM::Events::Base update(const SM::callbackParams &prams) override
        {
            request({{"password", ""}});
            return {SM::Events::Type::None, this};
        }
    };

} // namespace States

int main()
{
    return 0;
}