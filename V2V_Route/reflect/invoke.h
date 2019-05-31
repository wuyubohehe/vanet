#pragma once
#include"micro.h"
#include"object.h"

/*
* ��Ԫ��������ע��
*/
object* new_instance(const std::string& class_name);

/*
* ��Ԫ��������ע��
*/
void invoke(const object* obj, const std::string& method_name);

/*
* ��������ע��
* �������ͣ�int
*/
void invoke(const object* obj, const std::string& method_name, int param1);

/*
* ��������ע��
* �������ͣ�long
*/
void invoke(const object* obj, const std::string& method_name, long param1);

/*
* ��������ע��
* �������ͣ�float
*/
void invoke(const object* obj, const std::string& method_name, float param1);

/*
* ��������ע��
* �������ͣ�double
*/
void invoke(const object* obj, const std::string& method_name, double param1);

/*
* ��������ע��
* �������ͣ�void*
*/
void invoke(const object* obj, const std::string& method_name, void* param1);

/*
* ��������ע��
* �������ͣ�std::string
*/
void invoke(const object* obj, const std::string& method_name, const std::string& param1);


/*
* ��������ע��
* �������ͣ�object*
*/
void invoke(const object* obj, const std::string& method_name, object* param1);
