#include "geohash.h"
#include <iostream>
#include <utility>
#include <vector>
#include <ostream>
#include <sstream>


struct Place{
	GeoHash::Point		point;
	std::string      	name;
	double			    supposed_distance;
	GeoHash::buffer_t	buffer;
	std::string     	hash = GeoHash::encode(point, 12, buffer);
};

Place const places[] {
	{ { 42.69210669738513, 23.32489318092748 }, "Level Up"			,   10 },
	{ { 42.69238509863727, 23.32476304836964 }, "Luchiano"			,   42 },
	{ { 42.69175842097124, 23.32523636032996 }, "Bohemian Hall"		,   42 },
	{ { 42.69269382372572, 23.32491086796810 }, "Umamido"			,   74 },
	{ { 42.69199298885176, 23.32411779452691 }, "McDonald's"		,   74 },
	{ { 42.69141486008349, 23.32517469905046 }, "Ceiba"			,   74 },
	{ { 42.69210516803149, 23.32626916568055 }, "Happy"			,  110 },
	{ { 42.69151957227103, 23.32366568780231 }, "Ugo"			,  130 },
	{ { 42.69151957227103, 23.32654779876320 }, "Il Theatro"		,  140 },
	{ { 42.69205716858902, 23.32266435016691 }, "Nicolas"			,  200 },
	{ { 42.68970992959111, 23.32455331244497 }, "Manastirska Magernitsa"	,  300 },
	{ { 42.69325388808822, 23.32118939756011 }, "The Hadjidragana Tavern"	,  330 },
	{ { 42.69174997126510, 23.32100561258516 }, "Boho"			,  330 },
	{ { 42.69205885495273, 23.33781274367048 }, "Stastlivetza"		,  400 },
	{ { 42.68999839888917, 23.31970366848597 }, "Franco's Pizza"		,  500 },
	{ { 42.69074017093236, 23.31785351529929 }, "Villa Rosiche"		,  620 },
	{ { 42.68342508736217, 23.31633975360111 }, "Chevermeto"		, 1200 }
};

GeoHash::Point const test_points[] {
	{ 44.968046,   -94.420307  },
	{ 44.33328,    -89.132008  },
	{ 33.755787,   -116.359998 },
	{ 33.844843,   -116.54911  },
	{ 44.92057,    -93.44786   },
	{ 44.240309,   -91.493619  },
	{ 44.968041,   -94.419696  },
	{ 44.333304,   -89.132027  },
	{ 33.755783,   -116.360066 },
	{ 33.844847,   -116.549069 },
	{ 44.920474,   -93.447851  },
	{ 44.240304,   -91.493768  }
};

constexpr GeoHash::Point me{ 42.69207945916779, 23.325029867108032 };

namespace{

	std::ostream& operator<<(std::ostream& os, GeoHash::Point& arg)
    {
        os << arg.lat << " " << arg.lon;
        return os;
    }

    std::ostream& operator<<(std::ostream& os, GeoHash::Rectangle& arg)
    {
    	os << arg.sw.lat << " " << arg.sw.lon << " | " << arg.ne.lat << " " << arg.ne.lon;
        return os;
    }

    std::string to_string(GeoHash::Point& arg)
    {
        std::ostringstream ss;
        ss << arg;
        return std::move(ss).str();
    }

    std::string to_string(GeoHash::Rectangle& arg)
    {
        std::ostringstream ss;
        ss << arg;
        return std::move(ss).str();
    }

	void mySearchNearby(GeoHash::Point me, double radius, GeoHash::GeoSphere const &sphere){
		auto const cells = GeoHash::nearbyCells(me, radius, sphere);

		for(auto &hash : cells){
			std::cout << "Searching cell: " << hash << '\n';

			// send query to DB
			for(auto &p : places){
				if (p.hash.size() < hash.size() || std::equal(std::begin(hash), std::end(hash), std::begin(p.hash)) == false)
					continue;

				auto const dist = distance(me, p.point, sphere);

				if (dist > radius)
					continue;

				std::cout << '\t' << p.name << '\t' << dist << ' ' << sphere.units << '\n';
			}
		}
	}

	void myEncodeDecode(){
		GeoHash::buffer_t  buffer;
		std::vector<GeoHash::Rectangle> rectangles = {};
		std::vector<std::string> geohashes = {};
		std::vector<GeoHash::Point> lat_lon = {};

		for(auto &coord : test_points){
			lat_lon.push_back(coord);
			std::string geohash_addr = GeoHash::encode(coord, 20, buffer);
			std::cout << geohash_addr << '\n';
			geohashes.push_back(geohash_addr);
		}
		std::cout << '\n' << "test if appears correctly in a vector" << '\n';
		for (int i = 0; i < geohashes.size(); i++) {
        std::cout << geohashes[i] << " " << '\n';
    	}
    	std::cout << '\n' << "encode these back into rectangles, print out mbr points" << '\n';
    	for (int i = 0; i < geohashes.size(); i++) {
        rectangles.push_back(GeoHash::decode(geohashes[i]));
    	}
    	for (int i = 0; i < geohashes.size(); i++) {
        std::cout << to_string(rectangles[i].sw) << " "  << to_string(rectangles[i].ne) << '\n';
    	}
    	std::cout << '\n' << "test centroid method" << '\n';
    	lat_lon.clear();
    	for (int i = 0; i < geohashes.size(); i++) {
        lat_lon.push_back(GeoHash::decode_centroid(geohashes[i]));
    	}
    	for (int i = 0; i < geohashes.size(); i++) {
        std::cout << to_string(rectangles[i]) << " " << '\n';
    	}
	}
}

// if for some strange reason we want distances in decameters...
//constexpr auto UNITS = GeoHash::GeoSphere{ "Earth", "dm", GeoHash::EARTH_METERS.radius / 10 };
//constexpr auto UNITS = GeoHash::EARTH_YA;

constexpr auto UNITS = GeoHash::EARTH_METERS;

int main(){
	// 80 m - 3x3
	// 60 m - 2x3
	// 50 m - 2x2
	// 12 m - 3x2
	auto const radius = 80;

	//mySearchNearby(me, radius, UNITS);

	std::cout << "\n" << "Testing my own Encode and Decode:" << "\n";

	myEncodeDecode();
}