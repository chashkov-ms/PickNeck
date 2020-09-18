# PicNeck2_Firmware
В проекте реализуется прошивка для второй версии датчика осанки.
Основной управляющий контроллер от фирмы Nordic Semiconductor - nRF52832.

## Комплектующие
1. nRF52832
2. LSM6DSL
3. MX25L4006E

## Структура проекта
1. config - файлы конфигурации nrf52832
2. doc - документация проекта
3. libraries - библиотеки используемых компонентов
4. modules - библиотека стандартных модулей nrf
6. nRF/Device - 
7. nRF/Drivers/cmsis - 
8. nRF/Drivers/cmsis/cmsis_config - 
8. nRF/log -
9. Output - папка с файлами прошивки и объектными файлами

## IDE и вспомогательное ПО
В данном проекте используется IDE - [Segger Embedded Studio for ARM.](ttps://www.segger.com/downloads/embedded-studio/)

Для отладки BLE используется мобильное приложение от Nordic - [nRF Connect.](https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp)

## Порядок запуска
Для работы с проектом необходио запустить файл PicNeck_v2.emProject

## Файл прошивки
Файл прошивки находится здесь: ./Output/Debug/Exe/PickNeck_v2.hex

## Версионность
Использутеся **major.minor.maintenance** версионность

*major* - крупные изменения, либо изменения несовместимые с предыдущей версией;

*minor* - добавление функционала без нарушения совместимости;

*maintenance* - исправления.
