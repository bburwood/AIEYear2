#include <iostream>
#include <vector>
#include <algorithm>
#include <stdio.h>

//	To DO:
//	read through the file
//	count up the symbols
//	create our HuffmanNodes
//	build the tree
//	build our dictionary
//	write out the comrpessed file

struct HuffmanNode
{
	HuffmanNode*	parent;
	HuffmanNode*	zeroBitChild;
	HuffmanNode*	oneBitChild;

	int	iFrequency;
	unsigned char	value;
};

struct DictionaryEntry
{
	unsigned char	uiBitfield[32];	//	absolute maximum worst case bitfield size
	unsigned int	uiBitCount;
};

void FillDictionary(DictionaryEntry* Dictionary, HuffmanNode* node, DictionaryEntry entry)
{
	if (node->oneBitChild)
	{
		//	one bit branch
		DictionaryEntry	oneBitEntry = entry;
		oneBitEntry.uiBitCount++;
		unsigned int	uiBitToSet = oneBitEntry.uiBitCount % 8;
		unsigned int	uiIntToSet = oneBitEntry.uiBitCount / 8;

		oneBitEntry.uiBitfield[uiIntToSet] |= 1 << uiBitToSet;

		FillDictionary(Dictionary, node->oneBitChild, oneBitEntry);
	}
	if (node->zeroBitChild)
	{
		//	zero bit child
		DictionaryEntry	zeroBitEntry = entry;
		zeroBitEntry.uiBitCount++;

		FillDictionary(Dictionary, node->zeroBitChild, zeroBitEntry);
	}
	if (!node->zeroBitChild && !node->oneBitChild)
	{
		//	this is a leaf node
		Dictionary[node->value] = entry;
	}
}



int main(int argc, char**argv)
{
	//	usage:
	//	huffman <input-file> <output-file>

	if (argc != 3)
	{
		printf("Usage:\nhuffman <input-file> <output-file>\n");
		return 0;
	}

	const char* inputFileName = argv[1];
	const char* outputFileName = argv[2];

	FILE*	inputFile = fopen(inputFileName, "rb");
	unsigned int	uiByteCounts[256] = {};	//	set the arary to all zeros

	while (!feof(inputFile))
	{
		unsigned char	byte;
		fread(&byte, 1, 1, inputFile);

		uiByteCounts[byte]++;
	}
	
	HuffmanNode huffmanPool[512] = {};

	unsigned int iNodesUsed = 0;

	std::vector<HuffmanNode*>	aSymbolTree;

	for (unsigned int i = 0; i < 256; ++i)
	{
		if (uiByteCounts[i] != 0)
		{
			HuffmanNode*	node = &huffmanPool[iNodesUsed++];
			node->iFrequency = uiByteCounts[0];
			node->value = (unsigned char)i;

			aSymbolTree.push_back(node);
		}
	}

	while (aSymbolTree.size() != 1)
	{
		std::sort(aSymbolTree.begin(), aSymbolTree.end(),
			[](HuffmanNode *a, HuffmanNode* b)
		{
			return (a->iFrequency > b->iFrequency);
		}
		);

		unsigned int uiLastIndex = aSymbolTree.size() - 1;
		HuffmanNode*	first = aSymbolTree[uiLastIndex];
		HuffmanNode*	second = aSymbolTree[uiLastIndex - 1];

		HuffmanNode*	newNode = &huffmanPool[iNodesUsed++];
		newNode->iFrequency = first->iFrequency + second->iFrequency;
		newNode->oneBitChild = first;
		newNode->zeroBitChild = second;

		first->parent = newNode;
		second->parent = newNode;

		aSymbolTree.pop_back();
		aSymbolTree.pop_back();

		aSymbolTree.push_back(newNode);
	}

	DictionaryEntry	aDictionary[256];
	DictionaryEntry	emptyEntry = {};
	FillDictionary(aDictionary, aSymbolTree[0], emptyEntry);

	FILE*	outputFile = fopen(outputFileName, "wb");

	fseek(inputFile, 0, SEEK_SET);

	fwrite(aDictionary, sizeof (aDictionary), 1, outputFile);

	unsigned char	byteBuffer = 0;
	unsigned int	bitWriteCursor = 0;

	unsigned char	maskLookup[9] =
	{
		0,		//	0000 0000
		1,		//	0000 0001
		3,		//	0000 0011
		7,		//	0000 0111
		15,		//	0000 1111
		31,		//	0001 1111
		63,		//	0011 1111
		127,	//	0111 1111
		255		//	1111 1111
	};

	while (!feof(inputFile))
	{
		unsigned char	byte;
		fread(&byte, 1, 1, inputFile);
		DictionaryEntry* entry = &aDictionary[byte];
		unsigned int bitReadCursor = 0;

		while (bitReadCursor < entry->uiBitCount)
		{
			unsigned int	currReadBit = (bitReadCursor % 8);
			unsigned int	currReadByte = (bitReadCursor / 8);

			unsigned int	bitsLeftInReadByte = 8 - currReadBit;
			unsigned int	bitsLeftInWriteByte = 8 - bitWriteCursor;
			unsigned int	bitsLeftInRead = entry->uiBitCount - bitReadCursor;

			unsigned int	readBound = std::min(bitsLeftInReadByte, bitsLeftInRead);
			unsigned int	bitsToCopy = std::min(readBound, bitsLeftInWriteByte);

			unsigned char	mask = maskLookup[bitsToCopy] << currReadBit;

			int	shiftAmount = bitWriteCursor - currReadBit;

			if (shiftAmount >= 0)
			{
				//	actual write
				byteBuffer |= (entry->uiBitfield[currReadByte] & mask) << shiftAmount;
			}
			else
			{
				//	actual write
				byteBuffer |= (entry->uiBitfield[currReadByte] & mask) >> (-shiftAmount);
			}

			bitWriteCursor += bitsToCopy;
			bitReadCursor += bitsToCopy;

			if (bitWriteCursor == 8)
			{
				fwrite(&byteBuffer, 1, 1, outputFile);
				byteBuffer = 0;
				bitWriteCursor = 0;
			}
		}
	}
	fclose(inputFile);
	fclose(outputFile);

	return 0;
}
