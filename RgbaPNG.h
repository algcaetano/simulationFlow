#pragma once
//calss RgbaPNG -  forma um vetor sequencial de bytes linha por linha da imagem. As entradas do vetor aparecem sempre na ordem:
//B (um byte), G(um byte), R(um byte), (A(um byte), se houver) formando as cores de um pixel. cada linha é sempre um múltiplo de 4 bytes
//se houver bytes em excesso, eles serão zero e não são utilizados. Após formar o vetor, os dados brutos são apagados.
//Herda a classe RawPNG, utilizando o seu contrutor para extrair os dados brutos
//Membros:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// rgba - o vetor com o valor rgb de cada pixel (vector<BYTE>)
// stride - o número de BYTES (múltiplo de 4) em cada linha (int)
//Funções:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/*	int bitPower(int base, int exp);
	void createRgbaPNG(const int colorType, const int& height, const int& width, const int& bitDepth,
		const vector<BYTE>& imageData, const vector<BYTE>& palette, vector<BYTE>& rgba);
	Somente para simplificar a chamada. Essa função vai chamar uma das funções abaixo, de acordo com o tipo de cor
	contido em colorType

	void rgb_0(const int& height, const int& width, const int& bitDepth, const vector<BYTE>& imageData,
		vector<BYTE>& rgba);
	colorType 0: escala de cinza

	void rgb_2(const int& height, const int& width, const int& bitDepth, const vector<BYTE>& imageData,
		vector<BYTE>& rgba);
	colorType 2: rgb

	void rgb_3(const int& height, const int& width, const int& bitDepth, const vector<BYTE>& imageData,
		const vector<BYTE>& palette, vector<BYTE>& rgba);
	colorType 3: paleta de core

	void rgb_4(const int& height, const int& width, const int& bitDepth, const vector<BYTE>& imageData,
		vector<BYTE>& rgba);
	colorType 4: escala de cinza com transparência

	void rgb_6(const int& height, const int& width, const int& bitDepth, const vector<BYTE>& imageData,
		vector<BYTE>& rgba);
	colorType 6: RGBA
		*/

#include "RawPNG.h"
#include <objidl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "Gdiplus.lib")

class RgbaPNG : public RawPNG {
public:
	RgbaPNG();
	RgbaPNG(string fileName);
	~RgbaPNG();
public:
	vector<BYTE> rgba;
	int stride;
	int bytesPerPixel;
	PixelFormat format;
private:
	int bitPower(int base, int exp);
	void createRgbaPNG(const int colorType, const int& height, const int& width, const int& bitDepth,
		const vector<BYTE>& imageData, const vector<BYTE>& palette, vector<BYTE>& rgba);
	void rgb_0(const int& height, const int& width, const int& bitDepth, const vector<BYTE>& imageData,
		vector<BYTE>& rgba);
	void rgb_2(const int& height, const int& width, const int& bitDepth, const vector<BYTE>& imageData,
		vector<BYTE>& rgba);
	void rgb_3(const int& height, const int& width, const int& bitDepth, const vector<BYTE>& imageData,
		const vector<BYTE>& palette, vector<BYTE>& rgba);
	void rgb_4(const int& height, const int& width, const int& bitDepth, const vector<BYTE>& imageData,
		vector<BYTE>& rgba);
	void rgb_6(const int& height, const int& width, const int& bitDepth, const vector<BYTE>& imageData,
		vector<BYTE>& rgba);
};