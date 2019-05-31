/*
* =====================================================================================
*
*       Filename:  gtt_highspeed.cpp
*
*    Description:  ���ٳ�����ʵ��
*
*        Version:  1.0
*        Created:
*       Revision:
*       Compiler:  VS_2015
*
*         Author:  WYB
*            LIB:  ITTC
*
* =====================================================================================
*/

#include<fstream>
#include<memory.h>
#include"route.h"
#include"config.h"
#include"gtt_highspeed.h"
#include"vue.h"
#include"vue_physics.h"
#include"imta.h"
#include"function.h"
#include"reflect/context.h"
#include"time_stamp.h"

using namespace std;

void gtt_highspeed::initialize() {
	gtt_highspeed_config* __config = get_config();
	int* m_pupr = new int[__config->get_road_num()];//ÿ��·�ϵĳ�����
	double* TotalTime = new double[__config->get_road_num()];//ÿ����·��ʼ����������������г��������������������õ���ʱ��
	std::list<double>* possion = new std::list<double>[__config->get_road_num()];//ÿ����·��ʼ��������ĳ�������ʱ����list����λs

	/*srand((unsigned)time(0));*/

	//���ɸ�ָ���ֲ��ĳ���������
	int tempVeUENum = 0;
	double lambda = 1 / 2.5;//��ֵΪ1/lambda������Э�鳵������ʱ�����ľ�ֵΪ2.5s
	for (int roadId = 0; roadId != __config->get_road_num(); roadId++) {
		TotalTime[roadId] = 0;
		while (TotalTime[roadId] * (__config->get_speed() / 3.6) < __config->get_road_length()) {
			double pV = 0.0;
			while (true)
			{
				pV = (double)rand() / (double)RAND_MAX;
				if (pV != 1)
				{
					break;
				}
			}
			pV = (-1.0 / lambda)*log(1 - pV);
			possion[roadId].push_back(pV);
			TotalTime[roadId] += pV;
			double check = TotalTime[roadId];
		}
		m_pupr[roadId] = static_cast<int>(possion[roadId].size());//��ɵ�ǰ��·���ܳ������ĸ�ֵ
		tempVeUENum += m_pupr[roadId];
	}

	//���г���������
	m_vue_array = new vue[tempVeUENum];
	int vue_id = 0;
	cout << "vuenum: " << tempVeUENum << endl;

	ofstream vue_coordinate;
	vue_coordinate.open("log/vue_coordinate.txt");

	for (int roadId = 0; roadId != __config->get_road_num(); roadId++) {
		for (int uprIdx = 0; uprIdx != m_pupr[roadId]; uprIdx++) {
			auto p = get_vue_array()[vue_id++].get_physics_level();
			p->m_speed = __config->get_speed()/3.6;
		    p->m_absx = -1732 + (TotalTime[roadId] - possion[roadId].back())*(p->m_speed);
			p->m_absy = __config->get_road_topo_ratio()[roadId * 2 + 1]* __config->get_road_width();
			if (roadId < __config->get_road_num() / 2)  p->m_vangle = 180;
			else p->m_vangle = 0;
			//�����������������txt�ļ�
			vue_coordinate << p->m_absx << " ";
			vue_coordinate << p->m_absy << " ";
			vue_coordinate << endl;

			TotalTime[roadId] = TotalTime[roadId] - possion[roadId].back();
			possion[roadId].pop_back();
		
			//�����Ƿ����ʱ�ֵ���Դ�����㷨�����Ƿ�ά��m_slot_time_idx,����ǰ�����ܷ������ݵ�TTI
			int granularity = get_rrm_config()->get_time_division_granularity();
			if (granularity == 2) {
				double zone_length = 173.0f;
				int zone_idx = (int)abs((p->m_absx - (-1732.0f)) / zone_length);//0��9
				if ((zone_idx + 1) % 2 == 1) {
					p->m_slot_time_idx = 0;//����ǰ�������odd subframe����ֵ0
				}
				else {
					p->m_slot_time_idx = 1;//����ǰ�������even subframe����ֵ1
				}
			}
		}
	}

	vue_coordinate.close();

	memory_clean::safe_delete(m_pupr, true);
	memory_clean::safe_delete(TotalTime, true);
	memory_clean::safe_delete(possion, true);

	vue_physics::s_pl_all.assign(get_vue_num(), std::vector<double>(get_vue_num(), 0));
	vue_physics::s_distance_all.assign(get_vue_num(), std::vector<double>(get_vue_num(), 0));
}

int gtt_highspeed::get_vue_num() {
	return vue_physics::get_vue_num();
}

int gtt_highspeed::get_freshtime() {
	return get_config()->get_freshtime();
}

void gtt_highspeed::fresh_location() {
	//<Warn>:���ŵ�ˢ��ʱ���λ��ˢ��ʱ��ֿ�
	if (get_time()->get_tti() % get_config()->get_freshtime() != 0) {
		return;
	}
	for (int vue_id = 0; vue_id < get_vue_num(); vue_id++) {
		get_vue_array()[vue_id].get_physics_level()->update_location_highspeed();

		//ÿ�θ��³���λ��ʱ�����жϳ������ڵ�zone_idx
		auto p = get_vue_array()[vue_id].get_physics_level();
		int granularity = get_rrm_config()->get_time_division_granularity();
		if (granularity == 2) {
			double zone_length = 173.0f;
			p->m_zone_id = (int)abs((p->m_absx - (-1732.0f)) / zone_length);//0��9
			p->m_zone_shift = abs(p->m_absx + 1732.0f - (p->m_zone_id + 0.5f)*zone_length);
			if ((p->m_zone_id + 1) % 2 == 1) {
				p->m_slot_time_idx = 0;//����ǰ�������odd subframe����ֵ0
			}
			else {
				p->m_slot_time_idx = 1;//����ǰ�������even subframe����ֵ1
			}
		}
	}

	for (int vue_id1 = 0; vue_id1 < get_vue_num(); vue_id1++) {
		for (int vue_id2 = 0; vue_id2 < vue_id1; vue_id2++) {
			auto vuei = get_vue_array()[vue_id1].get_physics_level();
			auto vuej = get_vue_array()[vue_id2].get_physics_level();
			vue_physics::set_distance(vue_id2, vue_id1, sqrt(pow((vuei->m_absx - vuej->m_absx), 2.0f) + pow((vuei->m_absy - vuej->m_absy), 2.0f)));
			calculate_pl(vue_id1, vue_id2);
		}
	}
}


void gtt_highspeed::calculate_pl(int t_vue_id1, int t_vue_id2) {

	location _location;
	_location.eNBAntH = 5;
	_location.VeUEAntH = 1.5;
	_location.RSUAntH = 5;
	_location.locationType = None;
	_location.distance = 0;
	_location.distance1 = 0;
	_location.distance2 = 0;

	antenna _antenna;
	_antenna.antGain = 3;
	_antenna.byTxAntNum = 1;
	_antenna.byRxAntNum = 2;

	imta* __imta = new imta();

	auto vuei = get_vue_array()[t_vue_id1].get_physics_level();
	auto vuej = get_vue_array()[t_vue_id2].get_physics_level();

	_location.locationType = Los;
	_location.manhattan = false;

	double angle = 0;
	_location.distance = vue_physics::get_distance(t_vue_id1, t_vue_id2);

	angle = atan2(vuei->m_absy - vuej->m_absy, vuei->m_absx - vuej->m_absx) / imta::s_DEGREE_TO_PI;

	imta::randomGaussian(_location.posCor, 5, 0.0f, 1.0f);//������˹�������Ϊ�����ŵ�ϵ��ʹ��

	double m_FantennaAnglei;
	double m_FantennaAnglej;

	imta::randomUniform(&m_FantennaAnglei, 1, 180.0f, -180.0f, false);
	imta::randomUniform(&m_FantennaAnglej, 1, 180.0f, -180.0f, false);

	_antenna.TxAngle = angle - m_FantennaAnglei;
	_antenna.RxAngle = angle - m_FantennaAnglej;
	_antenna.TxSlantAngle = new double[_antenna.byTxAntNum];
	_antenna.TxAntSpacing = new double[_antenna.byTxAntNum];
	_antenna.RxSlantAngle = new double[_antenna.byRxAntNum];
	_antenna.RxAntSpacing = new double[_antenna.byRxAntNum];
	_antenna.TxSlantAngle[0] = 90.0f;
	_antenna.TxAntSpacing[0] = 0.0f;
	_antenna.RxSlantAngle[0] = 90.0f;
	_antenna.RxSlantAngle[1] = 90.0f;
	_antenna.RxAntSpacing[0] = 0.0f;
	_antenna.RxAntSpacing[1] = 0.5f;

	double t_Pl = 0;

	__imta->build(&t_Pl, imta::s_FC, _location, _antenna, vuei->m_speed, vuej->m_speed, vuei->m_vangle, vuej->m_vangle);//�����˽�������ŵ�ģ�ͼ���UE֮���ŵ�ϵ��

	vue_physics::set_pl(t_vue_id1, t_vue_id2, t_Pl);

	memory_clean::safe_delete(_antenna.TxSlantAngle, true);
	memory_clean::safe_delete(_antenna.TxAntSpacing, true);
	memory_clean::safe_delete(_antenna.RxSlantAngle, true);
	memory_clean::safe_delete(_antenna.RxAntSpacing, true);
	memory_clean::safe_delete(__imta);
}

