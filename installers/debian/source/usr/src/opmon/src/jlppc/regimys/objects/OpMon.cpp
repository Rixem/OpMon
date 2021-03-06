#include "OpMon.hpp"
#include "../start/main.hpp"
#include "../evolution/Evolution.hpp"
#include "../evolution/evolutions.hpp"
#include "../save/Save.hpp"
#include <sstream>
#include "item/IHeal.hpp"
#include "Attaques.hpp"
#include <fstream>
#include "../start/Initializer.hpp"

UNS

OpMon::~OpMon() {
    for (int i = 0; i < 4; i++) {
            if(attaques[i] != NULL) {
                    delete(attaques[i]);
                }

        }
}

float CalcCourbes::p(int x) {
    switch (x) {
            case 0:
                return 0;
            case 1:
                return 0.008;
            case 2:
                return 0.014;
            default:
                return 0.008;
        }
}

int CalcCourbes::erratique(int n) {
    if (0 < n && n <= 50) {
            return round(pow(n, 3) * ((100 - n) / 50));
        }
    else if (51 <= n && n <= 68) {
            return round(pow(n, 3) * ((150 - n) / 50));
        }
    else if (69 <= n && n <= 98) {
            return round(1.274f - ((1 / 50) * (n / 3)) - p(n % 3));
        }
    else if (n >= 99) {
            return round(pow(n, 3) * ((160 - n) / 100));
        }
    else {
            handleError("Erreur dans le calcul d'experience : niveau <= 0",
                        true);
            return 0;
        }
}

int CalcCourbes::fluctuante(int n) {
    if (0 < n && n <= 15) {
            return round(pow(n, 3) * ((24 + ((n + 1) / 3) / 50)));
        }
    else if (16 <= n && n <= 35) {
            return round(pow(n, 3) * ((14 + n) / 50));
        }
    else if (n >= 36) {
            return round(pow(n, 3) * ((32 + (n / 2)) / 50));
        }
    else {
            handleError("Erreur dans le calcul d'experience : niveau <= 0",
                        true);
            return 0;
        }
}

int CalcCourbes::lente(int n) {
    return round(1.25f * pow(n, 3));
}

int CalcCourbes::moyenne(int n) {
    return round(pow(n, 3));
}
int CalcCourbes::parabolique(int n) {
    return round(1.2f * pow(n, 3) - 15 * pow(n, 2) + (100 * n) - 140);
}
int CalcCourbes::rapide(int n) {
    return round(0.8f * pow(n, 3));
}
OpMon::OpMon(string surnom, Espece *espece, int level, Attaque *attaque1, Attaque *attaque2, Attaque *attaque3, Attaque *attaque4,
             CaractereClass caractere) {
    atkIV = Utils::randU(32);
    defIV = Utils::randU(32);
    atkSpeIV = Utils::randU(32);
    defSpeIV = Utils::randU(32);
    vitIV = Utils::randU(32);
    pvIV = Utils::randU(32);
    statATK = round(
                  ((((2 * espece->getBaseAtk() + atkIV + (atkEV / 4)) * level) / 100)
                   + 5)
                  * ((caractere.bonus == Stats::ATK) ?
                     1.1 : ((caractere.malus == Stats::ATK) ? 0.9 : 1)));
    statDEF = round(
                  ((((2 * espece->getBaseDef() + defIV + (defEV / 4)) * level) / 100)
                   + 5)
                  * ((caractere.bonus == Stats::DEF) ?
                     1.1 : ((caractere.malus == Stats::DEF) ? 0.9 : 1)));
    statATKSPE =
        round(
            ((((2 * espece->getBaseAtkSpe() + atkSpeIV + (atkSpeEV / 4))
               * level) / 100) + 5)
            * ((caractere.bonus == Stats::ATKSPE) ?
               1.1 :
               ((caractere.malus == Stats::ATKSPE) ? 0.9 : 1)));
    statDEFSPE =
        round(
            ((((2 * espece->getBaseDefSpe() + defSpeIV + (defSpeEV / 4))
               * level) / 100) + 5)
            * ((caractere.bonus == Stats::DEFSPE) ?
               1.1 :
               ((caractere.malus == Stats::DEFSPE) ? 0.9 : 1)));
    statVIT = round(
                  ((((2 * espece->getBaseVit() + vitIV + (vitEV / 4)) * level) / 100)
                   + 5)
                  * ((caractere.bonus == Stats::VIT) ?
                     1.1 : ((caractere.malus == Stats::VIT) ? 0.9 : 1)));
    statPV = round(((2 * espece->getBasePV() + pvIV + (pvEV / 4)) * level) / 100)
             + level + 10;
    this->espece = espece;
    this->level = level;

    this->attaques[0] = attaque1;
    this->attaques[1] = attaque2;
    this->attaques[2] = attaque3;
    this->attaques[3] = attaque4;

    //TODO attaquesChoix Quand les attaques seront ok
    this->caractere = caractere;
    this->surnom = surnom;
    tauxCapture = espece->getTauxDeCapture();
    PV = statPV;
    type1 = espece->getType1();
    type2 = espece->getType2();
    using namespace CalcCourbes;
    switch (this->espece->getCourbe()) {
            case CourbeExp::ERRATIQUE:
                toNextLevel = erratique(this->level + 1);
                exp = erratique(this->level);
                break;
            case CourbeExp::FLUCTUANTE:
                toNextLevel = fluctuante(this->level + 1);
                exp = fluctuante(this->level);
                break;
            case CourbeExp::LENTE:
                toNextLevel = lente(this->level + 1);
                exp = lente(this->level);
                break;
            case CourbeExp::MOYENNE:
                toNextLevel = moyenne(this->level + 1);
                exp = moyenne(this->level);
                break;
            case CourbeExp::PARABOLIQUE:
                toNextLevel = parabolique(this->level + 1);
                exp = parabolique(this->level);
                break;
            case CourbeExp::RAPIDE:
                toNextLevel = rapide(this->level + 1);
                exp = rapide(this->level);
                break;
        }
    held = NULL;
    statLove = 100;
    statPRE = 100;
    statESQ = 100;
    falsif = false;

}

bool OpMon::captured(I_Opball const &Opball) {
    int a = round((((3 * statPV - 2 * PV) * tauxCapture * Opball.getTauxCapture() * (status == Status::PARALYSED || status == Status::POISONED || status == Status::BURNING ? 1.5 : (status == Status::FROZEN || status == Status::SLEEPING ? 2 : 1))) / (3 * statPV)));
    int b = round((pow(2, 16) - 1) * pow(a / (pow(2, 8) - 1), 0.25));
    int c[] = { Utils::randU(65535), Utils::randU(65535), Utils::randU(65535),
                Utils::randU(65535)
              };
    int nbreOk = 0;
    int i = 0;
    for (i = 0; i < 4; i++) {
            if (c[i] <= b) {
                    nbreOk++;
                }
        }
    if (nbreOk == 0) {
            return false;
        }
    else {
            //System.out.println("Remue...");
            if (nbreOk == 1) {
                    return false;
                }
            //System.out.println("Remue...");
            if (nbreOk == 2) {
                    return false;
                }
            //System.out.println("Remue...");
            if (nbreOk == 3) {
                    return false;
                }
            if (nbreOk != 4) {
                    handleError("InternalError : OpMon.cpp : nbreOk != 4",
                                true);
                }
            return true;
        }
}

void OpMon::setStat(string const &stat, int newStat) {
    if (stat == "atk") {
            statATK = newStat;
        }
    else if (stat == "def") {
            statDEF = newStat;
        }
    else if (stat == "atkspe") {
            statATKSPE = newStat;
        }
    else if (stat == "defspe") {
            statDEFSPE = newStat;
        }
    else if (stat == "vit") {
            statVIT = newStat;
        }
    else if (stat == "pv") {
            statPV = newStat;
        }
    else {

        }
}

void OpMon::levelUp() {
    using namespace CalcCourbes;
    level++;
    switch (this->espece->getCourbe()) {
            case CourbeExp::ERRATIQUE:
                toNextLevel = erratique(this->level + 1);
                exp = erratique(this->level);
                break;
            case CourbeExp::FLUCTUANTE:
                toNextLevel = fluctuante(this->level + 1);
                exp = fluctuante(this->level);
                break;
            case CourbeExp::LENTE:
                toNextLevel = lente(this->level + 1);
                exp = lente(this->level);
                break;
            case CourbeExp::MOYENNE:
                toNextLevel = moyenne(this->level + 1);
                exp = moyenne(this->level);
                break;
            case CourbeExp::PARABOLIQUE:
                toNextLevel = parabolique(this->level + 1);
                exp = parabolique(this->level);
                break;
            case CourbeExp::RAPIDE:
                toNextLevel = rapide(this->level + 1);
                exp = rapide(this->level);
                break;
        }
    calcStats();
    if (espece->getEvolType()->checkEvolve(*this)) {
            if ((espece->getEvolType()->getEvolID()) == (Evolutions::ETrade)) {
                    evolve();
                }
        }
}

int OpMon::win(OpMon const &vaincu) {
    getEvs(vaincu);
    exp += ((vaincu.espece->getExp() * vaincu.level) / this->level) * expBoost;
    while (exp >= toNextLevel && level < 100) {
            if (exp < toNextLevel) {
                    break;
                }
            levelUp();
        }
    calcStats();
    return (((vaincu.espece->getExp() * vaincu.level) / this->level) * expBoost);
}

void OpMon::getEvs(OpMon const &vaincu) {
    if (!((atkEV + defEV + pvEV + atkSpeEV + defSpeEV + vitEV) > 510)) {
            vector<int> statsVaincu;
            for (int i = 0; i < vaincu.espece->getEvSize(); i++) {
                    statsVaincu.push_back(vaincu.espece->getEv()[i]);
                }

            for (unsigned int i = 0; i < statsVaincu.size(); i++) {
                    switch (statsVaincu[i]) { //Creer enumération Stats
                            case Stats::ATK:
                                if (atkEV < 252) {
                                        atkEV++;
                                    }
                                break;
                            case Stats::ATKSPE:
                                if (atkSpeEV < 252) {
                                        atkSpeEV++;
                                    }
                                break;
                            case Stats::DEF:
                                if (defEV < 252) {
                                        atkEV++;
                                    }
                                break;
                            case Stats::DEFSPE:
                                if (defSpeEV < 252) {
                                        defSpeEV++;
                                    }
                                break;
                            case Stats::PV:
                                if (pvEV < 252) {
                                        pvEV++;
                                    }
                                break;
                            case Stats::NOTHING:
                                break;
                            case Stats::VIT:
                                if (vitEV < 252) {
                                        vitEV++;
                                    }
                                break;

                        }
                }

        }
}

void OpMon::calcStats() {
    statATK = round(
                  ((((2 * espece->getBaseAtk() + atkIV + (atkEV / 4)) * level) / 100)
                   + 5)
                  * ((caractere.bonus == Stats::ATK) ?
                     1.1 : ((caractere.malus == Stats::ATK) ? 0.9 : 1)));
    statDEF = round(
                  ((((2 * espece->getBaseDef() + defIV + (defEV / 4)) * level) / 100)
                   + 5)
                  * ((caractere.bonus == Stats::DEF) ?
                     1.1 : ((caractere.malus == Stats::DEF) ? 0.9 : 1)));
    statATKSPE =
        round(
            ((((2 * espece->getBaseAtkSpe() + atkSpeIV + (atkSpeEV / 4))
               * level) / 100) + 5)
            * ((caractere.bonus == Stats::ATKSPE) ?
               1.1 :
               ((caractere.malus == Stats::ATKSPE) ? 0.9 : 1)));
    statDEFSPE =
        round(
            ((((2 * espece->getBaseDefSpe() + defSpeIV + (defSpeEV / 4))
               * level) / 100) + 5)
            * ((caractere.bonus == Stats::DEFSPE) ?
               1.1 :
               ((caractere.malus == Stats::DEFSPE) ? 0.9 : 1)));
    statVIT = round(
                  ((((2 * espece->getBaseVit() + vitIV + (vitEV / 4)) * level) / 100)
                   + 5)
                  * ((caractere.bonus == Stats::VIT) ?
                     1.1 : ((caractere.malus == Stats::VIT) ? 0.9 : 1)));
    statPV = round(((2 * espece->getBasePV() + pvIV + (pvEV / 4)) * level) / 100)
             + level + 10;
}

bool OpMon::itemUsed(Item *used) {
    if ((espece->getEvolType()->getEvolID()) == Evolutions::EItem) {
            if (espece->getEvolType()->itemEvolve(used)) {
                    evolve();
                    return true;
                }
        }
    if (used->getItemTypeID() == ItemType::IHeal) {

            I_Heal *usedI = dynamic_cast<I_Heal *>(used);
            if (usedI->getPvHeal() > 0) {
                    heal(usedI->getPvHeal());

                }
            if (usedI->isHealAll() && status == Status::NOTHING) {
                    setStatus(Status::NOTHING);

                }
            else if (usedI->getStatusHeald() != Status::NOTHING && status == usedI->getStatusHeald()) {
                    setStatus(Status::NOTHING);
                    switch (usedI->getStatusHeald()) {
                            case Status::NOTHING:
                                break;
                            case Status::BURNING:
                                //keyout(key("status.brulure.heal"), surnom);
                                break;
                            case Status::FROZEN:
                                //keyout(key("status.gel.heal"), surnom);
                                break;
                            case Status::PARALYSED:
                                //keyout(key("status.para.heal"), surnom);
                                break;
                            case Status::POISONED:
                                //keyout(key("status.poison.heal"), surnom);
                                break;
                            case Status::SLEEPING:
                                //keyout(key("status.sommeil.heal"), surnom);
                                break;
                            default:
                                break;

                        }
                }



        }
    return false;
}

Item *OpMon::hold(Item *item) {
    Item *ancien = held;
    held = item;
    return ancien;
}

void OpMon::traded() {
    expBoost = 1.5;
    toolEvTrade();
}

void OpMon::toolEvTrade() {
    if (espece->getEvolType()->getEvolID() == Evolutions::ETrade) {
            evolve();
        }
}

void OpMon::evolve() {
  //bool changeName = (surnom == espece->getNom());
    espece = espece->getEvolution();
}

void OpMon::setStats(int stats[], Attaque *attacks[], Espece *espece, int types[]) {
    statATK = stats[0];
    statDEF = stats[1];
    statATKSPE = stats[2];
    statDEFSPE = stats[3];
    statVIT = stats[4];
    statPRE = 100;
    statESQ = 100;
    type1 = types[0];
    type2 = types[1];
    this->espece = espece;
    attaques[0] = attacks[0];
    attaques[1] = attacks[1];
    attaques[2] = attacks[2];
    attaques[3] = attacks[3];
    type1 = types[0];
    type2 = types[1];

}

void OpMon::attacked(int pvPerdus) {
    PV -= pvPerdus;
    PV = (PV < 0) ? 0 : PV;
}

bool OpMon::changeATK(int power) {
    if (power < 0) {
            for (int i = 0; i > power; i--) {
                    switch (atkChange) {
                            case -6:
                                return false;
                            case -5:

                                statATK = round(statATK / 1.16);
                                atkChange--;
                                break;
                            case -4:

                                statATK = round(statATK * 0.878788);
                                atkChange--;
                                break;
                            case -3:

                                statATK = round(statATK * 0.825);
                                atkChange--;
                                break;
                            case -2:

                                statATK = round(statATK / 1.25);
                                atkChange--;
                                break;
                            case -1:

                                statATK = round(statATK / 1.34);
                                atkChange--;
                                break;
                            case 0:

                                statATK = round(statATK * 0.67);
                                atkChange--;
                                break;
                            case 1:

                                statATK = round(statATK / 1.5);
                                atkChange--;
                                break;
                            case 2:

                                statATK = round(statATK * 0.75);
                                atkChange--;
                                break;
                            case 3:

                                statATK = round(statATK / 1.25);
                                atkChange--;
                                break;
                            case 4:

                                statATK = round(statATK / 1.2);
                                atkChange--;
                                break;
                            case 5:

                                statATK = round(statATK / 1.1667);
                                atkChange--;
                                break;
                            case 6:

                                statATK = round(statATK * 0.875);
                                atkChange--;
                                break;
                            default:
                                break;
                        }
                }
        }
    else {

            for (int i = 0; i > power; i--) {
                    switch (atkChange) {
                            case -6:

                                statATK = round(statATK * 1.16);
                                atkChange++;
                                break;
                            case -5:

                                statATK = round(statATK / 0.878788);
                                atkChange++;
                                break;
                            case -4:

                                statATK = round(statATK / 0.825);
                                atkChange++;
                                break;
                            case -3:

                                statATK = round(statATK * 1.25);
                                atkChange++;
                                break;
                            case -2:

                                statATK = round(statATK * 1.34);
                                atkChange++;
                                break;
                            case -1:

                                statATK = round(statATK / 0.67);
                                atkChange++;
                                break;
                            case 0:

                                statATK = round(statATK * 1.5);
                                atkChange++;
                                break;
                            case 1:

                                statATK = round(statATK / 0.75);
                                atkChange++;
                                break;
                            case 2:

                                statATK = round(statATK * 1.25);
                                atkChange++;
                                break;
                            case 3:

                                statATK = round(statATK * 1.2);
                                atkChange++;
                                break;
                            case 4:

                                statATK = round(statATK * 1.1667);
                                atkChange++;
                                break;
                            case 5:

                                statATK = round(statATK / 0.875);
                                atkChange++;
                                break;
                            case 6:
                                return false;
                            default:
                                break;
                        }
                }

        }

    return true;
}

bool OpMon::changePRE(int power) {
    if (power < 0) {
            for (int i = 0; i > power; i--) {
                    switch (preChange) {
                            case -6:
                                return false;
                            case -5:

                                statPRE = round(statPRE * 0.888);
                                preChange--;
                                break;
                            case -4:

                                statPRE = round(statPRE / 1.144);
                                preChange--;
                                break;
                            case -3:

                                statPRE = round(statPRE * 0.858);
                                preChange--;
                                break;
                            case -2:

                                statPRE = round(statPRE / 1.2);
                                preChange--;
                                break;
                            case -1:

                                statPRE = round(statPRE / 1.25);
                                preChange--;
                                break;
                            case 0:

                                statPRE = round(statPRE * 0.75);
                                preChange--;
                                break;
                            case 1:

                                statPRE = round(statPRE / 1.333);
                                preChange--;
                                break;
                            case 2:

                                statPRE = round(statPRE * 0.799640);
                                preChange--;
                                break;
                            case 3:

                                statPRE = round(statPRE * 0.8335);
                                preChange--;
                                break;
                            case 4:

                                statPRE = round(statPRE / 1.1665);
                                preChange--;
                                break;
                            case 5:

                                statPRE = round(statPRE * 0.8747657);
                                preChange--;
                                break;
                            case 6:

                                statPRE = round(statPRE * 0.889);
                                preChange--;
                                break;
                            default:
                                break;
                        }
                }
        }
    else {

            for (int i = 0; i > power; i--) {
                    switch (preChange) {
                            case -6:

                                statPRE = round(statPRE / 0.888);
                                preChange++;
                                break;
                            case -5:

                                statPRE = round(statPRE * 1.144);
                                preChange++;
                                break;
                            case -4:

                                statPRE = round(statPRE / 0.858);
                                preChange++;
                                break;
                            case -3:

                                statPRE = round(statPRE * 1.2);
                                preChange++;
                                break;
                            case -2:

                                statPRE = round(statPRE * 1.25);
                                preChange++;
                                break;
                            case -1:

                                statPRE = round(statPRE / 0.75);
                                preChange++;
                                break;
                            case 0:

                                statPRE = round(statPRE * 1.333);
                                preChange++;
                                break;
                            case 1:

                                statPRE = round(statPRE / 0.799640);
                                preChange++;
                                break;
                            case 2:

                                statPRE = round(statPRE / 0.8335);
                                preChange++;
                                break;
                            case 3:

                                statPRE = round(statPRE * 1.1665);
                                preChange++;
                                break;
                            case 4:

                                statPRE = round(statPRE / 0.8747657);
                                preChange++;
                                break;
                            case 5:

                                statPRE = round(statPRE / 0.889);
                                preChange++;
                                break;
                            case 6:
                                return false;
                            default:
                                break;
                        }
                }

        }

    return true;
}

bool OpMon::changeESQ(int power) {
    if (power < 0) {
            for (int i = 0; i > power; i--) {
                    switch (esqChange) {
                            case -6:
                                return false;
                            case -5:

                                statESQ = round(statESQ / 1.16);
                                esqChange--;
                                break;
                            case -4:

                                statESQ = round(statESQ * 0.878788);
                                esqChange--;
                                break;
                            case -3:

                                statESQ = round(statESQ * 0.825);
                                esqChange--;
                                break;
                            case -2:

                                statESQ = round(statESQ / 1.25);
                                esqChange--;
                                break;
                            case -1:

                                statESQ = round(statESQ / 1.34);
                                esqChange--;
                                break;
                            case 0:

                                statESQ = round(statESQ * 0.67);
                                esqChange--;
                                break;
                            case 1:

                                statESQ = round(statESQ / 1.5);
                                esqChange--;
                                break;
                            case 2:

                                statESQ = round(statESQ * 0.75);
                                esqChange--;
                                break;
                            case 3:

                                statESQ = round(statESQ / 1.25);
                                esqChange--;
                                break;
                            case 4:

                                statESQ = round(statESQ / 1.2);
                                esqChange--;
                                break;
                            case 5:

                                statESQ = round(statESQ / 1.1667);
                                esqChange--;
                                break;
                            case 6:

                                statESQ = round(statESQ * 0.875);
                                esqChange--;
                                break;
                            default:
                                break;
                        }
                }
        }
    else {

            for (int i = 0; i > power; i--) {
                    switch (esqChange) {
                            case -6:

                                statESQ = round(statESQ * 1.16);
                                esqChange++;
                                break;
                            case -5:

                                statESQ = round(statESQ / 0.878788);
                                esqChange++;
                                break;
                            case -4:

                                statESQ = round(statESQ / 0.825);
                                esqChange++;
                                break;
                            case -3:

                                statESQ = round(statESQ * 1.25);
                                esqChange++;
                                break;
                            case -2:

                                statESQ = round(statESQ * 1.34);
                                esqChange++;
                                break;
                            case -1:

                                statESQ = round(statESQ / 0.67);
                                esqChange++;
                                break;
                            case 0:

                                statESQ = round(statESQ * 1.5);
                                esqChange++;
                                break;
                            case 1:

                                statESQ = round(statESQ / 0.75);
                                esqChange++;
                                break;
                            case 2:

                                statESQ = round(statESQ * 1.25);
                                esqChange++;
                                break;
                            case 3:

                                statESQ = round(statESQ * 1.2);
                                esqChange++;
                                break;
                            case 4:

                                statESQ = round(statESQ * 1.1667);
                                esqChange++;
                                break;
                            case 5:

                                statESQ = round(statESQ / 0.875);
                                esqChange++;
                                break;
                            case 6:
                                return false;
                            default:
                                break;
                        }
                }

        }

    return true;
}

bool OpMon::changeDEF(int power) {
    if (power < 0) {
            for (int i = 0; i > power; i--) {
                    switch (defChange) {
                            case -6:
                                return false;
                            case -5:

                                statDEF = round(statDEF / 1.16);
                                defChange--;
                                break;
                            case -4:

                                statDEF = round(statDEF * 0.878788);
                                defChange--;
                                break;
                            case -3:

                                statDEF = round(statDEF * 0.825);
                                defChange--;
                                break;
                            case -2:

                                statDEF = round(statDEF / 1.25);
                                defChange--;
                                break;
                            case -1:

                                statDEF = round(statDEF / 1.34);
                                defChange--;
                                break;
                            case 0:

                                statDEF = round(statDEF * 0.67);
                                defChange--;
                                break;
                            case 1:

                                statDEF = round(statDEF / 1.5);
                                defChange--;
                                break;
                            case 2:

                                statDEF = round(statDEF * 0.75);
                                defChange--;
                                break;
                            case 3:

                                statDEF = round(statDEF / 1.25);
                                defChange--;
                                break;
                            case 4:

                                statDEF = round(statDEF / 1.2);
                                defChange--;
                                break;
                            case 5:

                                statDEF = round(statDEF / 1.1667);
                                defChange--;
                                break;
                            case 6:

                                statDEF = round(statDEF * 0.875);
                                defChange--;
                                break;
                            default:
                                break;
                        }
                }
        }
    else {

            for (int i = 0; i > power; i--) {
                    switch (defChange) {
                            case -6:

                                statDEF = round(statDEF * 1.16);
                                defChange++;
                                break;
                            case -5:

                                statDEF = round(statDEF / 0.878788);
                                defChange++;
                                break;
                            case -4:

                                statDEF = round(statDEF / 0.825);
                                defChange++;
                                break;
                            case -3:

                                statDEF = round(statDEF * 1.25);
                                defChange++;
                                break;
                            case -2:

                                statDEF = round(statDEF * 1.34);
                                defChange++;
                                break;
                            case -1:

                                statDEF = round(statDEF / 0.67);
                                defChange++;
                                break;
                            case 0:

                                statDEF = round(statDEF * 1.5);
                                defChange++;
                                break;
                            case 1:

                                statDEF = round(statDEF / 0.75);
                                defChange++;
                                break;
                            case 2:

                                statDEF = round(statDEF * 1.25);
                                defChange++;
                                break;
                            case 3:

                                statDEF = round(statDEF * 1.2);
                                defChange++;
                                break;
                            case 4:

                                statDEF = round(statDEF * 1.1667);
                                defChange++;
                                break;
                            case 5:

                                statDEF = round(statDEF / 0.875);
                                defChange++;
                                break;
                            case 6:
                                return false;

                            default:
                                break;
                        }
                }

        }
    return true;

}

bool OpMon::changeATKSPE(int power) {
    if (power < 0) {
            for (int i = 0; i > power; i--) {
                    switch (atkSpeChange) {
                            case -6:
                                return false;
                            case -5:

                                statATKSPE = round(statATKSPE / 1.16);
                                atkSpeChange--;
                                break;
                            case -4:

                                statATKSPE = round(statATKSPE * 0.878788);
                                atkSpeChange--;
                                break;
                            case -3:

                                statATKSPE = round(statATKSPE * 0.825);
                                atkSpeChange--;
                                break;
                            case -2:

                                statATKSPE = round(statATKSPE / 1.25);
                                atkSpeChange--;
                                break;
                            case -1:

                                statATKSPE = round(statATKSPE / 1.34);
                                atkSpeChange--;
                                break;
                            case 0:

                                statATKSPE = round(statATKSPE * 0.67);
                                atkSpeChange--;
                                break;
                            case 1:

                                statATKSPE = round(statATKSPE / 1.5);
                                atkSpeChange--;
                                break;
                            case 2:

                                statATKSPE = round(statATKSPE * 0.75);
                                atkSpeChange--;
                                break;
                            case 3:

                                statATKSPE = round(statATKSPE / 1.25);
                                atkSpeChange--;
                                break;
                            case 4:

                                statATKSPE = round(statATKSPE / 1.2);
                                atkSpeChange--;
                                break;
                            case 5:

                                statATKSPE = round(statATKSPE / 1.1667);
                                atkSpeChange--;
                                break;
                            case 6:

                                statATKSPE = round(statATKSPE * 0.875);
                                atkSpeChange--;
                                break;
                            default:
                                break;
                        }
                }
        }
    else {

            for (int i = 0; i > power; i--) {
                    switch (atkSpeChange) {
                            case -6:

                                statATKSPE = round(statATKSPE * 1.16);
                                atkSpeChange++;
                                break;
                            case -5:

                                statATKSPE = round(statATKSPE / 0.878788);
                                atkSpeChange++;
                                break;
                            case -4:

                                statATKSPE = round(statATKSPE / 0.825);
                                atkSpeChange++;
                                break;
                            case -3:

                                statATKSPE = round(statATKSPE * 1.25);
                                atkSpeChange++;
                                break;
                            case -2:

                                statATKSPE = round(statATKSPE * 1.34);
                                atkSpeChange++;
                                break;
                            case -1:

                                statATKSPE = round(statATKSPE / 0.67);
                                atkSpeChange++;
                                break;
                            case 0:

                                statATKSPE = round(statATKSPE * 1.5);
                                atkSpeChange++;
                                break;
                            case 1:

                                statATKSPE = round(statATKSPE / 0.75);
                                atkSpeChange++;
                                break;
                            case 2:

                                statATKSPE = round(statATKSPE * 1.25);
                                atkSpeChange++;
                                break;
                            case 3:

                                statATKSPE = round(statATKSPE * 1.2);
                                atkSpeChange++;
                                break;
                            case 4:

                                statATKSPE = round(statATKSPE * 1.1667);
                                atkSpeChange++;
                                break;
                            case 5:

                                statATKSPE = round(statATKSPE / 0.875);
                                atkSpeChange++;
                                break;
                            case 6:
                                return false;
                            default:
                                break;
                        }
                }

        }
    return true;
}

bool OpMon::changeDEFSPE(int power) {
    if (power < 0) {
            for (int i = 0; i > power; i--) {
                    switch (defSpeChange) {
                            case -6:
                                return false;
                            case -5:

                                statDEFSPE = round(statDEFSPE / 1.16);
                                defSpeChange--;
                                break;
                            case -4:

                                statDEFSPE = round(statDEFSPE * 0.878788);
                                defSpeChange--;
                                break;
                            case -3:

                                statDEFSPE = round(statDEFSPE * 0.825);
                                defSpeChange--;
                                break;
                            case -2:

                                statDEFSPE = round(statDEFSPE / 1.25);
                                defSpeChange--;
                                break;
                            case -1:

                                statDEFSPE = round(statDEFSPE / 1.34);
                                defSpeChange--;
                                break;
                            case 0:

                                statDEFSPE = round(statDEFSPE * 0.67);
                                defSpeChange--;
                                break;
                            case 1:

                                statDEFSPE = round(statDEFSPE / 1.5);
                                defSpeChange--;
                                break;
                            case 2:

                                statDEFSPE = round(statDEFSPE * 0.75);
                                defSpeChange--;
                                break;
                            case 3:

                                statDEFSPE = round(statDEFSPE / 1.25);
                                defSpeChange--;
                                break;
                            case 4:

                                statDEFSPE = round(statDEFSPE / 1.2);
                                defSpeChange--;
                                break;
                            case 5:

                                statDEFSPE = round(statDEFSPE / 1.1667);
                                defSpeChange--;
                                break;
                            case 6:

                                statDEFSPE = round(statDEFSPE * 0.875);
                                defSpeChange--;
                                break;
                            default:
                                break;
                        }
                }
        }
    else {

            for (int i = 0; i > power; i--) {
                    switch (defSpeChange) {
                            case -6:

                                statDEFSPE = round(statDEFSPE * 1.16);
                                defSpeChange++;
                                break;
                            case -5:

                                statDEFSPE = round(statDEFSPE / 0.878788);
                                defSpeChange++;
                                break;
                            case -4:

                                statDEFSPE = round(statDEFSPE / 0.825);
                                defSpeChange++;
                                break;
                            case -3:

                                statDEFSPE = round(statDEFSPE * 1.25);
                                defSpeChange++;
                                break;
                            case -2:

                                statDEFSPE = round(statDEFSPE * 1.34);
                                defSpeChange++;
                                break;
                            case -1:

                                statDEFSPE = round(statDEFSPE / 0.67);
                                defSpeChange++;
                                break;
                            case 0:

                                statDEFSPE = round(statDEFSPE * 1.5);
                                defSpeChange++;
                                break;
                            case 1:

                                statDEFSPE = round(statDEFSPE / 0.75);
                                defSpeChange++;
                                break;
                            case 2:

                                statDEFSPE = round(statDEFSPE * 1.25);
                                defSpeChange++;
                                break;
                            case 3:

                                statDEFSPE = round(statDEFSPE * 1.2);
                                defSpeChange++;
                                break;
                            case 4:

                                statDEFSPE = round(statDEFSPE * 1.1667);
                                defSpeChange++;
                                break;
                            case 5:

                                statDEFSPE = round(statDEFSPE / 0.875);
                                defSpeChange++;
                                break;
                            case 6:
                                return false;
                            default:
                                break;
                        }
                }

        }
    return true;
}

bool OpMon::changeVIT(int power) {
    if (power < 0) {
            for (int i = 0; i > power; i--) {
                    switch (vitChange) {
                            case -6:
                                return false;
                            case -5:

                                statVIT = round(statVIT / 1.16);

                                vitChange--;
                                break;
                            case -4:

                                statVIT = round(statVIT * 0.878788);
                                vitChange--;
                                break;
                            case -3:

                                statVIT = round(statVIT * 0.825);
                                vitChange--;
                                break;
                            case -2:

                                statVIT = round(statVIT / 1.25);
                                vitChange--;
                                break;
                            case -1:

                                statVIT = round(statVIT / 1.34);
                                vitChange--;
                                break;
                            case 0:

                                statVIT = round(statVIT * 0.67);
                                vitChange--;
                                break;
                            case 1:

                                statVIT = round(statVIT / 1.5);
                                vitChange--;
                                break;
                            case 2:

                                statVIT = round(statVIT * 0.75);
                                vitChange--;
                                break;
                            case 3:

                                statVIT = round(statVIT / 1.25);
                                vitChange--;
                                break;
                            case 4:

                                statVIT = round(statVIT / 1.2);
                                vitChange--;
                                break;
                            case 5:

                                statVIT = round(statVIT / 1.1667);
                                vitChange--;
                                break;
                            case 6:

                                statVIT = round(statVIT * 0.875);
                                vitChange--;
                                break;
                            default:
                                break;
                        }
                }
        }
    else {

            for (int i = 0; i > power; i--) {
                    switch (vitChange) {
                            case -6:

                                statVIT = round(statVIT * 1.16);
                                vitChange++;
                                break;
                            case -5:

                                statVIT = round(statVIT / 0.878788);
                                vitChange++;
                                break;
                            case -4:

                                statVIT = round(statVIT / 0.825);
                                vitChange++;
                                break;
                            case -3:

                                statVIT = round(statVIT * 1.25);
                                vitChange++;
                                break;
                            case -2:

                                statVIT = round(statVIT * 1.34);
                                vitChange++;
                                break;
                            case -1:

                                statVIT = round(statVIT / 0.67);
                                vitChange++;
                                break;
                            case 0:

                                statVIT = round(statVIT * 1.5);
                                vitChange++;
                                break;
                            case 1:

                                statVIT = round(statVIT / 0.75);
                                vitChange++;
                                break;
                            case 2:

                                statVIT = round(statVIT * 1.25);
                                vitChange++;
                                break;
                            case 3:

                                statVIT = round(statVIT * 1.2);
                                vitChange++;
                                break;
                            case 4:

                                statVIT = round(statVIT * 1.1667);
                                vitChange++;
                                break;
                            case 5:

                                statVIT = round(statVIT / 0.875);
                                vitChange++;
                                break;
                            case 6:
                                return false;
                            default:
                                break;
                        }
                }

        }
    return true;
}

bool OpMon::setStatus(int status) {
    if (status == Status::BURNING && this->status == Status::BURNING) {
            //System.out.println(surnom + " est déjà  brulé!");
            return false;
        }
    else if (status == Status::SLEEPING && this->status == Status::SLEEPING) {
            //System.out.println("Mais " + surnom + " dort déjà !");
            return false;
        }
    else if (status == Status::PARALYSED && this->status == Status::PARALYSED) {
            //System.out.println(surnom + " est déjà  paralysé!");
            return false;
        }
    else if (status == Status::FROZEN && this->status == Status::FROZEN) {
            //System.out.println(surnom + " est déjà  gelé!");
            return false;
        }
    else if (status == Status::POISONED && this->status == Status::POISONED) {
            //System.out.println(surnom + " est déjà  empoisonné!");
            return false;
        }
    else if (this->status != Status::NOTHING && status != Status::NOTHING) {
            //System.out.println("Mais " + surnom + " a déjà  un status!");
            return false;
        }
    else if (status == Status::BURNING) {
            changeATK(-1);
            atkChange++;
        }
    else if (status == Status::NOTHING && this->status == Status::BURNING) {
            changeATK(1);
            atkChange--;
        }
    else if (status == Status::PARALYSED) {
            changeVIT(-1);
            vitChange++;
        }
    else if (status == Status::NOTHING && this->status == Status::PARALYSED) {
            changeVIT(1);
            vitChange--;
        }
    this->status = status;
    return true;
}

void OpMon::heal(int PV) {
    if ((PV + this->PV) > statPV) {
            this->PV = statPV;
        }
    else {
            this->PV += PV;
        }
}

void OpMon::setType1(int type) {
    this->type1 = type;
}
void OpMon::setType2(int type) {
    this->type2 = type;
}
string OpMon::save() {
    if(!falsif) {

            ostringstream oss;
            oss << surnom << endl;
            oss << Save::intToChar(atkIV) << endl;
            oss << Save::intToChar(defIV) << endl;
            oss << Save::intToChar(atkSpeIV) << endl;
            oss << Save::intToChar(defSpeIV) << endl;
            oss << Save::intToChar(vitIV) << endl;
            oss << Save::intToChar(pvIV) << endl;
            oss << Save::intToChar(atkEV) << endl;
            oss << Save::intToChar(defEV) << endl;
            oss << Save::intToChar(atkSpeEV) << endl;
            oss << Save::intToChar(defSpeEV) << endl;
            oss << Save::intToChar(vitEV) << endl;
            oss << Save::intToChar(pvEV) << endl;
            oss << Save::intToChar(statLove) << endl;
            oss << Save::intToChar(level) << endl;
            oss << Save::intToChar(caractere.id) << endl;
            /*for(unsigned int it = 0; it < 4; it++){
                cout << "Attaque : " << it << " Pointer : " << attaques[it] << endl;
                Attaque *atk = attaques[it];
                cout << "Pointer : " << atk << endl;
                if(atk != NULL){
                    cout << "Attaque no NULL" << endl;
                    oss << atk->save();
                }else{
                    cout << "Attaque NULL" << endl;
                    oss << "NULL" << endl;
                }
            }*/
            if(attaques[0] != NULL) {
                    oss << attaques[0]->save();
                }
            else {
                    oss << "NULL" << endl;
                }
            if(attaques[1] != NULL) {
                    oss << attaques[1]->save();
                }
            else {
                    oss << "NULL" << endl;
                }
            if(attaques[2] != NULL) {
                    oss << attaques[2]->save();
                }
            else {
                    oss << "NULL" << endl;
                }
            if(attaques[3] != NULL) {
                    oss << attaques[3]->save();
                }
            else {
                    oss << "NULL" << endl;
                }
            oss << Save::intToChar(espece->getOpdexNumber()) << endl;
            oss << Save::intToChar(PV) << endl;
            oss << Save::intToChar(exp) << endl;
            oss << Save::intToChar(toNextLevel) << endl;
            oss << Save::intToChar(expBoost * 10) << endl;
            if(held != NULL) {
                    oss << "Y" << endl;
                    oss << Save::intToChar(held->getID()) << endl;
                }
            else {
                    oss << "N" << endl;
                    oss << endl;
                }
            oss << Save::intToChar(tauxCapture) << endl;
            return oss.str();
        }
    else {
            return "NULL\n";
        }

}

OpMon::OpMon(ifstream &in) {
    this->surnom = Save::readLine(in);
    if(surnom != "NULL") {
            atkIV = in.get();
            in.get();
            defIV = in.get();
            in.get();
            atkSpeIV = in.get();
            in.get();
            defSpeIV = in.get();
            in.get();
            vitIV = in.get();
            in.get();
            pvIV = in.get();
            in.get();
            atkEV = in.get();
            in.get();
            defEV = in.get();
            in.get();
            atkSpeEV = in.get();
            in.get();
            defSpeEV = in.get();
            in.get();
            vitEV = in.get();
            in.get();

            pvEV = in.get();
            in.get();
            statLove = in.get();
            in.get();

            level = in.get();
            in.get();
            int toSearch = in.get();
            caractere = *Caractere::enumsList[toSearch];
            in.get();
            attaques[0] = Attaques::newAtk(Save::readLine(in));
            if(attaques[0] != NULL) {
                    attaques[0]->setPP(in.get());
                    in.get();
                    attaques[0]->setPPMax(in.get());
                    in.get();
                }

            attaques[1] = Attaques::newAtk(Save::readLine(in));
            if(attaques[1] != NULL) {
                    attaques[1]->setPP(in.get());
                    in.get();
                    attaques[1]->setPPMax(in.get());
                    in.get();
                }

            attaques[2] = Attaques::newAtk(Save::readLine(in));
            if(attaques[2] != NULL) {
                    attaques[2]->setPP(in.get());
                    in.get();
                    attaques[2]->setPPMax(in.get());
                    in.get();
                }
            attaques[3] = Attaques::newAtk(Save::readLine(in));
            if(attaques[3] != NULL) {
                    attaques[3]->setPP(in.get());
                    in.get();
                    attaques[3]->setPPMax(in.get());
                    in.get();
                }
            int especeID = in.get();
            espece = Initializer::listeOp[especeID];
            in.get();
            PV = in.get();
            in.get();
            exp = in.get();
            in.get();
            toNextLevel = in.get();
            in.get();
            expBoost = in.get() / 10.0;
            in.get();
            if(Save::intToChar(in.get()) == 'Y') {
                    in.get();
                    held = Item::itemsLst[in.get()];
                    in.get();
                }
            else {
                    in.get();
                    held = NULL;
                    in.get();
                }
            tauxCapture = in.get();
            in.get();
            calcStats();
            type1 = espece->getType1();
            type2 = espece->getType2();
            statPRE = 100;
            statESQ = 100;
            falsif = false;
        }
    else {
            falsif = true;
        }


}


