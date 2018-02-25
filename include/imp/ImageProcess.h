////////////////////////////////////////////////////////////////////////////////
#pragma once
////////////////////////////////////////////////////////////////////////////////

class ImageProcessHelper
{
public:
	//�ɻҶ�ͼ�õ�ֱ��ͼ
	static void CalcHistogram(const GrayData& gData, std::vector<double>& histogram)
	{
		histogram.resize(256);

		int height = gData.GetHeight();
		int width = gData.GetWidth();
		int num = height*width;//��������

		//ֱ��ͼ��С��ӦΪ256
		int bins = (int)histogram.size();
		for( int i = 0; i < bins; i++ ) {
			histogram[i] = 0.0;
		}

		const BYTE* ps = gData.GetAddress();

		for( int i = 0; i < height; i++ ) {
			for( int j = 0; j < width; j++ ) {
				int v = (int)(*ps++);
				histogram[v] ++; //����ֱ��ͼ
			}
		}

		for( int i = 0; i < bins; i++ ) {
			histogram[i] /= num;//Ƶ����ΪƵ��
		}
	}
	//��ֱ��ͼ�õ���ֵ,ostu
	static int CalcThresholdByOstu(const std::vector<double>& histogram) throw()
	{
		assert( histogram.size() == 256 );

		//ֱ��ͼ��С��ӦΪ256
		int iBins = (int)histogram.size();

		double variance[256];//��䷽��
		double pa = 0.0;//�������ָ���
		double pb = 0.0;//Ŀ����ָ���
		double wa = 0.0;//����ƽ���Ҷ�ֵ
		double wb = 0.0;//Ŀ��ƽ���Ҷ�ֵ
		double w0 = 0.0;//ȫ��ƽ���Ҷ�ֵ
		double dData1 = 0.0, dData2 = 0.0;

		::memset(variance, 0, sizeof(variance));

		//����ȫ��ƽ���Ҷ�ֵ
		for( int i = 0; i < iBins; i ++ ) {
			w0 += (i * histogram[i]);
		}
		//��ÿ���Ҷ�ֵ������䷽��
		for( int i = 0; i < iBins; i ++ ) {
			pa += histogram[i];
			pb = 1.0 - pa;
			dData1 += (i * histogram[i]);
			dData2 = w0 - dData1;
			wa = dData1 / pa;
			wb = dData2 / pb;
			variance[i] = pa * (wa - w0) * (wa - w0) + pb * (wb - w0) * (wb - w0);
		}
		//�������õ������󷽲��Ӧ�ĻҶ�ֵ
		double temp = 0.0;
		int threshold = 0;
		for( int i = 0; i < iBins; i ++ ) {
			if( variance[i] > temp ) {
				temp = variance[i];
				threshold = i;
			}
		}

		return threshold;
	}

private:
	//��ͨ�����㷨,��ǳ���ͨ��
	//�������ӵ�ı��
	static void label_one_growing(int iLabel, int x, int y, const GrayData& gData, std::vector<int>& matrix)
	{
		const BYTE* ps = gData.GetAddress();
		int height = gData.GetHeight();
		int width = gData.GetWidth();

		std::stack<std::pair<int, int>> coordinate_stack;
		coordinate_stack.push(std::make_pair(y, x));
		matrix[y * width + x] = iLabel;

		int t_ps;
		//��������,��ɫ��û�б�ǹ�
		while( !coordinate_stack.empty() ) {
			std::pair<int, int> t_coordinate = coordinate_stack.top();
			coordinate_stack.pop();
			int t_y = t_coordinate.first;  //height, y
			int t_x = t_coordinate.second; //width, x
			int n_y;
			int n_x;

			n_y = t_y;
			//���
			n_x = t_x - 1;
			if( n_x >= 0 ) {
				t_ps = (int)(*(ps + n_y * width + n_x));
				if( t_ps != 0 && matrix[n_y * width + n_x] == 0 ) {
					matrix[n_y * width + n_x] = iLabel;
					coordinate_stack.push(std::make_pair(n_y, n_x));
				}
			}
			//�ұ�
			n_x = t_x + 1;
			if( n_x < width ) {
				t_ps = (int)(*(ps + n_y * width + n_x));
				if( t_ps != 0 && matrix[n_y * width + n_x] == 0 ) {
					matrix[n_y * width + n_x] = iLabel;
					coordinate_stack.push(std::make_pair(n_y, n_x));
				}
			}
			n_x = t_x;
			//�ϱ�
			n_y = t_y - 1;
			if( n_y >= 0 ) {
				t_ps = (int)(*(ps + n_y * width + n_x));
				if( t_ps != 0 && matrix[n_y * width + n_x] == 0 ) {
					matrix[n_y * widdth + n_x] = iLabel;
					coordinate_stack.push(std::make_pair(n_y, n_x));
				}
			}
			//�±�
			n_y = t_y + 1;
			if( n_y < height ) {
				t_ps = (int)(*(ps + n_y * width + n_x));
				if( t_ps != 0 && matrix[n_y * width + n_x] == 0 ) {
					matrix[n_y * width + n_x] = iLabel;
					coordinate_stack.push(std::make_pair(n_y, n_x));
				}
			}
		}
	}

public:
	//���ж����
	static int Label(const GrayData& gData, std::vector<int>& matrix)
	{
		const BYTE* ps = gData.GetAddress();
		int height = gData.GetHeight();
		int width = gData.GetWidth();

		matrix.resize(width * height);  //overflow?

		for( int i = 0; i < height; i ++ ) {
			for( int j = 0; j < width; j ++ ) {
				matrix[i * width + j] = 0;
			}
		}

		int t_ps;
		int label = 0;
		//ִ����ɺ���matrix[i * width + j]��ֵΪ0���������������Ǻ�ɫ��������ͬ��ŵ�Ϊͬһ����
		for( int i = 0; i < height; i ++ ) {
			for( int j = 0; j < width; j ++ ) {
				t_ps = (int)(*(ps + i * width + j));
				if( t_ps != 0 && matrix[i * width + j] == 0 ) {
					label ++;
					//���һ�����ӵ�
					label_one_growing(label, j, i, gData, matrix);
				}
			}
		}
		return label;
	}

	//extract border
	static void ExtractBorder(GrayData& gData) throw()
	{
		const int c_coord_x[] = { -1,  0,  1, -1, 1, -1, 0, 1 };
		const int c_coord_y[] = { -1, -1, -1,  0, 0,  1, 1, 1 };
		const int c_num = 8;
		BYTE* pd = gData.GetAddress();
		int iW = gData.GetWidth();
		int iH = gData.GetHeight();
		for( int i = 0; i < iH; i ++ ) {
			for( int j = 0; j < iW; j ++ ) {
				if( *pd != 0 ) {
					for( int m = 0; m < c_num; m ++ ) {
						int x = j + c_coord_x[m];
						int y = i + c_coord_y[m];
						if( x < 0 || x >= iW || y < 0 || y >= iH
							|| pd[y * iW + x] == 0 ) {
							*pd = 128;
							break;
						}
					}
				}
				pd ++;
			}
		}
	}
};

////////////////////////////////////////////////////////////////////////////////
