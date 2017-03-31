 
#include "HMM.h"

int HMM_MODEL_CHOOSE = MODEL_FROM_DATA;
int TEST_TYPE = TEST_STR;

bool model_data_loaded = false;
bool model_trainfile_loaded = false;

map<string, double> prob_emit[STATE_NUM];
double prob_start[STATE_NUM];
double prob_trans[STATE_NUM][STATE_NUM];

map<string, double> prob_emit_train[STATE_NUM];
double prob_start_train[STATE_NUM];
double prob_trans_train[STATE_NUM][STATE_NUM];


double weight[STATE_NUM][SENTENCE_SIZE];
int path[STATE_NUM][SENTENCE_SIZE];

string transform_str[CHARACTER_NUM];//for input string
string transform_backup[CHARACTER_NUM];
int transform_cnt = 0;
int stopword_pos_record[CHARACTER_NUM];

int train_trans_cnt[STATE_NUM][STATE_NUM];
map<string, int> train_emit_cnt[STATE_NUM];
map<string, int> total_train_word_cnt;
int train_start_cnt[STATE_NUM];

struct stopword_list {
	wstring wstr_stopword;
	bool operator < (const stopword_list &a)const {
		if (wstr_stopword.size() == a.wstr_stopword.size()) {
			return wstr_stopword < a.wstr_stopword;
		}
		else {
			return	wstr_stopword.size() < a.wstr_stopword.size();
		}
	}
};
bool HMM_use_stopword = false;
bool stopword_loaded = false;

set<stopword_list> cns_stopword;
/*wstring to string*/
string wstr2str(const wstring &wstr)
{

	string result;
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
	char* buffer = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
	buffer[len] = '\0';
	result.append(buffer);
	delete[] buffer;
	return result;

}
/*string to wstring*/
wstring str2wstr(const string& s)
{
	setlocale(LC_ALL, "chs");
	const char* _Source = s.c_str();
	size_t _Dsize = s.size() + 1;
	wchar_t *_Dest = new wchar_t[_Dsize];
	wmemset(_Dest, 0, _Dsize);
	mbstowcs(_Dest, _Source, _Dsize);
	std::wstring result = _Dest;
	delete[]_Dest;
	setlocale(LC_ALL, "C");
	return result;
}
/*删除字符串标点*/
string delete_punctuation(const wstring wstr) {//删除标点符号
	std::locale old_loc = std::locale::global(std::locale("CHS"));

	wstring temp = wstr;
	boost::xpressive::wsregex re = boost::xpressive::wsregex::compile(_T("[[:punct:]]"));

	std::locale::global(old_loc);

	return wstr2str(boost::xpressive::regex_replace(temp, re, _T("")));
}
/*全角数字转半角*/
wstring num_transform(wstring num_input) {
	//wstring get_num = (_T("１２３４５６７８９０"));
	wstring get_num = num_input;
	boost::wregex e_0_0(L"\u25cb");
	wstring replace_0_0 = boost::regex_replace(get_num, e_0_0, L"\u0030", boost::match_default | boost::format_all);

	boost::wregex e_0_1(L"\u0030");
	wstring replace_0_1 = boost::regex_replace(replace_0_0, e_0_1, L"\u0030", boost::match_default | boost::format_all);

	boost::wregex e_0_2(L"\uFF10");
	wstring replace_0_2 = boost::regex_replace(replace_0_1, e_0_2, L"\u0030", boost::match_default | boost::format_all);

	boost::wregex e_1_1(L"\u0031");
	wstring replace_1_1 = boost::regex_replace(replace_0_2, e_1_1, L"\u0031", boost::match_default | boost::format_all);

	boost::wregex e_1_2(L"\uFF11");
	wstring replace_1_2 = boost::regex_replace(replace_1_1, e_1_2, L"\u0031", boost::match_default | boost::format_all);

	boost::wregex e_2_1(L"\u0032");
	wstring replace_2_1 = boost::regex_replace(replace_1_2, e_2_1, L"\u0032", boost::match_default | boost::format_all);

	boost::wregex e_2_2(L"\uFF12");
	wstring replace_2_2 = boost::regex_replace(replace_2_1, e_2_2, L"\u0032", boost::match_default | boost::format_all);

	boost::wregex e_3_1(L"\u0033");
	wstring replace_3_1 = boost::regex_replace(replace_2_2, e_3_1, L"\u0033", boost::match_default | boost::format_all);

	boost::wregex e_3_2(L"\uFF13");
	wstring replace_3_2 = boost::regex_replace(replace_3_1, e_3_2, L"\u0033", boost::match_default | boost::format_all);

	boost::wregex e_4_1(L"\u0034");
	wstring replace_4_1 = boost::regex_replace(replace_3_2, e_4_1, L"\u0034", boost::match_default | boost::format_all);

	boost::wregex e_4_2(L"\uFF14");
	wstring replace_4_2 = boost::regex_replace(replace_4_1, e_4_2, L"\u0034", boost::match_default | boost::format_all);

	boost::wregex e_5_1(L"\u0035");
	wstring replace_5_1 = boost::regex_replace(replace_4_2, e_5_1, L"\u0035", boost::match_default | boost::format_all);

	boost::wregex e_5_2(L"\uFF15");
	wstring replace_5_2 = boost::regex_replace(replace_5_1, e_5_2, L"\u0035", boost::match_default | boost::format_all);

	boost::wregex e_6_1(L"\u0036");
	wstring replace_6_1 = boost::regex_replace(replace_5_2, e_6_1, L"\u0036", boost::match_default | boost::format_all);

	boost::wregex e_6_2(L"\uFF16");
	wstring replace_6_2 = boost::regex_replace(replace_6_1, e_6_2, L"\u0036", boost::match_default | boost::format_all);
	
	boost::wregex e_7_1(L"\u0037");
	wstring replace_7_1 = boost::regex_replace(replace_6_2, e_7_1, L"\u0037", boost::match_default | boost::format_all);

	boost::wregex e_7_2(L"\uFF17");
	wstring replace_7_2 = boost::regex_replace(replace_7_1, e_7_2, L"\u0037", boost::match_default | boost::format_all);

	boost::wregex e_8_1(L"\u0038");
	wstring replace_8_1 = boost::regex_replace(replace_7_2, e_8_1, L"\u0038", boost::match_default | boost::format_all);

	boost::wregex e_8_2(L"\uFF18");
	wstring replace_8_2 = boost::regex_replace(replace_8_1, e_8_2, L"\u0038", boost::match_default | boost::format_all);

	boost::wregex e_9_1(L"\u0039");
	wstring replace_9_1 = boost::regex_replace(replace_8_2, e_9_1, L"\u0039", boost::match_default | boost::format_all);

	boost::wregex e_9_2(L"\uFF19");
	wstring replace_9_2 = boost::regex_replace(replace_9_1, e_9_2, L"\u0039", boost::match_default | boost::format_all);

	wstring res = replace_9_2;

	return res;

}

/*读入停止词表*/
void stopword_readin(string filename) {
	ifstream openfile(filename);
	string get_line;
	while (getline(openfile, get_line, '\n')) {
		stopword_list a = { str2wstr(get_line) };
		cns_stopword.insert(a);
	}
	openfile.close();
}

void testtxtfile_readin_split(string filename) {
	ifstream openfile(filename);
	string get_line;
	while (getline(openfile, get_line, '\n')) {
		init_Viterbi();
		/*init while loop*/
		for (int i = 0; i < CHARACTER_NUM; i++) {
			stopword_pos_record[i] = STATE_ERROR;
			transform_str[i] = '\0';
			transform_backup[i] = '\0';
			transform_cnt = 0;
		}


		//get_line = delete_punctuation(str2wstr(get_line));
		input_str_transform(get_line);
		int choose_final_state = Viterbi();
		int state_record[(CHARACTER_NUM)];
		//cout <<  transform_cnt << "  " << choose_final_state << endl;
		state_record[transform_cnt] = choose_final_state;
		int size = transform_cnt;
		int state = -1;
		for (int i = transform_cnt - 1; i > 0; i--) {
			if (i == transform_cnt - 1) {
				state = path[choose_final_state][i];
				state_record[i] = state;
			}
			else {
				
				state = path[state_record[i + 1]][i];
				if (state == STATE_ERROR) state = STATE_S;
				
				state_record[i] = state;

			}
		}
	/*	for (int i = 1; i <= transform_cnt; i++) {
			if (state_record[i] == STATE_B) {
				cout << transform_backup[i - 1];
			}
			else if (state_record[i] == STATE_E) {
				cout << transform_backup[i - 1] << " /";
			}
			else if (state_record[i] == STATE_M) {
				cout << transform_backup[i - 1];
			}
			else {
				cout << transform_backup[i - 1] << " /";
			}
		}
		cout << endl;
		for (int i = 1; i <= transform_cnt; i++) {
			if (state_record[i] == STATE_B) {
				cout << 'B';
			}
			else if (state_record[i] == STATE_E) {
				cout << 'E'<< " /";
			}
			else if (state_record[i] == STATE_M) {
				cout << 'M';
			}
			else {
				cout << 'S' << " /";
			}
		}
		cout << endl;
		system("pause");*/

	}
	openfile.close();
}

void traintxtfile_readin(string filename) {
	for (int i = 0; i < STATE_NUM; i++) {
		for (int j = 0; j < STATE_NUM; j++) {
			train_trans_cnt[i][j] = 0;
		}
	}
	for (int i = 0; i < STATE_NUM; i++) {
		train_emit_cnt[i].clear();
		train_start_cnt[i] = 0;
	}
	ifstream openfile(filename);
	string get_line;
	while (getline(openfile, get_line, '\n')) {
		//cout << get_line << endl;
		//system("pause");
		wstring temp_ws = str2wstr(get_line);
		string after_delete = delete_punctuation(temp_ws);
		wstring inputstr = str2wstr(after_delete);
		//L"([\u2E80-\u9FFF]+)"
		boost::wregex wreg(L"([\u0041-\u005A\u0061-\u007A\u0030-\u0039\u2E80-\uFFEE]+)", boost::regex::perl | boost::regbase::icase);
		boost::smatch what;
		boost::wsmatch wswhat;
		wstring::const_iterator start = inputstr.begin();
		wstring::const_iterator end = inputstr.end();
		int last_str_final_state = STATE_ERROR;
		while (boost::regex_search(start, end, wswhat, wreg)) {
			wstring temp = wstring(wswhat[0].first, wswhat[0].second);
			boost::wregex wreg_num(L"([\uFF10-\uFF19\u0030-\u0039]+)");
			wstring::const_iterator num_start = temp.begin();
			wstring::const_iterator num_end = temp.end();
			boost::wsmatch num_wswhat;
			while (boost::regex_search(num_start, num_end, num_wswhat, wreg_num)) {
				temp = num_transform(temp);
				break;
			}
			string temp_s = wstr2str(temp);
		

			if (temp_s.length() == 2) {

				if (last_str_final_state != STATE_ERROR) {
					train_trans_cnt[last_str_final_state][STATE_S] += 1;
				}

				
				train_emit_cnt[STATE_S][temp_s]++;
				total_train_word_cnt[temp_s]++;

				last_str_final_state = STATE_S;
				train_start_cnt[STATE_S]++;
			}
			else if (temp_s.length() > 2) {
				//int size_str = temp_s.length() / 2;

				/* trans of the first element*/
				if (last_str_final_state != STATE_ERROR) {
					train_trans_cnt[last_str_final_state][STATE_B] += 1;
				}
				for (int i = 0; i < temp_s.length(); i += 2) {
					if (i == 0) {

						/*emit*/
						

						train_emit_cnt[STATE_B][temp_s.substr(i, i + 2)]++;
						total_train_word_cnt[temp_s.substr(i, i + 2)]++;

						/* the first element trans already defined*/
					}
					else if (i == temp_s.length() - 2) {
						/*emit*/
						
						train_emit_cnt[STATE_E][temp_s.substr(i, i + 2)]++;
						total_train_word_cnt[temp_s.substr(i, i + 2)]++;

						/*trans*/
						if (temp_s.length() > 4) {
							train_trans_cnt[STATE_M][STATE_E] += 1;
						}
						else {
							train_trans_cnt[STATE_B][STATE_E] += 1;
						}
						break;
					}

					else {

						/*emit*/
						
						train_emit_cnt[STATE_M][temp_s.substr(i, i + 2)]++;
						total_train_word_cnt[temp_s.substr(i, i + 2)]++;

						/*trans*/
						if (i == 2 && temp_s.length() > 4) {
							train_trans_cnt[STATE_B][STATE_M] += 1;
						}
						else {
							train_trans_cnt[STATE_M][STATE_M] += 1;

						}

					}
				}

				last_str_final_state = STATE_E;

				train_start_cnt[STATE_B]++;

			}
			else {
				//cout << temp_s << endl;
				//printf("error str length");
				//system("pause");
				train_start_cnt[STATE_S]++;
				
			}
			start = wswhat[0].second;
		//	system("pause");
		}


	}

	openfile.close();
}
double get_prob_emit(string data) {
	int pos = 0;
	bool flag = true;
	while (pos < data.length() && (!(data[pos] >= '0' && data[pos] <= '9'))) {
		if (data[pos] == '-') flag = false;//负
		pos++;
	}
	int digit;
	for (digit = pos; digit < data.length() && data[digit] != '.'; digit++);
	digit -= pos;

	double multi = 1;
	for (int i = 0; i < (digit - 1); i++) {
		multi = multi * 10;
	}
	//cout << "test" << multi << endl;
	double res = 0;
	for (int j = pos; j < data.length(); j++) {
		if (data[j] >= '0' && data[j] <= '9') {
			res += (data[j] - 48) * multi;
			multi = multi * 1.0 / 10;
		}
	}
	if (flag == false) {
		res = 0 - res;
	}
	return res;
}

void prob_emit_readin() {
	ifstream openfile("prob_emit.utf8");
	string get_line;
	int now_state;
	while (getline(openfile, get_line, '\n')) {
		if (get_line[0] == 'B') {
			now_state = STATE_B;
		}
		else if (get_line[0] == 'S') {
			now_state = STATE_S;
		}
		else if (get_line[0] == 'E') {
			now_state = STATE_E;
		}
		else {
			now_state = STATE_M;
		}
		wstring inputstr = str2wstr(get_line);
		boost::wregex wreg(L"([\u2E80-\u9FFF]+)", boost::regex::perl | boost::regbase::icase);
		boost::smatch what;
		boost::wsmatch wswhat;
		wstring::const_iterator start = inputstr.begin();
		wstring::const_iterator end = inputstr.end();
		wstring::const_iterator data_start;
		wstring::const_iterator data_end;
		wstring::const_iterator last = inputstr.end();
		wstring data;
		bool first = true;
		string last_character;
		while (boost::regex_search(start, end, wswhat, wreg)) {
			if (first == true) {
				data_start = wswhat[0].second;
				first = false;
			}
			else {
				data_end = wswhat[0].first;
				data = wstring(data_start, data_end);
				string data_temp = wstr2str(data);
			
				prob_emit[now_state].insert(pair<string, double>(last_character, get_prob_emit(data_temp)));

				data_start = wswhat[0].second;
			}
			wstring temp = wstring(wswhat[0].first, wswhat[0].second);
			string temp_s = wstr2str(temp);

			last_character = temp_s;
		
			start = wswhat[0].second;
	
		}
		data = wstring(data_start, last);
		string data_temp = wstr2str(data);
		prob_emit[now_state].insert(pair<string, double>(last_character, get_prob_emit(data_temp)));
		

	}
	openfile.close();
}
void prob_start_readin() {
	ifstream openfile("prob_start.utf8");
	string get_line;
	int now_state = 0;
	while (getline(openfile, get_line, '\n')) {
		//system("pause");
		boost::regex e1("[-0-9.e+]+");//正则匹配
		std::string::const_iterator start, end;
		start = get_line.begin();
		end = get_line.end();
		boost::match_results<std::string::const_iterator> what;
		boost::match_flag_type flags = boost::match_default;
		while (regex_search(start, end, what, e1, flags)) {
			string data = string(what[0].first, what[0].second);
		//	cout << data << endl;
			start = what[0].second;
			prob_start[now_state++] = atof(data.c_str());
			
		}
	}
	openfile.close();
}
void prob_trans_readin() {
	ifstream openfile("prob_trans.utf8");
	string get_line;
	int now_cnt = 0;
	while (getline(openfile, get_line, '\n')) {
		//system("pause");
		boost::regex e1("[-0-9.e+]+");//正则匹配
		std::string::const_iterator start, end;
		start = get_line.begin();
		end = get_line.end();
		boost::match_results<std::string::const_iterator> what;
		boost::match_flag_type flags = boost::match_default;
		while (regex_search(start, end, what, e1, flags)) {
			string data = string(what[0].first, what[0].second);
			start = what[0].second;
		    prob_trans[now_cnt / 4][now_cnt % 4] = atof(data.c_str());
			//cout << prob_trans[now_cnt / 4][now_cnt % 4] << endl;
			now_cnt++;

		}
	}
	openfile.close();
}
void init_Viterbi() {
	for (int i = 0; i < STATE_NUM; i++) {
		for (int j = 0; j < SENTENCE_SIZE; j++) {
			weight[i][j] = -(DBL_MAX / 10);
			path[i][j] = STATE_ERROR;
		}
	}
}

double prob_emit_value(int state, string character) {
	if (HMM_MODEL_CHOOSE == MODEL_FROM_DATA) {
		map<string, double>::iterator find_character;
		find_character = prob_emit[state].find(character);
		if (find_character != prob_emit[state].end()) {
			return find_character->second;
		}
		else {
			return atof("-3.14e+100");
		}
	}
	else if(HMM_MODEL_CHOOSE == MODEL_FROM_TRAINFILE){
		map<string, double>::iterator find_character;
		find_character = prob_emit_train[state].find(character);
		if (find_character != prob_emit_train[state].end()) {
			return find_character->second;
		}
		else {
			return atof("-3.14e+100");
		}

	}
}

double prob_start_value(int state) {
	if (HMM_MODEL_CHOOSE == MODEL_FROM_DATA) {
		return prob_start[state];
	}
	else if (HMM_MODEL_CHOOSE == MODEL_FROM_TRAINFILE) {
		return prob_start_train[state];
	}
}

double prob_trans_value(int statefrom, int stateto) {
	if (HMM_MODEL_CHOOSE == MODEL_FROM_DATA) {
		return prob_trans[statefrom][stateto];
	}
	else if (HMM_MODEL_CHOOSE == MODEL_FROM_TRAINFILE) {
		return prob_trans_train[statefrom][stateto];
	}
}
void weight_first_element(string first_character) {
	if (HMM_use_stopword == true) {
		if (stopword_pos_record[0] == STATE_ERROR || stopword_pos_record[0] == STATE_CONTRADICT) {
			weight[STATE_B][0] = prob_start_value(STATE_B) + prob_emit_value(STATE_B, first_character);
			weight[STATE_E][0] = prob_start_value(STATE_E) + prob_emit_value(STATE_E, first_character);
			weight[STATE_M][0] = prob_start_value(STATE_M) + prob_emit_value(STATE_M, first_character);
			weight[STATE_S][0] = prob_start_value(STATE_S) + prob_emit_value(STATE_S, first_character);
		}
		else {
			weight[stopword_pos_record[0]][0] = prob_start_value(stopword_pos_record[0]) + prob_emit_value(stopword_pos_record[0], first_character);
			for (int i = 0; i < STATE_NUM; i++) {
				if (i != stopword_pos_record[0]) {
					weight[i][0] = -(DBL_MAX / 10);
				}
			}
		}
	}
	else {
		weight[STATE_B][0] = prob_start_value(STATE_B) + prob_emit_value(STATE_B, first_character);
		weight[STATE_E][0] = prob_start_value(STATE_E) + prob_emit_value(STATE_E, first_character);
		weight[STATE_M][0] = prob_start_value(STATE_M) + prob_emit_value(STATE_M, first_character);
		weight[STATE_S][0] = prob_start_value(STATE_S) + prob_emit_value(STATE_S, first_character);
	}
}

string char2str(char input) {
	string res;
	stringstream stream;
	stream << input;
	res = stream.str();
	return res;
}
int Viterbi() {
	//init_Viterbi();
	int str_len = transform_cnt;
	weight_first_element(transform_str[0]);
	if (HMM_use_stopword == true) {
		for (int i = 1; i < str_len; i++) {
			if (stopword_pos_record[i] == STATE_ERROR || stopword_pos_record[i] == STATE_CONTRADICT) {
				for (int j = 0; j < STATE_NUM; j++) {
					weight[j][i] = -(DBL_MAX / 10);
					path[j][i] = STATE_ERROR;
					if (stopword_pos_record[i - 1] == STATE_ERROR || stopword_pos_record[i - 1] == STATE_CONTRADICT) {
						for (int k = 0; k < STATE_NUM; k++) {
							if (prob_trans_value(k,j) < -10000) continue;
							double temp = weight[k][i - 1] + prob_trans_value(k,j) + prob_emit_value(j, transform_str[i]);
							if (temp > weight[j][i]) {
								weight[j][i] = temp;
								path[j][i] = k;
							}
						}
					}
					else {
						if (prob_trans_value(stopword_pos_record[i - 1],j) < -10000) {
							//printf("system error1");
							//system("pause");
							continue;
						}
						double temp = weight[stopword_pos_record[i - 1]][i - 1] + prob_trans_value(stopword_pos_record[i - 1],j) + prob_emit_value(j, transform_str[i]);
						if (temp > weight[j][i]) {
							weight[j][i] = temp;
							path[j][i] = stopword_pos_record[i - 1];
						}
					}

					if (path[j][i] == -1) {
						cout << "here error 1" << endl;
						system("pause");
					}
				}

			}
			else {

				weight[stopword_pos_record[i]][i] = -(DBL_MAX / 10);
				path[stopword_pos_record[i]][i] = STATE_ERROR;
				if (stopword_pos_record[i - 1] == STATE_ERROR || stopword_pos_record[i - 1] == STATE_CONTRADICT) {
					for (int k = 0; k < STATE_NUM; k++) {
						if (prob_trans_value(k,stopword_pos_record[i]) < -10000) continue;
						double temp = weight[k][i - 1] + prob_trans_value(k,stopword_pos_record[i]) + prob_emit_value(stopword_pos_record[i], transform_str[i]);
						if (temp > weight[stopword_pos_record[i]][i]) {
							weight[stopword_pos_record[i]][i] = temp;
							path[stopword_pos_record[i]][i] = k;
						}
					}
				}
				else {
					if (prob_trans_value(stopword_pos_record[i - 1],stopword_pos_record[i]) < -10000) {
						//	printf("system error2");
						//	system("pause");
					}
					double temp = weight[stopword_pos_record[i - 1]][i - 1] + prob_trans_value(stopword_pos_record[i - 1],stopword_pos_record[i]) + prob_emit_value(stopword_pos_record[i], transform_str[i]);
					if (temp > weight[stopword_pos_record[i]][i]) {
						weight[stopword_pos_record[i]][i] = temp;
						path[stopword_pos_record[i]][i] = stopword_pos_record[i - 1];
					}
				}

				//if (path[stopword_pos_record[i]][i] == -1) {
					//cout << "here error 2" << endl;
					//system("pause");
				//}

			}
		}
	}

	else {
		for (int i = 1; i < str_len; i++) {
			for (int j = 0; j < STATE_NUM; j++) {
				weight[j][i] = -(DBL_MAX / 10);
				path[j][i] = STATE_ERROR;

				for (int k = 0; k < STATE_NUM; k++) {
					if (prob_trans_value(k,j) < -10000) continue;
					double temp = weight[k][i - 1] + prob_trans_value(k,j) + prob_emit_value(j, transform_str[i]);
					if (temp > weight[j][i]) {
						weight[j][i] = temp;
						path[j][i] = k;
					}
				}

			}
		}
	}
	
	if (weight[STATE_S][str_len - 1] > weight[STATE_E][str_len - 1]) {
		return STATE_S;
	}
	else {
		return STATE_E;
	}
}
void input_str_transform(string input) {
	transform_cnt = 0;
	for (int i = 0; i < (CHARACTER_NUM); i++) {
		transform_str[i].clear();
		stopword_pos_record[i] = STATE_ERROR;
	}

	wstring temp_ws = str2wstr(input);
	string after_delete = wstr2str(temp_ws);
	//string after_delete = delete_punctuation(temp_ws);
	wstring inputwstr = str2wstr(after_delete);
	if (HMM_use_stopword == true) {
		set<stopword_list>::iterator  iter_;

		/*for stopword use*/
		for (iter_ = cns_stopword.begin(); iter_ != cns_stopword.end(); iter_++) {
			int begin = -1;
			stopword_list loop_get = *iter_;
			int len = loop_get.wstr_stopword.length();
			if (len == 1) {
				while ((begin = inputwstr.find(loop_get.wstr_stopword, begin + 1)) != wstring::npos) {
					if (stopword_pos_record[begin] == STATE_CONTRADICT) continue;
					stopword_pos_record[begin] = STATE_S;
				}
			}
			else if (len == 2) {
				while ((begin = inputwstr.find(loop_get.wstr_stopword, begin + 1)) != wstring::npos) {
					if (stopword_pos_record[begin] == STATE_CONTRADICT || stopword_pos_record[begin + 1] == STATE_CONTRADICT) continue;
					
					/*contradict*/
					if (stopword_pos_record[begin + 1] == STATE_B) {
						stopword_pos_record[begin] = STATE_CONTRADICT;
						stopword_pos_record[begin + 1] = STATE_CONTRADICT;
						stopword_pos_record[begin + 2] = STATE_CONTRADICT;
					}

					else if(stopword_pos_record[begin] == STATE_E) {
						stopword_pos_record[begin - 1] = STATE_CONTRADICT;
						stopword_pos_record[begin] = STATE_CONTRADICT;
						stopword_pos_record[begin + 1] = STATE_CONTRADICT;
					}
					else {
						stopword_pos_record[begin] = STATE_B;
						stopword_pos_record[begin + 1] = STATE_E;
					}
				}
			}
			else if (len > 2) {
				/*if (len % 2 != 0) {
					cout << "system error len" << endl;
					cout << wstr2str(loop_get.wstr_stopword) << endl;
					cout << len << endl;
					system("pause");
				}*/
				while ((begin = inputwstr.find(loop_get.wstr_stopword, begin + 1)) != wstring::npos) {
					bool flag_contradict = false;
					for (int k = 0; k < len; k++) {
						if (stopword_pos_record[begin + k] == STATE_CONTRADICT) {
							flag_contradict = true;
							break;
						}
					}
					if (flag_contradict == true) continue;

					/*contradict*/
					if (stopword_pos_record[begin + len - 1] == STATE_B || stopword_pos_record[begin + len - 1] == STATE_M) {
						if (stopword_pos_record[begin + len - 1] == STATE_B) {
							/* set original area contradict*/
							for (int k = 0;; k++) {
								if (stopword_pos_record[begin + len - 1 + k] == STATE_E) {
									stopword_pos_record[begin + len - 1 + k] = STATE_CONTRADICT;
									break;
								}
								else {
									stopword_pos_record[begin + len - 1 + k] = STATE_CONTRADICT;
								}
							}

							/*set new area contradict*/
							for (int k = 0; k < len; k++) {
								stopword_pos_record[begin + k] = STATE_CONTRADICT;
							}

						}
						else {
							/*set original area contradict*/
							for (int k = 0;; k++) {
								if (stopword_pos_record[begin + len - 1 + k] == STATE_E) {
									stopword_pos_record[begin + len - 1 + k] = STATE_CONTRADICT;
									break;
								}
								else {
									stopword_pos_record[begin + len - 1 + k] = STATE_CONTRADICT;
								}
							}

							for (int k = 0;; k++) {
								if (stopword_pos_record[begin + len - 1 - k] == STATE_B) {
									stopword_pos_record[begin + len - 1 - k] = STATE_CONTRADICT;
									break;
								}
								else {
									stopword_pos_record[begin + len - 1 - k] = STATE_CONTRADICT;
								}
							}

							/* set new area contradict*/
							for (int k = 0; k < len; k++) {
								stopword_pos_record[begin + k] = STATE_CONTRADICT;
							}

						}
					}
					else if (stopword_pos_record[begin] == STATE_M || stopword_pos_record[begin] == STATE_E) {
						if (stopword_pos_record[begin] == STATE_E) {
							/*set original area contradict*/
							for (int k = 0;; k++) {
								if (stopword_pos_record[begin - k] == STATE_B) {
									stopword_pos_record[begin - k] = STATE_CONTRADICT;
									break;
								}
								else {
									stopword_pos_record[begin - k] = STATE_CONTRADICT;
								}
							}

							/*set new area contradict*/
							for (int k = 0; k < len; k++) {
								stopword_pos_record[begin + k] = STATE_CONTRADICT;
							}
						}
						else {
							/*set original area contradict*/
							for (int k = 0;; k++) {
								if (stopword_pos_record[begin - k] == STATE_B) {
									stopword_pos_record[begin - k] = STATE_CONTRADICT;
									break;
								}
								else {
									stopword_pos_record[begin - k] = STATE_CONTRADICT;
								}
							}

							for (int k = 0;; k++) {
								if (stopword_pos_record[begin + k] == STATE_E) {
									stopword_pos_record[begin + k] = STATE_CONTRADICT;
									break;
								}
								else {
									stopword_pos_record[begin + k] = STATE_CONTRADICT;
								}
							}

							/*set new area contradict*/
							for (int k = 0; k < len; k++) {
								stopword_pos_record[begin + k] = STATE_CONTRADICT;
							}

						}
					}
					else {
						stopword_pos_record[begin] = STATE_B;
						stopword_pos_record[begin + len - 1] = STATE_E;
						for (int i = begin + 1; i < begin + len - 1; i++) {
							stopword_pos_record[i] = STATE_M;
						}
					}
				}
			}
			else {
				continue;
			}

		}

		boost::wregex data_picker(L"[\u0030-\u0039\uFF10-\uFF19\u25cb\u4E00\u4E8C\u4E09\u56DB\u4E94\u516D\u4E03\u516B\u4E5D\u96F6\u5341]+");
		boost::wsmatch wswhat;
		wstring::const_iterator start = inputwstr.begin();
		wstring::const_iterator end = inputwstr.end();
		while (boost::regex_search(start, end, wswhat, data_picker)) {
			wstring data_get = wstring(wswhat[0].first, wswhat[0].second);
			int find_begin = -1;
			int data_len = data_get.length();
			while ((find_begin = inputwstr.find(data_get, find_begin + 1)) != wstring::npos) {
				bool replace_flag = true;
				for (int k = find_begin; k < find_begin + data_len; k++) {
					if (stopword_pos_record[k] == STATE_S || stopword_pos_record[k] == STATE_ERROR) {
						continue;
					}
					else {
						replace_flag = false;
						break;
					}
				}
				if (replace_flag == true && data_get.length() >= 2) {
					for (int k = find_begin; k < find_begin + data_len; k++) {
						if (k == find_begin) stopword_pos_record[k] = STATE_B;
						else if (k == find_begin + data_len - 1) stopword_pos_record[k] = STATE_E;
						else {
							stopword_pos_record[k] = STATE_M;
						}
					}
				}
			}
			start = wswhat[0].second;
		}

		boost::wregex eng_picker(L"[\u0061-\u007a\u0041-\u005a]+");
		boost::wsmatch wswhat_1;
		wstring::const_iterator start_1 = inputwstr.begin();
		wstring::const_iterator end_1 = inputwstr.end();
		while (boost::regex_search(start_1, end_1, wswhat_1, eng_picker)) {
			wstring data_get = wstring(wswhat_1[0].first, wswhat_1[0].second);
			int find_begin = -1;
			int data_len = data_get.length();
			while ((find_begin = inputwstr.find(data_get, find_begin + 1)) != wstring::npos) {
				bool replace_flag = true;
				for (int k = find_begin; k < find_begin + data_len; k++) {
					if (stopword_pos_record[k] == STATE_S || stopword_pos_record[k] == STATE_ERROR) {
						continue;
					}
					else {
						replace_flag = false;
						break;
					}
				}
				if (replace_flag == true && data_get.length() >= 2) {
					for (int k = find_begin; k < find_begin + data_len; k++) {
						if (k == find_begin) stopword_pos_record[k] = STATE_B;
						else if (k == find_begin + data_len - 1) stopword_pos_record[k] = STATE_E;
						else {
							stopword_pos_record[k] = STATE_M;
						}
					}
				}
			}
			start_1 = wswhat_1[0].second;
		}
	}
	else {
		//inputwstr = str2wstr(delete_punctuation(inputwstr));
	}
	boost::wregex wreg(L"([\u0001-\uFFEE])", boost::regex::perl | boost::regbase::icase);
	boost::smatch what;
	boost::wsmatch wswhat;
	wstring::const_iterator start = inputwstr.begin();
	wstring::const_iterator end = inputwstr.end();
	while (boost::regex_search(start, end, wswhat, wreg)) {
		wstring temp = wstring(wswhat[0].first, wswhat[0].second);
		wstring temp_nonum = num_transform(temp);
		string temp_s = wstr2str(temp);
		string temp_s_nonum = wstr2str(temp_nonum);
		transform_backup[transform_cnt] = temp_s;
		transform_str[transform_cnt++] = temp_s_nonum;
		start = wswhat[0].second;
	}


}
void get_HMM_model() {
	/* get prob trans from train file*/
	for (int i = 0; i < STATE_NUM; i++) {
		int sum_trans = 0;
		for (int j = 0; j < STATE_NUM; j++) {
			sum_trans += train_trans_cnt[i][j];
		}
		for (int k = 0; k < STATE_NUM; k++) {
			if (train_trans_cnt[i][k] == 0) {
				prob_trans_train[i][k] = atof("-3.14e+100");
			}
			else {
				prob_trans_train[i][k] = log((double)train_trans_cnt[i][k] * 1.0 / sum_trans);
			}
		}
	}

	/*get prob emit from train file*/
	for (int i = 0; i < STATE_NUM; i++) {
		map<string, int>::iterator iter;
		for (iter = train_emit_cnt[i].begin(); iter != train_emit_cnt[i].end(); iter++) {
			double prob = log((double)iter->second * 1.0 / total_train_word_cnt[iter->first]);
			prob_emit_train[i].insert(pair<string, double>(iter->first, prob));
		}
	}

	/*get prob start from train file*/
	for (int i = 0; i < STATE_NUM; i++) {
		if (train_start_cnt[i] == 0) {
			prob_start_train[i] = atof("-3.14e+100");
		}
		else {
			prob_start_train[i] = log((double)train_start_cnt[i] * 1.0 / (train_start_cnt[STATE_B] + train_start_cnt[STATE_S]));
		}
	}


}
void load_stopword() {
	if (stopword_loaded == false) {
		stopword_readin("stop_words.utf8");
		stopword_loaded = true;
	}

}
void setup_HMM_model_data() {
	if (model_data_loaded == false) {
		prob_start_readin();
		prob_emit_readin();
		prob_trans_readin();
		model_data_loaded = true;
	}
}

void setup_HMM_model_train(string filepath, string filename) {
	if (model_trainfile_loaded == false) {
		traintxtfile_readin(filepath + filename);
		get_HMM_model();
		model_trainfile_loaded = true;
	}
}
void HMM_teststr(string input) {
	init_Viterbi();
	string a = input;
	//a = delete_punctuation(str2wstr(a));
	input_str_transform(a);
	int choose_final_state = Viterbi();
	int state_record[(CHARACTER_NUM)];
	state_record[transform_cnt] = choose_final_state;
	int size = transform_cnt;
	int state = -1;
	for (int i = transform_cnt - 1; i > 0; i--) {
		if (i == transform_cnt - 1) {
			state = path[choose_final_state][i];
			state_record[i] = state;
		}
		else {
			state = path[state][i];
			state_record[i] = state;
		}
	}
	for (int i = 1; i <= transform_cnt; i++) {
		if (state_record[i] == STATE_B) {
			cout << transform_backup[i - 1];
		}
		else if (state_record[i] == STATE_E) {
			cout << transform_backup[i - 1] << " /";
		}
		else if (state_record[i] == STATE_M) {
			cout << transform_backup[i - 1];
		}
		else {
			cout << transform_backup[i - 1] << " /";
		}
	}
	cout << endl;
	cout << "*******" << endl;
}

void HMM_testfile(string filepath, string filename) {
	testtxtfile_readin_split(filepath + filename);
}
int main() {
	while (true) {
	L0:cout << "Way to Setup HMM Model(TrainFile 0 / DataFile 1" << endl;
		int ans_0;
		cin >> ans_0;
		if (ans_0 == 0) {
			setup_HMM_model_train("", "pku_training.txt");
			HMM_MODEL_CHOOSE = MODEL_FROM_TRAINFILE;
		}
		else if (ans_0 == 1) {
			setup_HMM_model_data();
			HMM_MODEL_CHOOSE = MODEL_FROM_DATA;
		}
		else {
			cout << "error input" << endl;
			goto L0;
		}

	L1:	cout << "Use Stopword?(0/1)" << endl;
		int ans_1;
		cin >> ans_1;
		if (ans_1 == 1) {
			load_stopword();
			HMM_use_stopword = true;
		}
		else if (ans_1 == 0) {
			HMM_use_stopword = false;
		}
		else {
			cout << "error input" << endl;
			goto L1;
		}
	L2: cout << "Test Type(File 0 / Str 1)" << endl;
		int ans_2;
		cin >> ans_2;
		if (ans_2 == 1) {
			string input;
			cout << "input str(input quit to stop)" << endl;
			cin >> input;
			HMM_teststr(input);
		
		}
		else if (ans_2 == 0) {
			HMM_testfile("", "pku_test.txt");
		}
		else {
			cout << "error input" << endl;
			goto L2;
		}
	}
	return 0;
}