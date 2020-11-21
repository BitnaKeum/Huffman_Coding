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
	TreeNode* leftChild, * rightChild;	// 왼쪽 자식 노드, 오른쪽 자식 노드

	// 생성자
	TreeNode()
	{
		ascii = 0;
		leftChild = NULL;
		rightChild = NULL;
	}
};



int main()
{
	// 디코딩 
	char* decoding = new char[100000];	// 허프만 디코딩한 문자열을 담는 배열
	memset(decoding, 0, sizeof(char) * 100000);	// 0으로 초기화
	char* decoding_table = new char[100000];
	memset(decoding_table, 0, sizeof(char) * 100000);	// 0으로 초기화
	char** codeword = new char* [128];	// 허프만 테이블으로부터 얻은 codeword
	for (int i = 0; i < 128; i++) {
		codeword[i] = new char[30];		// 이차원 배열 동적 할당
		memset(codeword[i], 0, sizeof(char) * 30);	// 0으로 초기화
	}
	char* encoding = new char[100000];	// 16진수로 encoding된 데이터를 2진수로 변환
	memset(encoding, 0, sizeof(char) * 100000);	// 0으로 초기화
	int EOD_ascii = 36;		// 임의로 정한 EOD의 아스키 값('$')
	char EOD[14] = "1011001100100";	// EOD의 codeword


	// Huffman_table.hbs로부터 데이터 가져옴
	ifstream fin_table("Huffman_table.hbs", ios::in | ios::binary);
	if (!fin_table) {
		printf("error");
		return -1;
	}
	fin_table.seekg(0, fin_table.end);
	int table_len = (int)fin_table.tellg();	// 파일의 크기 구함
	fin_table.seekg(0, fin_table.beg);


	int size = 0;
	int stuffing_num = 0;
	for (int i = 0; i < table_len; i++) {
		char ch;
		fin_table.get(ch);	// 한 문자를 읽어들임
		int decimal = (int)ch;	// 문자를 10진수로 변환

		bitset<8> bs(decimal);	// 10진수를 binary로 변환
		string str = bs.to_string();
		const char* bin_str = str.c_str();	// string to const char*
		strcat(decoding_table, bin_str);
		size += strlen(bin_str);
	}
	fin_table.close();

	
	// decoding_table로부터 ascii, codeword bit, codeword를 가져옴
	char binary[30] = { 0, };
	TreeNode* root = new TreeNode();	// 루트 노드 생성
	TreeNode* pCur = NULL;
	
	int cnt = 0, j = 0;
	while(1){
		int index = 0;
		for (j = cnt; j < cnt + 13; j++) {	// EOD 체크
			binary[index] = decoding_table[j];	// 8bit binary 값 저장
			index++;
		}
		binary[index] = '\0';	// 마지막에 널 문자 추가
		if (!strcmp(binary, EOD))
			break;
		fill_n(binary, 14, 0);

		index = 0;
		for (j=cnt; j < cnt + 8; j++) {
			binary[index] = decoding_table[j];	// 8bit binary 값 저장
			index++;
		}
		binary[index] = '\0';	// 마지막에 널 문자 추가
		int ascii = strtol(binary, NULL, 2);	// 아스키 코드
		fill_n(binary, 9, 0);
		cnt += 8;
		
		index = 0;
		for (j = cnt; j < cnt + 8; j++) {
			binary[index] = decoding_table[j];	// 8bit binary 값 저장
			index++;
		}
		binary[index] = '\0';	// 마지막에 널 문자 추가
		int codeword_bit = strtol(binary, NULL, 2);	// codeword의 bit 수
		fill_n(binary, 9, 0);
		cnt += 8;

		index = 0;
		for (j=cnt; j < cnt + codeword_bit; j++) {
			binary[index] = decoding_table[j];	// 8bit binary 값 저장
			index++;
		}
		binary[index] = '\0';	// 마지막에 널 문자 추가
		strcpy(codeword[ascii], binary);	// codeword
		char code[30] = { 0, };					
		strcpy(code, binary);			// codeword
		fill_n(binary, 30, 0);
		cnt += codeword_bit;

	
		// 허프만 트리 생성
		pCur = root;
		for (int i = 0; i < codeword_bit; i++) {
			if (code[i] == '0') // code의 한 bit가 0이면 왼쪽 하위 노드로 이동
			{
				if (pCur->leftChild == NULL) {
					pCur->leftChild = new TreeNode();
				}

				pCur = pCur->leftChild;
			}
			else if (code[i] == '1')	// code의 한 bit가 1이면 오른쪽 하위 노드로 이동
			{
				if (pCur->rightChild == NULL) {
					pCur->rightChild = new TreeNode();
				}

				pCur = pCur->rightChild;
			}
			else // code의 한 bit가 0이나 1이 아닌 경우 break
			{
				break;
			}
		}

		pCur->ascii = ascii;	// leaf 노드에 아스키 값 저장
	}
	

	
	ifstream fin_code("Huffman_code.hbs", ios::in | ios::binary);
	if (!fin_code)
		return -1;
	fin_code.seekg(0, fin_code.end);
	int code_len = (int)fin_code.tellg();	// 파일의 크기 구함
	fin_code.seekg(0, fin_code.beg);
	

	// 파일의 16진수 데이터를 binary 데이터로 encoding 배열에 저장 (EOD 포함)
	for (int i = 0; i < code_len; i++) {
		char ch;
		fin_code.get(ch);
		int decimal = (int)ch;

		bitset<8> bs(decimal);	// 10진수를 binary로 변환
		string str = bs.to_string();
		const char* bin_str = str.c_str();	// string to const char*
		strcat(encoding, bin_str);
	}

	int idx = 0;
	pCur = root;
	for (int i = 0; i <strlen(encoding); i++) {
		char bit = encoding[i];

		if (bit == '0') // code의 한 bit가 0이면 왼쪽 하위 노드로 이동
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
		else if (bit == '1')	// code의 한 bit가 1이면 오른쪽 하위 노드로 이동
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
		else // 0이나 1이 아닌 경우 프로그램 종료
		{
			printf("error!");
			return -1;
		}
	}

	ofstream output("output.txt");	// "output.txt" 파일에 디코딩 텍스트 출력
	output << decoding;
	output.close();

	

	// 메모리 해제
	delete[] decoding;
	delete[] decoding_table;
	delete[] encoding;
	for (int i = 0; i < 128; i++)
		delete[] codeword[i];
	delete[] codeword;
}