#pragma once

//class RawPNG: recebe os dados do IHDR e os dados comprimidos do IDAT, descomprimindo e desfiltrando os dados.
//herda a classe CompressedPNG, utilizando o seu construtor para acessar os dados brutos da imagem
//Membros::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//rawPNG - dados que formam a imagem da forma como foram geradas, sem compress�o ou filtro
//Fun��es::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// void inf(vector<BYTE>& input, vector<BYTE>& output) - recebe os dados comprimidos de "input" e descompime em "output". o vetor "input" � apagado ap�s a opera��o
// void unfilter(vector<BYTE>& input, int bitsPerLine, int bitsPerPixel) - recebe os dados descomprimidos e filtrados de "input" e com a informa��o do n�mero de bits por pixel e bits por linha desfiltra o vetor
// unsigned char PaethPredictor(unsigned char a, unsigned char b, unsigned char c) - fun��o utilizada em um dos filtros do formato png
// int bitsPerLine(const int& colorType, const int& bitDepth, const int& width) - calcula o n�mero de bits por linha da imagem final
// int bitsPerPixel(const int& colorType, const int& bitDepth) - calcula o n�emero de bits por pixel completo da imagem final

#include "CompressedPNG.h"
#include <zlib.h>
#include <assert.h>

class RawPNG : public CompressedPNG {
public:
	RawPNG();
	RawPNG(string fileName);
	~RawPNG();
public:
	vector<BYTE> rawPNG;
private:
	void inf(vector<BYTE>& input, vector<BYTE>& output);
	void unfilter(vector<BYTE>& input, int bitsPerLine, int bitsPerPixel);
	unsigned char PaethPredictor(unsigned char a, unsigned char b, unsigned char c);
	int bitsPerLine(const int& colorType, const int& bitDepth, const int& width);
	int bitsPerPixel(const int& colorType, const int& bitDepth);
};