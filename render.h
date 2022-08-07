#ifndef RENDER_H
#define RENDER_H

#include<SFML/Graphics.hpp>

#include<vector>
#include<queue>

using namespace std;
using namespace sf;

enum Layer {
	_Zero,
	_Shape,
	_Sprite,
	_Text,
	_Total,
};

// vector sort function setting
template<typename T>
int custom_sort_up(pair<Layer, T> a, pair<Layer, T> b) {
	return a.first < b.first;
}

using vCircle = vector<pair<Layer, CircleShape> >;
using vSprite = vector<pair<Layer, Sprite> >;
using vText = vector<pair<Layer, Text> >;

class CustomDraw {
private:
	RenderWindow rw;
	/* data */
	vCircle circle;
	vSprite sprite;
	vText text;
public:
	RenderWindow* getRenderWindow() { return &rw; }

	void clear() {
		circle.clear();
		sprite.clear();
		text.clear();
	}

	void draw() {

		rw.clear();
		////////////////////////////////////////////////////////////
		/// sort container
		////////////////////////////////////////////////////////////
		sort(circle.begin(), circle.end(), custom_sort_up<CircleShape>);
		sort(sprite.begin(), sprite.end(), custom_sort_up<Sprite>);
		sort(text.begin(), text.end(), custom_sort_up<Text>);
		////////////////////////////////////////////////////////////
		/// draw
		////////////////////////////////////////////////////////////
		for (size_t layer_ = _Zero; layer_ < _Total; layer_++) {
			/* code */
			if (!circle.empty())
				for (auto iter = circle.begin(); iter != circle.end(); iter++)
					if ((*iter).first == layer_)
						rw.draw((*iter).second);

			if (!sprite.empty())
				for (auto iter = sprite.begin(); iter != sprite.end(); iter++)
					if ((*iter).first == layer_)
						rw.draw((*iter).second);

			if (!text.empty())
				for (auto iter = text.begin(); iter != text.end(); iter++)
					if ((*iter).first == layer_)
						rw.draw((*iter).second);

		}
		rw.display();
		clear();
	}

	////////////////////////////////////////////////////////////
	/// push object
	////////////////////////////////////////////////////////////
	void push_object(Layer layer_, CircleShape circle_) { circle.push_back({ layer_ ,circle_ }); }
	void push_object(Layer layer_, Sprite sprite_) { sprite.push_back({ layer_ ,sprite_ }); }
	void push_object(Layer layer_, Text text_) { text.push_back({ layer_ ,text_ }); }
	CustomDraw(/* args */) { rw.create(VideoMode(600, 600), "test"); }
	~CustomDraw() {};
};



#endif