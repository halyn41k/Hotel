#ifndef HOTELCONNECTING_H
#define HOTELCONNECTING_H

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

using namespace std;
using namespace sql;

class HotelConnecting {
public:
	static unique_ptr<Connection> con;
	void connectToDatabase();
	sql::Connection& getConnection();

};
#endif // HOTELCONNECTING_H
