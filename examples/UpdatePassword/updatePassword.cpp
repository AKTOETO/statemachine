#include <iostream>
#include <libstate.hpp>


void printMap(const std::map<std::string, std::string>& m) {
    std::cout<<std::endl;
    for (const auto& [key, value] : m) {
        std::cout << key << ": " << value << std::endl;
    }
    std::cout<<std::endl;
}


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
            printMap(prams);
            // если пароль есть, нужно его проверить
            if (prams.find("password") != prams.end() &&
                prams.at("password").length() > 0)
            {
                std::cout << "===> " << getName() << ": got password\n";

                return SM::Events::Switch{
                    this, Settings::CustomEvents::GotPassword, prams};
            }
            std::cout << "===> " << getName() << ": PasswordIsEmpty\n";

            // если пароля нет
            return SM::Events::Request{
                this, Settings::CustomEvents::PasswordIsEmpty};
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
            printMap(prams);
            if (prams.find("password") != prams.end() &&
                isPassworCorrect(prams.at("password")))
            {
                std::cout << "===> " << getName()
                          << ": PasswordIsCorrect\n";
                return SM::Events::Switch{
                    this, Settings::CustomEvents::PasswordIsCorrect};
            }

            std::cout << "===> " << getName() << ": PasswordIsIncorrect\n";
            return SM::Events::Switch{
                this, Settings::CustomEvents::PasswordIsIncorrect};
        }

      private:
        bool isPassworCorrect(const std::string &password)
        {

            std::cout << "===> " << getName() << ": is passwor Correct\n";
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
            printMap(prams);
            if (prams.find("password") != prams.end() &&
                prams.at("password").length() > 0)
            {
                std::cout << "===> " << getName() << ": GotPassword\n";
                return SM::Events::Switch{
                    this, Settings::CustomEvents::GotPassword, prams};
            }

            std::cout << "===> " << getName() << ": PasswordIsEmpty\n";

            return SM::Events::Request{
                this, Settings::CustomEvents::PasswordIsEmpty};
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
            printMap(prams);
            if (prams.find("password") != prams.end() &&
                prams.at("password").length() > 0)
            {
                std::cout << "===> " << getName()
                          << ": PasswordIsCorrect\n";
                return SM::Events::Request{
                    this, Settings::CustomEvents::PasswordIsCorrect,
                    prams};
            }
                std::cout << "===> " << getName()
                          << ": PasswordIsEmpty\n";

            return SM::Events::Request{
                this, Settings::CustomEvents::PasswordIsEmpty};
        }
    };
} // namespace States

class UpdatePassword : public Settings::MyScenario
{
  public:
    virtual Settings::MyEvent init(
        const SM::outsideParams &params) override
    {

        std::cout << "Добавление событий\n";
        // Добавляем состояния
        auto cp = addState<States::CheckPassword>();
        auto rnp = addState<States::RequestNewPassword>();
        auto rop = addState<States::RequestOldPassword>();
        auto sp = addState<States::SavePassword>();

        std::cout << "Добавление переходов\n";
        // Добавляем переходы между состояниями
        addTransfer(rop, cp, Settings::CustomEvents::GotPassword);
        addTransfer(rop, rop, Settings::CustomEvents::TryAgain);
        addTransfer(cp, rop, Settings::CustomEvents::PasswordIsIncorrect);
        addTransfer(cp, rnp, Settings::CustomEvents::PasswordIsCorrect);
        addTransfer(rnp, rnp, Settings::CustomEvents::TryAgain);
        addTransfer(rnp, sp, Settings::CustomEvents::GotPassword);
        addTransfer(sp, rnp, Settings::CustomEvents::PasswordIsEmpty);

        std::cout << "Установка граничных состояний\n";
        // Установка граничных состояний
        setStartState(rop);
        // setFinishState(sp);

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
    up.init({});
    up.update({});
    up.update({{"password", "456"}});
    up.update({});
    up.update({});
    up.update({});
    up.update({});


    return 0;
}