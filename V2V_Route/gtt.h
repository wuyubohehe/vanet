#pragma once
#include<string>
#include"enumeration.h"
#include"reflect/object.h"

class vue;
class v2x_time;
class rrm_config;

class gtt:public object {
public:
	/*
	* ��ȡʱ�����
	*/
	virtual v2x_time* get_time() = 0;

	/*
	* ��ȡ��������ָ��
	*/
	virtual vue* get_vue_array() = 0;

	/*
	* ��ȡ������Դ�������ö���
	*/
	virtual rrm_config* get_rrm_config() = 0;

	/*
	* ��һЩ��ʼ������
	*/
	virtual void initialize() = 0;

	/*
	* ��ȡ��������
	*/
	virtual int get_vue_num() = 0;

	/*
	* ��ȡ��������
	*/
	virtual int get_freshtime() = 0;

	/*
	* ���ڸ��³���λ��
	*/
	virtual void fresh_location() = 0;

	/*
	* ���ڼ���ָ���ŵ���Ӧ����
	*/
	virtual void calculate_pl(int t_vue_id1, int t_vue_id2) = 0;
};