#pragma once

#include<list>
#include <unordered_map>
#include <string>
#include <iostream>

typedef struct PixelCoordinates {
	int16_t x;
	int16_t y;
	bool operator==(const PixelCoordinates& other) const
	{
		return (x == other.x
			&& y == other.y);
	}
}PixelCoordinates;


// Custom specialization of std::hash can be injected in namespace std.
template<>
struct std::hash<PixelCoordinates>
{
	std::size_t operator()(const PixelCoordinates& s) const noexcept
	{
		std::size_t h1 = std::hash<int16_t>{}(s.x);
		std::size_t h2 = std::hash<int16_t>{}(s.y);
		return h1 ^ (h2 << 1); // or use boost::hash_combine
	}
};

// Custom hash can be a standalone function object.
struct MyHash
{
	std::size_t operator()(const PixelCoordinates& s) const noexcept
	{
		std::size_t h1 = std::hash<int16_t>{}(s.x);
		std::size_t h2 = std::hash<int16_t>{}(s.y);
		return h1 ^ (h2 << 1); // or use boost::hash_combine
	}
};



class ObjectEdges
{
public:
	ObjectEdges(){}
	~ObjectEdges(){}

	std::list<PixelCoordinates>::iterator getPixelAtCoordinates(int16_t x, int16_t y) {
		std::list<PixelCoordinates>::iterator it = std::find(edgePixels.begin(), edgePixels.end(), PixelCoordinates{x, y});
		return it;
	}
	void setEdges(std::unordered_map<PixelCoordinates, bool> edges) {
		edgePixels.clear();
		
		for (auto it = edges.begin(); it != edges.end();)	// remove elements form edges if are not pixel edges
		{
			if (it->second == false)
			{
				it = edges.erase(it);
			}
			else {
				++it;
			}
		}


		if (edges.empty()){
			return;
		}

		PixelCoordinates curEdge;
		bool upIsEdge, downIsEdge, leftIsEdge, rightIsEdge;
		curEdge = edges.begin()->first;
		edges.erase(edges.begin());
		edgePixels.push_back(curEdge);
		auto it_back = edgePixels.end();

		while (!edges.empty()) {
			it_back = edgePixels.end();
			
			(edges[PixelCoordinates{ it_back->x, (int16_t)(it_back->y + 1) }] == true) ? upIsEdge = true : upIsEdge = false;
			(edges[PixelCoordinates{ it_back->x, (int16_t)(it_back->y - 1) }] == true) ? downIsEdge = true : downIsEdge = false;
			(edges[PixelCoordinates{ (int16_t)(it_back->x + 1), it_back->y }] == true) ? rightIsEdge = true : rightIsEdge = false;
			(edges[PixelCoordinates{ (int16_t)(it_back->x - 1), it_back->y }] == true) ? leftIsEdge = true : leftIsEdge = false;

			if (!upIsEdge && !downIsEdge && rightIsEdge && leftIsEdge)
			{

			}
		}
	}
	std::list<PixelCoordinates>::iterator begin() { return edgePixels.begin(); }
	std::list<PixelCoordinates>::iterator end() { return edgePixels.end(); }
	size_t size() { return edgePixels.size(); }
	

private:
	std::list<PixelCoordinates> edgePixels;
};


