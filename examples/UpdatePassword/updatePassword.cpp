#include <iostream>
#include <libstate.hpp>

namespace States
{
    enum class CustomEvents
    {
        GotPassword,
        PasswordCorrect,
        PasswordIncorrect,
        RequestOldPassword,
        RequestNewPassword,
        SavePassword,
    };

    using MyState = SM::State<CustomEvents>;
    using MyEvent = SM::Events::Base<CustomEvents>;

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
    //         // SM::Base ev = {.m_type = SM::Type::TryAgain, .m_sender_state =
    //         this};

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

        // virtual MyEvent init(const SM::callbackParams &prams) override
        // {
        //     return SM::Events::Request{this, {{"password", ""}}};
        // };

        virtual MyEvent update(const SM::callbackParams &prams) override
        {
            // если пароля нет
            if (prams.at("password").length() == 0 ||
                prams.at("action") == "TryAgain")
                return SM::Events::Request{this,
                                           CustomEvents::RequestOldPassword};

            // если пароль есть, нужно его проверить
            return SM::Events::Switch{this, CustomEvents::GotPassword, prams};
        }
    };

    class CheckPassword : public MyState
    {
      public:
        CheckPassword()
            : MyState("CheckPassword"){};

        virtual MyEvent update(const SM::callbackParams &prams) override
        {
            if (isPassworCorrect(prams.at("password")))
                return SM::Events::Switch{this, CustomEvents::PasswordCorrect};
            return SM::Events::Switch{this, CustomEvents::PasswordIncorrect};
        }

      private:
        bool isPassworCorrect(const std::string &password)
        {
            return password == "123";
        }
    };

    class RequestNewPassword : public MyState
    {
      public:
        RequestNewPassword()
            : MyState("RequestNewPassword"){};

        virtual MyEvent update(const SM::callbackParams &prams) override
        {
            if (prams.at("password").length() == 0 ||
                prams.at("action") == "TryAgain")
                return SM::Events::Request{this,
                                           CustomEvents::RequestNewPassword};

            return SM::Events::Switch{this, CustomEvents::GotPassword, prams};
        }
    };

    class SavePassword : public MyState
    {
      public:
        SavePassword()
            : MyState("SavePassword"){};

        virtual MyEvent update(const SM::callbackParams &prams) override
        {
            if (prams.at("password").length() == 0 ||
                prams.at("action") == "TryAgain")
                return SM::Events::Request{this,
                                           CustomEvents::RequestNewPassword};

            // return SM::Events::Switch{this, CustomEvents::GotPassword,
            // prams};
            return SM::Events::Request{this, CustomEvents::SavePassword, prams};
        }
    };
} // namespace States

int main()
{
    States::RequestOldPassword ro;
    States::CheckPassword cp;
    States::RequestNewPassword rn;
    States::SavePassword sp;
    return 0;
}