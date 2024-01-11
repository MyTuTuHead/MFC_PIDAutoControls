#pragma once
//********************************************************************************/
/*
　　类名称： 循环队列模板类
	版  本： 1.1
	作  者： 
	日  期：
	版  权：
	说  明： 在VC++8中编译通过

		如果模板类型不是简单基本类型(int,char等等)而是自定义的类或结构体，在该类型
	中必须重载赋值操作符(=)，如果队列使用等于操作符(==)时，该类型必须实现重载等于操
	作符(==)。
*/
//*******************************************************************************/

/*
--------------------------------修改记录---------------------------------------
版    本：1.1
修改问题：修改了没有设置队列存储区导致的内存泄露问题
修 改 人：
修改日期：
说    明：
---------------------------------------------------------------------------------
修改问题：
修 改 人：
修改日期：
说    明：
---------------------------------------------------------------------------------
*/

#ifndef _CCYCQUEUE_H_HXHD_GHB_INCLUDE_
#define _CCYCQUEUE_H_HXHD_GHB_INCLUDE_

#include <assert.h>

template <class T>
class CCycQueue
{
public:
	//构造函数
	CCycQueue(void);
	CCycQueue(int maxLen);
	//拷贝构造函数
	CCycQueue(const CCycQueue<T>& cq);

	//析构函数
	virtual ~CCycQueue(void);

	//重载赋值操作符(=)
	CCycQueue<T>& operator = (const CCycQueue<T>& cq);

	//重载等于操作符(==)
	bool operator == (const CCycQueue<T>& cq);

	//取值
	T& operator [] (int index) const;

	//设置队列最大长度
	bool SetMaxLen(int len);
	//获得队列最大长度
	inline int GetMaxLen(void) const;

	//队列是否为空
	inline bool IsEmpty(void) const;
	//队列是否为满
	inline bool IsFull(void) const;

	//获得当前队列长度
	inline int GetQueLen(void) const;

	//数据入队列
	bool InQueue(T t);
	//数据出队列
	bool OutQueue(T* pt);

	//多数据入队列
	int InQueue(const T* pt, int num);
	//多数据出队列
	int OutQueue(T* pt, int num);
	//
	////获得队列里的数据，队列数据不改变
	//int GetQueueData(T *buf, int bufLen) const;

	//清空队列
	bool Clear(void);

	//数据覆盖数量
	int m_overlayNum;

protected:
	T* m_TArray;			//存储区向量
	int m_maxLen;			//存储区向量长度

	int m_head;			//队列头
	int m_tail;			//队列尾
};

//  声明结束
//---------------------------------------------------------------------------------------

// 名  称： CCycQueue
// 功  能： 构造函数
// 参  数： 
// 返回值：
template <class T>
CCycQueue<T>::CCycQueue(void)
{
	m_maxLen = 0;
	m_TArray = new T[m_maxLen + 1];
	m_tail = 0;
	m_head = 0;
	m_overlayNum = 0;
}

// 名  称： CCycQueue
// 功  能： 构造函数
// 参  数： maxLen(in)		-存储区大小
// 返回值：
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

// 名  称： CCycQueue
// 功  能： 拷贝构造函数
// 参  数： cq(in)		-本类实例引用
// 返回值：
template <class T>
CCycQueue<T>::CCycQueue(const CCycQueue& cq)
{
	m_tail = cq.m_tail;
	m_head = cq.m_head;
	m_maxLen = cq.m_maxLen;

	//分配空间
	m_TArray = new T[m_maxLen + 1];
	if (cq.IsEmpty())
	{
		//队列为空
		return;
	}
	//拷贝数据
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

// 名  称： ~CCycQueue
// 功  能： 析构函数
// 参  数：
// 返回值：
template <class T>
CCycQueue<T>::~CCycQueue(void)
{
	if (m_TArray)
	{
		delete[] m_TArray;
		m_TArray = NULL;
	}
}

// 名  称： operator=
// 功  能： 赋值操作符(=)重载函数
// 参  数： cq(in)		-本类实例引用，操作符右值
// 返回值： 本实例引用
template <class T>
CCycQueue<T>& CCycQueue<T>::operator=(const CCycQueue& cq)
{
	m_tail = cq.m_tail;
	m_head = cq.m_head;
	m_maxLen = cq.m_maxLen;

	//释放存储区
	if (m_TArray)
	{
		delete[] m_TArray;
		m_TArray = NULL;
	}

	//分配空间
	m_TArray = new T[cq.m_maxLen + 1];
	if (cq.IsEmpty())
	{
		//队列为空
		return *this;
	}
	//拷贝数据
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

// 名  称： operator==
// 功  能： 等于操作符(==)重载函数
// 参  数： cq(in)		-本类实例引用，操作符右值
// 返回值： 相等返回true，否则返回false
template <class T>
bool CCycQueue<T>::operator==(const CCycQueue& cq)
{
	if (IsEmpty() && cq.IsEmpty())
	{
		return true;
	}
	if (GetQueLen() != cq.GetQueLen())
	{
		//队列长度不同
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

// 名  称： SetMaxLen
// 功  能： 设置队列最大长度
// 参  数： len(in)			-存储区大小
// 返回值：	操作成功返回true,否则返回false
template <class T>
bool CCycQueue<T>::SetMaxLen(int len)
{
	assert(len >= 0);
	if (len < 0)
	{
		return false;
	}

	//释放存储区
	if (m_TArray)
	{
		delete[] m_TArray;
		m_TArray = NULL;
	}

	m_tail = 0;
	m_head = 0;
	//分配存储区
	m_maxLen = len;
	m_TArray = new T[m_maxLen + 1];

	return m_TArray ? true : false;
}

// 名  称： GetMaxLen
// 功  能： 获得队列最大长度
// 参  数：
// 返回值：	队列长度
template <class T>
int CCycQueue<T>::GetMaxLen(void) const
{
	return m_maxLen;
}

// 名  称： IsEmpty
// 功  能： 队列是否为空
// 参  数：
// 返回值：	队列空返回true，否则返回false
template <class T>
bool CCycQueue<T>::IsEmpty(void) const
{
	return m_tail == m_head ? true : false;
}

// 名  称： IsFull
// 功  能： 队列是否为满
// 参  数：
// 返回值：	队列满返回true，否则返回false
template <class T>
bool CCycQueue<T>::IsFull(void) const
{
	return (m_tail + 1) % (m_maxLen + 1) == m_head ? true : false;
}

// 名  称： GetQueLen
// 功  能： 获得当前队列长度
// 参  数：
// 返回值：	队列长度
template <class T>
int CCycQueue<T>::GetQueLen(void) const
{
	return (m_tail - m_head + m_maxLen + 1) % (m_maxLen + 1);
}

// 名  称： InQueue
// 功  能： 数据入队列
// 参  数： t(in)    -入队的数据
// 返回值：	成功返回true，队列满返回false
template <class T>
bool CCycQueue<T>::InQueue(T t)
{
	if (m_maxLen < 1)
	{
		return false;
	}
	//队列是否满
	if (IsFull())
	{
		++m_overlayNum;
		m_head = (m_head + 1) % (m_maxLen + 1);
	}
	m_TArray[m_tail] = t;
	m_tail = (m_tail + 1) % (m_maxLen + 1);
	return true;
}

// 名  称： OutQueue
// 功  能： 数据出队列
// 参  数： pt(in,out)	-出队列数据值
// 返回值：	正确返回为true，否则为false
template <class T>
bool CCycQueue<T>::OutQueue(T* pt)
{
	assert(pt);

	if (IsEmpty())
	{
		//队列为空
		return false;
	}

	*pt = m_TArray[m_head];
	m_head = (m_head + 1) % (m_maxLen + 1);

	return true;
}

// 名  称： InQueue
// 功  能： 多数据入队列
// 参  数： pt(in)		-数据存储区地址
//			num(in)		-数据数量
// 返回值：	进队列数据数量
template <class T>
int CCycQueue<T>::InQueue(const T* pt, int num)
{
	assert(pt && num >= 0);		//内存错误

	if (m_maxLen < 1)
	{
		return 0;
	}
	int inNum = 0;
	for (; inNum < num; ++inNum)
	{
		if (IsFull())
		{
			//队列满
			++m_overlayNum;
			m_head = (m_head + 1) % (m_maxLen + 1);
		}
		m_TArray[m_tail] = pt[inNum];
		m_tail = (m_tail + 1) % (m_maxLen + 1);
	}

	return inNum;
}

// 名  称： OutQueue
// 功  能： 多数据出队列
// 参  数： pt(in)		-数据存储区地址
//			num(in)		-存储区大小
// 返回值：	出队列数据数量
template <class T>
int CCycQueue<T>::OutQueue(T* pt, int num)
{
	assert(pt && num >= 0);		//内存错误

	int outNum = 0;
	for (; outNum < num; ++outNum)
	{
		if (IsEmpty())
		{
			//队列空
			break;
		}
		pt[outNum] = m_TArray[m_head];
		m_head = (m_head + 1) % (m_maxLen + 1);
	}

	return outNum;
}

// 名  称： Clear
// 功  能： 清空队列
// 参  数：
// 返回值：	成功返回true，队列满返回false
template <class T>
bool CCycQueue<T>::Clear(void)
{
	m_tail = 0;
	m_head = 0;
	return true;
}

// 名  称： operator []
// 功  能： 随机取值
// 参  数：	index(in)	-数据逻辑下标			
// 返回值：	数据项
template <class T>
T& CCycQueue<T>::operator [] (int index) const
{
	assert(index >= 0 && index < GetQueLen());

	return  m_TArray[(m_head + index) % (m_maxLen + 1)];
}

//// 名  称： GetQueueData
//// 功  能： 获得队列数据
//// 参  数：	buf(in)	- 数据缓存
////          bufLen (in) - 缓存大小
//// 返回值：	填充数据的大小
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
