
#pragma once
#include <iostream>
#include <string>
#include <tchar.h>
#include <boost/regex.hpp>
#include <boost/xpressive/xpressive_dynamic.hpp>  
#include <boost/xpressive/regex_primitives.hpp>
#include <boost/tr1/regex.hpp>
#include <locale.h>  
#include <Windows.h>  
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <map>
#include <float.h>
#include <set>


#define TEST_FILE 0
#define TEST_STR 1
#define MODEL_FROM_DATA 0
#define MODEL_FROM_TRAINFILE 1
#define SENTENCE_SIZE 1<<15
#define NAX_LINE 1<<14
#define CHARACTER_NUM 1<<14
#define STATE_NUM 4
#define STATE_ERROR -1
#define STATE_CONTRADICT -2
#define STATE_B 0
#define STATE_E 1
#define STATE_M 2
#define STATE_S 3

using namespace std;


string wstr2str(const wstring &wstr);//wstring to string
wstring str2wstr(const string& s);//string to wstring
string delete_punctuation(const wstring wstr);//删除标点
wstring num_transform(wstring num_input);//全角数字转半角数字
double get_prob_emit(string data);//emit_prob string to double
void input_str_transform(string input);//汉字序列处理与划分

void stopword_readin(string filename);//读入停止词表
void testtxtfile_readin_split(string filename);//读入测试文件并划分
void traintxtfile_readin(string filename);//读入训练文件并得到HMM参数
void prob_emit_readin();//读入发射矩阵
void prob_start_readin();//读入初始状态分布
void prob_trans_readin();//读入转移矩阵

void init_Viterbi();//Viterbi相关变量初始化
double prob_emit_value(int state, string character);//得到发射概率
double prob_start_value(int state);//得到初始概率
double prob_trans_value(int statefrom, int stateto);//得到转移概率
void weight_first_element(string first_character);//viterbi初始weight
int Viterbi();//Viterbi算法

void get_HMM_model();//获取HMM模型
void load_stopword();//模型装载停止词
void setup_HMM_model_data();//通过已有数据建立模型
void setup_HMM_model_train(string filepath, string filename);//通过监督学习建立模型
void HMM_teststr(string input);//测试输入语句
void HMM_testfile(string filepath, string filename);//测试输入文件