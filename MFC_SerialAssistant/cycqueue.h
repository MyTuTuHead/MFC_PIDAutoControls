#pragma once
//********************************************************************************/
/*
���������ƣ� ѭ������ģ����
	��  ���� 1.1
	��  �ߣ� 
	��  �ڣ�
	��  Ȩ��
	˵  ���� ��VC++8�б���ͨ��

		���ģ�����Ͳ��Ǽ򵥻�������(int,char�ȵ�)�����Զ�������ṹ�壬�ڸ�����
	�б������ظ�ֵ������(=)���������ʹ�õ��ڲ�����(==)ʱ�������ͱ���ʵ�����ص��ڲ�
	����(==)��
*/
//*******************************************************************************/

/*
--------------------------------�޸ļ�¼---------------------------------------
��    ����1.1
�޸����⣺�޸���û�����ö��д洢�����µ��ڴ�й¶����
�� �� �ˣ�
�޸����ڣ�
˵    ����
---------------------------------------------------------------------------------
�޸����⣺
�� �� �ˣ�
�޸����ڣ�
˵    ����
---------------------------------------------------------------------------------
*/

#ifndef _CCYCQUEUE_H_HXHD_GHB_INCLUDE_
#define _CCYCQUEUE_H_HXHD_GHB_INCLUDE_

#include <assert.h>

template <class T>
class CCycQueue
{
public:
	//���캯��
	CCycQueue(void);
	CCycQueue(int maxLen);
	//�������캯��
	CCycQueue(const CCycQueue<T>& cq);

	//��������
	virtual ~CCycQueue(void);

	//���ظ�ֵ������(=)
	CCycQueue<T>& operator = (const CCycQueue<T>& cq);

	//���ص��ڲ�����(==)
	bool operator == (const CCycQueue<T>& cq);

	//ȡֵ
	T& operator [] (int index) const;

	//���ö�����󳤶�
	bool SetMaxLen(int len);
	//��ö�����󳤶�
	inline int GetMaxLen(void) const;

	//�����Ƿ�Ϊ��
	inline bool IsEmpty(void) const;
	//�����Ƿ�Ϊ��
	inline bool IsFull(void) const;

	//��õ�ǰ���г���
	inline int GetQueLen(void) const;

	//���������
	bool InQueue(T t);
	//���ݳ�����
	bool OutQueue(T* pt);

	//�����������
	int InQueue(const T* pt, int num);
	//�����ݳ�����
	int OutQueue(T* pt, int num);
	//
	////��ö���������ݣ��������ݲ��ı�
	//int GetQueueData(T *buf, int bufLen) const;

	//��ն���
	bool Clear(void);

	//���ݸ�������
	int m_overlayNum;

protected:
	T* m_TArray;			//�洢������
	int m_maxLen;			//�洢����������

	int m_head;			//����ͷ
	int m_tail;			//����β
};

//  ��������
//---------------------------------------------------------------------------------------

// ��  �ƣ� CCycQueue
// ��  �ܣ� ���캯��
// ��  ���� 
// ����ֵ��
template <class T>
CCycQueue<T>::CCycQueue(void)
{
	m_maxLen = 0;
	m_TArray = new T[m_maxLen + 1];
	m_tail = 0;
	m_head = 0;
	m_overlayNum = 0;
}

// ��  �ƣ� CCycQueue
// ��  �ܣ� ���캯��
// ��  ���� maxLen(in)		-�洢����С
// ����ֵ��
template <class T>
CCycQueue<T>::CCycQueue(int maxLen)
{
	assert(maxLen >= 0);
	if (maxLen < 0)
	{
		return;
	}
	m_maxLen = maxLen;
	m_TArray = new T[m_maxLen + 1];
	m_tail = 0;
	m_head = 0;
	m_overlayNum = 0;
}

// ��  �ƣ� CCycQueue
// ��  �ܣ� �������캯��
// ��  ���� cq(in)		-����ʵ������
// ����ֵ��
template <class T>
CCycQueue<T>::CCycQueue(const CCycQueue& cq)
{
	m_tail = cq.m_tail;
	m_head = cq.m_head;
	m_maxLen = cq.m_maxLen;

	//����ռ�
	m_TArray = new T[m_maxLen + 1];
	if (cq.IsEmpty())
	{
		//����Ϊ��
		return;
	}
	//��������
	if (cq.m_tail > cq.m_head)
	{
		for (int i = cq.m_head; i < cq.m_tail; ++i)
		{
			m_TArray[i] = cq.m_TArray[i];
		}
	}
	else
	{
		for (int i = cq.m_head; i <= cq.m_maxLen; ++i)
		{
			m_TArray[i] = cq.m_TArray[i];
		}
		for (int i = 0; i < cq.m_tail; ++i)
		{
			m_TArray[i] = cq.m_TArray[i];
		}
	}
}

// ��  �ƣ� ~CCycQueue
// ��  �ܣ� ��������
// ��  ����
// ����ֵ��
template <class T>
CCycQueue<T>::~CCycQueue(void)
{
	if (m_TArray)
	{
		delete[] m_TArray;
		m_TArray = NULL;
	}
}

// ��  �ƣ� operator=
// ��  �ܣ� ��ֵ������(=)���غ���
// ��  ���� cq(in)		-����ʵ�����ã���������ֵ
// ����ֵ�� ��ʵ������
template <class T>
CCycQueue<T>& CCycQueue<T>::operator=(const CCycQueue& cq)
{
	m_tail = cq.m_tail;
	m_head = cq.m_head;
	m_maxLen = cq.m_maxLen;

	//�ͷŴ洢��
	if (m_TArray)
	{
		delete[] m_TArray;
		m_TArray = NULL;
	}

	//����ռ�
	m_TArray = new T[cq.m_maxLen + 1];
	if (cq.IsEmpty())
	{
		//����Ϊ��
		return *this;
	}
	//��������
	if (cq.m_tail > cq.m_head)
	{
		for (int i = cq.m_head; i < cq.m_tail; ++i)
		{
			m_TArray[i] = cq.m_TArray[i];
		}
	}
	else
	{
		for (int i = cq.m_head; i <= cq.m_maxLen; ++i)
		{
			m_TArray[i] = cq.m_TArray[i];
		}
		for (int i = 0; i < cq.m_tail; ++i)
		{
			m_TArray[i] = cq.m_TArray[i];
		}
	}


	return *this;
}

// ��  �ƣ� operator==
// ��  �ܣ� ���ڲ�����(==)���غ���
// ��  ���� cq(in)		-����ʵ�����ã���������ֵ
// ����ֵ�� ��ȷ���true�����򷵻�false
template <class T>
bool CCycQueue<T>::operator==(const CCycQueue& cq)
{
	if (IsEmpty() && cq.IsEmpty())
	{
		return true;
	}
	if (GetQueLen() != cq.GetQueLen())
	{
		//���г��Ȳ�ͬ
		return false;
	}

	int len = GetQueLen();
	for (int i = 0; i < len; ++i)
	{
		if (m_TArray[(m_head + i) % (m_maxLen + 1)] != cq.m_TArray[(cq.m_head + i) % (cq.m_maxLen + 1)])
		{
			return false;
		}
	}

	return true;
}

// ��  �ƣ� SetMaxLen
// ��  �ܣ� ���ö�����󳤶�
// ��  ���� len(in)			-�洢����С
// ����ֵ��	�����ɹ�����true,���򷵻�false
template <class T>
bool CCycQueue<T>::SetMaxLen(int len)
{
	assert(len >= 0);
	if (len < 0)
	{
		return false;
	}

	//�ͷŴ洢��
	if (m_TArray)
	{
		delete[] m_TArray;
		m_TArray = NULL;
	}

	m_tail = 0;
	m_head = 0;
	//����洢��
	m_maxLen = len;
	m_TArray = new T[m_maxLen + 1];

	return m_TArray ? true : false;
}

// ��  �ƣ� GetMaxLen
// ��  �ܣ� ��ö�����󳤶�
// ��  ����
// ����ֵ��	���г���
template <class T>
int CCycQueue<T>::GetMaxLen(void) const
{
	return m_maxLen;
}

// ��  �ƣ� IsEmpty
// ��  �ܣ� �����Ƿ�Ϊ��
// ��  ����
// ����ֵ��	���пշ���true�����򷵻�false
template <class T>
bool CCycQueue<T>::IsEmpty(void) const
{
	return m_tail == m_head ? true : false;
}

// ��  �ƣ� IsFull
// ��  �ܣ� �����Ƿ�Ϊ��
// ��  ����
// ����ֵ��	����������true�����򷵻�false
template <class T>
bool CCycQueue<T>::IsFull(void) const
{
	return (m_tail + 1) % (m_maxLen + 1) == m_head ? true : false;
}

// ��  �ƣ� GetQueLen
// ��  �ܣ� ��õ�ǰ���г���
// ��  ����
// ����ֵ��	���г���
template <class T>
int CCycQueue<T>::GetQueLen(void) const
{
	return (m_tail - m_head + m_maxLen + 1) % (m_maxLen + 1);
}

// ��  �ƣ� InQueue
// ��  �ܣ� ���������
// ��  ���� t(in)    -��ӵ�����
// ����ֵ��	�ɹ�����true������������false
template <class T>
bool CCycQueue<T>::InQueue(T t)
{
	if (m_maxLen < 1)
	{
		return false;
	}
	//�����Ƿ���
	if (IsFull())
	{
		++m_overlayNum;
		m_head = (m_head + 1) % (m_maxLen + 1);
	}
	m_TArray[m_tail] = t;
	m_tail = (m_tail + 1) % (m_maxLen + 1);
	return true;
}

// ��  �ƣ� OutQueue
// ��  �ܣ� ���ݳ�����
// ��  ���� pt(in,out)	-����������ֵ
// ����ֵ��	��ȷ����Ϊtrue������Ϊfalse
template <class T>
bool CCycQueue<T>::OutQueue(T* pt)
{
	assert(pt);

	if (IsEmpty())
	{
		//����Ϊ��
		return false;
	}

	*pt = m_TArray[m_head];
	m_head = (m_head + 1) % (m_maxLen + 1);

	return true;
}

// ��  �ƣ� InQueue
// ��  �ܣ� �����������
// ��  ���� pt(in)		-���ݴ洢����ַ
//			num(in)		-��������
// ����ֵ��	��������������
template <class T>
int CCycQueue<T>::InQueue(const T* pt, int num)
{
	assert(pt && num >= 0);		//�ڴ����

	if (m_maxLen < 1)
	{
		return 0;
	}
	int inNum = 0;
	for (; inNum < num; ++inNum)
	{
		if (IsFull())
		{
			//������
			++m_overlayNum;
			m_head = (m_head + 1) % (m_maxLen + 1);
		}
		m_TArray[m_tail] = pt[inNum];
		m_tail = (m_tail + 1) % (m_maxLen + 1);
	}

	return inNum;
}

// ��  �ƣ� OutQueue
// ��  �ܣ� �����ݳ�����
// ��  ���� pt(in)		-���ݴ洢����ַ
//			num(in)		-�洢����С
// ����ֵ��	��������������
template <class T>
int CCycQueue<T>::OutQueue(T* pt, int num)
{
	assert(pt && num >= 0);		//�ڴ����

	int outNum = 0;
	for (; outNum < num; ++outNum)
	{
		if (IsEmpty())
		{
			//���п�
			break;
		}
		pt[outNum] = m_TArray[m_head];
		m_head = (m_head + 1) % (m_maxLen + 1);
	}

	return outNum;
}

// ��  �ƣ� Clear
// ��  �ܣ� ��ն���
// ��  ����
// ����ֵ��	�ɹ�����true������������false
template <class T>
bool CCycQueue<T>::Clear(void)
{
	m_tail = 0;
	m_head = 0;
	return true;
}

// ��  �ƣ� operator []
// ��  �ܣ� ���ȡֵ
// ��  ����	index(in)	-�����߼��±�			
// ����ֵ��	������
template <class T>
T& CCycQueue<T>::operator [] (int index) const
{
	assert(index >= 0 && index < GetQueLen());

	return  m_TArray[(m_head + index) % (m_maxLen + 1)];
}

//// ��  �ƣ� GetQueueData
//// ��  �ܣ� ��ö�������
//// ��  ����	buf(in)	- ���ݻ���
////          bufLen (in) - �����С
//// ����ֵ��	������ݵĴ�С
//template <class T>
//int CCycQueue<T>::GetQueueData(T *buf, int bufLen) const
//{
//	assert(buf && bufLen > 0); 
//
//	int len = GetQueLen() > bufLen ? bufLen : GetQueLen();
//
//	for (int i = 0; i < len; ++i)
//	{
//		buf[i] = m_TArray[(m_head + i) % (m_maxLen + 1)];
//	}
//		
//	return len;
//}


#endif	//_CCYCQUEUE_H_HXHD_GHB_INCLUDE
