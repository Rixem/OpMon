/*
Initializer.hpp
Author : Jlppc
Contributeurs au fichier .cpp: Navet56, Aerzia
Fichier sous licence GPL-3.0
http://opmon-game.ga
Contient la définition des methodes qui permettent d'initialiser les éléments du jeu
*/
#ifndef INITIALIZER_HPP
#define INITIALIZER_HPP
#define OP_NUMBER 150

#include <iostream>
#include <vector>
#include "../objects/Species.hpp"
#include "../evolution/evolutions.hpp"

#define ATK_LIST(OpId) AtkArray arr##OpId[]
#define ATK_ADD_LIST(OpID, atkLength) pb(atkOpLvl[(OpId)], arr##OpId , (atkLength) )
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "../gui/Elements.hpp"
class Map;
//->NoDoc
namespace Initializer {
    extern std::map<unsigned int, Species> listOp;
    extern std::vector<std::map<int, std::string> > atkOpLvl; 
    extern std::map<std::string, Map> maps;
    extern std::vector<sf::Music *> townMusics;
    extern sf::Texture texturePP[4];
    extern sf::Texture walkingPP[4];
    extern sf::Texture walkingPP2[4];
    extern sf::Texture backPP;//Not init yet
    extern sf::Texture mapsFe[3];

    extern std::map<std::string, std::vector<sf::Texture> > charaTextures;

    extern sf::Texture tileset;
    //Battle
    extern sf::Texture bActButtons[4];
    extern sf::Texture bHpBar;
    extern sf::Texture bHpBg;
    extern std::vector<sf::Texture> battleBackgrounds;

    extern std::vector<sf::Texture> charaFront;

    extern std::vector<std::vector<sf::Texture> > doorsTextures;
    //std::string backgrounds[];
    extern std::vector<Stats> evs[OP_NUMBER];
    void initOpMons();
    void initAtkLvls();
    void initSprites();
    void initBackgrounds();
    void initKeys();
    void initMaps();
    void init();
};

#endif // INITIALIZER_HPP
