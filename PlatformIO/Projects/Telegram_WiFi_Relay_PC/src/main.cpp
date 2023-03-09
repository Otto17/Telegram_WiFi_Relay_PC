#include "Lib_All.h"    // Подключаем все библиотеки

//Объекты
WiFiManager wm;                             // Глобальная инициализация объекта "wm" класса "WiFiManager"
EncButton<EB_TICK, BUTTON_SW_RES > ButtSw;  // Создаём объект "ButtSw" класса "EncButton", "BUTTON_SW_RES" - это имя дерективы кнопки
FastBot* bot;                               // Создаём указатель для объекта телеграма



//Переменные
bool shouldSaveConfig = false;  // Флаг для сохранения данных (ТОКЕН/ID для Телеграма) из WiFiManager
bool flag = false;              // Флаг для быстрого мигания LED перед отпусканием кнопки после долгого удержания и последующим сбросом настроек WiFiManager и форматированием FS

char BOT_TOKEN[47] = "";  // Отсюда берётся ТОКЕН для Телеграма, тут менять ничего НЕ нужно.
char CHAT_ID[11] = "";    // Отсюда берётся ID для Телеграма, тут менять ничего НЕ нужно.

long tokenNum = 0, idNum = 0;  // Переменные для считывания байт и последющей конвертации символов из FS

//Переменные для считывания введённых данных из WiFiManager, последующей записи в конфиги FS и конвертации в char для "BOT_TOKEN" и "CHAT_ID"
String textToken;
String textID;



/*** ЦИКЛЫ ***/

//Функция Вкл./Выкл. PC (имитация короткого нажатия кнопки Power на PC)
void pc() {
  digitalWrite(LED, LOW);      // Подаём питание на LED
  digitalWrite(REL_SW, HIGH);  // Подаём питание на реле
  delay(DELAY_SW);             // Ждём указанное кол-во миллисекунд перед выключением
  digitalWrite(LED, HIGH);     // Убераем питание с LED
  digitalWrite(REL_SW, LOW);   // Убераем питание с реле
}


//Функция принудительного Выкл. PC. Необходима для принудительной перезагрузки PC в случае его зависания (имитация долгого нажатия кнопки Power на PC)
void pc_hard() {
  digitalWrite(LED, LOW);      // Подаём питание на LED
  digitalWrite(REL_SW, HIGH);  // Подаём питание на реле
  delay(DELAY_HARD);           // Ждём указанное кол-во миллисекунд перед выключением
  digitalWrite(LED, HIGH);     // Подаём питание на LED
  digitalWrite(REL_SW, LOW);   // Убераем питание с реле
}


//Обработчик сообщений Телеграма
void newMsg(FB_msg& msg) {
  if (msg.OTA) bot->update();  // Если прислали *.bin файл с любым именем в чат бота, то проверяем файл и успешно обновляем или выдаём ошибку

  if (msg.data == "Relay_switch") {             // Если получили с кнопки чата бота данные с id "Relay_switch", то...
    pc();                                       // Вызываем функцию с коротким Вкл. и Выкл. реле
    bot->answer("Реле сработало ✓", FB_NOTIF);  // Показываем в чате бота всплывающее уведомление
  }

  if (msg.data == "Hard_Relay") {           // Если получили с кнопки чата бота данные с id "Hard_Relay", то...
    pc_hard();                              // Вызываем функцию с долгим Вкл. и Выкл. реле
    bot->answer("ПК Выключен!", FB_NOTIF);  // Показываем в чате бота всплывающее уведомление
  }

  if (msg.text == "/start") {                                        // Если отправляем с чата бота команду "/start", то получаем в ответ меню с кнопками
    String choice_menu = F("Вкл./Выкл. ПК \t Hard Off ПК");          // Названия кнопок меню в Телеграме
    String back_call = F("Relay_switch, Hard_Relay");                // id для обратного вызова к кнопкам меню (присваиваем по порядку слева направо id к названиям кнопок)
    bot->inlineMenuCallback("Управление:", choice_menu, back_call);  // Выводим инлайн меню с обратным вызовом и поддержкой кнопок-ссылок
  }
}



//Функция смены флага для сохранения Токен/ID через WiFiManager
void saveConfigCallback() {
  shouldSaveConfig = true;  // Меняем флаг на ИСТИНУ
}


//Функция обработчика кнопки
void controlButton() {
  //Одинарное нажатие кнопки "BUTTON_SW_RES" (Вкл. реле на "DELAY_SW" миллисекунд и потом выключение реле). Эмитация короткого нажатия кнопки Power на ПК
  if (ButtSw.hasClicks(1)) {
    //Serial.print("Button Clicked 1 time");
    pc();  // Кратковременное нажатие и отпускание
  }

  // Тройное нажатие кнопки "BUTTON_SW_RES" (Вкл. реле на "DELAY_HARD" миллисекунд и потом выключение реле). Эмитация долгого удержания кнопки Power на ПК
  if (ButtSw.hasClicks(3)) {
    //Serial.print("Button Clicked 3 time");
    pc_hard();  // Долгое нажатие и отпускание
  }

  // Долгое удержание кнопки "BUTTON_SW_RES" (Сброс настроек WiFiManager и перезагрузка ESP8266 в режим AP)
  if (ButtSw.step()) {  // Если кнопка удерживается заданное время в " ButtSw.setHoldTimeout()", то быстро мыргаем LED
    //Serial.print("Button Long hold");
    flag = true;  // Поднимаем флаг
  }

  if (ButtSw.releaseStep()) {  // Как только отпустили кнопку после долгого удержания
    //Serial.print("Button released");
    LittleFS.format();                                          // Форматируем файловую систему (тем самым стираем конфиги с ТОКЕНОМ и ID Телеграма)
    flag = false;                                               // Опускаем флаг
    bot->sendMessage("Настройки \"WiFi Relay PC\" сброшены!");  // Отправим сообщение в чат о появлении ESP8266 в сети
    delay(300);                                                 // Ждём корректного стирания
    wm.resetSettings();                                         // Стираем настройки WiFiManager
    delay(200);                                                 // Ждём корректного стирания
    ESP.restart();                                              // Перезагружаем ESP8266
    delay(2000);                                                // Ждём для корректной перезагрузки
  }
}

//Функция загрузки из файловой системы ТОКЕН бота (BOT_TOKEN)
void getBotToken() {
  File file_Token = LittleFS.open("/confToken.conf", "r");  // Открываем файл "confToken.conf" для чтения и считываем его в переменную "file_Token"
  if (!file_Token) {                                        // Если не смогли прочитать файл, то сообщаем об ошибке
    //Serial.println("Failed to open file \"confToken.conf\" for reading");
    return;
  }

  byte nb = 0;                       // Счётчик для составления строки из символом (в данном случаем умещаемся с большим запасом в тип byte)
  while (file_Token.available()) {   // Получаем байты (символы) с буфера "file_Token" по кругу, пока они не закончатся
    tokenNum = file_Token.read();    // Считываем байты в переменную "tokenNum"
    BOT_TOKEN[nb] = (char)tokenNum;  // Конвертируем считанные байты в символьный тип char по счётчику "nb"
    nb += 1;                         // Прибавляем +1 к счётчику
  }
  file_Token.close();  // Закрываем файл "confToken.conf"
}


//Функция загрузки из файловой системы ID чата (CHAT_ID)
void getChatID() {
  File file_ID = LittleFS.open("/confID.conf", "r");  // Открываем файл "confID.conf" для чтения и считываем его в переменную "file_ID"
  if (!file_ID) {                                     // Если не смогли прочитать файл, то сообщаем об ошибке
                                                      // Serial.println("Failed to open file \"confID.conf\" for reading");
    return;
  }

  byte nb = 0;                   // Счётчик для составления строки из символом (в данном случаем умещаемся с большим запасом в тип byte)
  while (file_ID.available()) {  // Получаем байты (символы) с буфера "file_ID" по кругу, пока они не закончатся
    idNum = file_ID.read();      // Считываем байты в переменную "idNum"
    CHAT_ID[nb] = (char)idNum;   // Конвертируем считанные байты в символьный тип char по счётчику "nb"
    nb += 1;                     // Прибавляем +1 к счётчику
  }
  file_ID.close();  // Закрываем файл "file_ID.conf"
}


//Функция сохранения в файловую систему ТОКЕНА бота (BOT_TOKEN)
void putBotToken() {
  LittleFS.format();  // Сначала форматируем файловую систему (тем самым стираем старые конфиги с ТОКЕНОМ и ID Телеграма)
  delay(300);         // Ждём корректного стирания

  File file_Token = LittleFS.open("/confToken.conf", "w");  // Открываем файл "confToken.conf" для записи и будем писать в него строку с переменной "file_Token"
  if (file_Token) {                                         // Если файл успешно открыт, то...
    file_Token.print(textToken);                            // Записываем содержимое строки "textToken" в открытый файл "confToken.conf"
  }
  file_Token.close();  // Закрываем файл "confToken.conf"

  byte len = textToken.length() + 1;      // Считываем в переменную "len" кол-во символов с строки "textToken" и ещё прибавляем +1 символ
  textToken.toCharArray(BOT_TOKEN, len);  // Конвертируем строку "textToken" с длиной "len" в массив символов и записываем его в переменную "BOT_TOKEN"
}


//Функция сохранения в файловую систему ID чата (CHAT_ID)
void putChatID() {
  File file_ID = LittleFS.open("/confID.conf", "w");  // Открываем файл "confID.conf" для записи и будем писать в него строку с переменной "file_ID"
  if (file_ID) {                                      // Если файл успешно открыт, то...
    file_ID.print(textID);                            // Записываем содержимое строки "textID" в открытый файл "confID.conf"
  }
  file_ID.close();  // Закрываем файл "confID.conf"

  byte len = textID.length() + 1;    // Считываем в переменную "len" кол-во символов с строки "textID" и ещё прибавляем +1 символ
  textID.toCharArray(CHAT_ID, len);  // Конвертируем строку "textID" с длиной "len" в массив символов и записываем его в переменную "CHAT_ID"
}


void setup() {
  /*
  delay(2000);           // Ждём 2 сек. что бы успеть открыть Монитор порта
  Serial.begin(115200);  // Инициализируем Serial
  Serial.println();
*/

  //Конфигурация пинов
  pinMode(LED, OUTPUT);                  // Назначаем светодиод как ВЫХОД
  pinMode(REL_SW, OUTPUT);               // Назначаем реле как ВЫХОД
  pinMode(BUTTON_SW_RES, INPUT_PULLUP);  // Назначаем кнопку как ВХОД (с подтяжкой через внутренний резистор)

  digitalWrite(LED, HIGH);    // По умолчанию светодиод выключен (инверсия)
  digitalWrite(REL_SW, LOW);  // По умолчанию реле выключено (инверсия)


  //Кнопка
  ButtSw.setHoldTimeout(SET_HOLD_TIMEOUT);  // Устанавливаем время удержания кнопки


  //Файловая система
  LittleFS.begin();  // Инициализируем файловую систему "LittleFS"
  //Serial.println("Mount LittleFS");

  getBotToken();  // Считываем из FS ТОКЕН бота (BOT_TOKEN)
  getChatID();    // Считываем из FS ID чата (CHAT_ID)


  //Настройки WiFiManager
  WiFi.mode(WIFI_STA);                 // Режим Station
  wm.setDebugOutput(WIFISERIALDEBUG);  //Отключаем или включаем отладку для WiFiManager

  //Если нужен Статический IP адрес, тогда раскомментировать строку ниже (статика задаётся на странице WiFiManager в режиме AP)
  //wm.setSTAStaticIPConfig(IPAddress(192, 168, 50, 20), IPAddress(192, 168, 50, 1), IPAddress(255, 255, 255, 0)); // 4-й аргумент DNS - не обязательный

  //Установка личных параметров WiFiManager
  //Краткое описание - сначала идёт ID параметра, метка над полем ввода, значение по умолчанию в поле ввода, MAX длина символов для ввода
  WiFiManagerParameter custom_html("<p>Настройки Телеграма</p>");                                    // Выводим только кастомный HTML над формами ввода
  WiFiManagerParameter custom_token_Telega("token_Telega", "Введите ТОКЕН (46 символов):", "", 46);  // Пустое поле ввода Токена с ID "token_Telega" и MAX 46 символов
  WiFiManagerParameter custom_ID_Telega("ID_Telega", "Введите ID чата (10 символов):", "", 10);      // Пустое поле ввода Пароля с ID "ID_Telega" и MAX 11 символов

  //Добавляем параметры на WEB морду WiFiManager
  wm.addParameter(&custom_html);
  wm.addParameter(&custom_token_Telega);
  wm.addParameter(&custom_ID_Telega);

  wm.setDarkMode(DARKMODE);         // Установка тёмной темы
  wm.setHostname(HOST_NAME);        // Установка HOSTNAME
  wm.setConnectRetries(RECONNECT);  // Кол-во повторных попыток подключения к WiFI

  //При запуске ESP вызываем функцию для смены флага на сохранение ТОКЕНА и ID для Телеграма из WiFiManager,
  wm.setSaveConfigCallback(saveConfigCallback);

  //Пользовательское меню WiFiManager. Меняем пункты на какие нужно в первой строке
  std::vector<const char*> menu = { "wifi", "info", "sep", "erase", "update", "restart" };
  wm.setMenu(menu);  //  Устанавливаем параметры для отображения меню в WiFiManager

  //Для корректной работы "wm.autoConnect" должен быть включён после всех настроек WiFiManager
  wm.autoConnect(ssid_AP, pswd_AP);  // SSID и пароль для точки доступа AP


  //Запись ТОКЕНА и ID в FS после нажания кнопки Save в WiFiManager
  if (shouldSaveConfig) {                        // Если флаг "shouldSaveConfig" поднят, тогда выполняется сохранение Токена и ID в FS
    textToken = custom_token_Telega.getValue();  // Получаем введённый ТОКЕН из WiFiManager
    textID = custom_ID_Telega.getValue();        // Получаем введённый ID из WiFiManager

    putBotToken();  // Записываем ТОКЕН в FS (ОБЯЗАТЕЛЬНО ВЫЗЫВАТЬ ЭТУ ФУНКЦИЮ ПЕРВОЙ, ТАК КАК В НЕЙ ПРОИСХОДИТ УДАЛЕНИЕ СТАРЫХ КОНФИГОВ, ПЕРЕД СОЗДАНИЕМ НОВЫХ)
    putChatID();    // Записываем ID в FS
  }


  bot = new FastBot(BOT_TOKEN);  // Обращаемся к объекту Телеграма через указатель

  bot->setChatID(CHAT_ID);  // Разрешаем работу только для данного ID
  bot->attach(newMsg);      // Подключаем обработчик событий Телеграма

  bot->sendMessage("\"WiFi Relay PC\" в сети! (/start)");  // Отправляем сообщение в чат о появлении ESP8266 в сети
}


void loop() {
  ButtSw.tick();    // Тикер для обработчика кнопки (нужен для того, чтобы корректно считались все таймауты). Данная функция должна постоянно опрашиваться
  bot->tick();      // Тикер для проверки обновлений с Телеграма по таймеру. Данная функция должна постоянно опрашиваться
  controlButton();  // Постоянно проверяем функцию обработчика кнопки на нажатие или удержание

  //Если флаг поднят, то моргаем LED раз в 40 мс (без задержки кода).
  if (flag == true) digitalWrite(LED, (millis() / 40) % 2);  // Позволяет визуально определить когда можно отпускать кнопку для сброса настроек
}
