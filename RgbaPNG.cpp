#include "RgbaPNG.h"

RgbaPNG::RgbaPNG()
	:RawPNG()
{
	stride = 0;
	bytesPerPixel = 0;
	format = PixelFormat24bppRGB;
}

RgbaPNG::RgbaPNG(string fileName)
	:RawPNG(fileName)
{
	createRgbaPNG(ColorType, Height, Width, BitDepth, rawPNG, plte, rgba);
	int bpl = (rgba.size() / Height) % 4; //resto após divisão por 4
	if (bpl == 0) {
		stride = (int)(rgba.size() / Height);
	}
	else {
		stride = (int)((rgba.size() / Height) + (rgba.size() / Height) % 4);
	}
	rawPNG.clear();
	if (ColorType == 6 || ColorType == 4) {
		bytesPerPixel = 4;
		format = PixelFormat32bppARGB;
	}
	else {
		bytesPerPixel = 3;
		format = PixelFormat24bppRGB;
	}
}

RgbaPNG::~RgbaPNG()
{
	//
}

int RgbaPNG::bitPower(int base, int exp)
{
	int result = 1;
	for (int i = 0; i < exp;i++) {
		result = result * base;
	}
	return result;
}

void RgbaPNG::createRgbaPNG(const int colorType, const int& height,
	const int& width, const int& bitDepth, const vector<BYTE>& imageData,
	const vector<BYTE>& palette, vector<BYTE>& rgba)
{
	if (colorType == 0) {
		rgb_0(height, width, bitDepth, imageData, rgba);
	}
	else if (colorType == 2) {
		rgb_2(height, width, bitDepth, imageData, rgba);
	}
	else if (colorType == 3) {
		rgb_3(height, width, bitDepth, imageData, palette, rgba);
	}
	else if (colorType == 4) {
		rgb_4(height, width, bitDepth, imageData, rgba);
	}
	else if (colorType == 6) {
		rgb_6(height, width, bitDepth, imageData, rgba);
	}
}

void RgbaPNG::rgb_0(const int& height, const int& width, const int& bitDepth,
	const vector<BYTE>& imageData, vector<BYTE>& rgba)
{
	long long int position = 0; //position in the imageData vector
	int sacaleFactor = 255 / (bitPower(2, bitDepth) - 1); //numbers that can be representable must be reescaled to 0-255
	int imageBase = (bitPower(2, bitDepth)); //base used (2, 4, 16, 256, 65536)
	int numOfAlgar = (8 / bitDepth); //número de algarismos em um byte(8, 4, 2, 1, 0.5)
	if (numOfAlgar < 1) {//se for 16
		for (int i = 0; i < height; i++) {//lines
			position++; //first byte in each line is the filter method
			for (int j = 0; j < width;j += 1) {//collums - two bytes, one value
				for (int k = 0;k < 3;k++) {//converter 16 bits em 8 bits
					rgba.push_back(((int)imageData[position + 1] + (int)imageData[position] * 256) / 257);
				}
				position += 2;
			}
			if (rgba.size() % 4 != 0) {
				for (int k = 0; k < (rgba.size() % 4); k++) {//lembrar que o tamanho muda a cada iteração (é o mesmo que um while aqui)
					rgba.push_back(0); //WORD align
				}
			}
		}
	}
	else {
		int nextAlgarValue; //valor do próximo algarísmo em relação ao algarísmo atual
		int nextPosition; //qual seria a próxima posição
		for (int i = 0; i < height; i++) {//lines
			position++; //first byte in each line is the filter method
			for (int j = 0; j < width;j += numOfAlgar) {//collums
				nextPosition = 0; //start from the actual position
				nextAlgarValue = bitPower(imageBase, numOfAlgar); //maximum value
				while ((j + nextPosition) < width && nextPosition < numOfAlgar) {//not the end of line or more values that can be representable
					rgba.push_back(sacaleFactor * (((int)imageData[position] % nextAlgarValue) / (nextAlgarValue / imageBase))); //modulo
					rgba.push_back(sacaleFactor * (((int)imageData[position] % nextAlgarValue) / (nextAlgarValue / imageBase))); //modulo
					rgba.push_back(sacaleFactor * (((int)imageData[position] % nextAlgarValue) / (nextAlgarValue / imageBase))); //modulo
					nextAlgarValue = nextAlgarValue / imageBase; //new value
					nextPosition += 1;
				}
				position++;
			}
			if (rgba.size() % 4 != 0) {
				for (int k = 0; k < (rgba.size() % 4); k++) {//lembrar que o tamanho muda a cada iteração (é o mesmo que um while aqui)
					rgba.push_back(0); //WORD align
				}
			}
		}
	}
}

void RgbaPNG::rgb_2(const int& height, const int& width, const int& bitDepth,
	const vector<BYTE>& imageData, vector<BYTE>& rgba)
{
	long long int position = 0; //position in the imageData vector
	if (bitDepth == 8) {
		for (int i = 0; i < height; i++) {//lines
			position++; //first byte in each line is the filter method
			for (int j = 0; j < width;j += 1) {//collums - one bytes, one value
				for (long long int k = 0;k < 3;k++) {
					rgba.push_back(imageData[position + 2 - k]); //o rgb aparece trocado no gdi+
				}
				position += 3;
			}
			if (rgba.size() % 4 != 0) {
				for (int k = 0; k < (rgba.size() % 4); k++) {//lembrar que o tamanho muda a cada iteração (é o mesmo que um while aqui)
					rgba.push_back(0); //WORD align
				}
			}
		}
	}
	else if (bitDepth == 16) {
		for (int i = 0; i < height; i++) {//lines
			position++; //first byte in each line is the filter method
			for (int j = 0; j < width;j += 1) {//collums - two bytes, one value
				for (int k = 0; k < 6;k += 2) {
					//rgba.push_back(imageData[position + 5 - k]); //BGR tem que ser nesta ordem
					rgba.push_back(((int)imageData[position + 5 - k] + (int)imageData[position + 5 - k - 1] * 256) / 257);
				}
				position += 6;
			}
			if (rgba.size() % 4 != 0) {
				for (int k = 0; k < (rgba.size() % 4); k++) {//lembrar que o tamanho muda a cada iteração (é o mesmo que um while aqui)
					rgba.push_back(0); //WORD align
				}
			}
		}
	}
	else {
		cerr << "erro: not suported bit depth";
	}
}

void RgbaPNG::rgb_3(const int& height, const int& width, const int& bitDepth,
	const vector<BYTE>& imageData, const vector<BYTE>& palette, vector<BYTE>& rgba)
{
	int position = 0; //position in the imageData vector
	int imageBase = (bitPower(2, bitDepth)); //base used (2, 4, 16, 256, 65536)
	int numOfAlgar = (8 / bitDepth); //número de algarismos em um byte(8, 4, 2, 1, 0.5)
	int nextAlgarValue; //valor do próximo algarísmo em relação ao algarísmo atual
	int nextPosition; //qual seria a próxima posição
	long long int palettePosition;
	for (int i = 0; i < height; i++) {//lines
		position++; //first byte in each line is the filter method
		for (int j = 0; j < width;j += numOfAlgar) {//collums
			nextPosition = 0; //start from the actual position
			nextAlgarValue = bitPower(imageBase, numOfAlgar); //maximum value
			while ((j + nextPosition) < width && nextPosition < numOfAlgar) {//not the end of line or more values that can be representable
				palettePosition = (((int)imageData[position] % nextAlgarValue) / (nextAlgarValue / imageBase)); //valor do index
				if (((3 * palettePosition) + 2) > (int)palette.size()) {
					cerr << "Fewer PLTE entries than the bit depth would allow";
				}
				rgba.push_back(palette[(3 * palettePosition) + 2]); //index+2
				rgba.push_back(palette[(3 * palettePosition) + 1]); //index+1
				rgba.push_back(palette[(3 * palettePosition)]); //index
				nextAlgarValue = nextAlgarValue / imageBase; //new value
				nextPosition += 1;
			}
			position++;
		}
		if (rgba.size() % 4 != 0) {
			for (int k = 0; k < (rgba.size() % 4); k++) {//lembrar que o tamanho muda a cada iteração (é o mesmo que um while aqui)
				rgba.push_back(0); //WORD align
			}
		}
	}
}

void RgbaPNG::rgb_4(const int& height, const int& width, const int& bitDepth,
	const vector<BYTE>& imageData, vector<BYTE>& rgba)
{
	long long int position = 0; //position in the imageData vector
	if (bitDepth == 8) {
		for (int i = 0; i < height; i++) {//lines
			position++; //first byte in each line is the filter method
			for (int j = 0; j < width;j += 1) {//collums - one byte, one value
				rgba.push_back(imageData[position]);
				rgba.push_back(imageData[position]);
				rgba.push_back(imageData[position]);
				rgba.push_back(imageData[position + 1]);
				position += 2;
			}
			if (rgba.size() % 4 != 0) {
				for (int k = 0; (rgba.size() % 4); k++) {//lembrar que o tamanho muda a cada iteração (é o mesmo que um while aqui)
					rgba.push_back(0); //WORD align
				}
			}
		}
	}
	else if (bitDepth == 16) {
		for (int i = 0; i < height; i++) {//lines
			position++; //first byte in each line is the filter method
			for (int j = 0; j < width;j += 1) {//collums - two bytes, one value
				for (int k = 0;k < 3;k++) {//converter de 16 para 8 bits
					rgba.push_back(((int)imageData[position + 1] + (int)imageData[position] * 256) / 257);
				}
				rgba.push_back(((int)imageData[position + 3] + (int)imageData[position + 2] * 256) / 257);
				position += 4;
			}
			if (rgba.size() % 4 != 0) {
				for (int k = 0; k < (rgba.size() % 4); k++) {//lembrar que o tamanho muda a cada iteração (é o mesmo que um while aqui)
					rgba.push_back(0); //WORD align
				}
			}
		}
	}
}

void RgbaPNG::rgb_6(const int& height, const int& width, const int& bitDepth,
	const vector<BYTE>& imageData, vector<BYTE>& rgba)
{
	long long int position = 0; //position in the imageData vector
	if (bitDepth == 8) {
		for (int i = 0; i < height; i++) {//lines
			position++; //first byte in each line is the filter method
			for (int j = 0; j < width;j += 1) {//collums - onr byte, one value
				for (int k = 0; k < 3;k++) {
					rgba.push_back(imageData[position + 2 - k]);
				}
				rgba.push_back(imageData[position + 3]);
				position += 4;
			}
			if (rgba.size() % 4 != 0) {
				for (int k = 0; k < (rgba.size() % 4); k++) {//lembrar que o tamanho muda a cada iteração (é o mesmo que um while aqui)
					rgba.push_back(0); //WORD align
				}
			}
		}
	}
	else if (bitDepth == 16) {
		for (int i = 0; i < height; i++) {//lines
			position++; //first byte in each line is the filter method
			for (int j = 0; j < width;j += 1) {//collums - two bytes, one value
				for (int k = 0; k < 6;k += 2) {
					//rgba.push_back(imageData[position + 5 - k]);
					rgba.push_back(((int)imageData[position + 5 - k] + (int)imageData[position + 5 - k - 1] * 256) / 257);
				}
				//rgba.push_back(imageData[position + 7]);
				//rgba.push_back(imageData[position + 6]);
				rgba.push_back(((int)imageData[position + 7] + (int)imageData[position + 6] * 256) / 257);
				position += 8;
			}
			if (rgba.size() % 4 != 0) {
				for (int k = 0; k < (rgba.size() % 4); k++) {//lembrar que o tamanho muda a cada iteração (é o mesmo que um while aqui)
					rgba.push_back(0); //WORD align
				}
			}
		}
	}
	else {
		cerr << "erro: not suported bit depth";
	}
}
