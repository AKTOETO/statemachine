#include <iostream>
#include <libstate.hpp>

namespace Settings
{
    enum class CustomNames
    {
    };

    enum class CustomEvents : short
    {
        GotPassword,
        PasswordIsCorrect,
        PasswordIsIncorrect,
        PasswordIsEmpty,
        SavePassword,
        TryAgain,
    };

    using MyState = SM::State<CustomEvents>;
    using MyEvent = SM::Events::Base<CustomEvents>;
    using MyScenario = SM::Scenario<CustomEvents>;

} // namespace Settings

namespace States
{
    class RequestOldPassword : public Settings::MyState
    {
      public:
        RequestOldPassword()
            : Settings::MyState("RequestOldPassword"){};

        virtual Settings::MyEvent update(
            const SM::outsideParams &prams) override
        {
            // если пароля нет
            if (prams.at("password").length() == 0 ||
                prams.at("action") == "TryAgain")
                return SM::Events::Request{
                    this, Settings::CustomEvents::TryAgain};

            // если пароль есть, нужно его проверить
            return SM::Events::Switch{
                this, Settings::CustomEvents::GotPassword, prams};
        }
    };

    class CheckPassword : public Settings::MyState
    {
      public:
        CheckPassword()
            : Settings::MyState("CheckPassword"){};

        virtual Settings::MyEvent update(
            const SM::outsideParams &prams) override
        {
            if (isPassworCorrect(prams.at("password")))
                return SM::Events::Switch{
                    this, Settings::CustomEvents::PasswordIsCorrect};

            return SM::Events::Switch{
                this, Settings::CustomEvents::PasswordIsIncorrect};
        }

      private:
        bool isPassworCorrect(const std::string &password)
        {
            return password == "123";
        }
    };

    class RequestNewPassword : public Settings::MyState
    {
      public:
        RequestNewPassword()
            : Settings::MyState("RequestNewPassword"){};

        virtual Settings::MyEvent update(
            const SM::outsideParams &prams) override
        {
            if (prams.at("password").length() == 0 ||
                prams.at("action") == "TryAgain")
                return SM::Events::Request{
                    this, Settings::CustomEvents::TryAgain};

            return SM::Events::Switch{
                this, Settings::CustomEvents::GotPassword, prams};
        }
    };

    class SavePassword : public Settings::MyState
    {
      public:
        SavePassword()
            : Settings::MyState("SavePassword"){};

        virtual Settings::MyEvent update(
            const SM::outsideParams &prams) override
        {
            if (prams.at("password").length() == 0 ||
                prams.at("action") == "TryAgain")
                return SM::Events::Request{
                    this, Settings::CustomEvents::PasswordIsEmpty};

            return SM::Events::Request{
                this, Settings::CustomEvents::PasswordIsCorrect, prams};
        }
    };
} // namespace States

class UpdatePassword : public Settings::MyScenario
{
  public:
    virtual Settings::MyEvent init(
        const SM::outsideParams &params) override
    {
        // Добавляем состояния
        auto cp = addState<States::CheckPassword>();
        auto rnp = addState<States::RequestNewPassword>();
        auto rop = addState<States::RequestOldPassword>();
        auto sp = addState<States::SavePassword>();

        // Добавляем переходы между состояниями
        addTransfer(rop, cp, Settings::CustomEvents::GotPassword);
        addTransfer(rop, rop, Settings::CustomEvents::TryAgain);
        addTransfer(cp, rop, Settings::CustomEvents::PasswordIsIncorrect);
        addTransfer(cp, rnp, Settings::CustomEvents::PasswordIsCorrect);
        addTransfer(rnp, rnp, Settings::CustomEvents::TryAgain);
        addTransfer(rnp, sp, Settings::CustomEvents::GotPassword);
        addTransfer(sp, rnp, Settings::CustomEvents::PasswordIsEmpty);

        // Установка граничных состояний
        setStartState(rop);
        // setFinishState(sp);

        return Settings::MyEvent(SM::Events::Type::None);
    }

    virtual Settings::MyEvent update(
        const SM::outsideParams &params) override
    {
        // if (m_cur_state)
        // {
        //     std::cout << "Updating state: " <<
        //     m_cur_state->getName()
        //               << "\n";
        //     transfer(m_cur_state->update(params));
        // }
        return Settings::MyEvent(SM::Events::Type::None);
    }
};

int main()
{
    // States::RequestOldPassword ro;
    // States::CheckPassword cp;
    // States::RequestNewPassword rn;
    // States::SavePassword sp;
    UpdatePassword up;

    return 0;
}