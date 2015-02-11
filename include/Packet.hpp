#pragma once

#include <iostream>
#include <array>
#include <vector>

template<class Container> auto
inline accessElement(Container &container, size_t position)
-> decltype(*std::begin(container)) &{
	return reinterpret_cast<decltype(*std::begin(container)) &>(
		std::begin(container) + position
		);
}

template<class HeaderDataContainer, class BodyDataContainer>
struct Packet{
	HeaderDataContainer header;
	BodyDataContainer body;
};