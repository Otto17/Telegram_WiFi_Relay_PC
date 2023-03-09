#pragma once        // Директива, для проверки, что бы заголовочный файл подключился строго 1 раз


/*** НАСТРОЙКИ ***/

//SSID и пароль для точки доступа AP
#define ssid_AP "WiFi_Relay_PC"
#define pswd_AP "123456789"

//Настройки WiFiManager
#define HOST_NAME "WiFi_Relay_PC"  // Задаём HOSTNAME, что знать имя ESP8266 в локальной сети
#define DARKMODE true              // true - Вкл. тёмную тему, false - Откл. тёмную тему (белая тема)
#define RECONNECT 10               // Кол-во повторных попыток подключения к WiFI. После исчерпания переходит в режим AP
#define WIFISERIALDEBUG false      // Если false - то отладка отключена, true - включена

//Настройка кнопки
#define SET_HOLD_TIMEOUT 8000  // Установить время удержания кнопки, мс (до 8 000). Для сброса настроек WiFiManager, форматирования FS и перезагрузка ESP8266 в режим AP

//Настройка пинов
#define LED 2             // GPIO2 Пин с LED на ESP8266 (Arduino IDE pin 2)
#define REL_SW 5          // GPIO5 Пин с Реле (Arduino IDE pin 5)
#define BUTTON_SW_RES 14  // GPIO14 Пин с кнопкой для включения реле и для сброса настроек (Arduino IDE pin 14)

//Время удержания реле в открытом состоянии (в миллисекундах)
#define DELAY_SW 500     // Эмитация короткого нажатия кнопки Power (Вкл./Выкл.)
#define DELAY_HARD 5010  // Эмитация долгого удержания кнопки Power

