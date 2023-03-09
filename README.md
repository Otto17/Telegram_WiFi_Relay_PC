# "Telegram_WiFi_Relay_PC" - Защищённая система управления ПК через Телеграм бота.


  **ОПИСАНИЕ ВОЗМОЖНОСТЕЙ:**
  Кнопка на реле:
  - ОДНО короткое нажатие по кнопке - Вкл. реле на заданное время в настройке "DELAY_SW", после чего выключает реле. (Короткое нажатие кнопки Power на ПК);
  - ТРИ коротких нажатия по кнопке - Вкл. реле на заданное время в настройке "DELAY_HARD", после чего выключает реле. (Долгое удержание кнопки Power на ПК);
  - ДОЛГОЕ удержание кнопки в течение 8 сек. - начинает быстро моргать LED на ESP8266, после отпускания кнопки отправляется сообщение в Телеграм о сбросе настроек
  и сбрасываются настройки WiFiManager, форматируется файловая система "LittleFS", ESP8266 перезагружается и переходит в режим AP для первичной настройки.
  
  **Телеграм:**
  - При подключени ESP8266 к интернету отправляется уведомление в чат бота, о том, что ESP8266 подключился к Телеграму и ссылкой на вызов меню кнопок;
  - Можно вызвать меню с 2-мя кнопками командой "/start".

  Кнопка в Телеграме: "Вкл./Выкл. ПК" подаёт напряжение на реле на заданное время в настройке "DELAY_SW", после чего выключает реле. (Короткое нажатие кнопки Power на ПК).
  Кнопка в Телеграме: ""Hard Off ПК" подаёт напряжение на реле на заданное время в настройке "DELAY_HARD", после чего выключает реле. (Долгое удержание кнопки Power на ПК).

  - Если отправить *.bin файл прошивки с любым названием, сначала проходит проверка файла, в случае корректного считывания и достаточного места во Flash происходит
  автопрошивка ESP8266 и уведомление об успешной прошивке "OK" после чего ESP8266 перезагружается.
  Если прошивка не подходит или весит больше, чем доступно во Flash, то выводится сообщение "Error" и ничего больше не происходит.

******************************************

  **ПОРЯДОК ПОДКЛЮЧЕНИЯ:**
  В сброшенном состоянии ESP8266 работает как точка доступа (AP)!

  1) В Телеграме создаём бота через "BotFather - (@BotFather)" и ID чата через "IDBot - (@myidbot)".
  ID чата нужен для защиты, если кто либо, кроме владельца добавит бота себе и попытается отправлять ему команды - то ничего происходить не будет, другой пользователь будет игнорироваться.

  2) Включаем "WiFi Relay PC", подключаемся с телефона к AP сети "Telegram_WiFi_Relay_PC", пароль "123456789", жмём на всплывающее уведомление, открывается страница с настройками "WiFiManager".
  Если уведомления не было (зависит от телефона), то открываем в браузере адрес "192.168.4.1", жмём кнопку "Configure WiFi", выбираем из списка имя сети (SSID) роутера и вводим пароль от него.
  Так же копируем с Телеграма ТОКЕН и ID и вставляем в поля ниже, сохраняем. ESP8266 перезагрузится с принятыми параметрами.

  3) Если всё верно введено, то через секунд 15 в чат бота придёт уведомление, о том, что "WiFi Relay PC" в сети!

******************************************

  Автор: Otto
  Версия изменения: 2.0
  Создан: 25 Августа 2022 года.
