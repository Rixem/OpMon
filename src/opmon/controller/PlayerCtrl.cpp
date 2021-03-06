#include "PlayerCtrl.hpp"

#include "EventsCtrl.hpp"
#include "../model/storage/Data.hpp"

namespace OpMon{
  namespace Controller{
    void PlayerCtrl::checkMove(Model::Player& player, sf::Event const& events, View::Overworld & overworld){
      if(events.type == sf::Event::KeyPressed){
	if(events.key.code == sf::Keyboard::F5) {
	  overworld.tp("Mom's room", sf::Vector2i(0, 1));
	} else if(events.key.code == sf::Keyboard::F6) {
	  overworld.tp("Player's room", sf::Vector2i(0, 0));
	} else if(events.key.code == sf::Keyboard::F1) {
	  overworld.tp("Fauxbourg Euvi", sf::Vector2i(25, 28));
	} else if(events.key.code == sf::Keyboard::F2) {
	  overworld.tp("Player's home", sf::Vector2i(8, 14));
	} else if(events.key.code == sf::Keyboard::F3) {
	  overworld.tp("Laboratory", sf::Vector2i(15, 14));
	} else if(events.key.code == sf::Keyboard::F4) {
	  overworld.tp("Rival's house", sf::Vector2i(8, 14));
	} else if(events.key.code == sf::Keyboard::F7) {
	  overworld.tp("Route 14", sf::Vector2i(0, 31));
	} else if(events.key.code == sf::Keyboard::F8) {
	  overworld.tp("MysteriouCity", sf::Vector2i(12, 0));
	}
      }

      if(!overworld.justTp && !player.getPosition().isAnim()) {
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
	  overworld.setStartFrames();
	  player.getPosition().move(Model::Side::TO_UP);
	} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
	  overworld.setStartFrames();
	  player.getPosition().move(Model::Side::TO_DOWN);
	} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
	  overworld.setStartFrames();
	  player.getPosition().move(Model::Side::TO_LEFT);
	} else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
	  overworld.setStartFrames();
	  player.getPosition().move(Model::Side::TO_RIGHT);
	}
      }

      
    }

    void move(Model::Side direction, Model::Player& player, View::Overworld& overworld){
      player.getPosition().move(direction);
      
      Model::Map *map = Model::Data::World::maps.at(player.getMapId());
      auto eventList = map->getEvent(player.getPosition().getPosition());
      EventsCtrl::actionEvents(eventList, player, Model::Events::EventTrigger::GO_IN, overworld);
    }
  }
}
