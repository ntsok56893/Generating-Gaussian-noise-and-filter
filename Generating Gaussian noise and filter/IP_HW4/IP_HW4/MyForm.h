#pragma once

/**
* Created by 05 on 2018/8/20.
*/

#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cmath>
#include <ctime>
#include <iostream>
#define NVar 1000  //雜訊變異數
#define thr 0.5    //閥值

using namespace std;
using namespace cv;

void imageA(Mat, Mat, int, int, int, int);
float gasdev();

//修正圖，將邊緣映射，解決原圖邊緣無法做處理的問題
void imageA(Mat orImage, Mat result, int heigth, int width, int newHeigth, int newWidth) {
	//先填上四個邊角
	result.at<uchar>(0, 0) = orImage.at<uchar>(0, 0);
	result.at<uchar>(0, newWidth - 1) = orImage.at<uchar>(0, width - 1);
	result.at<uchar>(newHeigth - 1, 0) = orImage.at<uchar>(heigth - 1, 0);
	result.at<uchar>(newHeigth - 1, newWidth - 1) = orImage.at<uchar>(heigth - 1, width - 1);
	//將邊緣映射
	for (int i = 0; i < heigth; i++) {
		for (int j = 0; j < width; j++) {

			if (i == 0) {
				result.at<uchar>(i, j + 1) = orImage.at<uchar>(i, j);
			}
			if (i == heigth - 1) {
				result.at<uchar>(i + 2, j + 1) = orImage.at<uchar>(i, j);
			}
			if (j == 0) {
				result.at<uchar>(i + 1, j) = orImage.at<uchar>(i, j);
			}
			if (j == width - 1) {
				result.at<uchar>(i + 1, j + 2) = orImage.at<uchar>(i, j);
			}
			result.at<uchar>(i + 1, j + 1) = orImage.at<uchar>(i, j);
		}
	}
}
//將uniform亂數轉成高斯亂數
float gasdev() {

	
	static int iset = 0;
	static float gset;
	float fac, rsq, v1, v2;

	if (iset == 0) {

		do {
			v1 = 2.0 * ((double)rand() / RAND_MAX) - 1.0;  //(double)rand() / RAND_MAX 為產生uniform distribution[0,1]之浮點數，使v1介於-1.0 到1.0之間
			v2 = 2.0 * ((double)rand() / RAND_MAX) - 1.0;  //(double)rand() / RAND_MAX 為產生uniform distribution[0,1]之浮點數，使v2介於-1.0 到1.0之間
			rsq = v1 * v1 + v2 * v2;
		} while (rsq >= 1.0 || rsq == 0.0);

		fac = sqrt(-2.0 * log(rsq) / rsq);
		gset = (v1 * fac);  //暫存第一個高斯分布亂數
		iset = 1;
		return (v2 * fac);  //回傳第二個高斯分布亂數
	}
	else {

		iset = 0;
		return gset;  //回傳第一個高斯分布亂數
	}
}

namespace IP_HW4 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MyForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  btnHW4;
	private: System::Windows::Forms::Button^  btnNoise;
	protected:

	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->btnHW4 = (gcnew System::Windows::Forms::Button());
			this->btnNoise = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// btnHW4
			// 
			this->btnHW4->Location = System::Drawing::Point(95, 160);
			this->btnHW4->Name = L"btnHW4";
			this->btnHW4->Size = System::Drawing::Size(75, 23);
			this->btnHW4->TabIndex = 0;
			this->btnHW4->Text = L"HW4";
			this->btnHW4->UseVisualStyleBackColor = true;
			this->btnHW4->Click += gcnew System::EventHandler(this, &MyForm::btnHW4_Click);
			// 
			// btnNoise
			// 
			this->btnNoise->Location = System::Drawing::Point(95, 65);
			this->btnNoise->Name = L"btnNoise";
			this->btnNoise->Size = System::Drawing::Size(75, 23);
			this->btnNoise->TabIndex = 1;
			this->btnNoise->Text = L"撒雜訊";
			this->btnNoise->UseVisualStyleBackColor = true;
			this->btnNoise->Click += gcnew System::EventHandler(this, &MyForm::btnNoise_Click);
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(284, 261);
			this->Controls->Add(this->btnNoise);
			this->Controls->Add(this->btnHW4);
			this->Name = L"MyForm";
			this->Text = L"MyForm";
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void btnHW4_Click(System::Object^  sender, System::EventArgs^  e) {

		Mat input = imread("NoiseImage.bmp", 0); //讀圖
		Mat img(input.rows + 2, input.cols + 2, CV_8U, Scalar(0)); //將圖修正，用以處理
		float meanL(0.0); //MASK平均
		float var(0.0);  //MASK變異數
		float temp(0.0);  //用來與閥值比較的暫存變數
		
		//修正圖
		imageA(input, img, input.rows, input.cols, input.rows + 2, input.cols + 2);
		//判斷雜訊變異數是否為0，是即直接傳回原圖
		if (NVar == 0) {
			imshow("input", input);
			imwrite("Output.bmp", input);
		}
		else {
			//適應性均值濾波器
			for (int i = 1; i < img.rows - 1; i++) {
				for (int j = 1; j < img.cols - 1; j++) {
					//計算平均
					meanL = (float)(img.at<uchar>(i - 1, j - 1) + img.at<uchar>(i - 1, j) + img.at<uchar>(i - 1, j + 1) +
						            img.at<uchar>(i, j - 1) + img.at<uchar>(i, j) + img.at<uchar>(i, j + 1) +
						            img.at<uchar>(i + 1, j - 1) + img.at<uchar>(i + 1, j) + img.at<uchar>(i + 1, j + 1)) / 9;
					//計算E[X^2]
					var = (float)(img.at<uchar>(i - 1, j - 1)*img.at<uchar>(i - 1, j - 1) + img.at<uchar>(i - 1, j)*img.at<uchar>(i - 1, j) + img.at<uchar>(i - 1, j + 1)*img.at<uchar>(i - 1, j + 1) +
						          img.at<uchar>(i, j - 1)*img.at<uchar>(i, j - 1) + img.at<uchar>(i, j)*img.at<uchar>(i, j) + img.at<uchar>(i, j + 1)*img.at<uchar>(i, j + 1) +
						          img.at<uchar>(i + 1, j - 1) * img.at<uchar>(i + 1, j - 1) + img.at<uchar>(i + 1, j)*img.at<uchar>(i + 1, j) + img.at<uchar>(i + 1, j + 1)*img.at<uchar>(i + 1, j + 1))/9;
					//計算MASK區域變異數 VAR = E[X^2] - (E[X])^2
					var = var - (meanL*meanL);
					
					//將雜訊變異數除區域變異數
					temp = NVar / var;
					//判斷是否超過閥值，如果超過就做均值濾波
					if (temp >=  thr ) {
						input.at<uchar>(i - 1, j - 1) = meanL;
					}
				}
			}
			
			imshow("input", input);
			imwrite("Output.bmp", input);
		}
	}
	private: System::Void btnNoise_Click(System::Object^  sender, System::EventArgs^  e) {

		Mat NoiseInput = imread("lena512_8bit.bmp", 0);  //讀圖
		float temp(0.0);  //暫存亂數值加上圖的像素
		srand(time(NULL));  //以時間為種子
		//產生雜訊圖
		for (int i = 0; i < NoiseInput.rows; i++) {
			for (int j = 0; j < NoiseInput.cols; j++) {

				temp = gasdev();  //取亂數值
				temp = temp * sqrt(NVar);  //乘上標準差根號1000
				temp = temp + NoiseInput.at<uchar>(i, j);  //加上該點像素值
				//判斷是否超出範圍
				if (temp > 255) temp = 255;
				else if (temp < 0) temp = 0;

				NoiseInput.at<uchar>(i, j) = temp;  //放回該點
			}
		}
		imshow("test", NoiseInput);
		imwrite("NoiseImage.bmp", NoiseInput);
	}
	};
}
