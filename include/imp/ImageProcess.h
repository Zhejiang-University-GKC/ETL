////////////////////////////////////////////////////////////////////////////////
#pragma once
////////////////////////////////////////////////////////////////////////////////

class ImageProcessHelper
{
public:
	static void Invert(GrayData& gData) throw()
	{
		BYTE* pd = gData.GetAddress();
		int iW = gData.GetWidth();
		int iH = gData.GetHeight();
		for( int i = 0; i < iH; i ++ ) {
			for( int j = 0; j < iW; j ++ ) {
				*pd = 255 - *pd;
				pd ++;
			}
		}
	}
	static void BoolInvert(GrayData& gData) throw()
	{
		BYTE* pd = gData.GetAddress();
		int iW = gData.GetWidth();
		int iH = gData.GetHeight();
		for( int i = 0; i < iH; i ++ ) {
			for( int j = 0; j < iW; j ++ ) {
				if( *pd != 0 )
					*pd = 0;
				else
					*pd = 255;
				pd ++;
			}
		}
	}
};

////////////////////////////////////////////////////////////////////////////////
