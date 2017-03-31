#pragma once
#include<iostream>
#include<fstream>
#include<string>
#include<set>
#include<cstdio>
#include <vector>
#include<map>
#include <ctype.h>
#include<boost\regex.hpp>

#define PRICE_ERROR -1
#define PRICE_SMOOTH_R 0
#define PRICE_RISE 1
#define PRICE_FALL 2
#define PRICE_RISE_L 3
#define PRICE_FALL_L 4
#define PRICE_SMOOTH_F 5

#define PRICE_STATE_NUM 3
#define PRICE_STATE_NUM_L 6

#define RANGE_ERROR -1
#define RANGE_STATE_NUM 6
#define RANGE_O1 0
#define RANGE_O2 1
#define RANGE_O3 2
#define RANGE_O4 3
#define RANGE_O5 4
#define RANGE_O6 5

#define O1_RANGE_LEFT -100
#define O1_RANGE_RIGHT -0.03
#define O2_RANGE_LEFT O1_RANGE_RIGHT
#define O2_RANGE_RIGHT -0.01
#define O3_RANGE_LEFT O2_RANGE_RIGHT
#define O3_RANGE_RIGHT 0
#define O4_RANGE_LEFT O3_RANGE_RIGHT
#define O4_RANGE_RIGHT 0.01
#define O5_RANGE_LEFT O4_RANGE_RIGHT
#define O5_RANGE_RIGHT 0.03
#define O6_RANGE_LEFT O5_RANGE_RIGHT
#define O6_RANGE_RIGHT 100

#define MAX_NUM_DAY 17000

#define BEGIN_DATE_NUM	15096
//#define BEGIN_DATE_NUM 16000
#define END_DATE_NUM   16353

#define TEST_BEGIN 16354
#define TEST_END  16550
using namespace std;

void normalization_a(int t);//ǰ���㷨alpha��һ��
void normalization_b(int t);//�����㷨beta��һ��
void stockdata_readin();//��Ʊ���ݶ���
void forward();//ǰ���㷨
void backforward();//�����㷨
void state_estimation();//��������
void train();//ѵ��HMMģ��
void init_parameter_3();//3״̬��ʼ��
void init_parameter_6();//6״̬��ʼ��
void print_model();//��ӡģ�Ͳ���
void Viterbi();//Viterbi�㷨
