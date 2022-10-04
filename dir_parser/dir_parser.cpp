#include <iostream>
#include <filesystem>
#include <fstream>
#include <windows.h>
#include <format>
#include "CParser.h"
#include "CTest.h"

using namespace std;
namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
	if (argc < 3) {
		cout << "Arg cnt:" << argc << ". Need args : directory output_file_path [number test files]" << std::endl;
		return 0;
	}
	
	// ----- подготовка входных данных -----------------------------------------------

	fs::path in_dir = fs::absolute(argv[1]);
	fs::path out_file = fs::absolute(argv[2]);

	if (!fs::exists(in_dir)) {
		std::cout << "Directory " << in_dir << " not exist" << std::endl;
		return 0;
	}

	// ----- набор файлов для теста -------------------------------------------------------------------
	if (argc == 4) {
		int num = 0;
		try
		{
			num = stoi(argv[3]);
		}
		catch (const std::exception&)
		{

		}
		
		if (num > 0) {
			CTest::generate_files(in_dir.string(), num, out_file.parent_path().string());
		}
		
	}
	// --------------------------------------------------------------------------------

	fstream file_stream;
	file_stream.open(out_file, fstream::out);
	if (file_stream.fail()) {
		cout << "Thr: " << this_thread::get_id() << " " << out_file << " failed to open for write." << endl;
		return 1;
	}
		
	vector<string> files_str;
	for (const auto& entry : fs::directory_iterator(in_dir)) {
		if (entry.is_regular_file()) {
			files_str.push_back(entry.path().string());
		}
	}

	// ----- подготовка набора парсеров -----------------------------------------------

	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);

	int numCPU = sysinfo.dwNumberOfProcessors;
	cout << "main thread: " << this_thread::get_id() << " numCPU: " << numCPU << endl;


	vector<CParser*> parsers;
	for (int i = 0; i < numCPU; ++i) {
		parsers.push_back(new CParser(format("THR_{}", i)));
	}

	// ----- обработка файлов -----------------------------------------------

	// передача всего списка файлов парсерам, чтение запись результатов
	while (!files_str.empty()) {
		for (int i = 0; i < parsers.size(); ++i) {
			/*if (!parsers.at(i)->is_busy() && !files_str.empty()) {
				parsers.at(i)->queue_add(files_str.back());
				files_str.pop_back();
			}*/

			// Не смотря на то, что в потоке есть очередь и можно в нее отправить сразу много файлов на обработку
			// файлы добавляются по одному, чтобы обработка выполнялось равномернее и файлы не стояли в очереди,
			// когда к.н. из потоков уже освободился и готов их обработать.
			if (parsers.at(i)->queue_is_empty() && !files_str.empty()) {
				parsers.at(i)->queue_add(files_str.back());
				files_str.pop_back();
			}

			if (parsers.at(i)->has_out()) {
				file_stream << parsers.at(i)->pop_parsed();
				file_stream.flush();
			}
		}
	}

	// ожидание окончания работы парсеров
	while (1) {
		bool exit = true;

		for (int i = 0; i < parsers.size(); ++i) {		

			if (parsers.at(i)->has_out()) {
				file_stream << parsers.at(i)->pop_parsed();
				file_stream.flush();
			}

			if (parsers.at(i)->is_busy()) {
				exit = false;
			}
		}

		if (exit) {
			break;
		}
	}


	// ----- освобождение ресурсов -----------------------------------------------

	while(!parsers.empty()) {
		delete parsers.back();
		parsers.pop_back();
	}

	file_stream.close();	
}