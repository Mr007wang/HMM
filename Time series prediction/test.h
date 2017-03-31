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

void normalization_a(int t);//前向算法alpha归一化
void normalization_b(int t);//后向算法beta归一化
void stockdata_readin();//股票数据读入
void forward();//前向算法
void backforward();//后向算法
void state_estimation();//参数估计
void train();//训练HMM模型
void init_parameter_3();//3状态初始化
void init_parameter_6();//6状态初始化
void print_model();//打印模型参数
void Viterbi();//Viterbi算法
