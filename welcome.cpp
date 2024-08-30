//
// Created by alina on 11/21/2023.
//
#include <iostream>
#include <SFML/Graphics.hpp>
#include <string>
#include "TextureManager.h"
#include "welcome.h"
#include <fstream>
#include <chrono>
#include <cmath>
#include <random>

void allWindows::Board::readConfig() {
    fstream myFile("config.cfg");
    string info;

    getline(myFile, info);
    col_count = stoi(info);
    getline(myFile, info);
    row_count = stoi(info);
    getline(myFile, info);
    mine_count = stoi(info);
    tile_count = row_count * col_count;
    height = row_count * 32;
    width = col_count * 32;
    mines_left = mine_count;
}

void allWindows::setText(sf::Text &text, float x, float y) {
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin((textRect.left + textRect.width/2.0f), (textRect.top + textRect.height/2.0f));
    text.setPosition(sf::Vector2f(x, y));
}

allWindows::Board allWindows::getBoard() {
    Board board;
    board.readConfig();
    return board;
}

allWindows::allWindows() {
    board = getBoard();
}

void allWindows::setSprite(sf::Sprite &sprite, sf::Texture &texture, float x, float y) {
    sprite.setTexture(texture);
    sprite.setOrigin((float)texture.getSize().x / 2.0f, (float)texture.getSize().y / 2.0f);
    sprite.setPosition(sf::Vector2f(x, y));
}

void welcomeWindow::drawWelcomeWindow() {
    sf::RenderWindow window(sf::VideoMode((board.width), (board.height)+ 100), "Welcome Window", sf::Style::Close);

    sf::Font font;
    font.loadFromFile("font.ttf");

    if(!font.loadFromFile("font.ttf")){
        cout << "cant load file" << endl;
        return;
    }

    sf::Text welcomeText;
    welcomeText.setString("WELCOME TO MINESWEEPER!");
    welcomeText.setFont(font);
    welcomeText.setCharacterSize(24);
    welcomeText.setStyle(sf::Text::Bold | sf::Text::Underlined);
    welcomeText.setFillColor(sf::Color::White);
    setText(welcomeText,((float)(board.width)/2.0f) ,((float)(board.height)/2.0f)-150);

    sf::Text enterName;
    enterName.setString("Enter your name:");
    enterName.setFont(font);
    enterName.setCharacterSize(20);
    enterName.setStyle(sf::Text::Bold);
    enterName.setFillColor(sf::Color::White);
    setText(enterName,((float)(board.width)/2.0f),((float)(board.height)/2.0f)-75);


    sf::Text input;
    input.setFont(font);
    input.setCharacterSize(18);
    input.setStyle(sf::Text::Bold);
    input.setFillColor(sf::Color::Yellow);
    setText(input, ((float)(board.width)/2.0f),((float)(board.height)/2.0f)-45);

    string input_str;


    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
                return;
            }
            else if (event.type == sf::Event::TextEntered){
                if(isprint(event.text.unicode)){
                    if(input_str.length() >= 10){
                        continue;
                    }
                    if(!isalpha((char)event.text.unicode)){
                        continue;
                    }
                    if(input_str.length() == 0){
                        input_str += toupper((char)event.text.unicode);
                    } else {
                        input_str += tolower((char) event.text.unicode);
                    }
                }
            }

            else if(event.type == sf::Event::KeyPressed){
                if(input_str.length() > 0) {
                    if (event.key.code == sf::Keyboard::Enter) {
                        window.close();
                        break;
                    }
                }
                if(event.key.code == sf::Keyboard::BackSpace){
                    if(!input_str.empty())
                        input_str.pop_back();
                }
            }
        }

        setText(input, ((float)(board.width)/2.0f),((float)(board.height)/2.0f)-45);
        input.setString(input_str + "|");

        window.clear(sf::Color::Blue);
        window.draw(welcomeText);
        window.draw(enterName);
        window.draw(input);
        window.display();
    }
    player_name = input_str;
    gameWindow game_window;
    game_window.drawGameWindow(game_window, player_name);
}

string welcomeWindow::getName() {
    return player_name;
}

void gameWindow::drawGameWindow(gameWindow& game_window, string& name) {
    sf::RenderWindow gameWindow(sf::VideoMode((board.width), (board.height)+ 100), "Minesweeper", sf::Style::Close);

    sf::Font font;
    font.loadFromFile("font.ttf");

    if(!font.loadFromFile("font.ttf")){
        cout << "cant load file" << endl;
        return;
    }

    bool game = true;
    bool r_click = false;
    bool l_click = false;
    int time;
    int time_p;
    initializeBoard(gameWindow);
    leaderboardWindow leaderboard_window;
    sf::Clock clock;
    while(gameWindow.isOpen()) {
        sf::Event event;
        while (gameWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                gameWindow.close();
            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mouse;
                mouse = sf::Mouse::getPosition(gameWindow);
                if (event.mouseButton.button == sf::Mouse::Left) {
                    l_click = true;
                    toggleBoard(gameWindow, mouse, r_click, l_click, game, name, time);
                } else if (event.mouseButton.button == sf::Mouse::Right) {
                    r_click = true;
                    toggleBoard(gameWindow, mouse, r_click, l_click, game, name, time);
                }
                if(leaderboard_open){
                    if(game_on) {
                        pausePlay();
                    }
                }
                if(new_game){
                    time_p = 0;
                    time = 0;
                    clock.restart();
                    board.mines_left = board.mine_count;
                    new_game = false;
                }
                if(is_paused || !game_on || game_won){
                    time_p = time;
                }
            }
            l_click = false;
            r_click = false;
        }
        gameWindow.clear(sf::Color::White);
        if (is_paused || !game_on || game_won){
            drawCounter(gameWindow, time_p, board.mines_left);
            clock.restart();
        } else {
            sf::Time elapsed = clock.getElapsedTime();
            time = (int)elapsed.asSeconds() + time_p;
            drawCounter(gameWindow, time, board.mines_left);
        }
        drawBoard(gameWindow);
        gameWindow.display();
        if(leaderboard_open){
            leaderboard_window.drawLeaderboard();
            if(game_on) {
                pausePlay();
            }
            drawCounter(gameWindow, time_p, board.mines_left);
            clock.restart();
            leaderboard_open = false;
        }
    }
}

void gameWindow::toggleBoard(sf::RenderWindow& window, sf::Vector2i &mouse, bool right_click, bool left_click, bool game, string& name, int timer ){

    sf::Texture& flagT = TextureManager::getTexture("flag");
    sf::Sprite flag;

    sf::Texture& loseFaceT = TextureManager::getTexture("face_lose");
    sf::Sprite lose_face;
    setSprite(lose_face, loseFaceT, (((float)board.col_count/2.0f)*32)-32, 32 * ((float)board.row_count + 1.5f));

    sf::Texture& winFaceT = TextureManager::getTexture("face_win");
    sf::Sprite win_face;
    setSprite(win_face, winFaceT, (((float)board.col_count/2.0f)*32)-32, 32 * ((float)board.row_count + 1.5f));

    sf::Texture& debugButtonT = TextureManager::getTexture("debug");
    sf::Sprite debug_button;
    setSprite(debug_button, debugButtonT, (((float)board.col_count * 32) - 304), 32 * ((float)board.row_count + 1.5f));
    sf::FloatRect debug_bounds = debug_button.getGlobalBounds();

    sf::Texture& pauseButtonT = TextureManager::getTexture("pause");
    sf::Sprite pause_button;
    setSprite(pause_button, pauseButtonT, ((float)board.col_count * 32)-240, 32 * ((float)board.row_count + 1.5f));
    sf::FloatRect pause_bounds = pause_button.getGlobalBounds();

    sf::Texture& leaderboardButtonT = TextureManager::getTexture("leaderboard");
    sf::Sprite leaderboard_button;
    setSprite(leaderboard_button, leaderboardButtonT, ((float)board.col_count * 32) - 176, 32 * ((float)board.row_count + 1.5f));
    sf::FloatRect leaderboard_bounds = leaderboard_button.getGlobalBounds();

    sf::FloatRect face_bounds = lose_face.getGlobalBounds();
    leaderboardWindow leaderboard_window;


    if (face_bounds.contains((window.mapPixelToCoords(mouse)))){
        initializeBoard(window);
        game_on = true;
        game_won = false;
        game_over = false;
        is_paused = false;
        new_game = true;
        return;
    }

    if ((leaderboard_bounds.contains(window.mapPixelToCoords(mouse)))){
        leaderboard_open = true;
        return;
    }

    if (game_on && !leaderboard_open) {
        if (pause_bounds.contains(window.mapPixelToCoords(mouse))) {
            pausePlay();
        }
        if (!is_paused) {
            if (debug_bounds.contains((window.mapPixelToCoords(mouse)))) {
                debug();
            }
            if(!board.board_debug) {
                float x_coord = 16;
                float y_coord = 16;
                for (int i = 0; i < board.row_count; i++) {
                    for (int j = 0; j < board.col_count; j++) {
                        x_coord = all_tiles[i][j].x_pos;
                        y_coord = all_tiles[i][j].y_pos;
                        if (all_tiles[i][j].bounds.contains(window.mapPixelToCoords(mouse))) {
                            if (left_click) {
                                if (!all_tiles[i][j].is_flagged) {
                                    if (all_tiles[i][j].adjacent_mines == 0) {
                                        revealTiles(i, j, x_coord, y_coord);
                                    } else if (all_tiles[i][j].is_mine) {
                                        game_over = true;
                                        board.face = lose_face;
                                        game_on = false;
                                        revealTile(i, j, x_coord, y_coord, game_over);
                                        return;
                                    } else {
                                        revealTile(i, j, x_coord, y_coord, game_over);
                                        if(all_tiles[i][j].adjacent_mines != 9 && all_tiles[i][j].adjacent_mines != 0) {
                                            setSprite(all_tiles[i][j].number,
                                                      board.numbersT[all_tiles[i][j].adjacent_mines - 1], x_coord,
                                                      y_coord);
                                        }
                                    }
                                    if (checkIfWinner()) {
                                        for (int k = 0; k < board.row_count; k++) {
                                            for (int l = 0; l < board.col_count; l++) {
                                                if (all_tiles[k][l].is_mine) {
                                                    setSprite(flag, flagT, all_tiles[k][l].x_pos,
                                                              all_tiles[k][l].y_pos);
                                                    all_tiles[k][l].flag = flag;
                                                    all_tiles[k][l].is_flagged = true;
                                                }
                                            }
                                        }
                                        leaderboard_window.toggleLeaderboard(name, timer);
                                        board.face = win_face;
                                        game_on = false;
                                        game_won = true;
                                        return;
                                    }
                                }
                            } else if (right_click) {
                                setSprite(flag, flagT, x_coord, y_coord);
                                if (all_tiles[i][j].is_flagged) {
                                    all_tiles[i][j].is_flagged = false;
                                    board.mines_left++;
                                } else {
                                    all_tiles[i][j].flag = flag;
                                    all_tiles[i][j].is_flagged = true;
                                    board.mines_left--;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}


void gameWindow::drawBoard(sf::RenderWindow &window) {
    sf::Texture& debugButtonT = TextureManager::getTexture("debug");
    sf::Sprite debug_button;
    setSprite(debug_button, debugButtonT, (((float)board.col_count * 32) - 304), 32 * ((float)board.row_count + 1.5f));

    sf::Texture& pauseButtonT = TextureManager::getTexture("pause");
    sf::Sprite pause_button;
    setSprite(pause_button, pauseButtonT, ((float)board.col_count * 32)-240, 32 * ((float)board.row_count + 1.5f));

    sf::Texture& playButtonT = TextureManager::getTexture("play");
    sf::Sprite play_button;
    setSprite(play_button, playButtonT, ((float)board.col_count * 32) - 240, 32 * ((float)board.row_count + 1.5f));

    sf::Texture& leaderboardButtonT = TextureManager::getTexture("leaderboard");
    sf::Sprite leaderboard_button;
    setSprite(leaderboard_button, leaderboardButtonT, ((float)board.col_count * 32) - 176, 32 * ((float)board.row_count + 1.5f));


    if (is_paused){
        window.draw(play_button);
        for(int i = 0; i < board.row_count; i++) {
            for (int j = 0; j < board.col_count; j++) {
                window.draw(all_tiles[i][j].tile);
            }
        }
    } else {
        window.draw(pause_button);
        for (int i = 0; i < board.row_count; i++) {
            for (int j = 0; j < board.col_count; j++) {
                window.draw(all_tiles[i][j].tile);
                if (all_tiles[i][j].is_flagged)
                    window.draw(all_tiles[i][j].flag);
                if (!all_tiles[i][j].is_hidden)
                    window.draw(all_tiles[i][j].number);
                if(game_over || board.board_debug)
                    window.draw(all_tiles[i][j].mine);
            }
        }
    }
    window.draw(board.face);
    window.draw(debug_button);
    window.draw(leaderboard_button);
}

void gameWindow::drawCounter(sf::RenderWindow &window, int seconds, int flags) {
    int minutes;
    if(seconds >= 60){
        minutes = seconds / 60;
        seconds = seconds % 60;
    } else {
        minutes = 0;
    }

    int min1 = (minutes / 10) % 10;
    int min2 = minutes % 10;
    int sec1 = (seconds / 10) % 10;
    int sec2 = seconds % 10;
    int timer[4] = {min1, min2, sec1, sec2};


    float x = 97;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < board.digits.size(); j++) {
            if (timer[i] == j) {
                board.digits[j].setPosition(sf::Vector2f(((float) board.col_count * 32) - x,
                                                   32 * ((float) board.row_count + 0.5f) + 16));
                window.draw(board.digits[j]);
            }
        }
        if (i == 1) {
            x = 54;
        } else{
            x -= 21;
        }
    }

    int counter1 = abs((flags / 10) % 10);
    int counter2 = abs(flags % 10);

    if(flags < 0){
        board.digits[10].setPosition(sf::Vector2f(32,32 * ((float)board.row_count + 0.5f) + 16));
        window.draw(board.digits[10]);
    } else {
        board.digits[0].setPosition(sf::Vector2f(32,32 * ((float)board.row_count + 0.5f) + 16));
        window.draw(board.digits[0]);
    }
    for(int j = 0; j < board.digits.size(); j++){
        if (counter1 == j){
            board.digits[j].setPosition(sf::Vector2f(54, 32 * ((float)board.row_count + 0.5f) + 16));
            window.draw(board.digits[j]);
        }
        if (counter2 == j){
            board.digits[j].setPosition(sf::Vector2f (75, 32 *((float)board.row_count + 0.5f) + 16));
            window.draw(board.digits[j]);
        }
    }
}

void gameWindow::initializeBoard(sf::RenderWindow &window) {
    vector<vector<Tile>> tiles(board.row_count, vector<Tile>(board.col_count));
    all_tiles = tiles;
    random_device rd;
    mt19937 gen(static_cast<unsigned>(std::time(0)));
    uniform_int_distribution<> rowDis(0, board.row_count - 1);
    uniform_int_distribution<> colDis(0, board.col_count - 1);

    int mines_placed = 0;

    while (mines_placed < board.mine_count) {
        int rand_row = rowDis(gen);
        int rand_col = colDis(gen);

        if (!all_tiles[rand_row][rand_col].is_mine) {
            all_tiles[rand_row][rand_col].is_mine = true;
            mines_placed++;
        }
    }

    sf::Texture& number1T = TextureManager::getTexture("number_1");
    sf::Sprite number_1;

    sf::Texture& number2T = TextureManager::getTexture("number_2");
    sf::Sprite number_2;

    sf::Texture& number3T = TextureManager::getTexture("number_3");
    sf::Sprite number_3;

    sf::Texture& number4T = TextureManager::getTexture("number_4");
    sf::Sprite number_4;

    sf::Texture& number5T = TextureManager::getTexture("number_5");
    sf::Sprite number_5;

    sf::Texture& number6T = TextureManager::getTexture("number_6");
    sf::Sprite number_6;

    sf::Texture& number7T = TextureManager::getTexture("number_7");
    sf::Sprite number_7;

    sf::Texture& number8T = TextureManager::getTexture("number_8");
    sf::Sprite number_8;

    board.numbers.push_back(number_1);
    board.numbers.push_back(number_2);
    board.numbers.push_back(number_3);
    board.numbers.push_back(number_4);
    board.numbers.push_back(number_5);
    board.numbers.push_back(number_6);
    board.numbers.push_back(number_7);
    board.numbers.push_back(number_8);

    board.numbersT.push_back(number1T);
    board.numbersT.push_back(number2T);
    board.numbersT.push_back(number3T);
    board.numbersT.push_back(number4T);
    board.numbersT.push_back(number5T);
    board.numbersT.push_back(number6T);
    board.numbersT.push_back(number7T);
    board.numbersT.push_back(number8T);


    for(int i = 0; i < board.row_count; i++){
        for(int j = 0; j < board.col_count; j++){
            if(all_tiles[i][j].is_mine) {
                all_tiles[i][j].adjacent_mines = 9;

                for (int ni = -1; ni <= 1; ++ni) {
                    for (int nj = -1; nj <= 1; ++nj) {
                        int neighborR = i + ni;
                        int neighborC = j + nj;


                        if (neighborR >= 0 && neighborR < board.row_count && neighborC >= 0 &&
                            neighborC < board.col_count) {
                            if(!all_tiles[neighborR][neighborC].is_mine)
                                all_tiles[neighborR][neighborC].adjacent_mines++;
                        }
                    }
                }
            }
        }
    }

    for(int i = 0; i < board.row_count; i++)
    {
        for(int j = 0; j < board.col_count; j++)
        {
            cout << all_tiles[i][j].adjacent_mines << " ";
            if(all_tiles[i][j].adjacent_mines != 9 && all_tiles[i][j].adjacent_mines != 0) {
                all_tiles[i][j].number = board.numbers[all_tiles[i][j].adjacent_mines - 1];
            }
        }
        cout<< endl;
    }

    sf::Texture& tileHiddenT = TextureManager::getTexture("tile_hidden");
    sf::Sprite tile_hidden;

    float x_coord = 16;
    float y_coord = 16;
    for (int i = 0; i < board.row_count; i++){
        for(int j = 0; j < board.col_count; j++){
            setSprite(tile_hidden, tileHiddenT, x_coord, y_coord);
            if(all_tiles[i][j].adjacent_mines != 9 && all_tiles[i][j].adjacent_mines != 0) {
                setSprite(all_tiles[i][j].number, board.numbersT[all_tiles[i][j].adjacent_mines - 1], x_coord, y_coord);
            }
            all_tiles[i][j].bounds = tile_hidden.getGlobalBounds();
            all_tiles[i][j].tile = tile_hidden;
            all_tiles[i][j].x_pos = x_coord;
            all_tiles[i][j].y_pos = y_coord;
            x_coord += 32;
        }
        y_coord += 32;
        x_coord = 16;
    }

    sf::Texture& happyFaceT = TextureManager::getTexture("face_happy");
    sf::Sprite happy_face;
    setSprite(happy_face, happyFaceT, (((float)board.col_count/2.0f)*32)-32, 32 * ((float)board.row_count + 1.5f));
    board.face = happy_face;

    sf::Texture& digitsT = TextureManager::getTexture("digits");
    sf::Sprite digit_0;
    board.digits.push_back(digit_0);
    sf::Sprite digit_1;
    board.digits.push_back(digit_1);
    sf::Sprite digit_2;
    board.digits.push_back(digit_2);
    sf::Sprite digit_3;
    board.digits.push_back(digit_3);
    sf::Sprite digit_4;
    board.digits.push_back(digit_4);
    sf::Sprite digit_5;
    board.digits.push_back(digit_5);
    sf::Sprite digit_6;
    board.digits.push_back(digit_6);
    sf::Sprite digit_7;
    board.digits.push_back(digit_7);
    sf::Sprite digit_8;
    board.digits.push_back(digit_8);
    sf::Sprite digit_9;
    board.digits.push_back(digit_9);
    sf::Sprite digit_negative;
    board.digits.push_back(digit_negative);
    int rec_left = 0;
    for (int i = 0; i < board.digits.size(); i++){
        board.digits[i].setTexture(digitsT);
        board.digits[i].setTextureRect(sf::IntRect(rec_left, 0, 21, 32));
        rec_left += 21;
    }
}

void gameWindow::revealTiles(int row, int col, float x, float y){
    if (!all_tiles[row][col].is_mine && all_tiles[row][col].is_hidden && !all_tiles[row][col].is_flagged) {
        revealTile(row, col, x, y, false);
        if (all_tiles[row][col].adjacent_mines == 0) {
            for (int i = -1; i <= 1; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    if (row + i >= 0 && row + i < board.row_count &&
                        col + j >= 0 && col + j < board.col_count &&
                        !all_tiles[row + i][col + j].is_mine && all_tiles[row + i][col + j].is_hidden) {
                        revealTiles(row + i, col + j, all_tiles[row + i][col + j].x_pos,
                                    all_tiles[row + i][col + j].y_pos);
                    }
                }
            }
        }
    }
}

void gameWindow::revealTile(int row, int col, float x, float y, bool game_over){
    sf::Texture& tileRevealedT = TextureManager::getTexture("tile_revealed");
    sf::Sprite tile_revealed;

    sf::Texture& mineT = TextureManager::getTexture("mine");
    sf::Sprite mine;

    if(game_over){
        for(int i = 0; i < board.row_count; i++){
            for(int j = 0; j < board.col_count; j++){
                if(all_tiles[i][j].is_mine){
                    setSprite(mine, mineT, all_tiles[i][j].x_pos, all_tiles[i][j].y_pos);
                    setSprite(tile_revealed, tileRevealedT, all_tiles[i][j].x_pos, all_tiles[i][j].y_pos);
                    all_tiles[i][j].mine = mine;
                    all_tiles[i][j].tile = tile_revealed;
                    all_tiles[i][j].is_hidden = false;
                }
            }
        }
    } else {
        if(!all_tiles[row][col].is_flagged) {
            setSprite(tile_revealed, tileRevealedT, x, y);
            all_tiles[row][col].tile = tile_revealed;
            all_tiles[row][col].is_hidden = false;
        }
    }
}

bool gameWindow::checkIfWinner(){
    for (int i = 0; i < board.row_count; i++) {
        for (int j = 0; j < board.col_count; j++) {
            if (!all_tiles[i][j].is_mine && all_tiles[i][j].is_hidden) {
                return false;
            }
        }
    }
    return true;
}

void gameWindow::hideTile(int row, int col, float x, float y){
    sf::Texture& tileHiddenT = TextureManager::getTexture("tile_hidden");
    sf::Sprite tile_hidden;
    sf::Texture& flagT = TextureManager::getTexture("flag");
    sf::Sprite flag;


    setSprite(tile_hidden, tileHiddenT, x, y);
    all_tiles[row][col].is_hidden = true;
    all_tiles[row][col].tile = tile_hidden;
    if(all_tiles[row][col].is_flagged){
        setSprite(flag, flagT, all_tiles[row][col].x_pos, all_tiles[row][col].y_pos);
        all_tiles[row][col].flag = flag;
    }

}

void gameWindow::debug() {
    board.board_debug = !board.board_debug;
    game_over = !game_over;
    if (board.board_debug) {
        for (int i = 0; i < board.row_count; i++) {
            for (int j = 0; j < board.col_count; j++) {
                if (all_tiles[i][j].is_mine) {
                    revealTile(i, j, all_tiles[i][j].x_pos, all_tiles[i][j].y_pos, game_over);
                }
            }
        }
    } else if (!board.board_debug) {
        for (int i = 0; i < board.row_count; i++) {
            for (int j = 0; j < board.col_count; j++) {
                if (all_tiles[i][j].is_mine) {
                    hideTile(i, j, all_tiles[i][j].x_pos, all_tiles[i][j].y_pos);
                }
            }
        }
    }
}

void gameWindow::pausePlay() {

    sf::Texture& tileRevealedT = TextureManager::getTexture("tile_revealed");
    sf::Sprite tile_revealed;

    sf::Texture& tileHiddenT = TextureManager::getTexture("tile_hidden");
    sf::Sprite tile_hidden;

    is_paused = !is_paused;

    for (int i = 0; i < board.row_count; i++) {
        for (int j = 0; j < board.col_count; j++) {
            if (is_paused) {
                setSprite(tile_revealed, tileRevealedT, all_tiles[i][j].x_pos, all_tiles[i][j].y_pos);
                all_tiles[i][j].tile = tile_revealed;
            } else {
                if (all_tiles[i][j].is_hidden) {
                    setSprite(tile_hidden, tileHiddenT, all_tiles[i][j].x_pos, all_tiles[i][j].y_pos);
                    all_tiles[i][j].tile = tile_hidden;
                }
            }
        }
    }

}

void leaderboardWindow::drawLeaderboard() {
    readLeaderboard(false);
    float width = board.width / 2;
    float height = board.height/2;
    sf::RenderWindow leaderboardWindow(sf::VideoMode((int)width, (int)height + 50), "Leaderboard", sf::Style::Close);

    sf::Font font;
    font.loadFromFile("font.ttf");

    if(!font.loadFromFile("font.ttf")){
        cout << "cant load file" << endl;
        return;
    }

    sf::Text leaderboardText;
    leaderboardText.setString("LEADERBOARD");
    leaderboardText.setFont(font);
    leaderboardText.setCharacterSize(20);
    leaderboardText.setStyle(sf::Text::Bold | sf::Text::Underlined);
    leaderboardText.setFillColor(sf::Color::White);
    setText(leaderboardText,((float)(width)/2.0f) ,((float)(height)/2.0f)-60);

    sf::Text leadersText;
    leadersText.setString(leaders);
    leadersText.setFont(font);
    leadersText.setCharacterSize(18);
    leadersText.setStyle(sf::Text::Bold);
    leadersText.setFillColor(sf::Color::White);
    setText(leadersText,((float)(width)/2.0f) ,((float)(height/2.0f)+60));

    while (leaderboardWindow.isOpen()) {
        sf::Event event;
        while (leaderboardWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                leaderboardWindow.close();
                return;
            }
        }
        leaderboardWindow.clear(sf::Color::Blue);
        leaderboardWindow.draw(leaderboardText);
        leaderboardWindow.draw(leadersText);
        leaderboardWindow.display();
    }
}

void leaderboardWindow::readLeaderboard(bool toggle) {
    ifstream file("leaderboard.txt");
    if (!file.is_open()) {
        std::cerr << "Failed to open the leaderboard file." << std::endl;
        return;
    }

    vector<Player> temp;
    leaders = "";
    string comma = ",";
    string star = "*";
    int index;
    string line;
    string time;
    string name;
    int minutes;
    int sec;
    int total;
    while(getline(file, line)) {
        Player player;
        index = line.find(comma);
        time = line.substr(0, index);
        name = line.substr(index + 1);
        player.name = name;
        if(toggle) {
            if (name.find(star) != string::npos) {
                name.pop_back();
                player.name = name;
            } else {
                player.name = name;
            }
        }

        try {
            minutes = stoi(time.substr(0, 2));
            sec = stoi(time.substr(3, 2));
        } catch (const std::invalid_argument& e) {
            cerr << "Error converting minutes or seconds: " << e.what() << endl;
            continue;
        }
        total = (minutes * 60) + sec;
        player.time = total;
        temp.push_back(player);
    }
    file.close();
    leaderboard = temp;
    sort(leaderboard.begin(), leaderboard.end());
    int pos = 1;
    for(int i = 0; i < leaderboard.size(); i++){
        leaders += to_string(pos) + ".   " + toggleSeconds(leaderboard[i].time) + "   " + leaderboard[i].name + "\n\n";
        pos++;
    }
    cout << leaders;
}

void leaderboardWindow::toggleLeaderboard(string& player_name, int seconds) {
    readLeaderboard(true);
    string new_time = toggleSeconds(seconds);

    fstream file("leaderboard.txt");

    Player player;
    player.name = player_name + "*";
    player.time = seconds;
    leaderboard.push_back(player);
    sort(leaderboard.begin(), leaderboard.end());
    leaderboard.pop_back();


    for(int i = 0; i < leaderboard.size(); i++){
        file << toggleSeconds(leaderboard[i].time) << "," << leaderboard[i].name << endl;
    }
}

string leaderboardWindow::toggleSeconds(int seconds){
    int mins;
    if(seconds >= 60){
        mins = seconds / 60;
        seconds = seconds % 60;
    } else {
        mins = 0;
    }

    int min1 = (mins / 10) % 10;
    int min2 = mins % 10;
    int sec1 = (seconds / 10) % 10;
    int sec2 = seconds % 10;
    string new_time = to_string(min1) + to_string(min2) + ":" + to_string(sec1) + to_string(sec2);
    return new_time;
}