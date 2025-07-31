#include <iostream>
#include <libstate.hpp>

namespace States
{
    class TryAgain : public SM::State
    {
      public:
        TryAgain()
            : SM::State("TryAgain"){};

        virtual void update(const callbackParams &prams) override
        {
            std::cout << "Updating" << m_name << "\n";
            SM::Event ev(SM::Event::Type::GotPassword, this);

            needToSwitch(ev);
        };
    };

    class NewLogin : public SM::State
    {
      public:
        NewLogin()
            : SM::State("NewLogin"){};

        virtual void update(const callbackParams &prams) override
        {
            std::cout << "Updating" << m_name << "\n";
            // SM::Event ev = {.m_type = SM::Event::Type::TryAgain, .m_sender_state = this};

            SM::State::callbackParams param = {{"password", "true"}};
            request(param);

            // needToSwitch(ev);
        };
    };

    class RequestOldPassword : public SM::State
    {
      public:
        RequestOldPassword()
            : SM::State("RequestOldPassword"){};

        virtual void init(const callbackParams &prams) override
        {
            request({{"password", ""}});
        };

        virtual void update(const callbackParams &prams) override
        {
            // если пароля нет
            if (prams.at("password").length() == 0)
            {
                // TODO: можно возвращать тип события, от которого
                // потом будет отталкиваться сценария при выборе следующего
                // состояния. 
                needToSwitch({SM::Event::Type::TryAgain, this});
                return;
            }

            // если пароль есть, нужно его проверить
            needToSwitch({SM::Event::Type::GotPassword, this, prams});
        }
    };

    class CheckPassword : public SM::State
    {
      public:
        CheckPassword()
            : SM::State("CheckPassword"){};

        virtual void update(const callbackParams &prams) override
        {
            request({{"password", ""}});
        }
    };

} // namespace States

int main()
{
    return 0;
}