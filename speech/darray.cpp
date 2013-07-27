#include <stdlib.h>
#include "darray.h"

darray::darray()
{
	mAllocChunk = 128;
	mAllocated = mUsed = 0;
	mData = NULL;
}

darray::~darray()
{
	free(mData);
	mData = NULL;
}

char * darray::getDataInPos(int aPosition)
{
	if (aPosition < mAllocated && aPosition < mUsed)
		return mData + aPosition;

	if (aPosition >= mAllocated)
	{
		int newsize = mAllocated;

		while (newsize <= aPosition)
		{
			newsize += mAllocChunk;
			mAllocChunk *= 2;
		}

		char *newdata = (char*)realloc(mData, newsize);
		if (!newdata)
		{
			free(mData);
			mData = NULL;
			mAllocated = mUsed = 0;
			return NULL;
		}

		mData = newdata;
		mAllocated = newsize;			
	}

	if (aPosition >= mUsed)
	{
		mUsed = aPosition + 1;
	}

	return mData + aPosition;
}

void darray::put(int aData)
{
	char *s = getDataInPos(mUsed);

	*s = aData;
}