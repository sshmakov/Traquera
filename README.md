# Allrecall Traquera

Desktop client for any tracker (now has plugins for Atlassian JIRA and PVCS Tracker)

Универсальный "толстый" клиент для баг-трекеров. Сейчас реализованы плагины для работы с JIRA и PVCS Tracker 7.5

Для сборки используется Qt 4.8.7

## Содержимое
Основное приложение
- client      - Основное код приложения Allrecall Traquera
- common      - Общий код для приложения и плагинов
- tqplugapi - Основное API плагинов
- tqaxfactory	- Доработанные компоненты для работы с ActiveX
- tqcondapi	  - Редактор условий выборок
- tqgui	- Визуальные компоненты, доступные из плагинов

Плагины
- jira	  - Плагин для Atlassian Jira
- tracker	- Плагин для работы с PVCS Tracker
- trktool	- библиотеки API для PVCS Tracker
- msplans - Плагин для работы с планами Microsoft Project

Прочее
- install	  - Проект сборки инсталлятора через Inno Setup
- tqservice	- Проект web-сервиса ISAPI, реализующий унифициарованный доступ к трекерам
- trindex	- Утилита для выборки запросов из трекера
- tqtests	- Юнит-тесты
- doc         - Заготовка help-а
