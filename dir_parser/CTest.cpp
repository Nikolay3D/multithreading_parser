#include "CTest.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>

namespace fs = std::filesystem;

//static const string lorem{"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Venenatis cras sed felis eget velit aliquet sagittis id consectetur. Neque vitae tempus quam pellentesque. Donec massa sapien faucibus et molestie ac. Sed id semper risus in hendrerit gravida. Morbi non arcu risus quis varius quam. Vitae semper quis lectus nulla at volutpat diam. Risus at ultrices mi tempus. Tortor vitae purus faucibus ornare suspendisse sed. Quam adipiscing vitae proin sagittis nisl rhoncus mattis. Sed blandit libero volutpat sed cras ornare arcu dui. Quam id leo in vitae turpis massa."};
static const string lorem{ "Lorem ipsum dolor sit amet consectetur adipiscing elit potenti mollis lobortis urna in, massa dis dictum ridiculus montes erat aenean accumsan ante molestie vestibulum. Volutpat phasellus facilisi accumsan ultricies in ridiculus litora taciti integer risus, nascetur eros proin libero sociosqu at congue consequat elit curae sagittis, laoreet fames aenean netus dui nec est lacus class. Amet porttitor proin auctor ridiculus neque tempus sed at diam mus cubilia suspendisse pulvinar mattis, commodo integer non blandit convallis metus netus ut ultrices consequat ullamcorper nisl. Curae lectus dictum quam sagittis nulla lobortis pellentesque orci facilisis, habitant odio porttitor consequat ultricies at ipsum sodales, arcu rutrum eros parturient per suspendisse ridiculus bibendum. Eleifend accumsan rutrum risus nam eu vitae sodales neque mollis convallis, augue posuere libero imperdiet ultrices volutpat iaculis pulvinar non, ut scelerisque duis dolor magna quam cubilia vestibulum aptent. Est faucibus venenatis senectus aliquam inceptos ornare metus, non leo vitae eleifend dis etiam lacus pulvinar, sociosqu hac tellus nam quam nec. Placerat nullam venenatis a fames sociosqu elementum leo primis ultrices consectetur, iaculis inceptos et curabitur posuere non cum luctus fringilla, aliquam pulvinar ornare etiam congue mi dolor lobortis purus. Pretium gravida adipiscing eu dignissim consectetur nostra nulla, ad integer odio ante quisque magnis.Mus nulla himenaeos auctor iaculis sociis orci vulputate, et vehicula proin tempus morbi quam sodales, curabitur placerat tristique mauris porttitor tortor.Rhoncus maecenas nostra lorem ornare eu sagittis tincidunt convallis proin risus eleifend malesuada etiam, placerat pretium molestie inceptos duis in commodo luctus magna condimentum tortor.Ante hendrerit aptent faucibus mauris imperdiet consectetur sociosqu, ultricies montes aliquet vel semper diam non, purus ligula molestie iaculis ornare tellus.Tempor facilisi cubilia diam ad leo pulvinar justo luctus, scelerisque nulla pellentesque urna elit venenatis eu aptent dapibus, faucibus volutpat massa pharetra integer ligula porta.Netus sem tincidunt platea malesuada mi natoque himenaeos amet, nascetur mattis arcu proin ultricies ad at nam, porta tristique odio nulla nullam donec in.Nulla nascetur dapibus curae lorem taciti, libero primis suscipit leo eu, fermentum pharetra fusce tristique. Commodo rhoncus iaculis vel ultricies ullamcorper amet mauris semper, laoreet torquent volutpat ornare fusce sagittis duis praesent orci, interdum leo magnis turpis vitae ligula vivamus.Rutrum felis pellentesque gravida donec nibh per, ac ipsum sodales mollis leo aptent montes, habitasse inceptos potenti ad arcu.Libero hendrerit egestas sociis volutpat fusce habitant porttitor congue, turpis eu lobortis nascetur scelerisque donec integer et, vitae nam condimentum faucibus quam magnis cum.Fringilla bibendum tortor vitae penatibus tincidunt facilisis eu vehicula, nec metus libero cum suspendisse viverra.Tellus nibh augue integer sagittis est a in lacus, cum porta ligula vestibulum hac metus dolor diam dapibus, eleifend venenatis class phasellus curae ridiculus fames.Condimentum diam velit posuere dolor lobortis vulputate montes lacinia, est sit dictum metus vel consequat viverra, malesuada tempor nascetur augue dui ridiculus sed.Malesuada suscipit mus ad odio pellentesque pretium egestas leo, elit lacinia integer vel duis pulvinar viverra litora, convallis eros morbi ac nulla dapibus praesent.Vestibulum placerat dictumst convallis potenti mi elementum quam consequat auctor et class, fusce tortor porttitor curae condimentum metus semper parturient hac donec rhoncus lorem, ligula dignissim volutpat platea erat dolor scelerisque ullamcorper sit adipiscing. Pellentesque netus gravida natoque turpis leo magna dictum convallis class, praesent ultricies massa hendrerit erat facilisi sodales mattis neque maecenas, adipiscing aenean tempus quisque ultrices aptent commodo cursus.Litora mi elementum turpis magnis dictumst dis placerat fringilla, diam non rutrum ultricies ridiculus vitae magna mus auctor, semper mattis ullamcorper tempor aliquet at consequat.Aenean justo sit aliquam maecenas a dignissim vulputate ante himenaeos, suspendisse velit congue fringilla tempor in class turpis volutpat, elit orci sociis nostra senectus lacus nibh amet.Laoreet pretium semper habitant curabitur ornare, malesuada sagittis senectus praesent.Vel congue tempor arcu quam in accumsan natoque mattis nec ut, risus sodales taciti quisque feugiat cursus pulvinar class dignissim vitae, luctus cum eleifend penatibus sagittis ornare massa lacinia sem.Tempor rhoncus porta semper platea eros ligula nec quisque adipiscing aliquam fusce habitant non, auctor convallis dictum congue suscipit lorem commodo consectetur risus ullamcorper senectus.Facilisis sociis convallis luctus odio lobortis bibendum nullam interdum, phasellus nulla gravida aenean orci venenatis eleifend sagittis nibh, leo quam sodales consectetur nascetur senectus tellus." };

static const string delimeter_symbols{"@#$%^&*()):;"};

void CTest::generate_files(string path, int count, string path_etalon)
{
	

	fs::path in_dir = fs::absolute(path);
	const int NUM_DELIM_PER_FILE{ 5 };
	const int NUM_SYMB_PER_DELIMETER{ 3 };
	const int INSERT_DELIMETERS_MAX_STEP{ 30 };
	
	default_random_engine generator;
	generator.seed(time(nullptr));
	uniform_int_distribution<int> distribution_step(0, INSERT_DELIMETERS_MAX_STEP);
	uniform_int_distribution<int> distribution_delim_symb(0, (int)delimeter_symbols.length()-1);
	uniform_int_distribution<int> distribution_text_len((int)lorem.length()/5, (int)lorem.length() - 1);


	fstream file_etalon_stream;
	string filepath_etalon = format("{}\\Test_etalon.txt", path_etalon);
	file_etalon_stream.open(filepath_etalon, fstream::out);
	if (file_etalon_stream.fail()) {
		cout << "Error: " << filepath_etalon << " failed to open for write." << endl;
		return;
	}

	for (int file_cnt = 0; file_cnt < count; ++file_cnt) {
		vector<string> delimeters;
		for (int delim_cnt = 0; delim_cnt < NUM_DELIM_PER_FILE; ++delim_cnt) {
			string delim;
			for (int i = 0; i < NUM_SYMB_PER_DELIMETER; ++i) {
				
				delim += delimeter_symbols[distribution_delim_symb(generator)];
			}
			delimeters.push_back(delim);
		}
		
		uniform_int_distribution<int> distribution_delim_in_file(0, (int)delimeters.size()-1);


		vector<string> file_etalon;
		string str_for_parse;

		int pos = 0;
		int text_len = distribution_text_len(generator);
		while (pos < text_len) {
			int fragment_len = distribution_step(generator);
			string part = lorem.substr(pos, fragment_len);
			file_etalon.push_back(part);
			str_for_parse += part + delimeters[distribution_delim_in_file(generator)];
			pos += fragment_len;
		}

		string filepath = format("{}\\Test_{}.txt", in_dir.string(), file_cnt);

		file_etalon_stream << filepath << "\n";
		for (auto& str : file_etalon) {
			if (!str.empty()) {
				file_etalon_stream << str << "\n";
			}			
		}

		fstream file_stream;
		
		file_stream.open(filepath, fstream::out);
		if (file_stream.fail()) {
			cout << "Error: " << filepath << " failed to open for write." << endl;
			return;
		}
		file_stream << str_for_parse;
		for (auto &it : delimeters)
		{
			file_stream << "\n" << it;
		}
		file_stream.close();
		

		
	}

	file_etalon_stream.close();
}
