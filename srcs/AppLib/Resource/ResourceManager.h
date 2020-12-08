/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * リソース管理クラス
 * @author SPATZ.
 * @data   2011/11/27 15:30:43
 */
//---------------------------------------------------
#ifndef __RESOURCE_MANAGER_H__
#define __RESOURCE_MANAGER_H__

/*=====================================*
 * class
 *=====================================*/
class ResourceManager
{
 public:
	static ResourceManager* const GetInstance(void);

	void LoadCharaResource(const char* charaName);
};

#endif // __RESOURCE_MANAGER_H__

 /**** end of file ****/

