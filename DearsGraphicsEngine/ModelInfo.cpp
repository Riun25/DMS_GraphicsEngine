#pragma once
#include "ModelInfo.h"

NodeAnimation::NodeAnimation() : mNumPosKeys(0), mNumRotKeys(0), mNumScaKeys(0),
mpPosKey(nullptr), mpRotKey(nullptr), mpScaKey(nullptr)/*, mTargetNode(nullptr)*/
{
}

#include<iostream>
NodeAnimation::~NodeAnimation()
{
	static int i = 0;
	if (mpPosKey)
	{
		delete[] mpPosKey;
		mpPosKey = nullptr;
	}
	if (mpRotKey)
	{
		delete[] mpRotKey;
		mpRotKey = nullptr;
	}
	if (mpScaKey)
	{
		delete[] mpScaKey;
		mpScaKey = nullptr;
	}
}

Animation::Animation() : mName{}, mDuration{ 0 }, mTicksPerSecond{ 0 }, mNumChannels{ 0 }, mpChannels{ nullptr }
{
}

Animation::~Animation()
{
	if (mpChannels != nullptr)
	{
		delete[] mpChannels;
	}
}

Node::Node() : mNodeName{}, mTransformation{}, mWorldTransformation{}, mNumNextNode{ 0 }, mNextNode{ nullptr }
{
}

Node::~Node()
{
	if (mNextNode)
	{
		delete[] mNextNode;
	}
}

Bone::Bone() : mBoneName{}, mpTargetNode{ nullptr }, mOffsetMatrix{}, mNumWeight{ 0 }, mpVertexWeight{ nullptr }
{
}

Bone::~Bone()
{
	//delete와 delete[]는 nullptr에 대해 안전하게 동작되므로 굳이 if문을 쓸 필요는 없다.
	delete[] mpVertexWeight;
}

Mesh::Mesh() : mMeshName{}, mNumVertices{ 0 }, mpVertices{ nullptr }, mNumIndices{ 0 }, mpIndices{ nullptr },
/*mTextureName{ nullptr },*/ mNumBones{ 0 }, mpBone{ nullptr }
{
}

Mesh::~Mesh()
{
	if (mpVertices)
	{
		delete[] mpVertices;
	}
	if (mpIndices)
	{
		delete[] mpIndices;
	}
	if (mpBone)
	{
		delete[] mpBone;
	}
}

Model::Model() : mpRootNode{ nullptr }, mNumMesh{ 0 }, mpMeshData{ nullptr }//, mNumAnimation{ 0 }, mAnimation{ nullptr }
{
}

Model::~Model()
{
	if (mpRootNode)
	{
		delete mpRootNode;
	}

	if (mpMeshData)
	{
		delete[] mpMeshData;
	}
}

Vertex::Vertex()
{

}

Vertex::~Vertex()
{

}
