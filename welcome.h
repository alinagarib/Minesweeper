//
// Created by alina on 11/21/2023.
//

#ifndef UNTITLED2_HEADER_H
#define UNTITLED2_HEADER_H

#endif //UNTITLED2_HEADER_H

#include <iostream>
#include <SFML/Graphics.hpp>
#include "TextureManager.h"
#pragma once

class allWindows{
public:
    struct Board{
        int row_count;
        int col_count;
        int mine_count;
        int tile_count;
        int height;
        int width;
        void readConfig();
        sf::Sprite face;
        int mines_left;
        vector<sf::Sprite> numbers;
        vector<sf::Texture> numbersT;
        vector<sf::Sprite> digits;
        bool board_debug = false;
    };
    string player_name;
    Board board;
    allWindows();
    void setText(sf::Text &text, float x, float y);
    void setSprite(sf::Sprite &sprite, sf::Texture &texture, float x, float y);
    Board getBoard();
};

class welcomeWindow : virtual public allWindows{
public:
    void drawWelcomeWindow();
    string getName();
};

class gameWindow : virtual public allWindows{
public:
    struct Tile{
        bool is_mine;
        bool is_flagged = false;
        bool is_hidden = true;
        int adjacent_mines;
        sf::FloatRect bounds;
        sf::Sprite tile;
        sf::Sprite mine;
        sf::Sprite flag;
        sf::Sprite number;
        float x_pos;
        float y_pos;
    };

    vector<vector<Tile>> all_tiles;
    bool game_over = false;
    bool game_on = true;
    bool game_won = false;
    bool is_paused = false;
    bool new_game = false;
    bool leaderboard_open = false;
    void drawGameWindow(gameWindow& game_window, string& name);
    void drawBoard(sf::RenderWindow &window);
    void drawCounter(sf::RenderWindow &window, int seconds, int flags);
    void initializeBoard(sf::RenderWindow &window);
    void toggleBoard(sf::RenderWindow &window, sf::Vector2i &mouse, bool right_click, bool left_click, bool game, string& name, int timer);
    void revealTiles(int row, int col, float x, float y);
    void revealTile(int row, int col, float x, float y, bool game_over);
    bool checkIfWinner();
    void hideTile(int row, int col, float x, float y);
    void debug();
    void pausePlay();
};

class leaderboardWindow : virtual public allWindows{
public:
    struct Player{
        string name;
        int time;
        bool operator<(const Player& other) const {
            return time < other.time;
        }
    };
    vector<int> current_times;
    vector<Player> leaderboard;
    string leaders;
    bool is_open = false;
    void readLeaderboard(bool toggle);
    void drawLeaderboard();
    void toggleLeaderboard(string& player_name, int seconds);
    string toggleSeconds(int seconds);
};
