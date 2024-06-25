#include <SFML/Audio.hpp> // Подключаем библиотеку для работы с аудио SFML
#include <SFML/Graphics.hpp> // Подключаем библиотеку для работы с графикой SFML
#include <nlohmann/json.hpp> // Подключаем библиотеку для работы с JSON

#include <filesystem> // Подключ аем библиотеку для работы с файловой системой
#include <fstream> // Подключаем библиотеку для работы с файлами
#include <string> // Подключаем библиотеку для работы со строками
#include <vector> // Подключаем библиотеку для использования векторов

// Используем пространство имен nlohmann для работы с JSON
using json = nlohmann::json;

// Определение состояний игры
enum class GameState {
    Menu,     // Состояние меню
    Playing,  // Состояние игры
    Settings, // Состояние настроек
    SaveLoad, // Состояние сохранения/загрузки
    Choice    // Состояние выбора
};

// Структура для опций диалога
struct DialogueOption {
    std::string text;                       // Текст опции
    std::vector<std::string> nextDialogues; // Следующие диалоги
};

// Прототипы функций
void saveProgress( int dialogueIndex1,
                   bool isCharacter1Speaking ); // Функция сохранения прогресса
void loadProgress( int& dialogueIndex1,
                   bool& isCharacter1Speaking ); // Функция загрузки прогресса
void skipToChoices( int& currentDialogueIndex1,
                    std::vector<std::string>& dialogues1,
                    GameState& state,
                    sf::Text& dialogue1 ); // Функция пропуска к выбору

int main() {
    sf::RenderWindow window( sf::VideoMode( 1920, 1080 ),
                             "Menu Example" ); // Создаем окно приложения

    sf::Font font; // Создаем объект шрифта
    if ( !font.loadFromFile(
             "assets/Arial.ttf" ) ) { // Загружаем шрифт из файла
        return -1; // Если не удалось загрузить шрифт, возвращаем ошибку
    }

    // Создаем и настраиваем кнопки меню
    sf::Text startButton( "START", font, 24 );       // Кнопка "START"
    sf::Text settingsButton( "SETTINGS", font, 24 ); // Кнопка "SETTINGS"
    sf::Text saveButton( "SAVES", font, 24 );        // Кнопка "SAVES"
    sf::Text quitButton( "QUIT", font, 24 );         // Кнопка "QUIT"

    startButton.setPosition( 860, 450 ); // Устанавливаем позицию кнопки "START"
    settingsButton.setPosition(
        860, 500 ); // Устанавливаем позицию кнопки "SETTINGS"
    saveButton.setPosition( 860, 550 ); // Устанавливаем позицию кнопки "SAVES"
    quitButton.setPosition( 860, 600 ); // Устанавливаем позицию кнопки "QUIT"

    // Создаем и настраиваем прямоугольник для меню
    sf::RectangleShape menuBox(
        sf::Vector2f( 400, 300 ) ); // Прямоугольник для фона меню
    menuBox.setFillColor(
        sf::Color( 0, 0, 0, 150 ) ); // Устанавливаем цвет и прозрачность
    menuBox.setPosition( 760, 400 ); // Устанавливаем позицию

    // Создаем и настраиваем кнопки сохранения и загрузки
    sf::Text saveProgressButton(
        "SAVE PROGRESS", font, 24 ); // Кнопка "SAVE PROGRESS"
    sf::Text loadProgressButton(
        "LOAD PROGRESS", font, 24 ); // Кнопка "LOAD PROGRESS"

    saveProgressButton.setPosition(
        860, 450 ); // Устанавливаем позицию кнопки "SAVE PROGRESS"
    loadProgressButton.setPosition(
        860, 500 ); // Устанавливаем позицию кнопки "LOAD PROGRESS"

    GameState state =
        GameState::Menu; // Устанавливаем начальное состояние игры как меню

    // Загружаем фоны для меню и игры
    sf::Texture menuBackgroundTexture; // Текстура для фона меню
    if ( !menuBackgroundTexture.loadFromFile(
             "assets/night.jpg" ) ) { // Загружаем текстуру из файла
        return -1; // Если не удалось загрузить текстуру, возвращаем ошибку
    }
    sf::Sprite menuBackground(
        menuBackgroundTexture ); // Создаем спрайт для фона меню

    sf::Texture gameBackgroundTexture; // Текстура для фона игры
    if ( !gameBackgroundTexture.loadFromFile(
             "assets/map2.png" ) ) { // Загружаем текстуру из файла
        return -1; // Если не удалось загрузить текстуру, возвращаем ошибку
    }
    sf::Sprite gameBackground(
        gameBackgroundTexture ); // Создаем спрайт для фона игры

    // Инициализация диалогов
    std::vector<std::string> dialogues1 = {
        "Zvvvv, Zvvvvv",
        "GG: “What?...”",
        "The alarm clock woke me up on a hot spring day",
        "GG: “Aah, shut up”",
        "I rejected the call, thinking it was an alarm clock",
        "Zvvvv, Zvvvvv",
        "I sat up and picked up the ringing phone.",
        "GG: the headman?",
        "I swiped the green handset to the right and held the phone to my ear.",
        "GG: ``Hello?..''",
        "The headman: “where the hell are you, today is the last day of "
        "retaking the matanalysis, or do you want to fly out of the HSE like a "
        "cork out of a bottle?!”",
        "Masha is as usual in her repertoire.",
        "Almost screams, why is she so worried about the whole group?",
        "Her voice was so high and loud that my ears curled up into a tube.",
        "GG: “I'll come, I just prepared all night, I almost overslept because "
        "of it.”",
        "It's good that I hardly communicate with my classmates and no one "
        "will turn me in, saying that I played Dota all night.",
        "Headman Masha: “it's good that I was preparing, otherwise I'm already "
        "ashamed of our group, 8 people at the retake, this is wrong.”",
        "Her words make sense ... But I'd rather keep silent about her "
        "transfer in English",
        "GG: “Come on, I'm getting dressed and going out.",
        "I started to get dressed quickly, but my stomach rumbled.",
        "GG: “Damn, I need to have breakfast, although I may be late”" };

    // Создаем текст для отображения диалогов
    sf::Text dialogue1( dialogues1[0], font, 24 ); // Текст для первого диалога
    dialogue1.setPosition( 960, 940 ); // Устанавливаем позицию текста диалога

    // Центрируем текст диалога
    sf::FloatRect textRect1 =
        dialogue1.getLocalBounds(); // Получаем границы текста
    dialogue1.setOrigin( textRect1.left + textRect1.width / 2.0f,
                         textRect1.top +
                             textRect1.height /
                                 2.0f ); // Устанавливаем центрирование
    dialogue1.setPosition(
        sf::Vector2f( 1920 / 2.0f, 940 ) ); // Устанавливаем позицию текста

    int currentDialogueIndex1 = 0; // Индекс текущего диалога
    bool isCharacter1Speaking = true; // Флаг, говорит ли первый персонаж

    // Создаем прямоугольник для фона диалогов
    sf::RectangleShape dialogueBox(
        sf::Vector2f( 1920, 150 ) ); // Прямоугольник для диалогов
    dialogueBox.setFillColor(
        sf::Color( 0, 0, 0, 150 ) ); // Устанавливаем цвет и прозрачность
    dialogueBox.setPosition( 0, 900 ); // Устанавливаем позицию

    // Загружаем музыку
    sf::Music music; // Создаем объект музыки

    // if ( !music.openFromFile(
    //          "assets/01.Hollywood.mp3" ) ) { // Загружаем музыку из файла
    //     return -1; // Если не удалось загрузить музыку, возвращаем ошибку
    // }

    music.setLoop( true ); // Устанавливаем зацикливание музыки
    music.play(); // Воспроизводим музыку

    // Создаем и настраиваем слайдеры для настроек
    sf::RectangleShape volumeSlider(
        sf::Vector2f( 200, 10 ) ); // Слайдер громкости
    volumeSlider.setFillColor( sf::Color::White ); // Устанавливаем цвет
    volumeSlider.setPosition( 860, 500 ); // Устанавливаем позицию

    sf::RectangleShape brightnessSlider(
        sf::Vector2f( 200, 10 ) ); // Слайдер яркости
    brightnessSlider.setFillColor( sf::Color::White ); // Устанавливаем цвет
    brightnessSlider.setPosition( 860, 600 ); // Устанавливаем позицию

    sf::RectangleShape volumeKnob(
        sf::Vector2f( 10, 30 ) ); // Ползунок громкости
    volumeKnob.setFillColor( sf::Color::Red ); // Устанавливаем цвет
    volumeKnob.setPosition(
        860 + music.getVolume() * 2,
        485 ); // Устанавливаем позицию в зависимости от текущей громкости

    sf::RectangleShape brightnessKnob(
        sf::Vector2f( 10, 30 ) ); // Ползунок яркости
    brightnessKnob.setFillColor( sf::Color::Red ); // Устанавливаем цвет
    brightnessKnob.setPosition(
        860 + 100, 585 ); // Устанавливаем позицию, яркость по умолчанию 100%

    // Создаем и настраиваем текст для опций выбора
    sf::Text choice1(
        "Take a Breakfast?", font, 24 ); // Опция "Take a Breakfast?"
    sf::Text choice2(
        "Don't breakfast.", font, 24 ); // Опция "Don't breakfast."
    choice1.setPosition(
        860, 500 ); // Устанавливаем позицию опции "Take a Breakfast?"
    choice2.setPosition(
        860, 550 ); // Устанавливаем позицию опции "Don't breakfast."

    // Создаем и настраиваем прямоугольник для фона выбора опций
    sf::RectangleShape choiceBox(
        sf::Vector2f( 400, 150 ) ); // Прямоугольник для выбора опций
    choiceBox.setFillColor(
        sf::Color( 0, 0, 0, 150 ) ); // Устанавливаем цвет и прозрачность
    choiceBox.setPosition( 760, 450 ); // Устанавливаем позицию

    // Инициализация опций диалога
    std::vector<DialogueOption> choices = {
        { "Take a Breakfast?",
          { "I made myself 2 sandwiches with sausage and cheese, quickly made "
            "them and washed them down with water.",
            "I got dressed quickly and ran to the streets.",
            "Opening the entrance door, the sun began to shine in my eyes.",
            "It is so bright that I would gladly go to the beach right now.",
            "I open my phone to look at the bus and commuter train "
            "schedules" } },
        { "Don't breakfast.",
          { "GG: “no, I won't eat, otherwise I'll be late”",
            "I got dressed quickly and ran to the streets.",
            "I opened the entrance door",
            "The sun is shining brightly in my eyes.",
            "It's so bright that I would love to go to the beach right now.",
            "I open my phone to look at the bus and commuter train "
            "schedules" } } };

    // Основной цикл игры
    while ( window.isOpen() ) {
        sf::Event event; // Создаем объект события
        while ( window.pollEvent( event ) ) { // Обрабатываем события
            if ( event.type ==
                 sf::Event::Closed ) { // Если событие - закрытие окна
                window.close(); // Закрываем окно
            }

            // Обработка событий в зависимости от состояния игры
            if ( state == GameState::Menu ) { // Если состояние - меню
                if ( event.type == sf::Event::MouseButtonPressed &&
                     event.mouseButton.button ==
                         sf::Mouse::Left ) { // Если нажата левая кнопка мыши
                    sf::Vector2i mousePos = sf::Mouse::getPosition(
                        window ); // Получаем позицию мыши
                    if ( startButton.getGlobalBounds().contains(
                             mousePos.x,
                             mousePos.y ) ) { // Если нажата кнопка "START"
                        state =
                            GameState::Playing; // Переходим в состояние игры
                    } else if ( settingsButton.getGlobalBounds().contains(
                                    mousePos.x,
                                    mousePos.y ) ) { // Если нажата кнопка
                                                     // "SETTINGS"
                        state = GameState::Settings; // Переходим в состояние
                                                     // настроек
                    } else if ( saveButton.getGlobalBounds().contains(
                                    mousePos.x,
                                    mousePos
                                        .y ) ) { // Если нажата кнопка "SAVES"
                        state = GameState::SaveLoad; // Переходим в состояние
                                                     // сохранения/загрузки
                    } else if ( quitButton.getGlobalBounds().contains(
                                    mousePos.x,
                                    mousePos
                                        .y ) ) { // Если нажата кнопка "QUIT"
                        window.close(); // Закрываем окно
                    }
                }
            } else if ( state == GameState::Playing ) { // Если состояние - игра
                if ( event.type ==
                     sf::Event::KeyPressed ) { // Если нажата клавиша
                    if ( event.key.code ==
                         sf::Keyboard::Escape ) { // Если нажата клавиша ESC
                        state = GameState::Menu; // Возвращаемся в меню
                    } else if ( event.key.code == sf::Keyboard::LControl ||
                                event.key.code ==
                                    sf::Keyboard::
                                        RControl ) { // Если нажата клавиша
                                                     // LControl или RControl
                        skipToChoices( currentDialogueIndex1,
                                       dialogues1,
                                       state,
                                       dialogue1 ); // Пропускаем к выбору
                    }
                } else if ( event.type == sf::Event::MouseButtonPressed &&
                            event.mouseButton.button ==
                                sf::Mouse::Left ) { // Если нажата левая кнопка
                                                    // мыши
                    if ( isCharacter1Speaking ) { // Если говорит первый
                                                  // персонаж
                        currentDialogueIndex1++; // Переходим к следующему
                                                 // диалогу
                        if ( currentDialogueIndex1 <
                             dialogues1.size() ) { // Если есть еще диалоги
                            dialogue1.setString(
                                dialogues1[currentDialogueIndex1] ); // Устанавливаем
                                                                     // текст
                                                                     // текущего
                                                                     // диалога

                            sf::FloatRect textRect1 =
                                dialogue1.getLocalBounds(); // Получаем границы
                                                            // текста
                            dialogue1.setOrigin(
                                textRect1.left + textRect1.width / 2.0f,
                                textRect1.top + textRect1.height /
                                                    2.0f ); // Центрируем текст
                            dialogue1.setPosition( sf::Vector2f(
                                1920 / 2.0f,
                                940 ) ); // Устанавливаем позицию текста

                            if ( currentDialogueIndex1 ==
                                 dialogues1.size() -
                                     1 ) { // Если это последний диалог
                                state = GameState::Choice; // Переходим в
                                                           // состояние выбора
                            }
                        }
                    }
                    isCharacter1Speaking =
                        !isCharacter1Speaking; // Меняем флаг говорящего
                                               // персонажа
                }
            } else if ( state ==
                        GameState::Settings ) { // Если состояние - настройки
                if ( event.type == sf::Event::MouseButtonPressed &&
                     event.mouseButton.button ==
                         sf::Mouse::Left ) { // Если нажата левая кнопка мыши
                    sf::Vector2i mousePos = sf::Mouse::getPosition(
                        window ); // Получаем позицию мыши
                    if ( volumeSlider.getGlobalBounds().contains(
                             mousePos.x, mousePos.y ) ) { // Если нажата область
                                                          // слайдера громкости
                        float volume =
                            ( mousePos.x - volumeSlider.getPosition().x ) /
                            2.0f; // Вычисляем громкость
                        volume = std::max(
                            0.0f,
                            std::min( 100.0f,
                                      volume ) ); // Ограничиваем громкость
                                                  // значениями от 0 до 100
                        music.setVolume(
                            volume ); // Устанавливаем громкость музыки
                        volumeKnob.setPosition(
                            volumeSlider.getPosition().x + volume * 2,
                            volumeKnob.getPosition().y ); // Перемещаем ползунок
                    } else if ( brightnessSlider.getGlobalBounds().contains(
                                    mousePos.x,
                                    mousePos.y ) ) { // Если нажата область
                                                     // слайдера яркости
                        float brightness =
                            ( mousePos.x - brightnessSlider.getPosition()
                                               .x ); // Вычисляем яркость
                        brightness = std::max(
                            0.0f,
                            std::min( 200.0f,
                                      brightness ) ); // Ограничиваем яркость
                                                      // значениями от 0 до 200
                        brightnessKnob.setPosition(
                            brightnessSlider.getPosition().x + brightness,
                            brightnessKnob.getPosition()
                                .y ); // Перемещаем ползунок

                        // Регулировка яркости фона
                        sf::Color color =
                            gameBackground
                                .getColor(); // Получаем текущий цвет фона
                        color.r = color.g = color.b =
                            255 *
                            ( brightness / 200.0f ); // Вычисляем новый цвет
                        gameBackground.setColor(
                            color ); // Устанавливаем новый цвет
                    }
                } else if ( event.type == sf::Event::KeyPressed &&
                            event.key.code ==
                                sf::Keyboard::Escape ) { // Если нажата клавиша
                                                         // ESC
                    state = GameState::Menu; // Возвращаемся в меню
                }
            } else if ( state == GameState::Choice ) { // Если состояние - выбор
                if ( event.type == sf::Event::MouseButtonPressed &&
                     event.mouseButton.button ==
                         sf::Mouse::Left ) { // Если нажата левая кнопка мыши
                    sf::Vector2i mousePos = sf::Mouse::getPosition(
                        window ); // Получаем позицию мыши
                    if ( choice1.getGlobalBounds().contains(
                             mousePos.x,
                             mousePos.y ) ) { // Если нажата первая опция
                        dialogues1 =
                            choices[0]
                                .nextDialogues; // Устанавливаем новые диалоги
                        currentDialogueIndex1 = 0; // Сбрасываем индекс диалога
                        dialogue1.setString(
                            dialogues1[currentDialogueIndex1] ); // Устанавливаем
                                                                 // текст
                                                                 // текущего
                                                                 // диалога
                        isCharacter1Speaking =
                            true; // Устанавливаем флаг говорящего персонажа
                        state =
                            GameState::Playing; // Возвращаемся в состояние игры
                    } else if ( choice2.getGlobalBounds().contains(
                                    mousePos.x,
                                    mousePos.y ) ) { // Если нажата вторая опция
                        dialogues1 =
                            choices[1]
                                .nextDialogues; // Устанавливаем новые диалоги
                        currentDialogueIndex1 = 0; // Сбрасываем индекс диалога
                        dialogue1.setString(
                            dialogues1[currentDialogueIndex1] ); // Устанавливаем
                                                                 // текст
                                                                 // текущего
                                                                 // диалога
                        isCharacter1Speaking =
                            true; // Устанавливаем флаг говорящего персонажа
                        state =
                            GameState::Playing; // Возвращаемся в состояние игры
                    }
                }
            } else if ( state == GameState::SaveLoad ) { // Если состояние -
                                                         // сохранение/загрузка
                if ( event.type == sf::Event::MouseButtonPressed &&
                     event.mouseButton.button ==
                         sf::Mouse::Left ) { // Если нажата левая кнопка мыши
                    sf::Vector2i mousePos = sf::Mouse::getPosition(
                        window ); // Получаем позицию мыши
                    if ( saveProgressButton.getGlobalBounds().contains(
                             mousePos.x,
                             mousePos.y ) ) { // Если нажата кнопка сохранения
                        saveProgress(
                            currentDialogueIndex1,
                            isCharacter1Speaking ); // Сохраняем прогресс
                    } else if ( loadProgressButton.getGlobalBounds().contains(
                                    mousePos.x,
                                    mousePos
                                        .y ) ) { // Если нажата кнопка загрузки
                        loadProgress(
                            currentDialogueIndex1,
                            isCharacter1Speaking ); // Загружаем прогресс
                        dialogue1.setString(
                            dialogues1[currentDialogueIndex1] ); // Устанавливаем
                                                                 // текст
                                                                 // текущего
                                                                 // диалога
                        state =
                            GameState::Playing; // Возвращаемся в состояние игры
                    }
                } else if ( event.type == sf::Event::KeyPressed &&
                            event.key.code ==
                                sf::Keyboard::Escape ) { // Если нажата клавиша
                                                         // ESC
                    state = GameState::Menu; // Возвращаемся в меню
                }
            }
        }

        window.clear(); // Очищаем окно

        // Отрисовка элементов в зависимости от состояния игры
        if ( state == GameState::Menu ) { // Если состояние - меню
            window.draw( menuBackground ); // Рисуем фон меню
            window.draw( menuBox ); // Рисуем прямоугольник меню
            window.draw( startButton );    // Рисуем кнопку "START"
            window.draw( settingsButton ); // Рисуем кнопку "SETTINGS"
            window.draw( saveButton );     // Рисуем кнопку "SAVES"
            window.draw( quitButton );     // Рисуем кнопку "QUIT"
        } else if ( state == GameState::Playing ) { // Если состояние - игра
            window.draw( gameBackground ); // Рисуем фон игры
            window.draw( dialogueBox ); // Рисуем прямоугольник для диалогов
            window.draw( dialogue1 ); // Рисуем текущий диалог
        } else if ( state ==
                    GameState::Settings ) { // Если состояние - настройки
            window.draw( gameBackground ); // Рисуем фон игры
            window.draw( dialogueBox ); // Рисуем прямоугольник для диалогов
            window.draw( volumeSlider ); // Рисуем слайдер громкости
            window.draw( volumeKnob ); // Рисуем ползунок громкости
            window.draw( brightnessSlider ); // Рисуем слайдер яркости
            window.draw( brightnessKnob ); // Рисуем ползунок яркости
        } else if ( state == GameState::SaveLoad ) { // Если состояние -
                                                     // сохранение/загрузка
            window.draw( gameBackground ); // Рисуем фон игры
            window.draw( dialogueBox ); // Рисуем прямоугольник для диалогов
            window.draw( saveProgressButton ); // Рисуем кнопку "SAVE PROGRESS"
            window.draw( loadProgressButton ); // Рисуем кнопку "LOAD PROGRESS"
        } else if ( state == GameState::Choice ) { // Если состояние - выбор
            window.draw( gameBackground ); // Рисуем фон игры
            window.draw( choiceBox ); // Рисуем прямоугольник для выбора опций
            window.draw( choice1 ); // Рисуем первую опцию
            window.draw( choice2 ); // Рисуем вторую опцию
        }

        window.display(); // Отображаем содержимое окна
    }

    return 0; // Завершаем программу
}

// Функция сохранения прогресса
void saveProgress( int dialogueIndex1, bool isCharacter1Speaking ) {
    json saveData; // Создаем объект JSON
    saveData["dialogueIndex1"] =
        dialogueIndex1; // Сохраняем индекс текущего диалога
    saveData["isCharacter1Speaking"] =
        isCharacter1Speaking; // Сохраняем флаг говорящего персонажа

    std::filesystem::create_directory(
        "saves" ); // Создаем директорию для сохранений
    std::ofstream saveFile( "saves/mysave.json" ); // Открываем файл для записи
    saveFile << saveData.dump(
        4 ); // Записываем данные в файл с отступом в 4 пробела
    saveFile.close(); // Закрываем файл
}

// Функция загрузки прогресса
void loadProgress( int& dialogueIndex1, bool& isCharacter1Speaking ) {
    std::ifstream loadFile( "saves/mysave.json" ); // Открываем файл для чтения
    if ( loadFile.is_open() ) { // Если файл открыт
        json saveData;          // Создаем объект JSON
        loadFile >> saveData;   // Читаем данные из файла

        dialogueIndex1 =
            saveData["dialogueIndex1"]; // Загружаем индекс текущего диалога
        isCharacter1Speaking =
            saveData["isCharacter1Speaking"]; // Загружаем флаг говорящего
                                              // персонажа

        loadFile.close(); // Закрываем файл
    }
}

// Функция для пропуска к выбору
void skipToChoices( int& currentDialogueIndex1,
                    std::vector<std::string>& dialogues1,
                    GameState& state,
                    sf::Text& dialogue1 ) {
    currentDialogueIndex1 =
        dialogues1.size() - 1; // Устанавливаем индекс на последний диалог
    dialogue1.setString(
        dialogues1[currentDialogueIndex1] ); // Устанавливаем текст последнего
                                             // диалога
    sf::FloatRect textRect1 =
        dialogue1.getLocalBounds(); // Получаем границы текста
    dialogue1.setOrigin( textRect1.left + textRect1.width / 2.0f,
                         textRect1.top +
                             textRect1.height / 2.0f ); // Центрируем текст
    dialogue1.setPosition(
        sf::Vector2f( 1920 / 2.0f, 940 ) ); // Устанавливаем позицию текста
    state = GameState::Choice; // Переходим в состояние выбора
}
