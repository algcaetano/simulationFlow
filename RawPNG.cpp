#include "RawPNG.h"
RawPNG::RawPNG()
	:CompressedPNG()
{
}

RawPNG::RawPNG(string fileName)
	:CompressedPNG(fileName)
{
	inf(idat, rawPNG);
	unfilter(rawPNG, bitsPerLine(ColorType, BitDepth, Width), bitsPerPixel(ColorType, BitDepth));
}

RawPNG::~RawPNG()
{
}

void RawPNG::inf(vector<BYTE>& input, vector<BYTE>& output)
{
	const unsigned int IDATSIZE = (unsigned int)input.size(); //size of data
	const unsigned int CHUNK = 8192; //block sizes

	int ret; //return status
	unsigned have; //size of blocks decompressed
	z_stream strm; //z_stream object
	unsigned char out[CHUNK]; //output data
	unsigned char* next_out = (unsigned char*)out; //pointer to first byte in out

	/* allocate inflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.avail_out = 0;
	strm.next_in = Z_NULL;

	ret = inflateInit(&strm);
	if (ret != Z_OK) {
		cerr << "Zlib error.";
		EXIT_FAILURE;
	}

	unsigned char* next_in = (unsigned char*)input.data(); //pointer to first byte in idat vector
	unsigned int currPos = 0; //initial position
	unsigned int avail_in; //length of input block data

	//rodar até chegar no final
	do {
		if ((IDATSIZE - currPos) > CHUNK) {
			avail_in = CHUNK;
		}
		else {
			avail_in = IDATSIZE - currPos;
		}
		strm.avail_in = avail_in; /* number of bytes available at next_in */
		strm.next_in = next_in; /* next input byte */
		next_in += avail_in;
		currPos += avail_in;

		if (strm.avail_in == 0) //end of input with corrupted data
			break;

		//descomprimir esse bloco até o final
		do {
			strm.avail_out = CHUNK;
			strm.next_out = next_out;
			ret = inflate(&strm, Z_NO_FLUSH); //retur state
			assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			switch (ret) {
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR;     /* and fall through */
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				(void)inflateEnd(&strm);
				cerr << "Zlib error";
				EXIT_FAILURE;
			}
			have = CHUNK - strm.avail_out; /*check if all out buffer is fill*/
			output.insert(output.end(), next_out, (next_out + have)); /*write into out file*/
		} while (strm.avail_out == 0);
	} while (ret != Z_STREAM_END);

	/* clean up and return */
	(void)inflateEnd(&strm); //avoid memeory leak
	ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR; //return code
	if (ret < 0) {
		cerr << "zlib error";
		EXIT_FAILURE;
	}
	input.clear(); //não precisa mais dos dados originais
}

void RawPNG::unfilter(vector<BYTE>& input, int bitsPerLine, int bitsPerPixel)
{
	unsigned char raw_x_bpp; //left byte
	unsigned char prior_x; //up byte
	unsigned char prior_x_bpp; //up left byte
	for (long long int i = 0; i < (int)input.size();i += bitsPerLine) {
		int filterType = (int)input[i];
		for (long long int j = 1;j < bitsPerLine;j++) {
			if ((j - bitsPerPixel - 1) < 0) {//out of bounds (left)
				raw_x_bpp = (unsigned char)0;
				prior_x_bpp = (unsigned char)0;
				if ((i + j - bitsPerLine) < 0) {//out of bounds (up)
					prior_x = (unsigned char)0;
				}
				else {
					prior_x = input[i + j - (long long int)bitsPerLine]; //byte in prior line
				}
			}
			else {
				raw_x_bpp = input[i + j - bitsPerPixel];
				if ((i + j - bitsPerLine) < 0) {//out of bounds (up)
					prior_x = (unsigned char)0;
					prior_x_bpp = (unsigned char)0;
				}
				else {
					prior_x = input[i + j - bitsPerLine]; //byte in prior line
					prior_x_bpp = input[i + j - bitsPerLine - bitsPerPixel];
				}
			}
			if (filterType == 1) {
				input[i + j] = input[i + j] + raw_x_bpp;
			}
			else if (filterType == 2) {
				input[i + j] = input[i + j] + prior_x;
			}
			else if (filterType == 3) {
				input[i + j] = input[i + j] + (unsigned char)floor(((double)raw_x_bpp + (double)prior_x) / 2);
			}
			else if (filterType == 4) {
				input[i + j] = input[i + j] + PaethPredictor(raw_x_bpp, prior_x, prior_x_bpp);
			}
		}
	}
}

unsigned char RawPNG::PaethPredictor(unsigned char a, unsigned char b, unsigned char c)
{
	int p = (int)a + (int)b - (int)c; //initial estimate
	int pa = abs(p - (int)a); //distances to a, b, c
	int pb = abs(p - (int)b);
	int pc = abs(p - (int)c);
	// if pa <= pb AND pa <= pc then return a
	if (pa <= pb && pa <= pc) {
		return a;
	}
	//else if pb <= pc then return b
	else if (pb <= pc) {
		return b;
	}
	else {
		return c;
	}
}

int RawPNG::bitsPerLine(const int& colorType, const int& bitDepth, const int& width)
{
	if (colorType == 0) {
		return (int)ceil(1 + width * ((double)bitDepth / 8));
	}
	else if (colorType == 2) {
		return (int)(1 + 3 * (double)width * ((double)bitDepth / 8));
	}
	else if (colorType == 3) {
		return (int)ceil(1 + width * ((double)bitDepth / 8));
	}
	else if (colorType == 4) {
		return (1 + width * (2 * (bitDepth / 8)));
	}
	else if (colorType == 6) {
		return (1 + width * (4 * (bitDepth / 8)));
	}
	else {
		cerr << "Not suported color type";
		EXIT_FAILURE;
	}
}

int RawPNG::bitsPerPixel(const int& colorType, const int& bitDepth)
{
	switch (colorType)
	{
	case 0:
		return  (int)ceil(1 * ((double)bitDepth / 8)); //gray scale
		break;
	case 2:
		return (int)ceil(3 * ((double)bitDepth / 8)); //RGB
		break;
	case 3:
		return 1; //PLTE
		break;
	case 4:
		return (int)(2 * (bitDepth / 8)); //gray scale with alpha
		break;
	case 6:
		return (int)(4 * (bitDepth / 8)); //RGB with alpha
		break;
	default:
		cerr << "Not suported color type.";
		EXIT_FAILURE;
	}
}
