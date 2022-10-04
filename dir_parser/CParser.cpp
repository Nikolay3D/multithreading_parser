#include "CParser.h"
#include <iostream>
#include <fstream>
#include <string>
#include <format>
#include <sstream>

using namespace std;

/*
* �������� ���� ������
*/
void CParser::run()
{	
	while (1) {
        this_thread::sleep_for(chrono::milliseconds(1));

        if (has_out()) { continue; }

		unique_lock<mutex> ul(m_queue_mutex);

		if (!m_queue_data.empty()) {
			m_is_busy.store(true);
			string filepath = m_queue_data.front();
			m_queue_data.pop();
			ul.unlock();

            unique_lock<mutex> ul(m_splitted_mutex);
            m_splitted_data = parse(filepath);
            m_splitted_data.insert(m_splitted_data.begin(), filepath);

            m_processing_cnt++;

			m_is_busy.store(false);
		}

		if (m_stop.load()) {
			break;
		}

	} // while (1)
}

CParser::CParser(string name)
{
	m_name = name;
	start();
}

CParser::~CParser()
{
    finish();
	thread::id id = m_th->get_id();
	m_th->join();
    cout << format("Finish thread {}: {}. Processing: {}", m_name, m_th_id, m_processing_cnt) << endl;
}

/*
* � ������� ������������ ������� ������ ��� ���������.
* ������� ������ ���� � ��� �������
*/
void CParser::queue_add(string filepath)
{
    unique_lock<mutex> ul(m_queue_mutex);
	m_queue_data.push(filepath);
}

/*
* true: ������� ������ �� ��������� ������
*/
bool CParser::queue_is_empty() {
    unique_lock<mutex> ul(m_queue_mutex);
    return m_queue_data.empty();
}

/*
* true: ���� ������ ����� ���������� � ������ ������ ��� � ��� ������� ���� ���� ��� ���������
*/
//TODO ����������: �������� �������, �������� ���������?
bool CParser::is_busy()
{
	unique_lock<mutex> ul(m_queue_mutex);
	return m_is_busy.load() || !m_queue_data.empty();
}


/*
* true: ������������ �������� ������
*/
bool CParser::has_out()
{
    unique_lock<mutex> ul(m_splitted_mutex);
    return !m_splitted_data.empty();
}

/*
* ������� ������������ ������
* return: �����, �������������� � ������ � ���� �����������
*/
string CParser::pop_parsed()
{
    unique_lock<mutex> ul(m_splitted_mutex);

    string join;
    for (auto& s : m_splitted_data)
    {
        join += s + "\n";
    }

    m_splitted_data.clear();

    return join;
}

/*
* ��������� ������ ������ � ��������� ��������� ��������
*/
void CParser::finish()
{
	m_stop.store(true);
}

/*
* ������ ��������� ����� ������
*/
void CParser::start()
{
	m_th = new thread(&CParser::run, this);
    auto myid = m_th->get_id();
    stringstream ss;
    ss << myid;
    m_th_id = ss.str();
    cout << format("Create thread {}: {}", m_name, m_th_id) << endl;
}

/*
* ������� �����
* filepath: ���� � �����
* return: ������ �� ����������� ����� 
*/
vector<string> CParser::parse(const string filepath)
{
    ifstream file;
    file.open(filepath);

    if (file.fail())
    {
        cout << m_name << ": " << m_th->get_id() << " " << filepath << " failed to open." << endl;
        return vector<string>{};
    }

    int current_line = 0;

    struct {
        string data;
        vector<string> delimeters;
    }extracted;
    

    while (!file.eof())
    {
        current_line++;

        if (current_line == 1) {
            getline(file, extracted.data);
        }
        else {
            string temp;
            getline(file, temp);
            if (!temp.empty()) {
                extracted.delimeters.push_back(temp);
            }

        }
    }

    file.close();
    
    return split(extracted.data, extracted.delimeters);
}

/*
* ���������� �� ���������
* str: �������� ������
* delim: ����� �����-������������
* return: ������ �� ����������� �����
*/
vector<string> CParser::split(string str, vector<string> delim) {
    size_t pos = 0;
    string token;
    vector<string> out;

    if (delim.empty()) { return vector<string>{str}; }

    string curr_delim = delim.front();
    delim.erase(delim.begin());

    // ���������� �� �������� �����������
    while ((pos = str.find(curr_delim)) != std::string::npos) {
        token = str.substr(0, pos);
        vector<string> data = split(token, delim);
        for (auto& str : data) {
            if (!str.empty()) {
                out.push_back(str);
            }
        }
        str.erase(0, pos + curr_delim.length());
    }

    // ������� ������ ����� ��������� �� ��������� �����������
    if (!str.empty()) {
        vector<string> data = split(str, delim);
        for (auto& str : data) {
            if (!str.empty()) {
                out.push_back(str);
            }
        }
    }

    return out;
}