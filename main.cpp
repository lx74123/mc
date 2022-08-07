#include <SFML/Graphics.hpp>

#include"render.h"
#include"control.h"
#include"search.h"

#define DEBUG

#ifdef DEBUG
#include <cassert>
#endif

using namespace sf;

#define scopeSize { 10 , 10 }
#define unitSize 50
#define CircleSize 20.0f

// C++ not zero is true, not need compare
// #define GET_KEY_COMPARE(keys_states, key) ((keys_states & (1 << key)) == (1 << key))
#define GET_KEY(keys_states, key) ((keys_states & (1 << key)))


unordered_map<uint, unordered_map<Pos, Pos>(WeightsGrid::*)() > searchFucPtr;

unordered_map<uint, string> strSearchType;


enum {
	_Breadth,
	_Dijkstra,
	_Astar,
};

uint searchType = _Breadth;

bool flag_refresh_window = true;
bool flag_close_window = false;
bool flag_press_key = false;

Controller listen_keys;

enum Keys {
	W,//up
	S,//down
	A,//left
	D,//right
	L,//add wall or delete wall
	J,//add start position
	K,//add goal position
	N,//discovery next step
	B,//discovery previous step
	U,//add weight
	I,//decrease weight
	R,//change search type

};

vector<Keyboard::Key> keys{
	Keyboard::W,
	Keyboard::S,
	Keyboard::A,
	Keyboard::D,
	Keyboard::L,
	Keyboard::J,
	Keyboard::K,
	Keyboard::N,
	Keyboard::B,
	Keyboard::U,
	Keyboard::I,
	Keyboard::R,

};

// typedef struct Position {
	// /* data */
	// union {
		// int x;
		// int width;
	// };
	// union {
		// int y;
		// int hight;
	// };
// }Pos;

void loop_listen_keys(RenderWindow* rw_ptr) {
	while (rw_ptr->isOpen()) {
		Event event;
		while (rw_ptr->waitEvent(event)) {
			if (event.type == Event::Closed) {
				flag_close_window = true;
				return;
			}
			if (event.type == Event::Resized) {
				Vector2u now;
				now.x = event.size.width;
				now.y = event.size.height;
				rw_ptr->setSize(now);
				flag_refresh_window = true;
			}
			if (event.type == Event::KeyPressed) {
				listen_keys.press_key();
				flag_press_key = true;
			}
		}
	}
}

int main() {
	// RenderWindow window(VideoMode(200, 200), "SFML works!");
	CircleShape shape(100.f);
	shape.setFillColor(Color::Green);
	shape.setPosition(200, 200);

	Font font;
	font.loadFromFile("./BarlowCondensed-Regular.ttf");
	Text t;
	t.setFont(font);
	t.setString("ok");
	t.setPosition(250, 250);

	CustomDraw cd;
	RenderWindow* rw_ptr = cd.getRenderWindow();

	Texture texSprite;
	texSprite.loadFromFile("./cursor.png");

	Sprite sprite;
	sprite.setTexture(texSprite);
	sprite.setPosition(100, 100);

	Pos current = { 1,1 };

	listen_keys.setKeys(keys);
	Thread t_listen_keys(&loop_listen_keys, rw_ptr);
	t_listen_keys.launch();

	WeightsGrid graph;
	graph.setScope(scopeSize);

	bool insert = graph.setWall({ 5,5 });

#ifdef DEBUG
	assert(insert);
#endif

	searchFucPtr[_Breadth] = graph.breadth_search_ptr;
	searchFucPtr[_Dijkstra] = graph.dijkstra_search_ptr;
	searchFucPtr[_Astar] = graph.aStar_search_ptr;

	strSearchType[_Breadth] = "breadth";
	strSearchType[_Dijkstra] = "Dijkstra";
	strSearchType[_Astar] = "Astar";

	uint32_t nowStep = 0;
	while (1)
	{
		Time delta = milliseconds(10);
		sleep(delta);
		if (flag_close_window) {
			rw_ptr->close();
			break;
		}
		uint32_t keys_states = listen_keys.read_key();
		if (flag_press_key) {
			Pos temp = current;

			if (GET_KEY(keys_states, Keys::W)) {
				temp.y -= 1;
			}
			if (GET_KEY(keys_states, Keys::S)) {
				temp.y += 1;
			}
			if (GET_KEY(keys_states, Keys::A)) {
				temp.x -= 1;
			}
			if (GET_KEY(keys_states, Keys::D)) {
				temp.x += 1;
			}
			//change search type
			if (GET_KEY(keys_states, Keys::R)) {
				searchType++;
				(graph.*searchFucPtr[searchType % strSearchType.size()])();
			}
			if (GET_KEY(keys_states, Keys::L)) {
				if (graph.inBounds(temp)) {
					graph.setWall(temp);
					(graph.*searchFucPtr[searchType % strSearchType.size()])();
					nowStep = 0;
				}
			}
			if (GET_KEY(keys_states, Keys::J)) {
				if (graph.inBounds(temp) && graph.passable(temp))
					graph.setStart(temp);
				(graph.*searchFucPtr[searchType % strSearchType.size()])();
				nowStep = 0;
			}
			if (GET_KEY(keys_states, Keys::K)) {
				if (graph.inBounds(temp) && graph.passable(temp))
					graph.setGoal(temp);
				(graph.*searchFucPtr[searchType % strSearchType.size()])();
				nowStep = 0;
			}
			if (GET_KEY(keys_states, Keys::N)) {
				if (nowStep < graph.getDiscoveryPath().size())
					nowStep++;
			}
			if (GET_KEY(keys_states, Keys::B)) {
				if (nowStep != 0)
					nowStep--;
			}
			//add weight
			if (GET_KEY(keys_states, Keys::U)) {
				if (graph.inBounds(temp))
					(*graph.getWeight())[temp] += 1;
				(graph.*searchFucPtr[searchType % strSearchType.size()])();
			}
			//dec weight
			if (GET_KEY(keys_states, Keys::I)) {
				if (graph.inBounds(temp))
					if ((*graph.getWeight())[temp] != 0)
						(*graph.getWeight())[temp] -= 1;
				(graph.*searchFucPtr[searchType % strSearchType.size()])();
			}
			if (graph.inBounds(temp)) {
				current = temp;
				flag_press_key = false;
				flag_refresh_window = true;
			}
		}
		if (flag_refresh_window) {
			sprite.setPosition(current.x * unitSize, current.y * unitSize);

			if (!graph.getWalls().empty()) {
				for (Pos pos : graph.getWalls()) {
					CircleShape circle(CircleSize);
					circle.setFillColor(Color::Cyan);
					circle.setPosition(pos.x * unitSize, pos.y * unitSize);
					cd.push_object(_Shape, circle);
					/* code */
				}

			}
			////////////////////////////////////////////////////////////
			/// draw discovery path
			////////////////////////////////////////////////////////////
			vector<Pos> discoveryPath = graph.getDiscoveryPath();
			if (!discoveryPath.empty()) {
				int i = 0;
				for (Pos next : discoveryPath)
				{
					if (i > nowStep)
						break;
					Text text;
					text.setCharacterSize(20);
					text.setFont(font);
					text.setPosition(next.x * unitSize, next.y * unitSize);
					text.setFillColor(Color::Red);
					text.setString(to_string(i++));
					cd.push_object(_Text, text);
				}
			}
			//draw start position

			Pos start = graph.getStart();
			if (graph.inBounds(start) && graph.passable(start)) {
				CircleShape circle(CircleSize);
				circle.setFillColor(Color::Green);
				circle.setPosition(start.x * unitSize, start.y * unitSize);
				cd.push_object(_Sprite, circle);
			}

			// draw goal position

			Pos goal = graph.getGoal();
			if (graph.inBounds(goal) && graph.passable(goal)) {
				CircleShape circle(CircleSize);
				circle.setFillColor(Color::Blue);
				circle.setPosition(goal.x * unitSize, goal.y * unitSize);
				cd.push_object(_Sprite, circle);
			}
			////////////////////////////////////////////////////////////
			/// draw link path
			////////////////////////////////////////////////////////////
			vector<Pos> linkPath = graph.getLinkPath();
			if (!linkPath.empty()) {
				for (Pos pos : linkPath) {
					CircleShape circle(CircleSize);
					circle.setFillColor(Color::Magenta);
					circle.setPosition(pos.x * unitSize, pos.y * unitSize);
					cd.push_object(_Sprite, circle);
					/* code */
				}

			}
			////////////////////////////////////////////////////////////
			/// draw weight
			////////////////////////////////////////////////////////////
			unordered_map<Pos, uint>* weights = graph.getWeight();
			if (!(*weights).empty()) {
				for (auto weight : (*weights)) {
					if (!weight.second)
						continue;
					Text text;
					text.setCharacterSize(20);
					text.setFont(font);
					text.setPosition(weight.first.x * unitSize + 10, weight.first.y * unitSize + 10);
					text.setFillColor(Color::Yellow);
					text.setString(to_string(weight.second));
					cd.push_object(_Text, text);
				}
			}
			////////////////////////////////////////////////////////////
			/// draw text search type 
			////////////////////////////////////////////////////////////
			Text text;
			text.setCharacterSize(20);
			text.setFont(font);
			text.setPosition(graph.getGoal().x * unitSize + 10, graph.getGoal().y * unitSize + 10);
			text.setFillColor(Color::White);
			text.setString(strSearchType[searchType % strSearchType.size()]);
			cd.push_object(_Text, text);



			cd.push_object(_Sprite, sprite);
			// cd.push_object(_Shape, shape);
			cd.push_object(_Text, t);
			cd.draw();
			flag_refresh_window = false;
		}

		// window.clear();
		// window.draw(shape);
		// window.display();
	}

	return 0;
}