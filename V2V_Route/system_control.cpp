/*
* =====================================================================================
*
*       Filename:  system_control.cpp
*
*    Description:  ϵͳ��ʵ��
*
*        Version:  1.0
*        Created:
*       Revision:
*       Compiler:  VS_2015
*
*         Author:  HCF
*            LIB:  ITTC
*
* =====================================================================================
*/

#include"system_control.h"
#include"function.h"
#include"config.h"
#include"gtt.h"
#include"gtt_highspeed.h"
#include"tmc.h"
#include"vue.h"
#include"route.h"
#include"reflect/context.h"
#include"time_stamp.h"

using namespace std;

void system_control::process() {
	int *ptti = new int(0);

	while (get_time()->get_tti() < get_global_control_config()->get_ntti()) {
		cout << "TTI: " << get_time()->get_tti() << endl;

		//�����˶�
		get_gtt()->fresh_location();

		//·�ɲ����
		get_route()->process_per_tti();

		get_time()->increasement();
	}

	get_tmc()->statistic();

	delete ptti;
}

