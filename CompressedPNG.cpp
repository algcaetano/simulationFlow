#include "CompressedPNG.h"

CompressedPNG::CompressedPNG()
{
	Width = 0; // 4 bytes
	Height = 0; //4 bytes
	BitDepth = 0; //1 byte
	ColorType = 0; //1 byte
	CompressionMethod = 0; //1 byte
	FilterMethod = 0; //1 byte
	InterlaceMethod = 0; //1 byte
}

CompressedPNG::CompressedPNG(string fileName)
{
	std::ifstream ifile = openPNG(fileName);
	while (!ifile.eof()) {
		int dataSize = readInt(ifile, 4);
		std::string dataName = readString(ifile, 4);
		if (dataName == "IHDR") {
			Width = readInt(ifile, 4); // 4 bytes
			Height = readInt(ifile, 4); //4 bytes
			BitDepth = readInt(ifile, 1); //1 byte
			ColorType = readInt(ifile, 1); //1 byte
			CompressionMethod = readInt(ifile, 1); //1 byte
			FilterMethod = readInt(ifile, 1); //1 byte
			InterlaceMethod = readInt(ifile, 1); //1 byte
			readInt(ifile, 4); //crc
		}
		else if (dataName == "IDAT") {
			readBYTE(ifile, idat, dataSize);
			readInt(ifile, 4); //crc
		}
		else if (dataName == "PLTE") {
			readBYTE(ifile, plte, dataSize);
			readInt(ifile, 4); //crc
		}
		else if (dataName != "IEND") {
			readInt(ifile, dataSize);
			readInt(ifile, 4); //crc
		}
	}
	ifile.close();
}

CompressedPNG::~CompressedPNG()
{
}

ifstream CompressedPNG::openPNG(string fileName)
{
	std::ifstream ifile(fileName, ios::binary);
	if (!ifile.is_open()) {
		cerr << "Arquivo inexistente";
		ifile.close();
		EXIT_FAILURE;
	}
	if (!ispng(ifile)) {
		cerr << "Arquivo nao e .png";
		ifile.close();
		EXIT_FAILURE;
	}
	return ifile;
}

void CompressedPNG::readBYTE(ifstream& inputFile, vector<BYTE>& outputVector, int numOfBytes)
{
	unsigned char nextByte;
	for (int i = 0; i < numOfBytes; i++) {
		inputFile.read((char*)&nextByte, 1);
		outputVector.push_back((BYTE)nextByte);
	}
}

string CompressedPNG::readString(ifstream& inputFile, int numOfBytes)
{
	string name;
	unsigned char nextByte;
	for (int i = 0; i < numOfBytes; i++) {
		inputFile.read((char*)&nextByte, 1);
		name.push_back(nextByte);
	}
	return name;
}

bool CompressedPNG::ispng(ifstream& inputFile)
{
	const int HEADER[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
	unsigned char header[8];
	bool headerCheck = true;
	for (int i = 0; i < 8; i++) {
		inputFile.read((char*)&header[i], 1); //ler os primeiros 8 bytes
		headerCheck = headerCheck && ((int)header[i] == HEADER[i]); //comparar com o cabecalho padrao
	}
	return headerCheck;
}

int CompressedPNG::readInt(ifstream& inputFile, int numOfBytes)
{
	int result = 0;
	unsigned char value;
	for (int i = 0;i < numOfBytes;i++) {
		inputFile.read((char*)&value, 1); //ler os primeiros 8 bytes
		result = result + (int)value * bitPower(256, (numOfBytes - i - 1));
	}
	return result;
}

int CompressedPNG::bitPower(int base, int exp)
{
	int result = 1;
	for (int i = 0; i < exp;i++) {
		result = result * base;
	}
	return result;
}
