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
	//<Warn>:一跳就在指定频段传输，不再分成多个包分别选择频段传输了,Hello包与数据包频分
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

	//初始化初次发送hello包的时间，目的为了在规定的TTI内完成程序的启动，保证每辆车都建立了自己的邻接表
	int interval = ((route_config*)__context->get_bean("route_config"))->get_t_interval();
	uniform_int_distribution<int> u_tti_hello_start(0, interval);
	m_tti_next_hello = u_tti_hello_start(s_engine);
}

pair<int, int> route_udp_node::select_relay_information() {
	context*__context = context::get_context();
	pair<int, int> res = make_pair<int, int>(-1, -1);

	int pattern_num = ((rrm_config*)context::get_context()->get_bean("rrm_config"))->get_pattern_num();
	//先挑选路由车辆id
	//<Warn>可以增加其他算法
	int final_destination_node_id = peek_send_event_queue()->get_final_destination_node_id();
	if (final_destination_node_id != -1) {//判断是否为Hello包事件，如果是则不需挑选下一跳
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
			//GPSR：如果当前节点的邻接点到目的节点的距离都比自身到目的节点的距离远，则判断丢包
			//<Warn>加入边界转发模式
			//if (min_distance > vue_physics::get_distance(get_id(), peek_send_event_queue()->get_final_destination_node_id())) {
			//	res.first = -1;
			//}
			int j = 1;
		}

	}
	
	//挑选频段，如果是发送Hello包，则在信令专用pattern上发送(）最后一个pattern，如果是发送数据包，未占用的频段上随机选择。
	if (final_destination_node_id == -1) {
		res.second = pattern_num - 1;
	}
	else if (res.first != -1) {
		//随机选择与地理位置资源分配
		vector<int> candidate;
		for (int pattern_idx = 0; pattern_idx < pattern_num -1; pattern_idx++) {
				candidate.push_back(pattern_idx);
		}

		if (candidate.size() != 0) {
			//在未占用的频段上随机挑选一个
			//<Warn>可以增加其他算法
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

	route_udp_node::s_node_id_per_pattern = vector<set<int>>(get_rrm_config()->get_pattern_num());//Hello包与数据包频分
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
	//删除过时节点
	update_adjacent_list();

	//事件触发
	event_trigger();

    //触发要开始发送的事件
	start_sending_data();

	//传输当前TTI存在的事件
	transmit_data();
}

//简单的根据距离维护邻接表
void route_udp::update_route_table_from_physics_level() {

}

//删除过时节点
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

	//通过配置文件来控制hello包的传输平均间隔
	int interval = get_route_config()->get_t_interval();
	uniform_int_distribution<int> u_tti_hello_between(0.5*interval, 1.5*interval);

	//触发Hello包
	for (int origin_source_node_id = 0; origin_source_node_id < route_udp_node::s_node_count; origin_source_node_id++) {
		route_udp_node& source_node = get_node_array()[origin_source_node_id];

		//判断是否到该发送hello包的时间
		if (source_node.m_tti_next_hello == get_time()->get_tti()) {
			get_node_array()[origin_source_node_id].offer_send_event_queue(
				new route_udp_route_event(origin_source_node_id, -1, HEllO,get_time()->get_tti())
			);

			//给定下一次发送hello包的时间
			source_node.m_tti_next_hello += u_tti_hello_between(s_engine);
		}
	}

	//在初始化时间过后，触发数据传输事件
	if(get_time()->get_tti()>interval){
		for (int origin_source_node_id = 0; origin_source_node_id < route_udp_node::s_node_count; origin_source_node_id++) {
			if (u_rate(s_engine) < trigger_rate) {

				//接收节点与源节点之间的距离受约束
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
	//本着同一时刻发消息优先于收消息的原则，所有发消息的事件在传输前先选择传输频段并进行占用
	for (int source_node_id = 0; source_node_id < route_udp_node::s_node_count; source_node_id++) {
		route_udp_node& source_node = get_node_array()[source_node_id];

		if (source_node.sending_link_event.size() == 0) {//当前节点并非已在传输
			//当前车辆待发送事件列表为空，跳过即可
			if (source_node.is_send_event_queue_empty()) continue;

			if (source_node.m_send_event_queue.front()->get_route_event_type() == DATA) {//如果该事件是数据传输事件
				//选择中继车辆以及频段
				pair<int, int> select_res = source_node.select_relay_information();

				//如果找不到下一跳，则暂时储存route_event
				if (select_res.first == -1) {
					//<Warn>根据存储事件记录route_event丢包
					if (source_node.m_send_event_queue.empty()) throw logic_error("error");

					//放入队尾
					route_udp_route_event* temp = source_node.m_send_event_queue.front();
					source_node.m_send_event_queue.pop();
					delete temp;
					m_failed_route_event_num++;
					//source_node.m_send_event_queue.push(temp);
				}

				//如果暂时没有可用频段，推迟到下一个TTI再尝试发送
				if (select_res.second == -1) {
					continue;
				}

				////维护干扰列表
				//if (route_udp_node::s_node_id_per_pattern[select_res.second].find(source_node_id) != route_udp_node::s_node_id_per_pattern[select_res.second].end()) throw logic_error("error");
				//if (select_res.second < 0 || select_res.second > 4) throw logic_error("error");
				//route_udp_node::s_node_id_per_pattern[select_res.second].insert(source_node_id);

				//创建链路事件
				if (select_res.second < 0 || select_res.second > 4) throw logic_error("error");
				source_node.sending_link_event.push_back(new route_udp_link_event(
					source_node_id, select_res.first, select_res.second, source_node.peek_send_event_queue()->get_tti_num()));
			}

			else {//如果该事件是Hello包传输事件
				//选择频段
				pair<int, int> select_res = source_node.select_relay_information();

				////维护干扰列表
				//if (route_udp_node::s_node_id_per_pattern[select_res.second].find(source_node_id) != route_udp_node::s_node_id_per_pattern[select_res.second].end()) throw logic_error("error");
				//if (select_res.second < 0 || select_res.second > 4) throw logic_error("error");
				//route_udp_node::s_node_id_per_pattern[select_res.second].insert(source_node_id);

				//对除了该节点以外的其他节点创建链路事件
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

		//如果可以传输，先加入干扰列表
		if (get_time()->get_tti() % get_rrm_config()->get_time_division_granularity() == get_gtt()->get_vue_array()[source_node_id].get_physics_level()->get_slot_time_idx()) {
			int link_pattern_id = source_node.sending_link_event[0]->get_pattern_idx();
			if (route_udp_node::s_node_id_per_pattern[link_pattern_id].find(source_node_id) != route_udp_node::s_node_id_per_pattern[link_pattern_id].end()) throw logic_error("error");
			if (link_pattern_id < 0 || link_pattern_id > 4) throw logic_error("error");
			route_udp_node::s_node_id_per_pattern[link_pattern_id].insert(source_node_id);
		}
	}

	//对所有link_event进行第一遍遍历。目的1：传输所有事件。目的2：维护接收节点传输pattern的状态
	for (int source_node_id = 0; source_node_id < route_udp_node::s_node_count; source_node_id++) {
		route_udp_node& source_node = get_node_array()[source_node_id];
		if (source_node.sending_link_event.size() == 0) continue;

		if (get_time()->get_tti() % 2 == get_gtt()->get_vue_array()[source_node_id].get_physics_level()->get_slot_time_idx()) {
			//对当前结点的所有link_event进行遍历传输
			vector<route_udp_link_event*>::iterator it;
			for (it = source_node.sending_link_event.begin(); it != source_node.sending_link_event.end(); it++) {

				route_udp_node& destination_node = get_node_array()[(*it)->get_destination_node_id()];

				//事件传输
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

	//对所有link_event进行第二遍遍历，对已经传输完毕的事件进行操作。目的1：统计事件传输成功还是丢失。目的2：修改发送节点和接收节点当前pattern上的状态,维护干扰列表。目的3：销毁link_event，传递route_event
	for (int source_node_id = 0; source_node_id < route_udp_node::s_node_count; source_node_id++) {
		route_udp_node& source_node = get_node_array()[source_node_id];
		if (source_node.sending_link_event.size() == 0) continue;

		//对当前结点的所有link_event进行遍历维护
		vector<route_udp_link_event*>::iterator it;

		bool all_link_event_finished = false;//用于判断所有link_event是否传输完毕，以删除link_event
		for (it = source_node.sending_link_event.begin(); it != source_node.sending_link_event.end(); it++) {

			if ((*it)->is_finished()) {
				all_link_event_finished = true;

				int pattern_idx = (*it)->m_pattern_idx;

				route_udp_node& destination_node = get_node_array()[(*it)->get_destination_node_id()];

				////所有link_event处理完毕后，维护干扰列表
				//if (it == source_node.sending_link_event.end() - 1){
				//	if (route_udp_node::s_node_id_per_pattern[pattern_idx].find(source_node_id) == route_udp_node::s_node_id_per_pattern[pattern_idx].end()) throw logic_error("error");
				//	if (pattern_idx < 0 || pattern_idx > 5) throw logic_error("error");
				//	route_udp_node::s_node_id_per_pattern[pattern_idx].erase(source_node_id);
				//}

				//判断是否丢包
				if ((*it)->get_is_loss()) {
					if (source_node.m_send_event_queue.front()->get_route_event_type() == DATA) {//如果是数据事件，且收发距离小于500m，则记录该link_event
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
							//	abs(source_node.m_adjacent_list[count].second.receive_node_x - temp.receive_node_x) < 50 && abs(source_node.m_adjacent_list[count].second.receive_node_y - temp.receive_node_y) < 50)//将卷绕的车辆排除在外
								m_failed_route_event_num++;
						
					}

					if (source_node.m_send_event_queue.empty()) throw logic_error("error");

					//所有link_event处理完毕后，删除route_event
					if (it == source_node.sending_link_event.end() - 1) {
						route_udp_route_event* temp = source_node.m_send_event_queue.front();

						//删除route_event
						source_node.m_send_event_queue.pop();
						//if (source_node.m_send_event_queue.front()->get_route_event_type() == HEllO) delete temp;
						delete temp;
					}
				}

				else {
					//如果是数据事件，则更新当前节点id，并判断是否接收成功
					if (source_node.m_send_event_queue.front()->get_route_event_type() == DATA) {
						if (source_node.sending_link_event.size() != 1) throw logic_error("error");

						if (source_node.m_send_event_queue.front()->get_final_destination_node_id() == destination_node.get_id()) {

							//OP1:记录route_event传送成功,并且永久保存到列表里

							int now_tti = get_time()->get_tti();

							int delay = get_time()->get_tti() - source_node.m_send_event_queue.front()->get_start_tti();
							int hop = source_node.m_send_event_queue.front()->get_hop();
							s_logger_delay << delay << " ";
							s_logger_hop << hop << " ";
							m_success_route_event_num++;
							

							//OP2：只保留需要的数据，然后将route_event删除以释放空间
							route_udp_route_event* temp = source_node.m_send_event_queue.front();
							source_node.m_send_event_queue.pop();
							delete temp;
						}

						else {
							//如果还没有到最终目的节点则将route_event传递给下一个节点
							source_node.m_send_event_queue.front()->add_hop();
							destination_node.offer_send_event_queue(source_node.poll_send_event_queue());
						}
					}

					//如果是Hello包，则将接收到包的源地址加入当前车辆邻接表
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

						//所有link_event处理完毕后，删除route_event
						if (it == source_node.sending_link_event.end() - 1) {
							route_udp_route_event* temp = source_node.m_send_event_queue.front();
							source_node.m_send_event_queue.pop();
							delete temp;
						}
					}
				}
			}
		}

		//所有完成的link_event 处理完毕后，删除所有link_event
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

