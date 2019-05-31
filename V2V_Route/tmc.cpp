/*
* =====================================================================================
*
*       Filename:  tmc.cpp
*
*    Description:  业务模型与控制类实现
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

#include<random>
#include<fstream>
#include"gtt.h"
#include"tmc.h"
#include"vue.h"
#include"vue_physics.h"
#include"config.h"
#include"route_udp.h"
#include"reflect/context.h"


using namespace std;

void tmc::statistic() {
	context* __context = context::get_context();

	ofstream output;

	output.open("log/output.txt");

	object* __object = context::get_context()->get_bean("route");

	route_udp* __route_udp = (route_udp*)__object;
	int success_num = __route_udp->get_success_route_event_num();
	int failed_num = __route_udp->get_failed_route_event_num();
	int total_num = success_num + failed_num;
	double pdr = double(success_num) / double(total_num)*double(100);

	output << "total success event: " << success_num << endl;
	output << "total failed event: " << failed_num << endl;
	output << "total event number:" << total_num << endl;
	output << "pdr:" << pdr << "%" << endl;

}