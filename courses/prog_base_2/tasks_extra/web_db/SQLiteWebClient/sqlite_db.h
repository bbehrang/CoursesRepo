#pragma once

#include <sqlite3.h>

#include <string>
#include <iostream>
#include <vector>
#include <map>

class sqlite_db {
private:
	sqlite3 *db;
	std::string file_name;
	char *zErrMsg = 0;
	static std::vector<std::vector<std::pair<std::string, std::string>>> tmp_sql_results;
	static int callback(void *NotUsed, int argc, char **argv, char **azColName);
public:
	sqlite_db(std::string fname);
	int open();
	void close();
	int get_all_table_names(std::vector<std::string>& table_names);
	int get_table_data(std::string table_name, std::vector<std::vector<std::pair<std::string,std::string>>>& sql_res, int limit, int offset);
	int delete_table_row(std::string table_name, std::string row_id);
	//int edit_table_row(std::string table_name, std::string row_id);
	int insert_row_into_table(std::string table_name, std::map<std::wstring, std::wstring> row_data);
	int edit_row_into_table(std::string table_name, std::string row_id,std::map<std::wstring, std::wstring> row_data);
};