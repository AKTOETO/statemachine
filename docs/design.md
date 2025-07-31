# Система состояний

Система позволяет задавать различные сценарии работы всей системы, в зависимости от определенных условий.

## Задачи

Нужно чтобы система могла взаимодействовать с Внешней средой. Чтобы кто-то мог закинуть какое-то событие внутрь и система сама решила чо делать. И чтобы система могла запросить ввод какой-то информации.

Возможно стоит реализовать callback для запроса информации из системы.
И предоставить метод для отправки информации из вне в систему (есть метод update(...), у него похожая задача).

## Составляющие

**Пример диаграммы компонентов**

```plantuml
' left to right direction

[External Entity] as external

package "System: Scenario State Management" {

  [Scenario Manager] as scenario_mgr

    package "Scenarios" {
        component "Scenario 1" as scenario1
        component "Scenario N" as scenarioN
    }

    package "States" {
        [State A] as stateA
        [State B] as stateB
        [State C] as stateC
    }


  scenario_mgr --> scenario1 : update(data)
  scenario1 ..> scenario_mgr: callback(data)
  scenario_mgr --> scenarioN : update(data)
  scenarioN ..> scenario_mgr: callback(data)

  scenario1 --> stateA : update(data)
  stateA ..> scenario1 : callback(data)
  scenario1 --> stateB : update(data)
  stateB ..> scenario1 : callback(data)
  scenarioN --> stateC : update(data)
  stateC ..> scenarioN : callback(data)

  ' External interaction
  external --> scenario_mgr : update(data)
  scenario_mgr ..> external : callback(data)
}


note left of external
  Может быть клиентом, UI, или другой системой.
  Общается с менеджером сценариев через:
  update(data) и callback(data)
end note

note top of scenario_mgr
    Управляет множественными сценариями.
    Получает запросы от внешней сущности.
    Отправляет ответы внешней сущности.
end note

```


Система состоит из нескольких компонентов:
-  `Scanario Manager` - Управляющий сценариями. Может переключать сценарии, передавать запросы извне внутрь сценариев, отвечать сущностям. Реализует перключение между состояниями по такому отношению: `(Указатель на состояние, Результат выполнения) -> Указатель на новое состояние`
- `Scenarios` - Множество сценариев, которые описывают какуюто логику переключения состояний. Также позволяют передавать различные данные в состояния и из них.
- `States` - Множество состояний. Они могут выполнять какие-то действия внутри себя. 


> **Пример сценария исопльзования:** состояние `TryAgain` может запросить пароль у внешей сущности через `callback(data)` и, после получения ответа от внешней сущности через `update(data)`, обработать полученный пароль каким-либо образом.

## Сценарии
### Обновление пароля

В этом сценарии будет следущющая диаграмма состояний:

**Версия с отдельным состоянием `TryAgain`**

```plantuml
left to right direction

state "ReqestOldPassword" as ROP
state "CheckPassword" as CP
state "RequestNewPassword" as RNP
state "SavePassword" as SP
state "TryAgain" as TA

[*] -> ROP
ROP -> CP : Пароль пришел
ROP --> TA : Пароля нет
TA -> ROP : Ввод старого пароля
CP -> TA : Неправильный пароль
CP -> RNP : Старый пароль правильный
RNP --> TA : Пароля нет
RNP --> SP : Новый пароль пришел
SP -> [*]
```

**Версия без `TryAgain`**

```plantuml
' left to right direction

state "ReqestOldPassword" as ROP
state "CheckPassword" as CP
state "RequestNewPassword" as RNP
state "SavePassword" as SP

[*] -> ROP
ROP ---> CP : Пароль пришел
ROP -> ROP : Пароля нет
CP ---> ROP : Неправильный пароль
CP --> RNP : Старый пароль правильный
RNP -> RNP: Пароля нет
RNP --> SP : Новый пароль пришел
SP -> [*]
```