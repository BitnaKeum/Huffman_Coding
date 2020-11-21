#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <bitset>
using namespace std;

class TreeNode
{
public:
	int ascii;
	TreeNode* leftChild, * rightChild;	// ���� �ڽ� ���, ������ �ڽ� ���

	// ������
	TreeNode()
	{
		ascii = 0;
		leftChild = NULL;
		rightChild = NULL;
	}
};



int main()
{
	// ���ڵ� 
	char* decoding = new char[100000];	// ������ ���ڵ��� ���ڿ��� ��� �迭
	memset(decoding, 0, sizeof(char) * 100000);	// 0���� �ʱ�ȭ
	char* decoding_table = new char[100000];
	memset(decoding_table, 0, sizeof(char) * 100000);	// 0���� �ʱ�ȭ
	char** codeword = new char* [128];	// ������ ���̺����κ��� ���� codeword
	for (int i = 0; i < 128; i++) {
		codeword[i] = new char[30];		// ������ �迭 ���� �Ҵ�
		memset(codeword[i], 0, sizeof(char) * 30);	// 0���� �ʱ�ȭ
	}
	char* encoding = new char[100000];	// 16������ encoding�� �����͸� 2������ ��ȯ
	memset(encoding, 0, sizeof(char) * 100000);	// 0���� �ʱ�ȭ
	int EOD_ascii = 36;		// ���Ƿ� ���� EOD�� �ƽ�Ű ��('$')
	char EOD[14] = "1011001100100";	// EOD�� codeword


	// Huffman_table.hbs�κ��� ������ ������
	ifstream fin_table("Huffman_table.hbs", ios::in | ios::binary);
	if (!fin_table) {
		printf("error");
		return -1;
	}
	fin_table.seekg(0, fin_table.end);
	int table_len = (int)fin_table.tellg();	// ������ ũ�� ����
	fin_table.seekg(0, fin_table.beg);


	int size = 0;
	int stuffing_num = 0;
	for (int i = 0; i < table_len; i++) {
		char ch;
		fin_table.get(ch);	// �� ���ڸ� �о����
		int decimal = (int)ch;	// ���ڸ� 10������ ��ȯ

		bitset<8> bs(decimal);	// 10������ binary�� ��ȯ
		string str = bs.to_string();
		const char* bin_str = str.c_str();	// string to const char*
		strcat(decoding_table, bin_str);
		size += strlen(bin_str);
	}
	fin_table.close();

	
	// decoding_table�κ��� ascii, codeword bit, codeword�� ������
	char binary[30] = { 0, };
	TreeNode* root = new TreeNode();	// ��Ʈ ��� ����
	TreeNode* pCur = NULL;
	
	int cnt = 0, j = 0;
	while(1){
		int index = 0;
		for (j = cnt; j < cnt + 13; j++) {	// EOD üũ
			binary[index] = decoding_table[j];	// 8bit binary �� ����
			index++;
		}
		binary[index] = '\0';	// �������� �� ���� �߰�
		if (!strcmp(binary, EOD))
			break;
		fill_n(binary, 14, 0);

		index = 0;
		for (j=cnt; j < cnt + 8; j++) {
			binary[index] = decoding_table[j];	// 8bit binary �� ����
			index++;
		}
		binary[index] = '\0';	// �������� �� ���� �߰�
		int ascii = strtol(binary, NULL, 2);	// �ƽ�Ű �ڵ�
		fill_n(binary, 9, 0);
		cnt += 8;
		
		index = 0;
		for (j = cnt; j < cnt + 8; j++) {
			binary[index] = decoding_table[j];	// 8bit binary �� ����
			index++;
		}
		binary[index] = '\0';	// �������� �� ���� �߰�
		int codeword_bit = strtol(binary, NULL, 2);	// codeword�� bit ��
		fill_n(binary, 9, 0);
		cnt += 8;

		index = 0;
		for (j=cnt; j < cnt + codeword_bit; j++) {
			binary[index] = decoding_table[j];	// 8bit binary �� ����
			index++;
		}
		binary[index] = '\0';	// �������� �� ���� �߰�
		strcpy(codeword[ascii], binary);	// codeword
		char code[30] = { 0, };					
		strcpy(code, binary);			// codeword
		fill_n(binary, 30, 0);
		cnt += codeword_bit;

	
		// ������ Ʈ�� ����
		pCur = root;
		for (int i = 0; i < codeword_bit; i++) {
			if (code[i] == '0') // code�� �� bit�� 0�̸� ���� ���� ���� �̵�
			{
				if (pCur->leftChild == NULL) {
					pCur->leftChild = new TreeNode();
				}

				pCur = pCur->leftChild;
			}
			else if (code[i] == '1')	// code�� �� bit�� 1�̸� ������ ���� ���� �̵�
			{
				if (pCur->rightChild == NULL) {
					pCur->rightChild = new TreeNode();
				}

				pCur = pCur->rightChild;
			}
			else // code�� �� bit�� 0�̳� 1�� �ƴ� ��� break
			{
				break;
			}
		}

		pCur->ascii = ascii;	// leaf ��忡 �ƽ�Ű �� ����
	}
	

	
	ifstream fin_code("Huffman_code.hbs", ios::in | ios::binary);
	if (!fin_code)
		return -1;
	fin_code.seekg(0, fin_code.end);
	int code_len = (int)fin_code.tellg();	// ������ ũ�� ����
	fin_code.seekg(0, fin_code.beg);
	

	// ������ 16���� �����͸� binary �����ͷ� encoding �迭�� ���� (EOD ����)
	for (int i = 0; i < code_len; i++) {
		char ch;
		fin_code.get(ch);
		int decimal = (int)ch;

		bitset<8> bs(decimal);	// 10������ binary�� ��ȯ
		string str = bs.to_string();
		const char* bin_str = str.c_str();	// string to const char*
		strcat(encoding, bin_str);
	}

	int idx = 0;
	pCur = root;
	for (int i = 0; i <strlen(encoding); i++) {
		char bit = encoding[i];

		if (bit == '0') // code�� �� bit�� 0�̸� ���� ���� ���� �̵�
		{
			if (pCur->leftChild == NULL) {
				if (pCur->ascii == EOD_ascii)
					break;
				decoding[idx] = (char)pCur->ascii;
				idx++;
				pCur = root->leftChild;
			}
			else
				pCur = pCur->leftChild;
		}
		else if (bit == '1')	// code�� �� bit�� 1�̸� ������ ���� ���� �̵�
		{
			if (pCur->rightChild == NULL) {
				if (pCur->ascii == EOD_ascii)
					break;
				decoding[idx] = (char)pCur->ascii;
				idx++;
				pCur = root->rightChild;
			}
			else
				pCur = pCur->rightChild;
			
		}
		else // 0�̳� 1�� �ƴ� ��� ���α׷� ����
		{
			printf("error!");
			return -1;
		}
	}

	ofstream output("output.txt");	// "output.txt" ���Ͽ� ���ڵ� �ؽ�Ʈ ���
	output << decoding;
	output.close();

	

	// �޸� ����
	delete[] decoding;
	delete[] decoding_table;
	delete[] encoding;
	for (int i = 0; i < 128; i++)
		delete[] codeword[i];
	delete[] codeword;
}