#pragma once

//class RawPNG: recebe os dados do IHDR e os dados comprimidos do IDAT, descomprimindo e desfiltrando os dados.
//herda a classe CompressedPNG, utilizando o seu construtor para acessar os dados brutos da imagem
//Membros::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//rawPNG - dados que formam a imagem da forma como foram geradas, sem compressão ou filtro
//Funções::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// void inf(vector<BYTE>& input, vector<BYTE>& output) - recebe os dados comprimidos de "input" e descompime em "output". o vetor "input" é apagado após a operação
// void unfilter(vector<BYTE>& input, int bitsPerLine, int bitsPerPixel) - recebe os dados descomprimidos e filtrados de "input" e com a informação do número de bits por pixel e bits por linha desfiltra o vetor
// unsigned char PaethPredictor(unsigned char a, unsigned char b, unsigned char c) - função utilizada em um dos filtros do formato png
// int bitsPerLine(const int& colorType, const int& bitDepth, const int& width) - calcula o número de bits por linha da imagem final
// int bitsPerPixel(const int& colorType, const int& bitDepth) - calcula o núemero de bits por pixel completo da imagem final

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