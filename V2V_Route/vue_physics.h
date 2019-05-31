#pragma once

#include<vector>
#include<utility>
#include<random>
#include<list>

class vue;

class vue_physics {
	/*------------------��Ԫ����------------------*/
	/*
	* ��vue��Ϊ��Ԫ������vue_physics�Ĺ��캯����Ϊ˽�У�������vue������
	*/
	friend class vue;
	friend class gtt_highspeed;
	friend class gtt_urban;
	friend class receiver_event;
	friend class vue_network;
	friend class route_udp;

	/*----------------�������Ƴ�Ա----------------*/
private:
	/*
	* Ĭ�Ϲ��캯��
	* ��Ϊ˽�У����ʼ����ȫ����vue����ɣ���ֹ���ɸ����͵�ʵ��
	*/
	vue_physics();

public:
	/*
	* ��������������������Դ
	*/
	~vue_physics();

	/*
	* ���������캯������Ϊɾ��
	*/
	vue_physics(const vue_physics& t_vue_physics) = delete;

	/*
	* ���ƶ����캯������Ϊɾ��
	*/
	vue_physics(vue_physics&& t_vue_physics) = delete;

	/*
	* ��������ֵ���������Ϊɾ��
	*/
	vue_physics& operator=(const vue_physics& t_vue_physics) = delete;

	/*
	* ���ƶ���ֵ���������Ϊɾ��
	*/
	vue_physics& operator=(vue_physics&& t_vue_physics) = delete;

	/*------------------��̬��Ա------------------*/

	/*
	* ��������
	*/
private:
	static int s_vue_count;
public:
	static int get_vue_num();


	/*
	* ����֮��Ĵ�߶�˥��
	* ǰ�����±�Ϊ����id������s_pl_all[i][j],��i<j
	*/
private:
	static std::vector<std::vector<double>> s_pl_all;
	static void set_pl(int i, int j, double t_pl);
public:
	static double get_pl(int i, int j);

	/*
	* �����복��֮��ľ���
	* ǰ�����±�Ϊ����id������s_distance_all[i][j],��i<j
	*/
private:
	static std::vector<std::vector<double>> s_distance_all;
	static void set_distance(int i, int j, double t_distance);
public:
	static double get_distance(int i, int j);

	/*--------------------�ֶ�--------------------*/
	/*
	* ָ���ϲ��ָ��
	*/
private:
	vue* m_superior_level;
	void set_superior_level(vue* t_superior_level);
public:
	vue* get_superior_level();

	/*
	* �������
	*/
private:
	int m_vue_id = s_vue_count++;
public:
	int get_vue_id();

	/*
	* ��������id,������Դ����
	*/
private:
	int m_zone_id;
public:
	int get_zone_id() {
		return m_zone_id;
	}

	/*
	* ���٣�m/s
	*/
private:
	double m_speed = 0;
public:
	double get_speed() {
		return m_speed;
	}
	/*
	* �ٶȷ���,0�����򶫣�180��������,90�����򱱣�-90��������
	*/
private:
	double m_vangle = 0;
public:
	double get_angle() {
		return m_vangle;
	}

	/*
	* ���Ժ����꣬��λm
	*/
private:
	double m_absx = 0;
public:
	double get_absx() {
		return m_absx;
	}
	/*
	* ���������꣬��λm
	*/
private:
	double m_absy = 0;
public:
	double get_absy() {
		return m_absy;
	}
	/*
	*��Ժ����꣬��λm
	*/
private:
	double m_relx = 0;

	/*
	* ��������꣬��λm
	*/
private:
	double m_rely = 0;

	/*
	* ���ڽ���id
	*/
private:
	int m_road_id = -1;

	/*
	* ӵ���ȼ�<Warn>:Ŀǰ��ʱ��Ϊ0
	*/
private:
	int m_congestion_level = 0;
public:
	int get_congestion_level();

	/*
	* ������������ĵ�ƫ����
	*/
public:
	double m_zone_shift = 0;

	/*
	* ����ʱ϶��ţ�����ʱ�ָ��ã����������ڵ���λ�ø��º����
	* �ñ�ż����ɻ�վ��֪�����Ҽ����վ�޷����غϻ�����������
	* TTI%granularity==m_slot_time_idx���ж��Ƿ���Է���
	* ����granularityΪʱ�����ȣ�������ż���־���2
	*/
private:
	int m_slot_time_idx = 0;
	void set_slot_time_idx(int t_slot_time_idx);
public:
	int get_slot_time_idx();
	/*----------------������༭��----------------*/
public:

	/*--------------------����--------------------*/
public:
	void update_location_highspeed();
	void update_location_urban();
};