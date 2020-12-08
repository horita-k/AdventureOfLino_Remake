/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * リソース画像クラス
 * @author SPATZ.
 * @data   2012/02/18 17:17:18
 */
//---------------------------------------------------
#ifndef __RESOURCE_LAYOUT_H__
#define __RESOURCE_LAYOUT_H__

#include <vector>

/*=====================================*
 * class
 *=====================================*/
class ResourceLayout
{
 public:
	static ResourceLayout* const GetInstance(void);

	void		LoadLayoutResource(const char* layoutName);
	bool		CheckLoadComponent(const char* cmpName);
	int			GetHandle(const char* charaName);

	void		UnloadResource(char* resourceName);

private:
	ResourceLayout();
	~ResourceLayout();

	struct tComponentWork {
		char*	mName;
		int		mHandle;
	};

	std::vector<tComponentWork> mComponentList;

};

#endif // __RESOURCE_LAYOUT_H__

 /**** end of file ****/

