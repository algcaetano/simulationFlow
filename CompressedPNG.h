#pragma once

//class CompressedPNG: abre um arquivo .png e extrai os pricipais chunks:
//IHDR(tamanho, tipo de cor, etc. ) PLTE (se houver) IDAT (obrigatório) IEND (final do arquivo)
//Membros::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Width - número do colunas (int)
// Height - número de linhas (int)
// BitDepth - número de bits por pixel (int)
// ColorType tipo de cor (int)
// CompressionMethod - 0 (the only compression method presently defined for PNG) (int)
// FilterMethod - 0 defines five basic filter types (int)
// InterlaceMethod (int)
// idat - vetor com os dados da imagem comprimidos (vector<int>)
// plte - vetor com a paleta de cores se houver (vector<int>)
//Funções::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
// ifstream openPNG(string fileName) -  abre o arquivo no formato binário
// void readBYTE(ifstream& inputFile, vector<BYTE>& outputVector, int numOfBytes) - lê uma sequência de "numOfBytes" do "inputFile" e escreve no 'outputVector"
// string readString(ifstream& inputFile, int numOfBytes) - lê uma sequência de "numOfBytes" caracteres do "inputFile" e retorna uma string com os caracteres
// bool ispng(ifstream& inputFile) - lê os primeiros 8 bytes do arquivo e confere se é um arquivo png (true) ou não (false)
// int readInt(ifstream& inputFile, int numOfBytes) - lê uma sequência de "numOfBytes" caracteres do "inputFile" e retorna um inteiro com o valor considerando o byte mais à esquerda como o mais significativo
// int bitPower(int base, int exp) - calcula a potência base^exp

#include<iostream>
#include <string>
#include<fstream>
#include<vector>
#include<Windows.h>
using namespace std;

class CompressedPNG {
public:
	CompressedPNG();
	CompressedPNG(string fileName);
	~CompressedPNG();
public:
	int Width; // 4 bytes
	int Height; //4 bytes
	int BitDepth; //1 byte
	int ColorType; //1 byte
	int CompressionMethod; //1 byte
	int FilterMethod; //1 byte
	int InterlaceMethod; //1 byte
	std::vector<BYTE> idat; //compressed data
	std::vector<BYTE> plte; //color palette
private:
	ifstream openPNG(string fileName);
	void readBYTE(ifstream& inputFile, vector<BYTE>& outputVector, int numOfBytes);
	string readString(ifstream& inputFile, int numOfBytes);
	bool ispng(ifstream& inputFile);
	int readInt(ifstream& inputFile, int numOfBytes);
	int bitPower(int base, int exp);
};