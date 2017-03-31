#include "test.h"


int state_num = PRICE_STATE_NUM_L;

struct STOCK {
public:
	int date;
	double daily_change_range;
	double close;
	int O_type;
	STOCK(int a, double b, double c,int d) :date(a), daily_change_range(b), close(c), O_type(d) {}
	bool operator <(const STOCK& st) const;
};

bool STOCK::operator<(const STOCK &st) const {
	return date < st.date;
}

int date_cnt = 0;

vector<STOCK> stockdata;

double a[MAX_NUM_DAY][PRICE_STATE_NUM_L];
double b[MAX_NUM_DAY][PRICE_STATE_NUM_L];
double l[MAX_NUM_DAY][PRICE_STATE_NUM_L];
double trans[MAX_NUM_DAY][PRICE_STATE_NUM_L][PRICE_STATE_NUM_L];

double prob_price_start[PRICE_STATE_NUM_L];
double	prob_price_transform[PRICE_STATE_NUM_L][PRICE_STATE_NUM_L];
double prob_price_emit[PRICE_STATE_NUM_L][RANGE_STATE_NUM];

double weight[PRICE_STATE_NUM_L][MAX_NUM_DAY];
int path[PRICE_STATE_NUM_L][MAX_NUM_DAY];

int test_day_num = 0;

double prob_O = 0;

int end_pos = 0;
int state_record[MAX_NUM_DAY];

double money = 100;

void normalization_a(int t) {

		double sum_t = 0;
		for (int i = 0; i < state_num; i++) {
			sum_t += a[t][i];
		}

		for (int i = 0; i < state_num; i++) {
			a[t][i] = a[t][i] / sum_t;
		}
	

}

void normalization_b(int t) {
	
		double sum_t = 0;
		for (int i = 0; i < state_num; i++) {
			sum_t += b[t][i];
		}

		for (int i = 0; i < state_num; i++) {
			b[t][i] = b[t][i] / sum_t;
		}

}
void stockdata_readin() {
	date_cnt = 0;
	ifstream openfile("sp500.csv");
	if (!openfile) {
		cout << "cant open file" << endl;
		return;
	}
	string get_line;
	getline(openfile, get_line, '\n');
	while (getline(openfile, get_line, '\n')) {

		boost::regex e("[-0-9.]+");
		string::const_iterator start = get_line.begin();
		string::const_iterator end = get_line.end();
		boost::smatch what;

		boost::regex_search(start, end, what, e);
		string get_date = string(what[0].first, what[0].second);
		start = what[0].second;

		string year = get_date.substr(0, 4);
		int year_i = atoi(year.c_str());

		string month = get_date.substr(5, 2);
		int month_i = atoi(month.c_str());

		string day = get_date.substr(8, 2);
		int day_i = atoi(day.c_str());

		int date_i = day_i + month_i * 100 + year_i * 10000;

		for (int i = 0; i < 3; i++) {
			boost::regex_search(start, end, what, e);
			start = what[0].second;
		}

		boost::regex_search(start, end, what, e);
		string get_closeprice = string(what[0].first, what[0].second);
		double closeprice_i = atof(get_closeprice.c_str());
		start = what[0].second;

	
		stockdata.push_back(STOCK(date_i, 0, closeprice_i,RANGE_ERROR));
		sort(stockdata.begin(), stockdata.end(), less<STOCK>());
		date_cnt++;
	}

	openfile.close();

	sort(stockdata.begin(), stockdata.end(), less<STOCK>());

	stockdata[0].O_type = RANGE_O3;
	for (int i = 1; i < date_cnt; i++) {
		double daily_change = (stockdata[i].close - stockdata[i - 1].close) * 1.0 / stockdata[i - 1].close;
		stockdata[i].daily_change_range = daily_change;
		if (daily_change > O1_RANGE_LEFT && daily_change <= O1_RANGE_RIGHT) {
			stockdata[i].O_type = RANGE_O1;
		}
		else if (daily_change > O2_RANGE_LEFT && daily_change <= O2_RANGE_RIGHT) {
			stockdata[i].O_type = RANGE_O2;
		}
		else if (daily_change > O3_RANGE_LEFT && daily_change <= O3_RANGE_RIGHT) {
			stockdata[i].O_type = RANGE_O3;
		}
		else if (daily_change > O4_RANGE_LEFT && daily_change <= O4_RANGE_RIGHT) {
			stockdata[i].O_type = RANGE_O4;
		}
		else if (daily_change > O5_RANGE_LEFT && daily_change <= O5_RANGE_RIGHT) {
			stockdata[i].O_type = RANGE_O5;
		}
		else if (daily_change > O6_RANGE_LEFT && daily_change <= O6_RANGE_RIGHT) {
			stockdata[i].O_type = RANGE_O6;
		}
		else {
			stockdata[i].O_type = RANGE_ERROR;
			cout << "O_type error!" << endl;
			system("pause");
		}
	}

}

void forward() {

	//* get the first element*/
	for (int i = 0; i < state_num; i++) {
		a[BEGIN_DATE_NUM][i] = prob_price_start[i] * prob_price_emit[i][stockdata[BEGIN_DATE_NUM].O_type];//?
		normalization_a(BEGIN_DATE_NUM);
	}
	
	for (int t = BEGIN_DATE_NUM + 1; t < END_DATE_NUM + 1; t++) {
		for (int i = 0; i < state_num; i++) {
			double temp_trans = 0;
			for (int j = 0; j < state_num; j++) {
				temp_trans += a[t - 1][j] * prob_price_transform[j][i];
			}

			a[t][i] = temp_trans * prob_price_emit[i][stockdata[t].O_type];

		}
		normalization_a(t);
	}

	prob_O = 0;
	for (int i = 0; i < state_num; i++) {
		prob_O += a[END_DATE_NUM][i];
	}


}
void backforward() {
	/* get the first element*/
	for (int i = 0; i < state_num; i++) {
		b[END_DATE_NUM][i] = 1;
	}
	for (int t = END_DATE_NUM - 1; t >= BEGIN_DATE_NUM; t--) {
		for (int i = 0; i < state_num; i++) {
			double temp_trans = 0;
			for (int j = 0; j < state_num; j++) {
				temp_trans += prob_price_transform[i][j] * prob_price_emit[j][stockdata[t + 1].O_type] * b[t + 1][j];
			}
			b[t][i] = temp_trans;
		}

		normalization_b(t);

	}
	
}

void state_estimation() {
	for (int t = BEGIN_DATE_NUM; t < END_DATE_NUM + 1; t++) {
		double Deno = 0;
		for (int j = 0; j < state_num; j++) {
			Deno += a[t][j] * b[t][j];
		}
		for (int i = 0; i < state_num; i++) {
			double Mole = a[t][i] * b[t][i];
			l[t][i] = Mole / Deno;
		}
	}

	for (int t = BEGIN_DATE_NUM; t < END_DATE_NUM; t++) {

		double Deno = 0;

		for (int i = 0; i < state_num; i++) {
			double Deno_part = 0;
			for (int j = 0; j < state_num; j++) {
				Deno_part += a[t][i] * prob_price_transform[i][j] * prob_price_emit[j][stockdata[t + 1].O_type] * b[t + 1][j];
			}
			Deno += Deno_part;
		}

		for (int i = 0; i < state_num; i++) {
			for (int j = 0; j < state_num; j++) {
				double Mole = (a[t][i] * prob_price_transform[i][j] * prob_price_emit[j][stockdata[t + 1].O_type] * b[t + 1][j]);
				trans[t][i][j] = Mole / Deno;
			}
		}
	}
}

void train() {
	/* get prob_trans*/
	for (int i = 0; i < state_num; i++) {
		double Deno = 0;
		for (int t = BEGIN_DATE_NUM; t < END_DATE_NUM; t++) {
			Deno += l[t][i];
		}

		for (int j = 0; j < state_num; j++) {
			double Mole = 0;
			for (int t = BEGIN_DATE_NUM; t < END_DATE_NUM; t++) {
				Mole += trans[t][i][j];
			}

			prob_price_transform[i][j] = Mole / Deno;
		}
	}

	/*get prob start*/
	for (int i = 0; i < state_num; i++) {
		prob_price_start[i] = l[BEGIN_DATE_NUM][i];
	}

	/*get prob emit*/
	for (int j = 0; j < state_num; j++) {
		double Deno = 0;
		for (int t = BEGIN_DATE_NUM; t < END_DATE_NUM + 1; t++) {
			Deno += l[t][j];
		}

		for (int k = 0; k < RANGE_STATE_NUM; k++) {
			double Mole = 0;
			for (int t = BEGIN_DATE_NUM; t < END_DATE_NUM + 1; t++) {
				if (stockdata[t].O_type == k) {
					Mole += l[t][j];
				}
			}

			prob_price_emit[j][k] = Mole / Deno;
		}
	}


}

void init_parameter_3() {
	// state num : 3

	/* get first prob transform*/
	for (int i = 0; i < PRICE_STATE_NUM; i++) {
		for (int j = 0; j < PRICE_STATE_NUM; j++) {
		if (i == j) {
				prob_price_transform[i][j] = 0.4;
			}
			else {
				prob_price_transform[i][j] = 0.3;
			}
		}
	}

	/* get first prob start*/
	for (int i = 0; i < PRICE_STATE_NUM; i++) {
		prob_price_start[i] = (double)1 * 1.0 / PRICE_STATE_NUM;

	}
	//prob_price_start[PRICE_SMOOTH_R] = 0.7;
//	prob_price_start[PRICE_FALL] = 0.15;
	//prob_price_start[PRICE_RISE] = 0.15;

	/*get first prob emit*/
	for (int i = 0; i < PRICE_STATE_NUM; i++) {
		for (int k = 0; k < RANGE_STATE_NUM; k++) {
			prob_price_emit[i][k] = (double)1 * 1.0 / 6;
		}
	}

}
void print_model() {
	cout << "prob start" << endl;
	for (int i = 0; i < state_num; i++) {
		cout << prob_price_start[i] << endl;
	}
	cout << "prob transform" << endl;
	for (int i = 0; i < state_num; i++) {
		for (int j = 0; j < state_num; j++) {
			cout << prob_price_transform[i][j] << "  ";
		}
		cout << endl;
	}

	cout << "prob emit" << endl;
	for (int i = 0; i < state_num; i++) {
		for (int j = 0; j < RANGE_STATE_NUM; j++) {
			cout << prob_price_emit[i][j] << "   ";
		}
		cout << endl;
	}
}
void init_parameter_6() {
	for (int i = 0; i < PRICE_STATE_NUM_L; i++) {
		for (int j = 0; j < PRICE_STATE_NUM_L; j++) {
			if (i == j) {
				prob_price_transform[i][j] = 0.5;
			}
			else {
				prob_price_transform[i][j] = 0.1;
			}
		}
	}

	/* get first prob start*/
	for (int i = 0; i < PRICE_STATE_NUM_L; i++) {
		prob_price_start[i] = (double)1 * 1.0 / PRICE_STATE_NUM_L;

	}
/*	prob_price_start[PRICE_SMOOTH_R] = 0.3;
	prob_price_start[PRICE_SMOOTH_F] = 0.3;
	prob_price_start[PRICE_RISE] = 0.15;
	prob_price_start[PRICE_FALL] = 0.15;
	prob_price_start[PRICE_RISE_L] = 0.05;
	prob_price_start[PRICE_FALL_L] = 0.05;*/

	/*get first prob emit*/
	for (int i = 0; i < PRICE_STATE_NUM_L; i++) {
		for (int k = 0; k < RANGE_STATE_NUM; k++) {
			prob_price_emit[i][k] = (double)1 * 1.0 / 6;
		}
	}
}

void Viterbi() {
	/*init first weight*/
	for (int i = 0; i < state_num; i++) {
		weight[i][TEST_BEGIN] = log(prob_price_start[i]) + log(prob_price_emit[i][stockdata[TEST_BEGIN].O_type]);
	}
	for (int t = TEST_BEGIN + 1; t < TEST_END + 1; t++) {
		for (int j = 0; j < state_num; j++) {
			weight[j][t] = -DBL_MAX;
			path[j][t] = PRICE_ERROR;
			for (int k = 0; k < state_num; k++) {
				double temp = weight[k][t - 1] + log(prob_price_transform[k][j]) + log(prob_price_emit[j][stockdata[t].O_type]);
				if (temp > weight[j][t]) {
					weight[j][t] = temp;
					path[j][t] = k;
				}
			}
		}
	}

	end_pos = 0;
	double end_val = -DBL_MAX;
	int state = PRICE_ERROR;
	for (int i = 0; i < state_num; i++) {
		if (weight[i][TEST_END] > end_val) {
			end_val = weight[i][TEST_END];
			end_pos = i;
		}
	}

	state_record[TEST_END + 1] = end_pos;
	cout << "prediction result:" << endl;
	for (int i = TEST_END; i > 0; i--) {
		state = path[state_record[i + 1]][i];
		state_record[i] = state;
	}

	for (int i = TEST_BEGIN + 1; i <= TEST_END + 1; i++) {
		cout << state_record[i] ;
	}
	cout << endl;

}

void print_price() {
	string test_data = "test_data.txt";
	remove(test_data.c_str());
	ofstream fout(test_data, ios::out);
	for (int i = TEST_BEGIN; i < TEST_END + 1; i++) {
		fout << stockdata[i].daily_change_range * 100;
		fout << "\n";
	}
	fout.close();
}
void print_predict_3() {
	string predict_data = "predict-data3.txt";
	remove(predict_data.c_str());
	ofstream fout(predict_data, ios::out);
	for (int i = TEST_BEGIN + 1; i <= TEST_END; i++) {
		if (state_record[i] == PRICE_RISE) {
			fout << "-2";
			fout << "\n";
		}
		if (state_record[i] == PRICE_SMOOTH_R) {
			fout << "-3";
			fout << "\n";
		}
		if (state_record[i] == PRICE_FALL) {
			fout << "-4";
			fout << "\n";
		}
	}
	fout.close();
}

void print_predict_6() {
	string predict_data = "predict-data6.txt";
	remove(predict_data.c_str());
	ofstream fout(predict_data, ios::out);
	for (int i = TEST_BEGIN + 1; i <= TEST_END; i++) {
		if (state_record[i] == PRICE_RISE) {
			fout << "-5";
			fout << "\n";
		}
		if (state_record[i] == PRICE_SMOOTH_R) {
			fout << "-6";
			fout << "\n";
		}
		if (state_record[i] == PRICE_SMOOTH_F) {
			fout << "-7";
			fout << "\n";
		}
		if (state_record[i] == PRICE_FALL) {
			fout << "-8";
			fout << "\n";
		}
		if (state_record[i] == PRICE_FALL_L) {
			fout << "-9";
			fout << "\n";
		}
	}
	fout.close();
}
void buy_3() {
	for (int i = TEST_BEGIN; i <= TEST_END - 1; i++) {
		if (state_record[i + 1] == PRICE_RISE) {
			double get_rate = stockdata[i].daily_change_range;
			money = money + money * get_rate;
		}
		else if (state_record[i + 1] == PRICE_FALL) {
			continue;
		}
		else {
		//	double get_rate = stockdata[i].daily_change_range;
		//	money = money + (money * 0.5) * get_rate;
		}
	}
	cout << money << endl;
}

void buy_6() {
	for (int i = TEST_BEGIN; i <= TEST_END - 1; i++) {
		if (state_record[i + 1] == PRICE_RISE_L) {
			double get_rate = stockdata[i].daily_change_range;
			money = money + money * get_rate;
		}
		else if (state_record[i + 1] == PRICE_RISE) {
			double get_rate = stockdata[i].daily_change_range;
			money = money + (money * 0.8)* get_rate;
		}
		else if (state_record[i + 1] == PRICE_SMOOTH_R) {
			double get_rate = stockdata[i].daily_change_range;
			money = money + (money * 0.5)* get_rate;
		}
		
		else {
			continue;
		}
	}
	cout << money << endl;
}
int main() {
	stockdata_readin();
L1:	cout << "Choose state num:(3/6)" << endl;
	int ans_1;
	cin >> ans_1;
	if (ans_1 == 3) {
		state_num = PRICE_STATE_NUM;
	}
	else if (ans_1 == 6) {
		state_num = PRICE_STATE_NUM_L;
	}
	else {
		cout << "error input" << endl;
		goto L1;
	}


	if (state_num == PRICE_STATE_NUM) {
		init_parameter_3();
	}
	else {
		init_parameter_6();
	}
	for (int i = 0; i < 1000; i++) {
		forward();
		backforward();
		state_estimation();
		train();
	}
	print_model();
	Viterbi();
	cout << date_cnt << endl;
	print_price();
	if(state_num==PRICE_STATE_NUM){
		print_predict_3();
		buy_3();
	}
	else {
		print_predict_6();
		buy_6();
	}
	return 0;
}