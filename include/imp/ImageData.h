////////////////////////////////////////////////////////////////////////////////
#pragma once
////////////////////////////////////////////////////////////////////////////////
//灰度数组

class GrayData
{
public:
	GrayData() throw() : m_iW(0), m_iH(0)
	{
	}
	~GrayData() throw()
	{
	}

	void Clear() throw()
	{
		m_buffer.clear();
		m_iW = m_iH = 0;
	}

	bool IsNull() const throw()
	{
		return m_buffer.empty();
	}

	BYTE* GetAddress() throw()
	{
		return m_buffer.data();
	}
	const BYTE* GetAddress() const throw()
	{
		return m_buffer.data();
	}
	int GetWidth() const throw()
	{
		return m_iW;
	}
	int GetHeight() const throw()
	{
		return m_iH;
	}

	void Allocate(int iW, int iH)
	{
		assert( iW > 0 && iH > 0 );
		Clear();
		m_buffer.resize(iW * iH);  //overflow?
		m_iW = iW;
		m_iH = iH;
	}

	//copy
	void CopyTo(GrayData& dest)
	{
		if( IsNull() ) {
			dest.Clear();
			return ;
		}
		dest.Allocate(m_iW, m_iH);
		::memcpy(dest.GetAddress(), GetAddress(), m_iW * m_iH * sizeof(BYTE));
	}

private:
	std::vector<BYTE> m_buffer;  //gray array
	int m_iW, m_iH;//宽度、高度
};

class ColorData
{
public:
	ColorData() throw() : m_iW(0), m_iH(0)
	{
	}
	~ColorData() throw()
	{
	}

	void Clear() throw()
	{
		m_spR.clear();
		m_spG.clear();
		m_spB.clear();
		m_iW = m_iH = 0;
	}

	bool IsNull() const throw()
	{
		return m_spR.empty() || m_spG.empty() || m_spB.empty();
	}

	BYTE* GetAddressR() throw()
	{
		return m_spR.data();
	}
	const BYTE* GetAddressR() const throw()
	{
		return m_spR.data();
	}
	BYTE* GetAddressG() throw()
	{
		return m_spG.data();
	}
	const BYTE* GetAddressG() const throw()
	{
		return m_spG.data();
	}
	BYTE* GetAddressB() throw()
	{
		return m_spB.data();
	}
	const BYTE* GetAddressB() const throw()
	{
		return m_spB.data();
	}
	
	int GetWidth() const throw()
	{
		return m_iW;
	}
	int GetHeight() const throw()
	{
		return m_iH;
	}

	void Allocate(int iW, int iH)
	{
		ATLASSERT( iW > 0 && iH > 0 );
		Clear();
		m_spR.resize(iW * iH);
		m_spG.resize(iW * iH);
		m_spB.resize(iW * iH);
		m_iW = iW;
		m_iH = iH;
	}

private:
	std::vector<BYTE> m_spR;
	std::vector<BYTE> m_spG;
	std::vector<BYTE> m_spB;
	int m_iW, m_iH;
};

// ImageDataHelper

class ImageDataHelper
{
public:
	//CImage->ColorData
	static void ImageToColorData(const CImage& image, ColorData& data)
	{
		data.Clear();
		if( image.IsNull() )
			return ;

		int iBPP = image.GetBPP();//得到单位像素的位数
		assert( iBPP == 8 || iBPP == 24 );  //必须是灰度图或彩色图才转换
		int iW = image.GetWidth();
		int iH = image.GetHeight();
		data.Allocate(iW, iH);  //may throw

		RGBQUAD table[256];//结构数组,调色板，彩色图没有调色板
		if( iBPP == 8 )
			image.GetColorTable(0, 256, table);//得到图像的RGB信息

		BYTE* ps = (BYTE*)image.GetBits();//返回图像数据buffer指针
		BYTE* pdR = data.GetAddressR();
		BYTE* pdG = data.GetAddressG();
		BYTE* pdB = data.GetAddressB();

		for( int i = 0; i < iH; i ++ ) {
			BYTE* psr = ps;
			for( int j = 0; j < iW; j ++ ) {
				if( iBPP == 8 ) {//8位灰度图,每一字节是一个像素灰度级
					BYTE v = *psr ++;
					*pdR ++ = table[v].rgbRed;
					*pdG ++ = table[v].rgbGreen;
					*pdB ++ = table[v].rgbBlue;
				}
				else {//24位彩色,每三字节是一个像素RGB(存储顺序是BGR)
					*pdB ++ = *psr ++;
					*pdG ++ = *psr ++;
					*pdR ++ = *psr ++;
				}
			}
			ps += image.GetPitch();
		} //end for
	}
	//ColorData->CImage
	static void ColorDataToImage(const ColorData& data, CImage& image)
	{
		image.Destroy();
		if( data.IsNull() )
			return ;
		int iW = data.GetWidth();
		int iH = data.GetHeight();
		if( !image.Create(iW, iH, 24) )
			return ;

		const BYTE* psR = data.GetAddressR();
		const BYTE* psG = data.GetAddressG();
		const BYTE* psB = data.GetAddressB();
		BYTE* pd = (BYTE*)image.GetBits();
		for( int i = 0; i < iH; i ++ ) {
			BYTE* pdr = pd;
			for( int j = 0; j < iW; j ++ ) {
				*pdr ++ = *psB ++;
				*pdr ++ = *psG ++;
				*pdr ++ = *psR ++;
			}
			pd += image.GetPitch();
		} //end for
	}
	//GrayData->CImage
	static void GrayDataToImage(const GrayData& data, CImage& image)
	{
		image.Destroy();
		if( data.IsNull() )
			return ;
		int iW = data.GetWidth();
		int iH = data.GetHeight();
		if( !image.Create(iW, iH, 8) )
			return ;

		RGBQUAD table[256];
		for( int i = 0; i < 256; i ++ ) {
			table[i].rgbRed = i;
			table[i].rgbGreen = i;
			table[i].rgbBlue = i;
			table[i].rgbReserved = 0;
		}
		image.SetColorTable(0, 256, table);

		const BYTE* ps = data.GetAddress();
		BYTE* pd = (BYTE*)image.GetBits();
		for( int i = 0; i < iH; i ++ ) {
			BYTE* pdr = pd;
			for( int j = 0; j < iW; j ++ ) {
				*pdr ++ = *ps ++;
			}
			pd += image.GetPitch();
		} //end for
	}
	//ColorData->GrayData
	static void ColorDataToGrayData(const ColorData& cData, GrayData& gData)
	{
		gData.Clear();
		if( cData.IsNull() )
			return ;

		int height = cData.GetHeight();
		int width = cData.GetWidth();
		gData.Allocate(width, height);

		const BYTE* psR = cData.GetAddressR();
		const BYTE* psG = cData.GetAddressG();
		const BYTE* psB = cData.GetAddressB();
		BYTE* pd  = gData.GetAddress();

		for( int i = 0; i < height; i ++ ) {
			for( int j = 0; j < width; j ++ ) {
				double dR = (double)(*psR ++);
				double dG = (double)(*psG ++);
				double dB = (double)(*psB ++);
				double v = 0.11 * dR + 0.59 * dG + 0.3 * dB;
				*pd ++ = (BYTE)((v > 255.0) ? 255.0 : ((v < 0.0) ? 0.0 : v));
			}
		}
	}

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
	//然后对于大于阈值的pixel重新处理，将灰度图变为二值图
	static void ToBinary(int iThreshold, GrayData& gData) throw()
	{
		BYTE* pd = gData.GetAddress();
		int height = gData.GetHeight();
		int width = gData.GetWidth();

		for( int i = 0; i < height; i ++ ) {
			for( int j = 0; j < width; j ++ ) {
				int temp = (int)(*pd);

				if( temp >= iThreshold )
					*pd = (BYTE)255;
				else
					*pd = (BYTE)0;
				pd++;
			}
		}
	}
};

////////////////////////////////////////////////////////////////////////////////
