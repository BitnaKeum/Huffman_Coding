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
	int prob;
	TreeNode *leftChild, *rightChild;	// ���� �ڽ� ���, ������ �ڽ� ���
	int root_idx;	// �ڽ��� ����Ű�� root_array�� index
	char code;		// 0 �Ǵ� 1�� codeword
	TreeNode* parent;	// �θ� ���

	// ������
	TreeNode(int _ascii, int _prob, TreeNode* _leftChild, TreeNode* _rightChild, int _root_idx, char _code, TreeNode* _parent)
	{
		ascii = _ascii;
		prob = _prob;
		leftChild = _leftChild;
		rightChild = _rightChild;
		root_idx = _root_idx;
		code = _code;
		parent = _parent;
	}
};



int* min_idx(int*, int);
void recursive_code(TreeNode*, TreeNode*, char);
void get_codeword(TreeNode*, char*);

TreeNode* root_array[128] = { 0, };		// ���� �ӽ� root�� �����ϴ� �迭

int main()
{
	int prob[128] = { 0, };				// �ƽ�Ű�ڵ� 0~127�� ��(Ȯ��)�� ���� �迭 
	int prob_copy[128] = { 0, };		// prob �纻 �迭
	int prob_root[128] = { 0, };		// root_array�� �ӽ� root���� ����Ű�� �ִ� TreeNode�� prob�� ����
	int* min_idx_prob = NULL;			// prob_copy �迭���� �ּҰ� �ΰ��� index 
	int* min_idx_root = NULL;			// root_array �迭���� �ּҰ� �ΰ��� index
	int* min_idx_prob_root = NULL;		// min_prob_root���� �ּҰ� �ΰ��� index
	int min_prob_root[4] = { 0, };		// prob_copy �ּҰ� �ΰ��� prob_root �ּҰ� �ΰ��� ���ʷ� ����
	int idx0, idx1, idx2, idx3, idx4, idx5;
	char** codeword = new char*[128];	// ������ �ڵ����� ���� codeword
	for (int i = 0; i < 128; i++) {
		codeword[i] = new char[30];		// ������ �迭 ���� �Ҵ�
		memset(codeword[i], 0, sizeof(char) * 30);	// 0���� �ʱ�ȭ
	}
	char* encoding = new char[100000];	// ������ ���ڵ��� ���ڿ��� ��� �迭
	memset(encoding, 0, sizeof(char) * 100000);	// 0���� �ʱ�ȭ
	char* encoding_table = new char[100000];		// ������ ���̺��� ��� �迭
	memset(encoding_table, 0, sizeof(char) * 100000);	// 0���� �ʱ�ȭ

	int EOD_ascii = 36;		// ���Ƿ� ���� EOD�� �ƽ�Ű ��('$')
	char EOD[20] = { 0, };	// EOD�� codeword
	
	char* file_data = new char[100000];	// ������ �����͸� ������ �迭, �޸� �Ҵ� �� 0���� �ʱ�ȭ
	memset(file_data, 0, sizeof(char) * 100000);


	ifstream fin("input.txt");	// "input.txt" ������ ���� �о����
	if (!fin.is_open())	// ������ ������ ���� ��� ���α׷� ����
	{
		printf("File can't be opened!\n");
		return -1;
	}
	int len;	// file_data�� ����Ǵ� �������� ��
	for (len = 0; !fin.eof(); len++) {
		fin.get(file_data[len]);	// fin���� �� ���ھ� �о��  file_data�� ����
	}
	file_data[--len] = 0;	// EOF �� �̻��� ������ ����
	fin.close();


	for (int i = 0; i<len; i++) {
		int ascii = (int)file_data[i];	// �� ������ �ƽ�Ű �ڵ� ��
		prob[ascii]++;	// �ƽ�Ű �ڵ� ���� �ش��ϴ� index������ ���� 1 �������� ī����
	}
	prob[EOD_ascii]++;	// EOD�� �ѹ� �־��� (EOD: '$')
	// prob ���
	/*for (int j = 0; j < 128; j++) {
		if (prob[j] == 0) {	// ������ ���� ���ڸ� �ǳʶ�
			continue;
		}
		printf("'%c' : %d\n", j, prob[j]);
	}
	*/
	for (int i = 0; i < 128; i++) {
		prob_copy[i] = prob[i];	// prob�� ���� ������ prob_copy
	}


	// Tree�� ����ִ� �ʱ� ����
	min_idx_prob = min_idx(prob_copy, 128);	// prob_copy���� �ּҰ� �ΰ��� index ����
	idx0 = min_idx_prob[0];
	idx1 = min_idx_prob[1];
	root_array[0] = new TreeNode(idx0, prob_copy[idx0], NULL, NULL, 0, '0', 0);	// prob: �ּҰ�1, root0�� ����Ŵ
	root_array[1] = new TreeNode(idx1, prob_copy[idx1], NULL, NULL, 1, '0', 0);	// prob: �ּҰ�2, root1�� ����Ŵ
	prob_copy[idx0] = prob_copy[idx1] = 0;		// TreeNode�� ����������Ƿ� �ش� �� ����

	root_array[0] = new TreeNode(0, root_array[0]->prob + root_array[1]->prob, root_array[0], root_array[1], 0, '0', 0);	// prob: �ּҰ�1+�ּҰ�2, root0�� ����Ŵ
	root_array[1] = 0;
	root_array[0]->leftChild->root_idx = -1;	// �ڽ��� ����Ű�� root�� �ٸ��� ����Ű�Ƿ� root_idx = -1�� ����
	root_array[0]->rightChild->root_idx = -1;
	root_array[0]->leftChild->parent = root_array[0]->rightChild->parent = root_array[0];	// ���ο� ��带 �� ����� parent�� ����


	

	while(1) {
		// prob_copy���� �ּҰ� �ΰ��� index ����
		min_idx_prob = min_idx(prob_copy, 128);
		idx0 = min_idx_prob[0];
		idx1 = min_idx_prob[1];
		min_prob_root[0] = prob_copy[idx0];
		min_prob_root[1] = prob_copy[idx1];
		//printf("[%d]: %d [%d]: %d\n", idx0, prob_copy[idx0], idx1, prob_copy[idx1]);

		// prob_root���� �ּҰ� �ΰ��� index ����
		for (int i = 0; i < 128; i++) {	  // �ӽ� root���� prob���� ����
			prob_root[i] = 0;	// 0���� �ʱ�ȭ
			if (root_array[i] == 0)
				continue;
			prob_root[i] = root_array[i]->prob;
		}
		min_idx_root = min_idx(prob_root, 128);
		idx2 = min_idx_root[0];
		idx3 = min_idx_root[1];
		min_prob_root[2] = prob_root[idx2];
		min_prob_root[3] = prob_root[idx3];
		//printf("[%d]: %d [%d]: %d\n", idx2, prob_root[idx2], idx3, prob_root[idx3]);

		// min_prob_root���� �ּҰ� �ΰ��� index ��ȯ
		min_idx_prob_root = min_idx(min_prob_root, 4);
		idx4 = min_idx_prob_root[0];
		idx5 = min_idx_prob_root[1];
		if (min_prob_root[idx5] == 0) {// prob_copy�� ��� ���� ����ְ� ���� root �� �ϳ��� ���� ��� �ݺ��� Ż��
			//printf("[%d]: %d\n", idx2, root_array[idx2]->prob);
			break;
		}
		//printf("[%d]: %d [%d]: %d\n", min_idx_prob_root[0], min_prob_root[idx4], min_idx_prob_root[1], min_prob_root[idx5]);


		int root_index[2] = { 0, };	// ������ �� ��带 ����Ű�� root_array�� index
		for (int i = 0; i < 2; i++) {
			int idx = (i == 0) ? idx4 : idx5;
	
			if (idx == 0 || idx == 1) {	// prob_copy�� ���� ���
				for (int j = 0; j < 128; j++) {
					if (root_array[j] == 0) // ����ִ� root�� index�� ã��
					{
						root_index[i] = j;		 // ���� ������ ��带 ����ų root_array�� index
						
						int ascii = (idx == 0) ? idx0 : idx1;
						root_array[j] = new TreeNode(ascii, min_prob_root[idx], NULL, NULL, j, '0', 0);	// prob_copy�� ���� ���� TreeNode ����
						//printf("root[%d] -> prob:%d!!\n", j, min_prob_root[idx]);
						//int idx1 = min_idx_prob[idx];	// prob_copy������ �ش� ���� index�� ����
						idx = (idx == 0) ? idx0 : idx1;
						prob_copy[idx] = 0;			// TreeNode�� ����������Ƿ� �ش� �� ����
						break;
					}
				}
			}
			else {		// prob_root�� ���� ���
				root_index[i] = (idx == 2) ? idx2 : idx3;	// prob_root������ index = root_array������ index
				//int r = root_index[i];
				//printf("root[%d] -> prob:%d\n", r, root_array[r]->prob);
			}
		}

		// �� ��带 ���� �θ� ��� ����
		int root_index0 = root_index[0];
		int root_index1 = root_index[1];
		int root_idx = (root_index0 < root_index1) ? root_index0 : root_index1;	// �� ���� root_index ���� ����, ���� ��带 ����Ű�� ����
		root_array[root_idx] = new TreeNode(0, root_array[root_index0]->prob + root_array[root_index1]->prob, root_array[root_index0], root_array[root_index1], root_idx, '0', 0);	// �� ����� ���� ��� ����
		//printf("root[%d] -> prob:%d\n", root_idx, root_array[root_idx]->prob);
		root_array[root_idx]->leftChild->root_idx = -1;	// �ڽĳ�带 ����Ű�� root�� �θ��带 ����Ű�Ƿ� root_idx = -1�� ����
		root_array[root_idx]->rightChild->root_idx = -1;
		root_array[root_idx]->leftChild->parent = root_array[root_idx]->rightChild->parent = root_array[root_idx];	// ���ο� ��带 �� ����� parent�� ����
		root_idx = (root_index0 > root_index1) ? root_index0 : root_index1;	// �� ū root_index ���� ����, �� �̻� ����Ű�� �����Ƿ� NULL�� �Ҵ��ϱ� ����
		root_array[root_idx] = 0;

	}


	recursive_code(0, root_array[0], '2');	// root ��带 �Ѱ���, root�� code�� ������� �����Ƿ� 2�� ����

	for (int i = 1; i < 128; i++)
	{
		if (root_array[i] == 0)
			continue;
		get_codeword(root_array[i], codeword[i]);	// �ش� �ƽ�Ű ���� ���� Symbol�� codeword�� ����
	}
	//������ ���̺� ���
	/*for (int j = 1; j < 128; j++) {
		if (*codeword[j] == 0)
			continue;
		printf("'%c'\t\t%d\t\t%s\n", j, prob[j], codeword[j]);	// Symbol, probability, codeword
	}
	*/


	// encoding
	for (int i = 0; i < len; i++) {
		int ascii = (int)file_data[i];
		strcat(encoding, codeword[ascii]);	// �ش� ������ codeword�� �ٿ�����
	}

	strcpy(EOD, codeword[EOD_ascii]);	// EOD codeword ����
	strcat(encoding, EOD);	// encoding ���ڿ� ���� EOD ����

	int size = strlen(encoding);	// encoding �迭�� ��ü ������ �� 
	int stuffing_num = 8 - (size % 8);	// �� �κ��� 8bit�� ����� ���� 0���� ä�� ��Ʈ�� �� 
	for (int i = 0; i < stuffing_num; i++) {
		if (stuffing_num == 8)		// �� 8bit�� ���� �� ä���������� Ż��
			break;
		encoding[size++] = '0';
	}
	encoding[size] = '\0';


	// Huffman_code.hbs ���� ����
	ofstream fout_code("Huffman_code.hbs", ios::binary);
	char buffer[9] = { 0, };		// encoding �迭�κ��� �о�� 8bit ���ڿ��� ����

	int i = 0, j = 0;
	while(encoding[j] != '\0') {
		int index = 0;
		for(j=8*i; j<8*i+8; j++) {	// 8bit�� ����
			buffer[index] = encoding[j];	// 8bit ���ڿ� ����
			index++;
		}
		buffer[8] = '\0';

		int decimal = strtol(buffer, NULL, 2);	// ���ڿ��� 10������ ��ȯ
		char ch = (char)decimal;	// 10������ -127~128�� char������ ��ȯ

		fout_code << ch;	// -127~128�� char���� ���Ͽ� write�ϸ� 16������ �����

		i++;
	}
	fout_code.close();

	
	// ������ ���̺�
	for (int i = 0; i < 128; i++) {
		if (prob[i] == 0)	// �󵵼��� 0�� ���ڴ� �ǳʶ�
			continue;

		// ascii �ڵ� ��ֱ�
		bitset<8> bs1(i);			// 10������ 8bit binary�� ��ȯ
		string str = bs1.to_string();
		const char* ch = str.c_str();	// string to const char*
		strcat(encoding_table, ch);

		// codeword�� bit �� ��ֱ�
		int code_len = strlen(codeword[i]);
		bitset<8> bs2(code_len);	// 10������ binary�� ��ȯ
		str = bs2.to_string();
		ch = str.c_str();	// string to const char*
		strcat(encoding_table, ch);
		
		// codeword ��ֱ�
		strcat(encoding_table, codeword[i]);
	}

	strcat(encoding_table, EOD);	// encoding_table ���� EOD ����

	size = strlen(encoding_table);
	stuffing_num = 8 - (size % 8);	// �� �κ��� 8bit�� ����� ���� 0���� ä�� ��Ʈ�� �� 
	for (int i = 0; i < stuffing_num; i++) {
		if (stuffing_num == 8)		// �� 8bit�� ���� �� ä���������� Ż��
			break;
		encoding_table[size++] = '0';	// byte align
	}
	encoding_table[size] = '\0';


	// Huffman_table.hbs ���� ����
	ofstream fout_table("Huffman_table.hbs", ios::out | ios::binary);
	memset(buffer, 0, sizeof(char)*9);	

	i = 0, j = 0;
	while (encoding_table[j] != '\0') {
		int index = 0;
		for (j = 8 * i; j < 8 * i + 8; j++) {	// 8bit�� ����
			buffer[index] = encoding_table[j];	// 8bit ���ڿ� ����
			index++;
		}
		buffer[index] = '\0';	// �������� �� ���� �߰�

		int decimal = strtol(buffer, NULL, 2);	// ���ڿ��� 10������ ��ȯ
		char ch = (char)decimal;	// 10������ -127~128�� char������ ��ȯ

		fout_table << ch;	// -127~128�� char���� ���Ͽ� write�ϸ� 16������ �����

		i++;
	}
	fout_table.close();



	// �޸� ����
	delete[] encoding;
	delete[] encoding_table;
	delete[] file_data;
	for (int i = 0; i < 128; i++)
		delete[] codeword[i];
	delete[] codeword;


	
	return 0;	
}


int* min_idx(int* arr, int size)	// arr���� 0���� ū �ּҰ��� index�� ��ȯ�ϴ� �Լ�
{
	int min_2_index[2] = { 0, };	// �ּҰ� 2���� index (��ȯ ��)
	int min1, min2;
	int min_index1, min_index2;

	for (int i = 0; i < 2; i++) {
		min1 = arr[0];
		min_index1 = 0;	// �ּҰ��� index
		for (int j = 1; j < size; j++)
		{
			if (min1 == 0) {		// min�� 0�̸� ������ arr[j]�� �Ҵ�
				min1 = arr[j];
				min_index1 = j;
			}
			else {
				if (min1 > arr[j] && arr[j] != 0) {	// arr[j]�� min���� ���� ���
					min1 = arr[j];
					min_index1 = j;
				}
			}
		}

		if (i == 0) {	// ù��°
			min_2_index[0] = min_index1;	// ù��° �ּҰ��� index ����
			min2 = min1;
			min_index2 = min_index1;
			arr[min_index1] = 0;	// ù��° �ּҰ��� 0���� �����Ͽ� �ι�° �ּҰ� ���� ���ܽ�Ŵ
		}
		else {	// �ι�°
			min_2_index[1] = min_index1;	// �ι�° �ּҰ��� index ����
			arr[min_index2] = min2;		// ù��° �ּҰ��� 0���� �����ص� ���� ����
		}
	}

	return min_2_index;
}

void recursive_code(TreeNode* parent, TreeNode* node, char code)	// ������ Ʈ���� �� ��忡 0 �Ǵ� 1�� �Ҵ�
{
	node->code = code;		// 0 �Ǵ� 1�� code �� ���� (root�� ��츸 2)
	node->parent = parent;	// �θ� ��� ����

	if (node->leftChild == 0 && node->rightChild == 0) {	// leaf node
		int idx = node->ascii;
		root_array[idx] = node;
		
		return;

	}
	recursive_code(node, node->leftChild, '0');	// ���� �ڽĳ��� code�� 0
	recursive_code(node, node->rightChild, '1');	// ������ �ڽĳ��� code�� 1
}

void get_codeword(TreeNode* node, char* codeword)
{
	char reverse_codeword[30] = { 0, };	// leaf���� root �������� �����ϹǷ� ����
	int	len;	// codeword�� ����
	for (len = 0; node->code != '2'; len++) { // root�� �����ϸ� ����, root�� code ���� X
		reverse_codeword[len] = node->code;
		node = node->parent;	// �θ� ���� �̵�
	}
	for (int i = 0; i < len; i++)
		codeword[i] = reverse_codeword[len-1 - i];	// root���� leaf ���������� codeword
}
