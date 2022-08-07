#ifndef CONTROL_H
#define CONTROL_H

#include<SFML/Graphics.hpp>
#include<vector>

using namespace std;
using namespace sf;


class Controller {
private:
	uint32_t key_states;
	vector<Keyboard::Key> keys;
	/* data */
public:
	void press_key() {
		key_states = 0;
		for (size_t button = 0; button < keys.size(); button++) {
			key_states |= (Keyboard::isKeyPressed(keys[button])) << button;
		}
	}
	void setKeys(vector<Keyboard::Key>& keys_) { keys = keys_; }
	uint32_t& read_key(){return key_states;}
	Controller(/* args */) :key_states(0), keys(0) {};
	~Controller() {}
};

#endif