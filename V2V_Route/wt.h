#pragma once

#include<set>
#include<random>
#include<utility>
#include<memory>
#include"reflect/object.h"

class sender_event;

class wt :public object{
	REGISTE_MEMBER_HEAD(wt)

	/*---------------��̬��Ա---------------*/
private:
	/*
	* ��������棬���๲��
	*/
	static std::default_random_engine s_engine;

	/*
	* ������Դ
	*/
	static std::vector<double>* m_qpsk_mi;

	/*
	* ������Դ��Դ��ʼ����������������
	*/
	static void set_resource();
	/*----------------�ֶ�------------------*/
private:
	/*
	* ����������
	*/
	int m_nt = 1;

	/*
	* ����������
	*/
	int m_nr = 2;

	/*
	* ���书��
	*/
	double m_pt;

	/*
	* ����·�����
	*/
	double m_ploss;

	/*
	* ���ž���·�����
	*/
	std::vector<double> m_inter_ploss;

	/*
	* ��˹�����Ĺ��ʣ���λ��mw
	*/
	double m_sigma;

	/*------------------�ӿ�------------------*/
public:
	/*
	* �����ظɱ�
	* t_send_vue_id:��ǰ��·�ķ��Ͷ˳���id
	* t_receive_vue_id:��ǰ��·�Ľ��ն˳���id
	* t_pattern_idx:��ǰռ�õ�pattern_idx
	* t_sending_vue_id_set:�ڸ����ز�������з��͵ĳ���id�б�����t_send_vue_id
	*/
	double calculate_sinr(int t_send_vue_id, int t_receive_vue_id, int t_pattern_idx, const std::set<int>& t_sending_vue_id_set);

private:
	/*
	* ���ַ������㷨
	*/
	int closest(std::vector<double> t_vec, double t_target);

	/*
	* ���
	*/
	double get_mutual_information(std::vector<double> t_vec, int t_index);
};