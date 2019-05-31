#pragma once
#include<iostream>
#include<vector>
#include<list>
#include<queue>
#include<set>
#include<utility>
#include<random>
#include<string>
#include"route.h"
#include"config.h"
#include"reflect/object.h"
#include"reflect/context.h"

using namespace std;

enum route_udp_route_event_type {
	HEllO,//����ά���ڽӱ��hello��
	DATA//���ڴ������ݵ����ݰ�
};

enum route_udp_link_event_loss_reason {
	INTER,//����
	TIMELY,//SINR������ֵ
};

struct adjacent_message {
	int pattern_id;
	std::vector<double> sinr;
	std::vector<set<int>> infer_node_id;
	int life_time;
	double send_x, send_y, receive_x, receive_y;
	double pl;
};

/*
* ����·�ɲ㣬���漰����������������Ϊnode
* ����һ���ڵ㣬�շ�ì�ܡ���ͬһʱ��ֻ���գ�����ֻ�ܷ�
* ��һ���ڵ㴦���շ�״̬ʱ���ؾ�һ������
* ���ڵ㴦����״̬ʱ���ýڵ����ΪԴ�ڵ����ϢҲ��������Ϻ��ٽ��з��ͣ�������ת����Ϣ֮��
* ���ڵ㴦�ڿ���״̬����ͬʱ�յ���������ת���������Ӧ��һ�����ܾ�����(������չΪ���ȼ�Ӧ��)
*/

class route_udp_node;

class route_udp_route_event {
	/*
	* �¼�������
	*/
private:
	int m_hop = 1;

public:
	int get_hop() {
		return m_hop;
	}

	void add_hop() {
		m_hop++;
	}

	/*
	* �¼����ڵ���ʼʱ��
	*/
private:
	int m_start_tti = 0;

public:
	int get_start_tti() {
		return m_start_tti;
	}

private:
	static int s_event_count;

private:
	/*
	* �������ͣ������ڰ�ͷ�����ã�������ȷ��ͬ���õ����ݰ�
	*/
	const route_udp_route_event_type m_route_event_type;
public:
	route_udp_route_event_type get_route_event_type() {
		return m_route_event_type;
	}

	/*
	* Դ�ڵ�
	*/
private:
	const int m_origin_source_node_id;
public:
	int get_origin_source_node_id() {
		return m_origin_source_node_id;
	}

	/*
	* Ŀ�Ľڵ�
	*/
private:
	const int m_final_destination_node_id;
public:
	int get_final_destination_node_id() {
		return m_final_destination_node_id;
	}

	/*
	* �¼�id
	*/
private:
	const int m_event_id;
public:
	int get_event_id() { return m_event_id; }

	/*
	* ���ݰ���С��ӳ���TTI
	*/
private:
	const int m_tti_num;
public:
	int get_tti_num() {
		return m_tti_num;
	}

	/*
	* Hello����С��ӳ��ΪTTI
	*/
private:
	const int m_hello_tti_num;
public:
	int get_hello_tti_num() {
		return m_hello_tti_num;
	}

public:
	/*
	* ���캯�����ṩ���¼�����ģ�����
	*/
	route_udp_route_event(int t_source_node, int t_destination_node,route_udp_route_event_type t_route_event_type,int current_tti) :
		m_route_event_type(t_route_event_type),
		m_event_id(s_event_count++),
		m_origin_source_node_id(t_source_node),
		m_final_destination_node_id(t_destination_node),
		m_hello_tti_num(((tmc_config*)context::get_context()->get_bean("tmc_config"))->get_hello_tti()),
		m_start_tti(current_tti),
		m_tti_num(((tmc_config*)context::get_context()->get_bean("tmc_config"))->get_package_num()) {
	}
};

class route_udp_link_event {
	friend class route_udp_node;
	friend class route_udp;


private:

	/*
	* ��link_evnent��SINR��������Ҫ����TTI�����������TTI�ϵ�SINR
	*/
	vector<double> sinr_per_tti;

	/*
	* ��link_evnent�ĸ��Žڵ��б�������Ҫ����TTI�����������TTI�ϵĸ��Žڵ��б�
	*/
	vector<set<int>> infer_node_per_tti;

	/*
	* ��link_evnent����ʧ�ܵ�ԭ��
	*/
	route_udp_link_event_loss_reason m_loss_reason;

private:
	/*
	* ��ǰ��·Դ�ڵ�id
	*/
	const int m_source_node_id;
public:
	int get_source_node_id() {
		return m_source_node_id;
	}

	/*
	* ��ǰ��·Ŀ�Ľڵ�id
	*/
private:
	const int m_destination_node_id;
public:
	int get_destination_node_id() {
		return m_destination_node_id;
	}

	/*
	* ���ݰ�����
	* ��Ҫ���͵����ݷ�װ��IP���ݰ�����������ЩIP���ݰ���ʧ����һ������ô�������ݰ����㶪ʧ
	*/
private:
	const int m_tti_num;

	/*
	* ռ�õ�pattern���
	*/
private:
	int m_pattern_idx;
	void set_pattern_idx(int t_pattern_idx) {
		m_pattern_idx = t_pattern_idx;
	}
public:
	int get_pattern_idx() {
		return m_pattern_idx;
	}

	/*
	* ��Ǳ�����ǰʱ�̴��䵽�׼���TTI
	*/
private:
	int m_tti_idx = 0;
public:
	int get_tti_idx() { return m_tti_idx; }

	/*
	* ��Ǳ����Ƿ������(�����Ƿ�������)
	*/
private:
	bool m_is_finished = false;
public:
	bool is_finished() { return m_is_finished; }

	/*
	* ��Ǳ����Ƿ�������
	*/
private:
	bool m_is_loss = false;
public:
	bool get_is_loss() { return m_is_loss; }

public:
	route_udp_link_event(int t_source_node_id, int t_destination_node_id, int t_pattern_idx, int t_package_num) :
		m_source_node_id(t_source_node_id),
		m_destination_node_id(t_destination_node_id),
		m_pattern_idx(t_pattern_idx),
		m_tti_num(t_package_num) {}

	/*
	* �������ݰ��ķ���
	*/
	void transimit();
};

class route_udp_node {
	friend class route_udp;

private:

	/*
	* ��һ�η���Hello����ʱ��
	*/
	int m_tti_next_hello;

	/*
	* ���ڷ��͵�link_eventָ�룬��Ϊ�������ݰ���һ���ڵ�ͬʱֻ����һ������Ϊ����Hello����Ϊһ���������Լ��������нڵ������
	*/
	std::vector<route_udp_link_event*> sending_link_event;

	/*
	* �ڵ���
	*/
	static int s_node_count;

	/*
	* ���������
	*/
	static std::default_random_engine s_engine;

	/*
	* ���ڷ���(ǿ��һ��:��״̬�Ľڵ�)��node�ڵ�
	* ����±�Ϊpattern���
	*/
	static std::vector<std::set<int>> s_node_id_per_pattern;
public:
	static const std::set<int>& get_node_id_set(int t_pattern_idx);

	/*
	* ��ǰ�ڵ�����ͳ�������
	*/
private:
	std::queue<route_udp_route_event*> m_send_event_queue;
public:
	void offer_send_event_queue(route_udp_route_event* t_event) {
		m_send_event_queue.push(t_event);
	}
	route_udp_route_event* poll_send_event_queue() {
		route_udp_route_event* temp = m_send_event_queue.front();
		m_send_event_queue.pop();
		return temp;
	}
	route_udp_route_event* peek_send_event_queue() {
		return m_send_event_queue.front();
	}
	bool is_send_event_queue_empty() {
		return m_send_event_queue.empty();
	}

private:
	/*
	* �ڵ�id
	*/
	const int m_id = s_node_count++;
public:
	int get_id() {
		return m_id;
	}

	/*
	* �ڽ��б�
	*/
private:

	//�ڽӱ�pair�ĵ�һ��Ϊ����id���ڶ���Ϊ�ϴν��յ��ó�hello����ʱ�䣨TTI��
	std::vector<pair<int, adjacent_message>> m_adjacent_list;

public:
	//�����ڽӱ�
	void add_to_adjacent_list(int t_node_id, adjacent_message config) {
		context* __context = context::get_context();
		pair<int, adjacent_message> temp(t_node_id, config);
		vector<pair<int, adjacent_message>>::iterator it = m_adjacent_list.begin();
		while (it < m_adjacent_list.end() && it->first != t_node_id) {
			++it;
		}
		//������������ڽӱ�������ڽӱ��м���ó���
		if (it == m_adjacent_list.end()) {
			m_adjacent_list.push_back(temp);
		}
		//��������Ѿ����ڽӱ����ˢ�¸ó����ĸ���ʱ��Ϊ��ǰʱ��
		else {
			m_adjacent_list.erase(it);
			m_adjacent_list.push_back(temp);
		}
	}

	//<Warn>
	const std::vector<pair<int, adjacent_message>> get_adjacent_list() {
		return m_adjacent_list;
	}


public:
	/*
	* ���캯��
	*/
	route_udp_node();

public:
	/*
	* ѡ������ת���ĳ����Լ���Ӧ��Ƶ��
	* first�ֶ�Ϊ����id
	* second�ֶ�ΪƵ�α��
	* ����һ���ֶ�Ϊ-1���ʹ���ѡ��ʧ��
	*/
	std::pair<int, int> select_relay_information();
};

class v2x_time;
class gtt;
class wt;
class rrm_config;
class tmc_config;
class route_config;

class route_udp :public route {
	REGISTE_MEMBER_HEAD(route_udp)

private:
	/*
	* ���������
	*/
	static std::default_random_engine s_engine;

	/*
	* ��־�����
	*/
	static std::ofstream s_logger_delay;
	static std::ofstream s_logger_event;
	static std::ofstream s_logger_hop;

	/*
	* �ڵ�����
	*/
	route_udp_node* m_node_array;
public:
	route_udp_node* get_node_array() {
		return m_node_array;
	}

private:

	/*
	* �ɹ�/ʧ��/��Ч������¼�����
	*/
	int m_success_route_event_num = 0;

	int m_failed_route_event_num = 0;

	int m_invalid_route_event_num = 0;

public:

	int get_success_route_event_num(){
		return m_success_route_event_num;
	}
	int get_failed_route_event_num() {
		return m_failed_route_event_num;
	}
	int get_invalid_route_event_num() {
		return m_invalid_route_event_num;
	}
	void add_invalid_event_num() {
		m_invalid_route_event_num++;
	}
private:	
	/*
	* �ɹ�/ʧ�ܴ�����¼�
	*/
	std::vector<route_udp_route_event*> m_successful_route_event_vec;
	std::vector<route_udp_route_event*> m_failed_route_event_vec;
	std::vector<route_udp_link_event*> m_failed_link_event_vec;
	void add_successful_route_event(route_udp_route_event* t_successful_route_event_vec) {
		m_successful_route_event_vec.push_back(t_successful_route_event_vec);
	}
	void add_failed_route_event(route_udp_route_event* t_failed_route_event_vec) {
		m_failed_route_event_vec.push_back(t_failed_route_event_vec);
	}
	void add_failed_link_event(route_udp_link_event* t_failed_link_event_vec) {
		m_failed_link_event_vec.push_back(t_failed_link_event_vec);
	}

public:
	const std::vector<route_udp_route_event*>& get_successful_route_event_vec() {
		return m_successful_route_event_vec;
	}
	const std::vector<route_udp_link_event*>& get_failed_link_event_vec() {
		return m_failed_link_event_vec;
	}
	const std::vector<route_udp_route_event*>& get_failed_route_event_vec() {
		return m_failed_route_event_vec;
	}

private:
	v2x_time* m_time;
	gtt* m_gtt;
	wt* m_wt;
	rrm_config* m_rrm_config;
	tmc_config* m_tmc_config;
	route_config* m_route_config;

	void set_time(object* t_time) {
		m_time = (v2x_time*)t_time;
	}
	void set_gtt(object* t_gtt) {
		m_gtt = (gtt*)t_gtt;
	}
	void set_wt(object* t_wt) {
		m_wt = (wt*)t_wt;
	}
	void set_rrm_config(object* t_rrm_config) {
		m_rrm_config = (rrm_config*)t_rrm_config;
	}
	void set_tmc_config(object* t_tmc_config) {
		m_tmc_config = (tmc_config*)t_tmc_config;
	}
	void set_route_config(object* t_route_config) {
		m_route_config = (route_config*)t_route_config;
	}
public:
	v2x_time* get_time() override {
		return m_time;
	}

	gtt* get_gtt() override {
		return m_gtt;
	}

	wt* get_wt() override {
		return m_wt;
	}

	rrm_config* get_rrm_config() override {
		return m_rrm_config;
	}

	tmc_config* get_tmc_config() override {
		return m_tmc_config;
	}

	route_config* get_route_config() override {
		return m_route_config;
	}

	void initialize() override;

	void process_per_tti() override;

	void update_route_table_from_physics_level() override;

private:
	/*
	* ��������¼�
	*/
	void event_trigger();

	/*
	* ά���ڽӱ���Ҫɾ����ʱ�Ľڵ�
	*/
	void update_adjacent_list();

	/*
	* ����Ҫ��ʼ���͵��¼�
	*/
	void start_sending_data();

	/*
	* ���䵱ǰTTI���ڵ��¼�
	*/
	void transmit_data();
	
	/*
	* ���link_event����ϸ��Ϣ
	*/
	void log_link(int source_id,int destination_id, adjacent_message previous, adjacent_message current);
};
