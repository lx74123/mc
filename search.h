#ifndef SEARCH_H
#define SEARCH_H

#include<unordered_map>
#include<unordered_set>
#include<vector>
#include<queue>

using namespace std;

typedef struct Position {
	/* data */
	union {
		int x;
		int width;
	};
	union {
		int y;
		int hight;
	};
	bool operator==(const Position& pos)const noexcept {
		return x == pos.x && y == pos.y;
	}
	bool operator!=(const Position& pos)const noexcept {
		return x != pos.x || y != pos.y;
	}
	Position operator+(const Position& pos) noexcept {
		return Position{ x + pos.x , y + pos.y };
	}
}Pos;

namespace std {
	template<> struct hash<Position> {
		size_t operator()(const Position& id) const noexcept {
			//return custom struct hash value
			return hash<int>() (id.x & (id.y << 16));
		}
	};
}

// priority_queue sort function - small to big
struct custom_sort_queue {
	bool operator() (pair<int, Pos> a, pair<int, Pos> b) {
		return a.first > b.first;
	}
};

class SquareGrid {
protected:
	static array<Pos, 4> Dirs;
	Pos scope;
	Pos start;
	Pos goal;
	unordered_set<Pos> walls;
	unordered_map<Pos, Pos> came_from;
	vector<Pos > discoveryPath;
	vector<Pos > linkPath;
	bool existLinkPath;
	/* data */
public:
	unordered_set<Pos> getWalls() { return walls; }
	void setScope(Pos scope_) { scope = scope_; }
	void setStart(Pos start_) { start = start_; }
	void setGoal(Pos goal_) { goal = goal_; }

	Pos getStart() { return start; }
	Pos getGoal() { return goal; }


	vector<Pos> getLinkPath() { return linkPath; }
	vector<Pos> getDiscoveryPath() { return discoveryPath; }

	unordered_map<Pos, Pos> breadth_search() {
		if (inBounds(start) && passable(start) &&
			inBounds(goal) && passable(goal) && start != goal)
			return breadth_search(start, goal);
		return came_from;
	}
	////////////////////////////////////////////////////////////
	/// \brief 
	/// \param start start location in graph
	/// \param goal goal location in graph
	/// \return came_from
	////////////////////////////////////////////////////////////

	unordered_map<Pos, Pos> breadth_search(Pos start_, Pos goal_) {
		queue<Pos> frontier;
		frontier.push(start_);

		discoveryPath.clear();
		discoveryPath.push_back(start_);

		came_from.clear();
		came_from[start_] = start_;

		existLinkPath = false;
		linkPath.clear();
		while (!frontier.empty()) {
			Pos current = frontier.front();
			frontier.pop();

			if (current == goal) {
				existLinkPath = true;
				reconstructPath();
				break;
			}

			for (Pos next : neighbors(current)) {
				// position is not exist ,add to map
				if (came_from.find(next) == came_from.end()) {
					discoveryPath.push_back(next);
					came_from[next] = current;
					frontier.push(next);
				}
			}
		}
		return came_from;
	}
	vector<Pos> reconstructPath() {
		Pos current = goal;
		while (current != start) {
			linkPath.push_back(current);
			current = came_from[current];
		}
		linkPath.erase(linkPath.begin());
		return linkPath;
	}

	vector<Pos> neighbors(Pos pos) {
		vector<Pos> results;
		for (Pos dir : Dirs) {
			Pos next = pos + dir;
			if (inBounds(next) && passable(next))
				results.push_back(next);
		}
		return results;
	}
	////////////////////////////////////////////////////////////
	/// \return insert is true
	////////////////////////////////////////////////////////////
	bool setWall(Pos pos) {
		if (walls.find(pos) == walls.end()) {
			walls.insert(pos);
			return true;
		}
		else {
			walls.erase(pos);
			return false;
		}
	}
	////////////////////////////////////////////////////////////
	/// \return  position is not in walls - True
	////////////////////////////////////////////////////////////
	bool passable(Pos pos) {
		return walls.find(pos) == walls.end();
	}
	bool inBounds(Pos pos) {
		return pos.x <= scope.width&& pos.x > 0 &&
			pos.y <= scope.hight&& pos.y > 0;
	}
	SquareGrid(/* args */) {}
	~SquareGrid() {}
};
array<Pos, 4> SquareGrid::Dirs = {
		Pos{1,0},
		Pos{-1,0},
		Pos{0,1},
		Pos{0,-1},
};
class WeightsGrid :public SquareGrid {
private:
	unordered_map<Pos, uint> weights;
	unordered_map<Pos, uint> costSoFar;
	/* data */

	////////////////////////////////////////////////////////////
	/// \brief a * core code
	///
	/// \param  a start position
	/// \param  b start position
	///
	/// \return abs(a.x - b.x) + abs(a.y - b.y);
	////////////////////////////////////////////////////////////
	inline uint heuristic(Pos a, Pos b) {
		return abs(a.x - b.x) + abs(a.y - b.y);
	}
public:
	unordered_map<Pos, Pos>(WeightsGrid::* breadth_search_ptr)();
	unordered_map<Pos, Pos>(WeightsGrid::* dijkstra_search_ptr)();
	unordered_map<Pos, Pos>(WeightsGrid::* aStar_search_ptr)();

	unordered_map<Pos, uint>* getWeight() { return &weights; }

	unordered_map<Pos, Pos> dijkstra_search() {
		if (inBounds(start) && passable(start) &&
			inBounds(goal) && passable(goal) && start != goal)
			return dijkstra_search(start, goal);
		return came_from;
	}

	unordered_map<Pos, Pos> dijkstra_search(Pos start_, Pos goal_) {
		priority_queue<pair<uint, Pos>, vector<pair<uint, Pos> >, custom_sort_queue> frontier;
		frontier.push({ 0,start_ });

		discoveryPath.clear();
		discoveryPath.push_back(start_);

		came_from.clear();
		came_from[start_] = start_;

		costSoFar.clear();
		costSoFar[start_] = 0;


		existLinkPath = false;
		linkPath.clear();
		while (!frontier.empty()) {
			Pos current = frontier.top().second;
			frontier.pop();

			if (current == goal) {
				existLinkPath = true;
				reconstructPath();
				break;
			}

			for (Pos next : neighbors(current)) {
				uint newCost = costSoFar[current] + unitCost(next);
				// position is not exist ,add to map
				if (came_from.find(next) == came_from.end() || newCost < costSoFar[next]) {
					discoveryPath.push_back(next);
					came_from[next] = current;
					costSoFar[next] = newCost;
					frontier.push({ newCost, next });
				}
			}
		}
		return came_from;
	}
	uint unitCost(Pos pos) {
		return weights[pos] + 1;
	}

	unordered_map<Pos, Pos> aStar_search() {
		if (inBounds(start) && passable(start) &&
			inBounds(goal) && passable(goal) && start != goal)
			return aStar_search(start, goal);
		return came_from;
	}


	unordered_map<Pos, Pos> aStar_search(Pos start_, Pos goal_) {
		priority_queue<pair<uint, Pos>, vector<pair<uint, Pos> >, custom_sort_queue> frontier;
		frontier.push({ 0,start_ });

		discoveryPath.clear();
		discoveryPath.push_back(start_);

		came_from.clear();
		came_from[start_] = start_;

		costSoFar.clear();
		costSoFar[start_] = 0;


		existLinkPath = false;
		linkPath.clear();
		while (!frontier.empty()) {
			Pos current = frontier.top().second;
			frontier.pop();

			if (current == goal) {
				existLinkPath = true;
				reconstructPath();
				break;
			}

			for (Pos next : neighbors(current)) {
				uint newCost = costSoFar[current] + unitCost(next);
				// position is not exist ,add to map
				if (came_from.find(next) == came_from.end() || newCost < costSoFar[next]) {
					discoveryPath.push_back(next);
					came_from[next] = current;
					costSoFar[next] = newCost;
					frontier.push({ heuristic(next ,goal_) + newCost, next });
				}
			}
		}
		return came_from;
	}

	WeightsGrid(/* args */) {
		breadth_search_ptr = &WeightsGrid::breadth_search;
		dijkstra_search_ptr = &WeightsGrid::dijkstra_search;
		aStar_search_ptr = &WeightsGrid::aStar_search;
	}
	~WeightsGrid() {}
};


#endif