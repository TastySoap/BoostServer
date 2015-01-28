#pragma once

#include <iostream>
#include <array>
#include <vector>
#include <cstring>
#include <cstdint>

struct Packet{
	struct Header{
		static const size_t SIZE = 24;
		enum Position : std::uint32_t{ 
			VERSION = 0, PACKET_TYPE = VERSION+1,  DATA_TYPE = PACKET_TYPE+1, 
			ADDITIONAL_INFOS_BEGIN = DATA_TYPE+1, ADDITIONAL_INFOS_END = SIZE
		};

		std::array<std::uint8_t, SIZE> data;
		
		inline auto version() const -> std::uint8_t
		{ return data[Position::VERSION]; }
		inline auto version() -> uint8_t &
		{ return data[Position::VERSION]; }
		
		inline auto packetType() const -> std::uint8_t
		{ return data[Position::PACKET_TYPE]; }
		inline auto packetType() -> uint8_t &
		{ return data[Position::PACKET_TYPE]; }
		
		inline auto dataType() const -> std::uint8_t
		{ return data[Position::DATA_TYPE]; }
		inline auto dataType() -> uint8_t &
		{ return data[Position::DATA_TYPE]; }
		
		inline auto begin() const-> decltype(std::cbegin(data))
		{ return data.begin() + Position::ADDITIONAL_INFOS_BEGIN; }
		inline auto begin() -> decltype(std::begin(data))
		{ return data.begin() + Position::ADDITIONAL_INFOS_BEGIN; }
		
		inline auto end() const -> decltype(std::cend(data))
		{ return data.begin() + Position::ADDITIONAL_INFOS_END; }
		inline auto end() -> decltype(std::end(data))
		{ return data.begin() + Position::ADDITIONAL_INFOS_END; }
	} header;
	
	struct Body{
		std::vector<uint8_t> data;
		inline void push(std::uint8_t val)
		{ data.push_back(val); }
		template<typename InputIterator>
		inline void push(InputIterator first, InputIterator last)
		{ data.insert(std::end(data), first, last); } 
		
		inline auto begin() const -> decltype(std::cbegin(data))
		{ return data.begin(); }
		inline auto begin() -> decltype(std::begin(data))
		{ return data.begin(); }
		
		inline auto end() const -> decltype(std::cend(data))
		{ return data.end(); }
		inline auto end() -> decltype(std::end(data))
		{ return data.end(); }
	} body;
};