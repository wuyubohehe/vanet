#pragma once

/*
* ǰ������
*/
class vue_physics;

class vue {
	/*------------------˽���ֶ�------------------*/
private:
	/*
	* ָ������������ʵ��
	*/
	vue_physics* m_physics_level = nullptr;


	/*----------------�������Ƴ�Ա----------------*/
public: 
	/*
	* Ĭ�Ϲ��캯������������ʵ��ָ��ĳ�ʼ��
	*/
	vue();

	/*
	* ��������������������Դ
	*/
	~vue();

	/*
	* ���������캯������Ϊɾ��
	*/
	vue(const vue& t_vue) = delete;

	/*
	* ���ƶ����캯������Ϊɾ��
	*/
	vue(vue&& t_vue) = delete;

	/*
	* ��������ֵ���������Ϊɾ��
	*/
	vue& operator=(const vue& t_vue) = delete;

	/*
	* ���ƶ���ֵ���������Ϊɾ��
	*/
	vue& operator=(vue&& t_vue) = delete;

	/*----------------������༭��----------------*/
public:
	/*
	* ��������㲿�ַ�����
	*/
	vue_physics* get_physics_level();
};