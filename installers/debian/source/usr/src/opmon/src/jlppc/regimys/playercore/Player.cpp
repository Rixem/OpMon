#include "Player.hpp"
#include "../start/main.hpp"
#include "../save/Save.hpp"

Player::Player(std::string const& name):
    name(name), dressID(Utils::randUI(0xFFFFFFFF)) {
    equipe = Equipe(name);
    for(unsigned int i = 0; i < ITEM_NUMBER; i++) {
            bag[i] = 0;
        }

}

Player::Player():
    dressID(Utils::randUI(0xFFFFFFFF)) {
    equipe = Equipe(name);
    for(unsigned int i = 0; i < ITEM_NUMBER; i++) {
            bag[i] = 0;
        }
}

Equipe *Player::getEquipe() {
    return &equipe;
}

void Player::addItem(int itemID) {
    bag[itemID]++;
}

int Player::checkItem(int itemID) {
    unsigned int itemID2 = itemID;
    if (itemID2 > ITEM_NUMBER || itemID2 < 0) {
            handleError("Player : itemID invalide", true);
        }
    return bag[itemID];
}

bool Player::deleteItem(int itemID) {

    if (bag[itemID] != 0 || itemID < 0) {
            handleError("Player : itemID invalide", true);
        }
    if (bag[itemID] != 0) {
            bag[itemID]--;
            return true;
        }
    else {
            return false;
        }
}

void Player::healOp() {
    equipe.heal();
    for (int i = 0; i < equipe.getSize(); i++) {
            OpMon *pkmn = equipe[i];

            pkmn->amour = false;
            pkmn->malediction = false;
            pkmn->vampigraine = false;
            for (int j = 0; j < 4; j++) {
                    Attaque *atk = (pkmn->getAttaques())[j];
                    atk->healPP();

                }


        }
}

bool Player::addOpToEquipe(OpMon *toAdd) {
    if (equipe.addOpMon(toAdd)) {
            return true;
        }
    else {
            addOpMonToPC(toAdd);
            return false;
        }
}

void Player::save() {
    UNS
    SOUT << this->name << endl;
    SOUT << Save::intToChar(dressID) << endl;
    SOUT << Save::intToChar(ITEM_NUMBER) << endl;
    for(unsigned int it = 0; it < ITEM_NUMBER; it++) {
            SOUT << Save::intToChar(bag[it]) << endl;
        }
    SOUT << Save::intToChar(pc.size()) << endl;
    for(unsigned int it = 0; it < pc.size(); it++) {
            SOUT << pc[it]->save();
        }
    for(unsigned int it = 0; it < 6; it++) {
            SOUT << equipe[it]->save();
        }

}
#include "../objects/OpMon.hpp"
Player::Player(std::ifstream &in, std::string &name):
    name(name), dressID(in.get()) {
    UNS
    in.get();
    int iNber = in.get();
    in.get();
    if(iNber > ITEM_NUMBER) {
            iNber = ITEM_NUMBER;
        }
    for(int i = 0; i < iNber; i++) {
            this->bag[i] = in.get();
            in.get();
        }
    if(iNber < ITEM_NUMBER) {
            for(unsigned int i = iNber - 1; i < ITEM_NUMBER; i++) {
                    bag[i] = 0;
                }
        }
    int pcNbre = in.get();
    in.get();
    for(int i = 0; i < pcNbre; i++) {
            OpMon *op = new OpMon(in);
            this->pc.push_back(op);
        }
    for(unsigned int i = 0; i < 6; i++) {
            OpMon *op = new OpMon(in);
            equipe.addOpMon(op);
        }

}
