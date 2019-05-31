#include<iostream>
#include<iomanip>
#include<fstream>
#include<sstream>
#include"route_udp.h"
#include"config.h"
#include"gtt.h"
#include"wt.h"
#include"vue.h"
#include"vue_physics.h"
#include"function.h"
#include"reflect/context.h"
#include"time_stamp.h"

using namespace std;

int route_udp_route_event::s_event_count = 0;

default_random_engine route_udp::s_engine;

int route_udp_node::s_node_count = 0;

default_random_engine route_udp_node::s_engine;

std::vector<std::set<int>> route_udp_node::s_node_id_per_pattern;

void route_udp_link_event::transimit() {
	//<Warn>:һ������ָ��Ƶ�δ��䣬���ٷֳɶ�����ֱ�ѡ��Ƶ�δ�����,Hello�������ݰ�Ƶ��
	int total_pattern_num = ((rrm_config*)context::get_context()->get_bean("rrm_config"))->get_pattern_num();

	if (++m_tti_idx == m_tti_num) {
		m_is_finished = true;
	}

	if (get_pattern_idx() < 0 || get_pattern_idx() > total_pattern_num ) throw logic_error("error");
	double sinr = ((wt*)context::get_context()->get_bean("wt"))->calculate_sinr(
		get_source_node_id(),
		get_destination_node_id(),
		get_pattern_idx(),
		route_udp_node::get_node_id_set(get_pattern_idx()));
	sinr_per_tti.push_back(sinr);
	infer_node_per_tti.push_back(route_udp_node::get_node_id_set(get_pattern_idx()));

	if (sinr < ((rrm_config*)context::get_context()->get_bean("rrm_config"))->get_drop_sinr_boundary()){
		m_is_loss = true;
	}
}

const std::set<int>& route_udp_node::get_node_id_set(int t_pattern_idx) {
	return s_node_id_per_pattern[t_pattern_idx];
}

route_udp_node::route_udp_node() {
	context* __context = context::get_context();

	//��ʼ�����η���hello����ʱ�䣬Ŀ��Ϊ���ڹ涨��TTI����ɳ������������֤ÿ�������������Լ����ڽӱ�
	int interval = ((route_config*)__context->get_bean("route_config"))->get_t_interval();
	uniform_int_distribution<int> u_tti_hello_start(0, interval);
	m_tti_next_hello = u_tti_hello_start(s_engine);
}

pair<int, int> route_udp_node::select_relay_information() {
	context*__context = context::get_context();
	pair<int, int> res = make_pair<int, int>(-1, -1);

	int pattern_num = ((rrm_config*)context::get_context()->get_bean("rrm_config"))->get_pattern_num();
	//����ѡ·�ɳ���id
	//<Warn>�������������㷨
	int final_destination_node_id = peek_send_event_queue()->get_final_destination_node_id();
	if (final_destination_node_id != -1) {//�ж��Ƿ�ΪHello���¼��������������ѡ��һ��
		int final_destination_zone_id = ((gtt*)__context->get_bean("gtt"))->get_vue_array()[final_destination_node_id].get_physics_level()->get_zone_id();
		//double vanglei = ((gtt*)__context->get_bean("gtt"))->get_vue_array()[get_id()].get_physics_level()->get_angle();
		//double xi = ((gtt*)__context->get_bean("gtt"))->get_vue_array()[get_id()].get_physics_level()->get_absx();
		//double yi = ((gtt*)__context->get_bean("gtt"))->get_vue_array()[get_id()].get_physics_level()->get_absy();
		double min_distance = (numeric_limits<double>::max)();
		int source_zone_id = ((gtt*)__context->get_bean("gtt"))->get_vue_array()[get_id()].get_physics_level()->get_zone_id();
		int source_slot = ((gtt*)__context->get_bean("gtt"))->get_vue_array()[get_id()].get_physics_level()->get_slot_time_idx();

		for (auto p : m_adjacent_list) {
			//double vi, vj;
			
			//double speed = ((gtt*)__context->get_bean("gtt"))->get_vue_array()[near_node_id].get_physics_level()->get_speed();
			//double vanglej = ((gtt*)__context->get_bean("gtt"))->get_vue_array()[near_node_id].get_physics_level()->get_angle();

			//double xj = ((gtt*)__context->get_bean("gtt"))->get_vue_array()[near_node_id].get_physics_level()->get_absx();
			//double yj = ((gtt*)__context->get_bean("gtt"))->get_vue_array()[near_node_id].get_physics_level()->get_absy();

			//if (vanglei == 0) vi = speed;
			//else vi = -speed;

			//if (vanglej == 0) vj = speed;
			//else vj = -speed;

			//double radius = 300;

			//if (vi != vj) {

			//	double A = pow(vi - vj, 2);
			//	double B = 2 * (xi - xj)*(vi - vj);
			//	double C = pow(xi - xj, 2) + pow(yi - yj, 2) - radius*radius;


			//	double temp = B*B - 4 * A*C;
			//	double t1 = (-1 * B + sqrt(temp)) / (2 * A);
			//	double t2 = (-1 * B - sqrt(temp)) / (2 * A);
			//	double t;
			//	if (t1 > 0 && t2 < 0) t = t1;
			//	if (t1 < 0 && t2>0) t = t2;
			//	if (t1 > 0 && t2 > 0) t = min(t1, t2);
			//	if (t < 7) continue;
			//}
			//else {
			//	double distance = sqrt(pow(xi - xj, 2) + pow(yi - yj, 2));
			//	if (distance > radius) continue;
			//}
			//object* __object = context::get_context()->get_bean("route");
			//route_udp* __route_udp = (route_udp*)__object;
			//double neighbor_num = __route_udp->get_node_array()[near_node_id].get_adjacent_list().size();
			//if (vanglej != vanglei) {
			//	double temp1 = xi - xj;
			//	if (temp1*vanglej < 0) continue;
			//}
			//double cur_distance = vue_physics::get_distance(final_destination_node_id, near_node_id);
			//double cur_value = 0.5*(cur_distance / speed) + 0.5*(radius / neighbor_num);
			int near_node_id = p.first;
			int near_slot = ((gtt*)__context->get_bean("gtt"))->get_vue_array()[near_node_id].get_physics_level()->get_slot_time_idx();
			int near_zone_id = ((gtt*)__context->get_bean("gtt"))->get_vue_array()[near_node_id].get_physics_level()->get_zone_id();
			double cur_distance;
			if (p.second.sinr[0] > 5) {
				if (final_destination_zone_id == source_zone_id) {
					cur_distance = vue_physics::get_distance(final_destination_node_id, near_node_id);
					if (cur_distance < min_distance) {
						min_distance = cur_distance;
						res.first = near_node_id;
					}
				}
				else {
					int next_zone_id;
					if (final_destination_zone_id > source_zone_id) {
						next_zone_id = source_zone_id + 1;
					}
					else {
						next_zone_id = source_zone_id - 1;
					}
					double transimit_distance = vue_physics::get_distance(get_id(), near_node_id);
					cur_distance = vue_physics::get_distance(final_destination_node_id, near_node_id);
					if (near_zone_id== next_zone_id&&cur_distance < min_distance&&transimit_distance<500) {
						min_distance = cur_distance;
						res.first = near_node_id;
					}
				}
			}
			//if (cur_distance < min_distance) {
			//	min_distance = cur_distance;
			//	res.first = near_node_id;
			//}
		}
		//int current_distance;
		//int zone_id = ((gtt*)__context->get_bean("gtt"))->get_vue_array()[res.first].get_physics_level()->get_zone_id();
		//double min_distance1 = (numeric_limits<double>::max)();
		//if (final_destination_zone_id != source_zone_id) {
		//	for (auto p : m_adjacent_list) {
		//		if (p.second.sinr[0] > 10) {
		//			int near_node_id = p.first;
		//			int near_zone_id = ((gtt*)__context->get_bean("gtt"))->get_vue_array()[near_node_id].get_physics_level()->get_zone_id();
		//			current_distance = vue_physics::get_distance(get_id(), near_node_id);
		//			if (near_zone_id == zone_id&&current_distance < min_distance1) {
		//				min_distance = current_distance;
		//				res.first = near_node_id;
		//			}
		//		}
		//	}
		//}
		if (res.first != -1) {
			ofstream s_logger_select;
			s_logger_select.open("log/select.txt");
			s_logger_select << ((gtt*)__context->get_bean("gtt"))->get_vue_array()[get_id()].get_physics_level()->get_absx() << " " << ((gtt*)__context->get_bean("gtt"))->get_vue_array()[get_id()].get_physics_level()->get_absy() << endl;
			s_logger_select << ((gtt*)__context->get_bean("gtt"))->get_vue_array()[final_destination_node_id].get_physics_level()->get_absx() << " " << ((gtt*)__context->get_bean("gtt"))->get_vue_array()[final_destination_node_id].get_physics_level()->get_absy() << endl;
			for (auto p : m_adjacent_list) {
				s_logger_select << ((gtt*)__context->get_bean("gtt"))->get_vue_array()[p.first].get_physics_level()->get_absx() << " " << ((gtt*)__context->get_bean("gtt"))->get_vue_array()[p.first].get_physics_level()->get_absy() << endl;
			}
			s_logger_select << ((gtt*)__context->get_bean("gtt"))->get_vue_array()[res.first].get_physics_level()->get_absx() << " " << ((gtt*)__context->get_bean("gtt"))->get_vue_array()[res.first].get_physics_level()->get_absy() << endl;
			s_logger_select.close();
			//GPSR�������ǰ�ڵ���ڽӵ㵽Ŀ�Ľڵ�ľ��붼������Ŀ�Ľڵ�ľ���Զ�����ж϶���
			//<Warn>����߽�ת��ģʽ
			//if (min_distance > vue_physics::get_distance(get_id(), peek_send_event_queue()->get_final_destination_node_id())) {
			//	res.first = -1;
			//}
			int j = 1;
		}

	}
	
	//��ѡƵ�Σ�����Ƿ���Hello������������ר��pattern�Ϸ���(�����һ��pattern������Ƿ������ݰ���δռ�õ�Ƶ�������ѡ��
	if (final_destination_node_id == -1) {
		res.second = pattern_num - 1;
	}
	else if (res.first != -1) {
		//���ѡ�������λ����Դ����
		vector<int> candidate;
		for (int pattern_idx = 0; pattern_idx < pattern_num -1; pattern_idx++) {
				candidate.push_back(pattern_idx);
		}

		if (candidate.size() != 0) {
			//��δռ�õ�Ƶ���������ѡһ��
			//<Warn>�������������㷨
			uniform_int_distribution<int> u(0, static_cast<int>(candidate.size()) - 1);
			res.second = candidate[u(s_engine)];
		}

		//int temp = get_id();
		//context* __context = context::get_context();
		//int pattern_idx = ((gtt*)__context->get_bean("gtt"))->get_vue_array()[get_id()].get_physics_level()->get_zone_id();
		//	res.second = pattern_idx;
	}
	return res;
}

ofstream route_udp::s_logger_delay;
ofstream route_udp::s_logger_event;
ofstream route_udp::s_logger_hop;

void route_udp::initialize() {
	context* __context = context::get_context();
	int vue_num = get_gtt()->get_vue_num();
	m_node_array = new route_udp_node[vue_num];

	s_logger_delay.open("log/route_udp_delay.txt");
	s_logger_event.open("log/route_udp_event.txt");
	s_logger_hop.open("log/route_udp_hop.txt");

	route_udp_node::s_node_id_per_pattern = vector<set<int>>(get_rrm_config()->get_pattern_num());//Hello�������ݰ�Ƶ��
}

void route_udp::log_link(int source_id, int destination_id, adjacent_message previous, adjacent_message current) {
	context*__context = context::get_context();
	s_logger_event << "[" << previous.receive_x << "," << previous.receive_y << "],[" << previous.send_x << "," << previous.send_y << "],Pattern:" << previous.pattern_id << ",SINR:" << previous.sinr[0] << endl;
	s_logger_event << "[" << current.send_x << "," << current.send_y << "],[" << current.receive_x << "," << current.receive_y << "],Pattern:" << current.pattern_id << ",SINR:";
	int count = 0;
	while (count != current.sinr.size()) {
		s_logger_event << current.sinr[count] << ",";
		count++;
	}
	s_logger_event << endl << "Source_zone_id:" << ((gtt*)__context->get_bean("gtt"))->get_vue_array()[source_id].get_physics_level()->get_zone_id() << " Destination_zone_id:" << ((gtt*)__context->get_bean("gtt"))->get_vue_array()[destination_id].get_physics_level()->get_zone_id() << endl;
	for (int i = 0; i < 4; i++) {
		set<int>::iterator count1 = current.infer_node_id[i].begin();
		while (count1 != current.infer_node_id[i].end()) {
			if (*count1 == source_id) {
				count1++;
				continue;
			} 
			s_logger_event << "[" << get_gtt()->get_vue_array()[*count1].get_physics_level()->m_absx << "," << get_gtt()->get_vue_array()[*count1].get_physics_level()->m_absy << "]";
			s_logger_event << "Infer_zone_id:" << ((gtt*)__context->get_bean("gtt"))->get_vue_array()[(*count1)].get_physics_level()->get_zone_id() << ";  ";
			count1++;
		}
		s_logger_event << endl;
	}
	s_logger_event << endl;
}

void route_udp::process_per_tti() {
	//ɾ����ʱ�ڵ�
	update_adjacent_list();

	//�¼�����
	event_trigger();

    //����Ҫ��ʼ���͵��¼�
	start_sending_data();

	//���䵱ǰTTI���ڵ��¼�
	transmit_data();
}

//�򵥵ĸ��ݾ���ά���ڽӱ�
void route_udp::update_route_table_from_physics_level() {

}

//ɾ����ʱ�ڵ�
void route_udp::update_adjacent_list() {
	for (int source_node_id = 0; source_node_id < route_udp_node::s_node_count; source_node_id++) {
		route_udp_node& source_node = get_node_array()[source_node_id];

		context* __context = context::get_context();
		int current_tti = get_time()->get_tti();
		int interval = 1.5*get_route_config()->get_t_interval();
		vector<pair<int, adjacent_message>>::iterator it= source_node.m_adjacent_list.begin();
		while (it != source_node.m_adjacent_list.end()) {
			if ((current_tti - it->second.life_time) > interval) {
				it = source_node.m_adjacent_list.erase(it);
			}
			else {
				it++;
			}
		}
	}
}

void route_udp::event_trigger() {
	context* __context = context::get_context();
	double trigger_rate = get_tmc_config()->get_trigger_rate();

	uniform_real_distribution<double> u_rate(0, 1);

	//ͨ�������ļ�������hello���Ĵ���ƽ�����
	int interval = get_route_config()->get_t_interval();
	uniform_int_distribution<int> u_tti_hello_between(0.5*interval, 1.5*interval);

	//����Hello��
	for (int origin_source_node_id = 0; origin_source_node_id < route_udp_node::s_node_count; origin_source_node_id++) {
		route_udp_node& source_node = get_node_array()[origin_source_node_id];

		//�ж��Ƿ񵽸÷���hello����ʱ��
		if (source_node.m_tti_next_hello == get_time()->get_tti()) {
			get_node_array()[origin_source_node_id].offer_send_event_queue(
				new route_udp_route_event(origin_source_node_id, -1, HEllO,get_time()->get_tti())
			);

			//������һ�η���hello����ʱ��
			source_node.m_tti_next_hello += u_tti_hello_between(s_engine);
		}
	}

	//�ڳ�ʼ��ʱ����󣬴������ݴ����¼�
	if(get_time()->get_tti()>interval){
		for (int origin_source_node_id = 0; origin_source_node_id < route_udp_node::s_node_count; origin_source_node_id++) {
			if (u_rate(s_engine) < trigger_rate) {

				//���սڵ���Դ�ڵ�֮��ľ�����Լ��
				vector<int> node_set_indistance;
				for (int receive_node_id = 0; receive_node_id < route_udp_node::s_node_count; receive_node_id++) {
					if (receive_node_id == origin_source_node_id) continue;
					if (vue_physics::get_distance(receive_node_id, origin_source_node_id) < 1000) {
						node_set_indistance.push_back(receive_node_id);
					}
				}

				uniform_int_distribution<int> u_node_id(0, node_set_indistance.size());
				int final_destination_node_id = u_node_id(s_engine);
				
				get_node_array()[origin_source_node_id].offer_send_event_queue(
					new route_udp_route_event(origin_source_node_id, final_destination_node_id, DATA,get_time()->get_tti())
				);
			}
		}
	}
}

void route_udp::start_sending_data() {
	//����ͬһʱ�̷���Ϣ����������Ϣ��ԭ�����з���Ϣ���¼��ڴ���ǰ��ѡ����Ƶ�β�����ռ��
	for (int source_node_id = 0; source_node_id < route_udp_node::s_node_count; source_node_id++) {
		route_udp_node& source_node = get_node_array()[source_node_id];

		if (source_node.sending_link_event.size() == 0) {//��ǰ�ڵ㲢�����ڴ���
			//��ǰ�����������¼��б�Ϊ�գ���������
			if (source_node.is_send_event_queue_empty()) continue;

			if (source_node.m_send_event_queue.front()->get_route_event_type() == DATA) {//������¼������ݴ����¼�
				//ѡ���м̳����Լ�Ƶ��
				pair<int, int> select_res = source_node.select_relay_information();

				//����Ҳ�����һ��������ʱ����route_event
				if (select_res.first == -1) {
					//<Warn>���ݴ洢�¼���¼route_event����
					if (source_node.m_send_event_queue.empty()) throw logic_error("error");

					//�����β
					route_udp_route_event* temp = source_node.m_send_event_queue.front();
					source_node.m_send_event_queue.pop();
					delete temp;
					m_failed_route_event_num++;
					//source_node.m_send_event_queue.push(temp);
				}

				//�����ʱû�п���Ƶ�Σ��Ƴٵ���һ��TTI�ٳ��Է���
				if (select_res.second == -1) {
					continue;
				}

				////ά�������б�
				//if (route_udp_node::s_node_id_per_pattern[select_res.second].find(source_node_id) != route_udp_node::s_node_id_per_pattern[select_res.second].end()) throw logic_error("error");
				//if (select_res.second < 0 || select_res.second > 4) throw logic_error("error");
				//route_udp_node::s_node_id_per_pattern[select_res.second].insert(source_node_id);

				//������·�¼�
				if (select_res.second < 0 || select_res.second > 4) throw logic_error("error");
				source_node.sending_link_event.push_back(new route_udp_link_event(
					source_node_id, select_res.first, select_res.second, source_node.peek_send_event_queue()->get_tti_num()));
			}

			else {//������¼���Hello�������¼�
				//ѡ��Ƶ��
				pair<int, int> select_res = source_node.select_relay_information();

				////ά�������б�
				//if (route_udp_node::s_node_id_per_pattern[select_res.second].find(source_node_id) != route_udp_node::s_node_id_per_pattern[select_res.second].end()) throw logic_error("error");
				//if (select_res.second < 0 || select_res.second > 4) throw logic_error("error");
				//route_udp_node::s_node_id_per_pattern[select_res.second].insert(source_node_id);

				//�Գ��˸ýڵ�����������ڵ㴴����·�¼�
				for (int dst_id = 0; dst_id < route_udp_node::s_node_count; dst_id++) {
					if (dst_id == source_node_id) continue;
					source_node.sending_link_event.push_back(new route_udp_link_event(
						source_node_id, dst_id, select_res.second, source_node.peek_send_event_queue()->get_hello_tti_num()));
				}
			}
		}
	}
}

void route_udp::transmit_data() {
	for (int source_node_id = 0; source_node_id < route_udp_node::s_node_count; source_node_id++) {
		route_udp_node& source_node = get_node_array()[source_node_id];
		if (source_node.sending_link_event.size() == 0) continue;

		//������Դ��䣬�ȼ�������б�
		if (get_time()->get_tti() % get_rrm_config()->get_time_division_granularity() == get_gtt()->get_vue_array()[source_node_id].get_physics_level()->get_slot_time_idx()) {
			int link_pattern_id = source_node.sending_link_event[0]->get_pattern_idx();
			if (route_udp_node::s_node_id_per_pattern[link_pattern_id].find(source_node_id) != route_udp_node::s_node_id_per_pattern[link_pattern_id].end()) throw logic_error("error");
			if (link_pattern_id < 0 || link_pattern_id > 4) throw logic_error("error");
			route_udp_node::s_node_id_per_pattern[link_pattern_id].insert(source_node_id);
		}
	}

	//������link_event���е�һ�������Ŀ��1�����������¼���Ŀ��2��ά�����սڵ㴫��pattern��״̬
	for (int source_node_id = 0; source_node_id < route_udp_node::s_node_count; source_node_id++) {
		route_udp_node& source_node = get_node_array()[source_node_id];
		if (source_node.sending_link_event.size() == 0) continue;

		if (get_time()->get_tti() % 2 == get_gtt()->get_vue_array()[source_node_id].get_physics_level()->get_slot_time_idx()) {
			//�Ե�ǰ��������link_event���б�������
			vector<route_udp_link_event*>::iterator it;
			for (it = source_node.sending_link_event.begin(); it != source_node.sending_link_event.end(); it++) {

				route_udp_node& destination_node = get_node_array()[(*it)->get_destination_node_id()];

				//�¼�����
				(*it)->transimit();
			}
		}
	}

	for (int source_node_id = 0; source_node_id < route_udp_node::s_node_count; source_node_id++) {
		route_udp_node& source_node = get_node_array()[source_node_id];
		if (source_node.sending_link_event.size() == 0) continue;

		if (get_time()->get_tti() % get_rrm_config()->get_time_division_granularity() == get_gtt()->get_vue_array()[source_node_id].get_physics_level()->get_slot_time_idx()) {
			int link_pattern_id = source_node.sending_link_event[0]->get_pattern_idx();
			if (route_udp_node::s_node_id_per_pattern[link_pattern_id].find(source_node_id) == route_udp_node::s_node_id_per_pattern[link_pattern_id].end()) throw logic_error("error");
			if (link_pattern_id < 0 || link_pattern_id > 5) throw logic_error("error");
			route_udp_node::s_node_id_per_pattern[link_pattern_id].erase(source_node_id);
		}
	}

	//������link_event���еڶ�����������Ѿ�������ϵ��¼����в�����Ŀ��1��ͳ���¼�����ɹ����Ƕ�ʧ��Ŀ��2���޸ķ��ͽڵ�ͽ��սڵ㵱ǰpattern�ϵ�״̬,ά�������б�Ŀ��3������link_event������route_event
	for (int source_node_id = 0; source_node_id < route_udp_node::s_node_count; source_node_id++) {
		route_udp_node& source_node = get_node_array()[source_node_id];
		if (source_node.sending_link_event.size() == 0) continue;

		//�Ե�ǰ��������link_event���б���ά��
		vector<route_udp_link_event*>::iterator it;

		bool all_link_event_finished = false;//�����ж�����link_event�Ƿ�����ϣ���ɾ��link_event
		for (it = source_node.sending_link_event.begin(); it != source_node.sending_link_event.end(); it++) {

			if ((*it)->is_finished()) {
				all_link_event_finished = true;

				int pattern_idx = (*it)->m_pattern_idx;

				route_udp_node& destination_node = get_node_array()[(*it)->get_destination_node_id()];

				////����link_event������Ϻ�ά�������б�
				//if (it == source_node.sending_link_event.end() - 1){
				//	if (route_udp_node::s_node_id_per_pattern[pattern_idx].find(source_node_id) == route_udp_node::s_node_id_per_pattern[pattern_idx].end()) throw logic_error("error");
				//	if (pattern_idx < 0 || pattern_idx > 5) throw logic_error("error");
				//	route_udp_node::s_node_id_per_pattern[pattern_idx].erase(source_node_id);
				//}

				//�ж��Ƿ񶪰�
				if ((*it)->get_is_loss()) {
					if (source_node.m_send_event_queue.front()->get_route_event_type() == DATA) {//����������¼������շ�����С��500m�����¼��link_event
							//string loss_reason;
							//if ((*it)->m_loss_reason == INTER) loss_reason = "INTER";
							//else if ((*it)->m_loss_reason == TIMELY) loss_reason = "TIMELY";
							int count = 0;
							while (source_node.m_adjacent_list[count].first != (*it)->get_destination_node_id())
							{
								count++;
								if (count == source_node.m_adjacent_list.size()) break;
							}
							if (count != source_node.m_adjacent_list.size()) {
								context* __context = context::get_context();
								adjacent_message temp;
								temp.pattern_id = pattern_idx;
								temp.sinr = (*it)->sinr_per_tti;
								temp.infer_node_id = (*it)->infer_node_per_tti;
								temp.send_x = get_gtt()->get_vue_array()[destination_node.get_id()].get_physics_level()->m_absx;
								temp.send_y = get_gtt()->get_vue_array()[destination_node.get_id()].get_physics_level()->m_absy;
								temp.receive_x = get_gtt()->get_vue_array()[source_node_id].get_physics_level()->m_absx;
								temp.receive_y = get_gtt()->get_vue_array()[source_node_id].get_physics_level()->m_absy;
								temp.pl = vue_physics::get_pl(source_node_id, destination_node.get_id());
								log_link(source_node_id, (*it)->get_destination_node_id(),source_node.m_adjacent_list[count].second, temp);
							}

							//if (abs(source_node.m_adjacent_list[count].second.send_node_x - temp.send_node_x) < 50 && abs(source_node.m_adjacent_list[count].second.send_node_y - temp.send_node_y) < 50 &&
							//	abs(source_node.m_adjacent_list[count].second.receive_node_x - temp.receive_node_x) < 50 && abs(source_node.m_adjacent_list[count].second.receive_node_y - temp.receive_node_y) < 50)//�����Ƶĳ����ų�����
								m_failed_route_event_num++;
						
					}

					if (source_node.m_send_event_queue.empty()) throw logic_error("error");

					//����link_event������Ϻ�ɾ��route_event
					if (it == source_node.sending_link_event.end() - 1) {
						route_udp_route_event* temp = source_node.m_send_event_queue.front();

						//ɾ��route_event
						source_node.m_send_event_queue.pop();
						//if (source_node.m_send_event_queue.front()->get_route_event_type() == HEllO) delete temp;
						delete temp;
					}
				}

				else {
					//����������¼�������µ�ǰ�ڵ�id�����ж��Ƿ���ճɹ�
					if (source_node.m_send_event_queue.front()->get_route_event_type() == DATA) {
						if (source_node.sending_link_event.size() != 1) throw logic_error("error");

						if (source_node.m_send_event_queue.front()->get_final_destination_node_id() == destination_node.get_id()) {

							//OP1:��¼route_event���ͳɹ�,�������ñ��浽�б���

							int now_tti = get_time()->get_tti();

							int delay = get_time()->get_tti() - source_node.m_send_event_queue.front()->get_start_tti();
							int hop = source_node.m_send_event_queue.front()->get_hop();
							s_logger_delay << delay << " ";
							s_logger_hop << hop << " ";
							m_success_route_event_num++;
							

							//OP2��ֻ������Ҫ�����ݣ�Ȼ��route_eventɾ�����ͷſռ�
							route_udp_route_event* temp = source_node.m_send_event_queue.front();
							source_node.m_send_event_queue.pop();
							delete temp;
						}

						else {
							//�����û�е�����Ŀ�Ľڵ���route_event���ݸ���һ���ڵ�
							source_node.m_send_event_queue.front()->add_hop();
							destination_node.offer_send_event_queue(source_node.poll_send_event_queue());
						}
					}

					//�����Hello�����򽫽��յ�����Դ��ַ���뵱ǰ�����ڽӱ�
					else {
						context* __context = context::get_context();
						adjacent_message temp;
						temp.pattern_id = pattern_idx;
						temp.life_time = get_time()->get_tti();
						/*temp.infer_node_id = route_udp_node::s_node_id_per_pattern[pattern_idx];*/
						temp.sinr = (*it)->sinr_per_tti;
						temp.send_x = get_gtt()->get_vue_array()[destination_node.get_id()].get_physics_level()->m_absx;
						temp.send_y = get_gtt()->get_vue_array()[destination_node.get_id()].get_physics_level()->m_absy;
						temp.receive_x = get_gtt()->get_vue_array()[source_node_id].get_physics_level()->m_absx;
						temp.receive_y = get_gtt()->get_vue_array()[source_node_id].get_physics_level()->m_absy;
						temp.pl = vue_physics::get_pl(destination_node.get_id(), source_node_id);

						destination_node.add_to_adjacent_list(source_node.get_id(),temp);

						//����link_event������Ϻ�ɾ��route_event
						if (it == source_node.sending_link_event.end() - 1) {
							route_udp_route_event* temp = source_node.m_send_event_queue.front();
							source_node.m_send_event_queue.pop();
							delete temp;
						}
					}
				}
			}
		}

		//������ɵ�link_event ������Ϻ�ɾ������link_event
		if (all_link_event_finished == true) {
			vector<route_udp_link_event*>::iterator it = source_node.sending_link_event.begin();
			while (it != source_node.sending_link_event.end()) {
				delete *it;
				it++;
			}
			source_node.sending_link_event.clear();
		}
	}
}

