#pragma once
#include<string>
#include"micro.h"

/*
* �����˷���Ļ���
*/
class object {
	INVOKE_FRIEND_DECLARED
public:
	void* operator new(size_t size, long class_id) {
		//�Է���һ��long����Ϊ����ͷ
		void* raw = ::operator new(size + sizeof(long));
		char* pc = (char*) raw;
		*((long*)pc) = class_id;
		return pc + sizeof(long);
	}

	void operator delete(void *p) {
		::operator delete ((char*)p - sizeof(long));
	}

public:
	/*
	* ��ȡ���ͱ��
	* ��������Ϊ�麯��,�ǳ�����,����������������̳��Դ����麯����������������ɵĵ��ڴ�ƫ��
	*/
	virtual long get_class_id() const {
		return *((long*)((char*)this - sizeof(long)));
	}

protected:
	static long class_id;
};


