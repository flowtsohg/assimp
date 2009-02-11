/*
Open Asset Import Library (ASSIMP)
----------------------------------------------------------------------

Copyright (c) 2006-2008, ASSIMP Development Team
All rights reserved.

Redistribution and use of this software in source and binary forms, 
with or without modification, are permitted provided that the 
following conditions are met:

* Redistributions of source code must retain the above
  copyright notice, this list of conditions and the
  following disclaimer.

* Redistributions in binary form must reproduce the above
  copyright notice, this list of conditions and the
  following disclaimer in the documentation and/or other
  materials provided with the distribution.

* Neither the name of the ASSIMP team, nor the names of its
  contributors may be used to endorse or promote products
  derived from this software without specific prior
  written permission of the ASSIMP Development Team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

----------------------------------------------------------------------
*/

#include "AssimpPCH.h"
#include "Hash.h"

using namespace Assimp;

// ------------------------------------------------------------------------------------------------
// Get a specific property from a material
aiReturn aiGetMaterialProperty(const aiMaterial* pMat, 
	const char* pKey,
	unsigned int type,
    unsigned int index,
	const aiMaterialProperty** pPropOut)
{
	ai_assert (pMat != NULL);
	ai_assert (pKey != NULL);
	ai_assert (pPropOut != NULL);

	for (unsigned int i = 0; i < pMat->mNumProperties;++i)
	{
		aiMaterialProperty* prop = pMat->mProperties[i];

		if (prop && !::strcmp( prop->mKey.data, pKey ) &&
			prop->mSemantic == type && prop->mIndex == index)
		{
			*pPropOut = pMat->mProperties[i];
			return AI_SUCCESS;
		}
	}
	*pPropOut = NULL;
	return AI_FAILURE;
}

// ------------------------------------------------------------------------------------------------
// Get an array of floating-point values from the material.
aiReturn aiGetMaterialFloatArray(const aiMaterial* pMat, 
	const char* pKey,
	unsigned int type,
    unsigned int index,
	float* pOut,
	unsigned int* pMax)
{
	ai_assert (pMat != NULL);
	ai_assert (pKey != NULL);
	ai_assert (pOut != NULL);

	for (unsigned int i = 0; i < pMat->mNumProperties;++i)
	{
		aiMaterialProperty* prop = pMat->mProperties[i];

		if (prop && !::strcmp( prop->mKey.data, pKey ) &&
			prop->mSemantic == type && prop->mIndex == index)
		{
			// data is given in floats, simply copy it
			if( aiPTI_Float == pMat->mProperties[i]->mType ||
				aiPTI_Buffer == pMat->mProperties[i]->mType)
			{
				unsigned int iWrite = pMat->mProperties[i]->mDataLength / sizeof(float);

				if (pMax)iWrite = *pMax < iWrite ? *pMax : iWrite;
				::memcpy (pOut, pMat->mProperties[i]->mData, iWrite * sizeof (float));

				if (pMax)*pMax = iWrite;
			}
			// data is given in ints, convert to float
			else if( aiPTI_Integer == pMat->mProperties[i]->mType)
			{
				unsigned int iWrite = pMat->mProperties[i]->mDataLength / sizeof(int);

				if (pMax)iWrite = *pMax < iWrite ? *pMax : iWrite;
				for (unsigned int a = 0; a < iWrite;++a)
				{
					pOut[a] = (float) ((int*)pMat->mProperties[i]->mData)[a];
				}
				if (pMax)*pMax = iWrite;
			}
			// it is a string ... no way to read something out of this
			else
			{
				if (pMax)*pMax = 0;
				return AI_FAILURE;
			}
			return AI_SUCCESS;
		}
	}
	return AI_FAILURE;
}

// ------------------------------------------------------------------------------------------------
// Get an array if integers from the material
aiReturn aiGetMaterialIntegerArray(const aiMaterial* pMat, 
	const char* pKey,
	unsigned int type,
    unsigned int index,
	int* pOut,
	unsigned int* pMax)
{
	ai_assert (pMat != NULL);
	ai_assert (pKey != NULL);
	ai_assert (pOut != NULL);
	for (unsigned int i = 0; i < pMat->mNumProperties;++i)
	{
		aiMaterialProperty* prop = pMat->mProperties[i];

		if (prop && !::strcmp( prop->mKey.data, pKey ) &&
			prop->mSemantic == type && prop->mIndex == index)
		{
			// data is given in ints, simply copy it
			if( aiPTI_Integer == pMat->mProperties[i]->mType ||
				aiPTI_Buffer == pMat->mProperties[i]->mType)
			{
				unsigned int iWrite = pMat->mProperties[i]->mDataLength / sizeof(int);

				if (pMax)iWrite = *pMax < iWrite ? *pMax : iWrite;
				::memcpy (pOut, pMat->mProperties[i]->mData, iWrite * sizeof (int));

				if (pMax)*pMax = iWrite;
			}
			// data is given in floats convert to int (lossy!)
			else if( aiPTI_Float == pMat->mProperties[i]->mType)
			{
				unsigned int iWrite = pMat->mProperties[i]->mDataLength / sizeof(float);

				if (pMax)iWrite = *pMax < iWrite ? *pMax : iWrite;
				for (unsigned int a = 0; a < iWrite;++a)
				{
					pOut[a] = (int) ((float*)pMat->mProperties[i]->mData)[a];
				}
				if (pMax)*pMax = iWrite;
			}
			// it is a string ... no way to read something out of this
			else
			{
				if (pMax)*pMax = 0;
				return AI_FAILURE;
			}
			return AI_SUCCESS;
		}
	}
	return AI_FAILURE;
}

// ------------------------------------------------------------------------------------------------
// Get a color (3 or 4 floats) from the material
aiReturn aiGetMaterialColor(const aiMaterial* pMat, 
	const char* pKey,
	unsigned int type,
    unsigned int index,
	aiColor4D* pOut)
{
	unsigned int iMax = 4;
	aiReturn eRet = aiGetMaterialFloatArray(pMat,pKey,type,index,(float*)pOut,&iMax);

	// if no alpha channel is provided set it to 1.0 by default
	if (3 == iMax)pOut->a = 1.0f;
	return eRet;
}

// ------------------------------------------------------------------------------------------------
// Get a string from the material
aiReturn aiGetMaterialString(const aiMaterial* pMat, 
	const char* pKey,
	unsigned int type,
    unsigned int index,
	aiString* pOut)
{
	ai_assert (pMat != NULL);
	ai_assert (pKey != NULL);
	ai_assert (pOut != NULL);

	for (unsigned int i = 0; i < pMat->mNumProperties;++i)
	{
		aiMaterialProperty* prop = pMat->mProperties[i];

		if (prop && !::strcmp( prop->mKey.data, pKey ) &&
			prop->mSemantic == type && prop->mIndex == index)
		{
			if( aiPTI_String == pMat->mProperties[i]->mType)
			{
				const aiString* pcSrc = (const aiString*)pMat->mProperties[i]->mData; 
				::memcpy (pOut->data, pcSrc->data, (pOut->length = pcSrc->length)+1);
			}
			// Wrong type
			else return AI_FAILURE;
			return AI_SUCCESS;
		}
	}
	return AI_FAILURE;
}

// ------------------------------------------------------------------------------------------------
// Construction. Actually the one and only way to get an aiMaterial instance
MaterialHelper::MaterialHelper()
{
	// Allocate 5 entries by default
	mNumProperties = 0;
	mNumAllocated = 5;
	mProperties = new aiMaterialProperty*[5];
}

// ------------------------------------------------------------------------------------------------
MaterialHelper::~MaterialHelper()
{
	_InternDestruct();
}

// ------------------------------------------------------------------------------------------------
aiMaterial::~aiMaterial()
{
	// This is safe: aiMaterial has a private constructor,
	// so instances must be created indirectly via MaterialHelper.
	((MaterialHelper*)this)->_InternDestruct();
}

// ------------------------------------------------------------------------------------------------
// Manual destructor
void MaterialHelper::_InternDestruct()
{
	// First clean up all properties
	Clear();

	// Then delete the array that stored them
	delete[] mProperties;
	AI_DEBUG_INVALIDATE_PTR(mProperties);

	// Update members
	mNumAllocated = 0;
}

// ------------------------------------------------------------------------------------------------
void MaterialHelper::Clear()
{
	for (unsigned int i = 0; i < mNumProperties;++i)
	{
		// delete this entry
		delete mProperties[i];
		AI_DEBUG_INVALIDATE_PTR(mProperties[i]);
	}
	mNumProperties = 0;

	// The array remains allocated, we just invalidated its contents
}

// ------------------------------------------------------------------------------------------------
uint32_t MaterialHelper::ComputeHash(bool includeMatName /*= false*/)
{
	uint32_t hash = 1503; // magic start value, choosen to be my birthday :-)
	for (unsigned int i = 0; i < this->mNumProperties;++i)
	{
		aiMaterialProperty* prop;

		// If specified, exclude the material name from the hash
		if ((prop = mProperties[i]) && (includeMatName || ::strcmp(prop->mKey.data,"$mat.name")))
		{
			hash = SuperFastHash(prop->mKey.data,(unsigned int)prop->mKey.length,hash);
			hash = SuperFastHash(prop->mData,prop->mDataLength,hash);

			// Combine the semantic and the index with the hash
			// We print them to a string to make sure the quality
			// of the hashing state isn't affected (our hashing
			// procedure was originally intended for plaintest).
			char buff[32];
			unsigned int len;
			
			len = ASSIMP_itoa10(buff,prop->mSemantic);
			hash = SuperFastHash(buff,len-1,hash);

			len = ASSIMP_itoa10(buff,prop->mIndex);
			hash = SuperFastHash(buff,len-1,hash);
		}
	}
	return hash;
}

// ------------------------------------------------------------------------------------------------
aiReturn MaterialHelper::RemoveProperty (const char* pKey,unsigned int type,
    unsigned int index)
{
	ai_assert(NULL != pKey);

	for (unsigned int i = 0; i < mNumProperties;++i)
	{
		aiMaterialProperty* prop = mProperties[i];

		if (prop && !::strcmp( prop->mKey.data, pKey ) &&
			prop->mSemantic == type && prop->mIndex == index)
		{
			// Delete this entry
			delete mProperties[i];

			// collapse the array behind --.
			--mNumProperties;
			for (unsigned int a = i; a < mNumProperties;++a)
			{
				mProperties[a] = mProperties[a+1];
			}
			return AI_SUCCESS;
		}
	}

	return AI_FAILURE;
}

// ------------------------------------------------------------------------------------------------
aiReturn MaterialHelper::AddBinaryProperty (const void* pInput,
	unsigned int pSizeInBytes,
	const char* pKey,
	unsigned int type,
    unsigned int index,
	aiPropertyTypeInfo pType)
{
	ai_assert (pInput != NULL);
	ai_assert (pKey != NULL);
	ai_assert (0 != pSizeInBytes);

	// first search the list whether there is already an entry
	// with this name.
	unsigned int iOutIndex = 0xFFFFFFFF;
	for (unsigned int i = 0; i < mNumProperties;++i)
	{
		aiMaterialProperty* prop = mProperties[i];

		if (prop && !::strcmp( prop->mKey.data, pKey ) &&
			prop->mSemantic == type && prop->mIndex == index)
		{
			// delete this entry
			delete this->mProperties[i];
			iOutIndex = i;
		}
	}

	// Allocate a new material property
	aiMaterialProperty* pcNew = new aiMaterialProperty();

	// Fill this
	pcNew->mType = pType;
	pcNew->mSemantic = type;
	pcNew->mIndex = index;

	pcNew->mDataLength = pSizeInBytes;
	pcNew->mData = new char[pSizeInBytes];
	::memcpy (pcNew->mData,pInput,pSizeInBytes);

	pcNew->mKey.length = ::strlen(pKey);
	ai_assert ( MAXLEN > pcNew->mKey.length);
	::strcpy( pcNew->mKey.data, pKey );

	if (0xFFFFFFFF != iOutIndex)
	{
		mProperties[iOutIndex] = pcNew;
		return AI_SUCCESS;
	}

	// resize the array ... double the storage
	if (mNumProperties == mNumAllocated)
	{
		unsigned int iOld = mNumAllocated;
		mNumAllocated *= 2;

		aiMaterialProperty** ppTemp = new aiMaterialProperty*[mNumAllocated];
		if (NULL == ppTemp)return AI_OUTOFMEMORY;

		::memcpy (ppTemp,mProperties,iOld * sizeof(void*));

		delete[] mProperties;
		mProperties = ppTemp;
	}
	// push back ...
	mProperties[mNumProperties++] = pcNew;
	return AI_SUCCESS;
}

// ------------------------------------------------------------------------------------------------
aiReturn MaterialHelper::AddProperty (const aiString* pInput,
	const char* pKey,
	unsigned int type,
    unsigned int index)
{
	// Fix ... don't keep the whole string buffer
	return this->AddBinaryProperty(pInput,(unsigned int)pInput->length+1+
		(unsigned int)(((uint8_t*)&pInput->data - (uint8_t*)&pInput->length)),
		pKey,type,index, aiPTI_String);
}

// ------------------------------------------------------------------------------------------------
void MaterialHelper::CopyPropertyList(MaterialHelper* pcDest, 
	const MaterialHelper* pcSrc)
{
	ai_assert(NULL != pcDest);
	ai_assert(NULL != pcSrc);

	unsigned int iOldNum = pcDest->mNumProperties;
	pcDest->mNumAllocated += pcSrc->mNumAllocated;
	pcDest->mNumProperties += pcSrc->mNumProperties;

	aiMaterialProperty** pcOld = pcDest->mProperties;
	pcDest->mProperties = new aiMaterialProperty*[pcDest->mNumAllocated];

	if (iOldNum && pcOld)
	{
		for (unsigned int i = 0; i < iOldNum;++i)
			pcDest->mProperties[i] = pcOld[i];

		delete[] pcOld;
	}
	for (unsigned int i = iOldNum; i< pcDest->mNumProperties;++i)
	{
		aiMaterialProperty* propSrc = pcSrc->mProperties[i];

		// search whether we have already a property with this name
		// (if yes we overwrite the old one)
		aiMaterialProperty* prop;
		for (unsigned int q = 0; q < iOldNum;++q)
		{
			prop = pcDest->mProperties[q];
			if (prop && prop->mKey == propSrc->mKey &&
			prop->mSemantic == propSrc->mSemantic && prop->mIndex == propSrc->mIndex)
			{
				delete prop;

				// collapse the whole array ...
				::memmove(&pcDest->mProperties[q],&pcDest->mProperties[q+1],i-q);
				i--;
				pcDest->mNumProperties--;
			}
		}

		// Allocate the output property and copy the source property
		prop = pcDest->mProperties[i] = new aiMaterialProperty();
		prop->mKey = propSrc->mKey;
		prop->mDataLength = propSrc->mDataLength;
		prop->mType = propSrc->mType;
		prop->mSemantic = propSrc->mSemantic;
		prop->mIndex = propSrc->mIndex;

		prop->mData = new char[propSrc->mDataLength];
		::memcpy(prop->mData,propSrc->mData,prop->mDataLength);
	}
	return;
}

// ------------------------------------------------------------------------------------------------
aiReturn aiGetMaterialTexture(const C_STRUCT aiMaterial* mat,
    aiTextureType type,
    unsigned int  index,
    C_STRUCT aiString* path,
	aiTextureMapping* _mapping	/*= NULL*/,
    unsigned int* uvindex		/*= NULL*/,
    float* blend				/*= NULL*/,
    aiTextureOp* op				/*= NULL*/,
	aiTextureMapMode* mapmode	/*= NULL*/,
	unsigned int* flags         /*= NULL*/
	)
{
	ai_assert(NULL != mat && NULL != path);

	// Get the path to the texture
	if (AI_SUCCESS != aiGetMaterialString(mat,AI_MATKEY_TEXTURE(type,index),path))	{
		return AI_FAILURE;
	}
	// Determine mapping type 
	aiTextureMapping mapping = aiTextureMapping_UV;
	aiGetMaterialInteger(mat,AI_MATKEY_MAPPING(type,index),(int*)&mapping);
	if (_mapping)
		*_mapping = mapping;

	// Get UV index 
	if (aiTextureMapping_UV == mapping && uvindex)	{
		aiGetMaterialInteger(mat,AI_MATKEY_UVWSRC(type,index),(int*)uvindex);
	}
	// Get blend factor 
	if (blend)	{
		aiGetMaterialFloat(mat,AI_MATKEY_TEXBLEND(type,index),blend);
	}
	// Get texture operation 
	if (op){
		aiGetMaterialInteger(mat,AI_MATKEY_TEXOP(type,index),(int*)op);
	}
	// Get texture mapping modes
	if (mapmode)	{
		aiGetMaterialInteger(mat,AI_MATKEY_MAPPINGMODE_U(type,index),(int*)&mapmode[0]);
		aiGetMaterialInteger(mat,AI_MATKEY_MAPPINGMODE_V(type,index),(int*)&mapmode[1]);		
	}
	// Get texture flags
	if (flags){
		aiGetMaterialInteger(mat,AI_MATKEY_TEXFLAGS(type,index),(int*)flags);
	}

	return AI_SUCCESS;
}

