#pragma once

#include <array>
#include <vector>

template<class Container> auto
inline accessElement(Container &container, size_t position)
-> decltype(*std::begin(container)) &{
	return reinterpret_cast<decltype(*std::begin(container)) &>(
		std::begin(container) + position
	);
}

template<
	class HeaderDataContainer = std::array<int16_t, 4>, 
	class BodyDataContainer = std::vector<char>
>
struct Packet{
	HeaderDataContainer header;
	BodyDataContainer body;
};