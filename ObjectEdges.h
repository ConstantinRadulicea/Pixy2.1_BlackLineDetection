#pragma once

#include<list>
#include <unordered_map>
#include <string>

typedef struct PixelCoordinates {
	int x;
	int y;
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
		std::size_t h1 = std::hash<int>{}(s.x);
		std::size_t h2 = std::hash<int>{}(s.y);
		return h1 ^ (h2 << 1); // or use boost::hash_combine
	}
};

// Custom hash can be a standalone function object.
struct MyHash
{
	std::size_t operator()(const PixelCoordinates& s) const noexcept
	{
		std::size_t h1 = std::hash<int>{}(s.x);
		std::size_t h2 = std::hash<int>{}(s.y);
		return h1 ^ (h2 << 1); // or use boost::hash_combine
	}
};



class ObjectEdges
{
public:
	ObjectEdges(){}
	~ObjectEdges(){}

	std::list<PixelCoordinates>::iterator getPixelAtCoordinates(int x, int y) {
		std::list<PixelCoordinates>::iterator it = std::find(edgePixels.begin(), edgePixels.end(), PixelCoordinates{x, y});
		return it;
	}
	std::list<PixelCoordinates>::iterator begin() { return edgePixels.begin(); }
	std::list<PixelCoordinates>::iterator end() { return edgePixels.end(); }
	size_t size() { return edgePixels.size(); }
	

private:
	std::list<PixelCoordinates> edgePixels;
};


