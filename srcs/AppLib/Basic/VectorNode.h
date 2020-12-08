/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * VectorNode.h
 * std::vector ÇÃè„à ÉNÉâÉX
 * @author SPATZ.
 * @data   2012/12/29 15:36:10
 */
//---------------------------------------------------
#ifndef __VECTOR_NODE_H__
#define __VECTOR_NODE_H__

/*=====================================*
 * include
 *=====================================*/
#include <vector>

/*=====================================*
 * class
 *=====================================*/
template <typename T>
class VectorNode
{
	/* ä÷êî */
 public:
	VectorNode(T b) {
		a = b;
	}

	void EntryNode(T* node) 
	{
		typename std::vector<T *>::iterator ite;
		for (ite = mNodeVector.begin(); ite != mNodeVector.end(); ite ++) {
			if (*ite == node) {
				return;
			}
		}
		mNodeVector.push_back(&node);
	}

	T out(void) {
		return a;
	}

private:
	std::vector<T *>		mNodeVector;
	T a;
};

#endif // __VECTOR_NODE_H__

/**** end of file ****/


