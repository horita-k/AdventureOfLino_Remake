/* -*- mode: c++; coding: shift_jis-dos; tab-width: 4; -*- */
//---------------------------------------------------
/**
 * Œ`óƒNƒ‰ƒX
 * @author SPATZ.
 * @data   2012/05/13 14:44:56
 */
//---------------------------------------------------
#ifndef __SHAPE_H__
#define __SHAPE_H__

/*=====================================*
 * class 
 *=====================================*/
class ShapeBase
{
 public:
	ShapeBase()			{}
	~ShapeBase()		{}

	virtual void Draw(void)	{}
};

/*=====================================*
 * ShapeLine
 *=====================================*/
class ShapeLine : public ShapeBase
{
public:
	void Draw(void) {
		DrawLine3D(mStart, mEnd, mColor);
	}
	VECTOR		mStart;
	VECTOR		mEnd;
	int			mColor;
};

/*=====================================*
 * ShapeCapsule
 *=====================================*/
class ShapeCapsule : public ShapeBase
{
public:

	ShapeCapsule()		{}
	ShapeCapsule(VECTOR posA, VECTOR posB, float radius)
	{
		this->mPosA = posA;
		this->mPosB = posB;
		this->mRadius = radius;
	}

	void Draw(void) {
		DrawCapsule3D(mPosA, mPosB, mRadius, 1, GetColor(255, 0, 0), GetColor(255, 0, 0), FALSE);
	}
	VECTOR		mPosA;
	VECTOR		mPosB;
	float		mRadius;
};

/*=====================================*
 * ShapeBox
 *=====================================*/
class ShapeBox : public ShapeBase
{
public:
	ShapeBox()			{}
	ShapeBox(VECTOR rPosA, VECTOR rPosB)
	{
		this->mPosA = rPosA;
		this->mPosB = rPosB;
	}

	void Draw(void) {
		DrawCube3D(mPosA, mPosB, GetColor(255, 0, 0), GetColor(255, 0, 0), FALSE);
	}
	VECTOR		mPosA;
	VECTOR		mPosB;
};

/*=====================================*
 * ShapeBoard
 *=====================================*/
class ShapeBoard : public ShapeBase
{
public:
	void Draw(void) {
		VERTEX3D vertexs[6];
		int array_index;
		memset(vertexs, 0x00, sizeof(vertexs));
		array_index = 0;
		vertexs[array_index].pos = mPosA;
		vertexs[array_index].norm = VGet(0.0f, 1.0f, 0.0f);
		vertexs[array_index].dif.a = 255;vertexs[array_index].dif.r = 255;vertexs[array_index].dif.g = 255;vertexs[array_index].dif.b = 255;
		vertexs[array_index].spc.a = 255;vertexs[array_index].spc.r = 255;vertexs[array_index].spc.g = 255;vertexs[array_index].spc.b = 255;
		array_index = 1;
		vertexs[array_index].pos = mPosB;
		vertexs[array_index].norm = VGet(0.0f, 1.0f, 0.0f);
		vertexs[array_index].dif.a = 255;vertexs[array_index].dif.r = 255;vertexs[array_index].dif.g = 255;vertexs[array_index].dif.b = 255;
		vertexs[array_index].spc.a = 255;vertexs[array_index].spc.r = 255;vertexs[array_index].spc.g = 255;vertexs[array_index].spc.b = 255;
		array_index = 2;
		vertexs[array_index].pos = mPosC;
		vertexs[array_index].norm = VGet(0.0f, 1.0f, 0.0f);
		vertexs[array_index].dif.a = 255;vertexs[array_index].dif.r = 255;vertexs[array_index].dif.g = 255;vertexs[array_index].dif.b = 255;
		vertexs[array_index].spc.a = 255;vertexs[array_index].spc.r = 255;vertexs[array_index].spc.g = 255;vertexs[array_index].spc.b = 255;
		memcpy(&vertexs[3], &vertexs[1], sizeof(VERTEX3D));
		memcpy(&vertexs[4], &vertexs[2], sizeof(VERTEX3D));
		array_index = 5;
		vertexs[array_index].pos = mPosD;
		vertexs[array_index].norm = VGet(0.0f, 1.0f, 0.0f);
		vertexs[array_index].dif.a = 255;vertexs[array_index].dif.r = 255;vertexs[array_index].dif.g = 255;vertexs[array_index].dif.b = 255;
		vertexs[array_index].spc.a = 255;vertexs[array_index].spc.r = 255;vertexs[array_index].spc.g = 255;vertexs[array_index].spc.b = 255;
		DrawPolygon3D(vertexs, 2, DX_NONE_GRAPH, FALSE);

	}
	VECTOR		mPosA;
	VECTOR		mPosB;
	VECTOR		mPosC;
	VECTOR		mPosD;
};

#endif // __SHAPE_H__

 /**** end of file ****/

