/*
 * =====================================================================================
 *
 *       Filename:  BalloonArchivement.h
 *
 *    Description:  气球游戏的成就管理类，纯数据管理
 *
 *        Version:  1.0
 *        Created:  07/30/2014 22:16:39
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jason Tou (), sosoayaen@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __BALLOON_ARCHIVEMENT_H__
#define __BALLOON_ARCHIVEMENT_H__
#include <string>
#include <vector>
#include <map>

// 保存对应成就描述的数组
typedef std::vector<std::string > ArchStringArray;
// 对应的里程碑数量，每个表示对应的目标量，顺序排列
typedef std::vector<long long > ArchInt64Array;

// 成就数据的结构体
typedef struct tagArchivementData
{
	long nID;							// 成就的ID，对应某个统计值
	int nSize;							// 成就的级别个数，下面每个数组的长度
	ArchStringArray titleArray;			// 当前成就的名称
	ArchStringArray descArray;			// 成就目标的描述，比如，点破30000个气球
	ArchInt64Array milestoteArray;		// 里程碑
	
} ArchData;

// 里程碑数据列表
typedef std::map<long, ArchData* > ArchDataByIDMap;

class BalloonArchivement
{
public:
	BalloonArchivement();
	virtual ~BalloonArchivement();

	static BalloonArchivement* sharedBalloonArchivement();

	// 加载成就数据
	bool loadData();

	// 根据ID得到成就数据结构
	const ArchData* getArchDataByID(long nID);

	// 获得整个数据对象，可以用于迭代获取所有内容
	// const ArchDataByIDMap* getArchDataMap();

protected:

private:
	// 所有成就的数据，动态生成，结束时要清理数据
	// 是以id值作为索引存放
	ArchDataByIDMap m_archDataMap;
};

#endif // __BALLOON_ARCHIVEMENT_H__
