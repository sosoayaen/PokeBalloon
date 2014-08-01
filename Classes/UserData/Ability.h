/*
 * =====================================================================================
 *
 *       Filename:  Ability.h
 *
 *    Description:	用户技能类头文件
 *
 *					技能管理类负责从配置文件中读取所有的技能描述和配置，生成对应的技能描
 *					述结构体，以便后续程序能从内存中方便获取对应的技能数据，但是仅限用于
 *					描述，并不负责技能对应数据如何影响游戏，这些需要在逻辑文件中自由组合
 *
 *					其中，对应的数据都是生成在堆上的，并且只有一份，使用完后需要释放
 *
 *        Version:  1.0
 *        Created:  07/31/2014 23:52:43
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Tou (), sosoayaen@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __BALLOON_ABILITY_H__
#define __BALLOON_ABILITY_H__

#include <vector>
#include <map>
#include <string>

typedef struct tagAbilityDesc
{
	int nID;					// 技能的独立ID，同样的技能拥有相同的id
	int nLvl;					// 级别，最高255
	int nCost;					// 升级需要的消耗
	std::string strTitle;		// 技能标题
	std::string strDescription; // 技能描述
} AbilityDesc;

// 技能列表，顺序存储，内部存储指针对象，需要手动释放
typedef std::vector<AbilityDesc*> AbilityDescArray;

// 技能哈希表，以能力的ID作为关键字存储，不需要释放，但是内部数组数据需要释放
typedef std::map<int, AbilityDescArray> AbilityDescMap; 

// 技能描述类，在配置文件中读取对应的技能属性，属性都是只读的
class AbilityManager
{
public:
	AbilityManager();
	virtual ~AbilityManager();

	// 返回单例对象
	static AbilityManager* sharedAbilityManager();

	// 加载配置数据
	bool loadData();

	/**
	 * @brief 根据技能ID和及嗯那个的等级获得对应的技能描述结构体
	 * @param nID 技能ID
	 * @param nLvl 技能等级
	 * @return AbilityDesc* 如果找到了就返回对应的对象，否则返回NULL
	 */
	const AbilityDesc* getAbilityDescByLvl(int nID, int nLvl) const;

	/**
	 * @brief 通过技能的ID得到整个技能相关的数据数组
	 * @param nID
	 * @return AbilityDescArray* 如果找到了就返回对象，否则返回NULL
	 */
	const AbilityDescArray* getAbilityDescArray(int nID) const;

protected:

private:
	// 保存了全局唯一的所有技能的描述数据
	AbilityDescMap m_mapAbilityDesc;
};

#endif
