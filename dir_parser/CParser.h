#pragma once
#include <thread>
#include <queue>
#include <mutex>

using namespace std;

class CParser
{
	string m_name;
	thread *m_th;
	string m_th_id;
	void run();
	void start();

	vector<string> split(string str, vector<string> delim);
	vector<string> parse(const string filepath);

	mutex m_queue_mutex;
	queue<string> m_queue_data;

	atomic<bool> m_is_busy = false;
	atomic<bool> m_stop = false;

	mutex m_splitted_mutex;
	vector<string> m_splitted_data;

	int m_processing_cnt = 0;

public:
	CParser(string name);
	~CParser();

	void queue_add(string filepath);
	bool queue_is_empty();
	bool is_busy();
	bool has_out();
	string pop_parsed();
	void finish();	
};

