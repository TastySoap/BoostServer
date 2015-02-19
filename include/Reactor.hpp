#pragma once

#include <vector>
#include <functional>

template<class TPacket = Packet<>>
using Reaction = std::function<Message<TPacket>(const Message<TPacket> &)>;

template<class TPacket = Packet<>>
using IndexBasedReactions = std::vector<Reaction<TPacket>>;


template<class TPacket = Packet<>>
using Validation = std::function<bool(const Message<TPacket> &)>;

template<class TPacket = Packet<>>
using IndexBasedValidations = std::vector<Validation<TPacket>>;

template<
	class TPacket = Packet<>,
	template<class> class TMessage = Message,
	template<class> class TIndexBasedReactions = IndexBasedReactions,
	template<class> class TIndexBasedValidations = IndexBasedValidations
>
class Reactor final{
public:
	using PacketType = TPacket;
	using MessageType = TMessage<PacketType>;
	using IndexBasedReactions = TIndexBasedReactions<PacketType>;
	using IndexBasedValidations = TIndexBasedValidations<PacketType>;
	using IBRs = IndexBasedReactions;
	using IBVs = IndexBasedValidations;
public:
	Reactor(const IBRs &ibrs, const IBVs &ibvs) :
		_indexBasedReactions(ibrs),
		_indexBasedValidations(ibvs)
	{
		assert(ibrs.size() == ibvs.size() && "Lbrs and Lbvs must be the same size!");
	}

	Reactor(IBRs &&ibrs, IBVs &&ibvs) :
		_indexBasedReactions(move(ibrs)),
		_indexBasedValidations(move(ibvs))
	{
		assert(ibrs.size() == ibvs.size() && "Lbrs and Lbvs must be the same size!");
	}
public:
	inline MessageType processMessage(const MessageType &message){
		for (size_t i = 0; i < _indexBasedValidations.size(); ++i)
			if (validate(i, message))
				return react(i, message);
		return echo(message);
	}
private:
	inline bool validate(size_t id, const MessageType &message){
		return _indexBasedValidations.at(id)(message);
	}
	inline MessageType react(size_t id, const MessageType &message){
		return _indexBasedReactions.at(id)(message);
	};
	inline MessageType echo(const MessageType &message){
		return message;
	}
private:
	IBRs _indexBasedReactions;
	IBVs _indexBasedValidations;
};