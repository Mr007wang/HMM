
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
string delete_punctuation(const wstring wstr);//ɾ�����
wstring num_transform(wstring num_input);//ȫ������ת�������
double get_prob_emit(string data);//emit_prob string to double
void input_str_transform(string input);//�������д����뻮��

void stopword_readin(string filename);//����ֹͣ�ʱ�
void testtxtfile_readin_split(string filename);//��������ļ�������
void traintxtfile_readin(string filename);//����ѵ���ļ����õ�HMM����
void prob_emit_readin();//���뷢�����
void prob_start_readin();//�����ʼ״̬�ֲ�
void prob_trans_readin();//����ת�ƾ���

void init_Viterbi();//Viterbi��ر�����ʼ��
double prob_emit_value(int state, string character);//�õ��������
double prob_start_value(int state);//�õ���ʼ����
double prob_trans_value(int statefrom, int stateto);//�õ�ת�Ƹ���
void weight_first_element(string first_character);//viterbi��ʼweight
int Viterbi();//Viterbi�㷨

void get_HMM_model();//��ȡHMMģ��
void load_stopword();//ģ��װ��ֹͣ��
void setup_HMM_model_data();//ͨ���������ݽ���ģ��
void setup_HMM_model_train(string filepath, string filename);//ͨ���ලѧϰ����ģ��
void HMM_teststr(string input);//�����������
void HMM_testfile(string filepath, string filename);//���������ļ�