#pragma once

#include <libpq-fe.h>
#include <libpq-events.h>
#include <libpq\libpq-fs.h>

#include <memory>
#include <functional>

#include "typedefs.hpp"

namespace Postgres{
	using ResultPtr = std::unique_ptr<PGresult, std::function<void(PGresult *)>>;
	inline ResultPtr MakeResult(PGconn *conn, cstrref query){
		return ResultPtr(PQexec(conn, query.c_str()), PQclear);
	}

	using ConnectionPtr = std::unique_ptr<PGconn, std::function<void(PGconn *)>>;
	inline ConnectionPtr MakeConnection(cstrref host, cstrref port, cstrref db, cstrref login, cstrref password){
		return ConnectionPtr(
			PQsetdbLogin(host.c_str(), port.c_str(), NULL, NULL, db.c_str(), login.c_str(), password.c_str()), PQfinish
			);
	}

	class Database{
	public:
		using ResultTable = std::vector<std::vector<std::string>>;
	public:
		struct ConnectionException : std::runtime_error
		{ ConnectionException(cstrref what) : std::runtime_error(what){} };
		struct EmptyQuery : std::runtime_error
		{ EmptyQuery(cstrref what) : std::runtime_error(what){} };
		struct BadResponse : std::runtime_error
		{ BadResponse(cstrref what) : std::runtime_error(what){} };
		struct FatalError : std::runtime_error
		{ FatalError(cstrref what) : std::runtime_error(what){} };
		struct ResultNotAvaiable : std::runtime_error
		{ ResultNotAvaiable(cstrref what) : std::runtime_error(what){} };
	public:
		Database(){}
		Database(cstrref host, cstrref port, cstrref db, cstrref login, cstrref pwd)
		{
			connect(host, port, db, login, pwd);
		}
		Database(const Database &) = delete;
	public:
		void connect(cstrref host, cstrref port, cstrref db, cstrref login, cstrref pwd){
			_conn = MakeConnection(host, port, db, login, pwd);
			if (PQstatus(_conn.get()) != CONNECTION_OK){
				auto msg = std::string(PQerrorMessage(_conn.get())));
				_conn = nullptr;
				throw ConnectionException(msg);
			}
		}

		Database &execute(cstrref query){
			_hasResult = false;
			_result = MakeResult(_conn.get(), query);
			std::string error = PQerrorMessage(_conn.get());
			_status = PQresultStatus(_result.get());
			if (_status == PGRES_EMPTY_QUERY) throw EmptyQuery(error);
			if (_status == PGRES_BAD_RESPONSE) throw BadResponse(error);
			if (_status == PGRES_FATAL_ERROR) throw FatalError(error);
			if (_status == PGRES_TUPLES_OK) _hasResult = true;
			return *this;
		}

		ResultTable collectResult(){
			if (_status != PGRES_TUPLES_OK)
				throw ResultNotAvaiable("This command does not support results.");
			if (!_hasResult)
				throw ResultNotAvaiable("Any results avaiable.");
			ResultTable result;
			size_t n_tuples = PQntuples(_result.get()), n_fields = PQnfields(_result.get());
			for (size_t y = 0; y < n_tuples; ++y){
				ResultTable::value_type row;
				for (size_t x = 0; x < n_fields; ++x)
					row.push_back(PQgetvalue(_result.get(), y, x));
				result.push_back(row);
			}
			return result;
		}
	private:
		bool _hasResult;
		ExecStatusType _status;
		ConnectionPtr _conn;
		ResultPtr _result;
	};
}