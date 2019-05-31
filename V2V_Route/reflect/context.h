#pragma once
#include<unordered_map>
#include<string>
#include<assert.h>
#include"object.h"
#include"bean_loader.h"
#include"bean_definition.h"

/*
* ������
*/
class context {
private:
	/*
	* �����ļ�·��
	*/
	const std::string configuration_path;

	/*
	* beanӳ���ֵ�
	*/
	std::unordered_map<std::string, object*> bean_map;

	/*
	* �����ļ�������
	*/
	bean_loader* loader = nullptr;

	std::vector<bean_definition*> bean_definitions;

	/*
	* ����ģʽ,˽�л����캯��
	*/
private:
	static context* singleton;
private:
	context(const std::string& path);
public:
	~context();
	static void create_context(const std::string& path);
	static context* get_context();

private:
	/*
	* ������ʼ��
	*/
	void init();

	/*
	* ���������ļ�
	*/
	void parse_configuration();

	/*
	* ����ָ��˳���ʼ������bean
	*/
	void init_bean();

	/*
	* ����ע�룬����һ�������Լ�bean����
	*/
	void property_injection();

	/*
	* Ԥ������
	*/
	void pre_process();

	/*
	* ������
	*/
	void post_process();
public:
	object* get_bean(std::string bean_id);
};