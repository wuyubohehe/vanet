#pragma once
#include<iostream>

/*----------------------------------------------------------------------------
* >>>>>>>>>>>>>>>>>>>>>>>>ɾ���������Ƴ�Ա��(START)<<<<<<<<<<<<<<<<<<<<<<<<<<<
* ---------------------------------------------------------------------------*/
/*
* ���������캯�����ƶ����캯����������ֵ��������ƶ���ֵ���������Ϊɾ��
*/
#define DELETE_COPY_MEMBER(class_type) \
public:\
class_type(){}\
class_type(const class_type&) = delete;\
class_type(class_type&&) = delete;\
class_type& operator=(const class_type&) = delete;\
class_type& operator=(class_type&&) = delete;\


/*----------------------------------------------------------------------------
* >>>>>>>>>>>>>>>>>>>>>>>>>ɾ���������Ƴ�Ա��(END)<<<<<<<<<<<<<<<<<<<<<<<<<<<<
* ---------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
* >>>>>>>>>>>>>>>>>>>>>>>>>>>>bean��ע���(START)<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
* ---------------------------------------------------------------------------*/

/*
* ��Ԫ����
*/
#define INVOKE_FRIEND_DECLARED \
friend void invoke(const object* obj, const std::string& method_name);\
\
friend void invoke(const object* obj, const std::string& method_name, int param1); \
\
friend void invoke(const object* obj, const std::string& method_name, long param1); \
\
friend void invoke(const object* obj, const std::string& method_name, float param1); \
\
friend void invoke(const object* obj, const std::string& method_name, double param1); \
\
friend void invoke(const object* obj, const std::string& method_name, void* param1); \
\
friend void invoke(const object* obj, const std::string& method_name, const std::string& param1); \
\
friend void invoke(const object* obj, const std::string& method_name, object* param1); \


/*
* ע������id�ĺ�
*/
#define REGISTE_CLASS_ID_HEAD(class_type)\
public:\
	static const long class_id;\

/*
* ���幤������
*/
#define REGISTE_FACTORY_METHOD(class_type)\
public:\
	static object* new_instance(){\
		return new class_type();\
	}\
private:

/*
* ����operator new�������ĺ�
*/
#define OVERRIDE_OPERATOR_NEW(class_type)\
public:\
	void* operator new(size_t size) {\
		return object::operator new(size, class_type::class_id);\
	}\
private:

/*
* ����operator delete�������ĺ�
*/
#define OVERRIDE_OPERATOR_DELETE(class_type)\
public:\
	void operator delete(void*p){\
		object::operator delete(p);\
	}\
private:

/*
* �ú���������漸���꣬���ڱ�����������ע��
*/
#define REGISTE_MEMBER_HEAD(class_type)\
DELETE_COPY_MEMBER(class_type) \
INVOKE_FRIEND_DECLARED \
REGISTE_CLASS_ID_HEAD(class_type) \
REGISTE_FACTORY_METHOD(class_type) \
OVERRIDE_OPERATOR_NEW(class_type) \
OVERRIDE_OPERATOR_DELETE(class_type) \

/*
* ��Դ�ļ��г�ʼ������id
*/
#define REGISTE_CLASS_ID_RESOURCE(class_type)\
const long class_type::class_id = ++object::class_id;

/*----------------------------------------------------------------------------
* >>>>>>>>>>>>>>>>>>>>>>>>>>>>bean��ע���(END)<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
* ---------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------
* >>>>>>>>>>>>>>>>>>>>>>>>>>���䷽��ע���(START)<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
* ---------------------------------------------------------------------------*/
//ע�⣬���¼������м����̶��ַ�:class_name,obj,method_name,param1
#define FACTORY_INVOKE_START \
if(class_name==""){\
	return nullptr;\
}\

#define FACTORY_INVOKE(class_type) \
else if(#class_type==class_name){\
	return class_type::new_instance();\
}\

#define FACTORY_INVOKE_END \
else {\
	return nullptr;\
}\


/*
* �����жϿ�ʼ
*/
#define METHOD_INVOKE_CLASS_START(class_type)\
if(class_id==class_type::class_id){\
	class_type * __##class_type = (class_type*)obj;\
	if (method_name == "") {\
		std::cout<<"���䷽��������Ϊ��"<<std::endl;\
		system("pause");\
		exit(0);\
	}

/*
* �����жϽ���
*/
#define METHOD_INVOKE_CLASS_END(class_type)\
	else {\
		std::cout<<"����<"<<#class_type<<"."<<method_name<<">��δע��"<<std::endl;\
        system("pause");\
		exit(0);\
	}\
}

/*
* ��Ԫ�����жϼ�����
*/
#define METHOD_INVOKE_ZERO_PARAM(class_type, method)\
	else if(method_name == #method){\
		__##class_type -> method();\
	}

/*
* һԪ�����жϼ�����
*/
#define METHOD_INVOKE_ONE_PARAM(class_type, method)\
	else if(method_name == #method){\
		__##class_type -> method(param1);\
	}


/*----------------------------------------------------------------------------
* >>>>>>>>>>>>>>>>>>>>>>>>>>>���䷽��ע���(END)<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
* ---------------------------------------------------------------------------*/


