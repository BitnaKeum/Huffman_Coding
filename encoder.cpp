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
	TreeNode *leftChild, *rightChild;	// 왼쪽 자식 노드, 오른쪽 자식 노드
	int root_idx;	// 자신을 가리키는 root_array의 index
	char code;		// 0 또는 1의 codeword
	TreeNode* parent;	// 부모 노드

	// 생성자
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

TreeNode* root_array[128] = { 0, };		// 여러 임시 root를 저장하는 배열

int main()
{
	int prob[128] = { 0, };				// 아스키코드 0~127의 빈도(확률)를 세는 배열 
	int prob_copy[128] = { 0, };		// prob 사본 배열
	int prob_root[128] = { 0, };		// root_array의 임시 root들이 가리키고 있는 TreeNode의 prob을 저장
	int* min_idx_prob = NULL;			// prob_copy 배열에서 최소값 두개의 index 
	int* min_idx_root = NULL;			// root_array 배열에서 최소값 두개의 index
	int* min_idx_prob_root = NULL;		// min_prob_root에서 최소값 두개의 index
	int min_prob_root[4] = { 0, };		// prob_copy 최소값 두개와 prob_root 최소값 두개를 차례로 저장
	int idx0, idx1, idx2, idx3, idx4, idx5;
	char** codeword = new char*[128];	// 허프만 코딩으로 얻은 codeword
	for (int i = 0; i < 128; i++) {
		codeword[i] = new char[30];		// 이차원 배열 동적 할당
		memset(codeword[i], 0, sizeof(char) * 30);	// 0으로 초기화
	}
	char* encoding = new char[100000];	// 허프만 인코딩한 문자열을 담는 배열
	memset(encoding, 0, sizeof(char) * 100000);	// 0으로 초기화
	char* encoding_table = new char[100000];		// 허프만 테이블을 담는 배열
	memset(encoding_table, 0, sizeof(char) * 100000);	// 0으로 초기화

	int EOD_ascii = 36;		// 임의로 정한 EOD의 아스키 값('$')
	char EOD[20] = { 0, };	// EOD의 codeword
	
	char* file_data = new char[100000];	// 파일의 데이터를 저장할 배열, 메모리 할당 후 0으로 초기화
	memset(file_data, 0, sizeof(char) * 100000);


	ifstream fin("input.txt");	// "input.txt" 파일을 열어 읽어오기
	if (!fin.is_open())	// 파일이 열리지 않을 경우 프로그램 종료
	{
		printf("File can't be opened!\n");
		return -1;
	}
	int len;	// file_data에 저장되는 데이터의 수
	for (len = 0; !fin.eof(); len++) {
		fin.get(file_data[len]);	// fin에서 한 글자씩 읽어와  file_data에 저장
	}
	file_data[--len] = 0;	// EOF 앞 이상한 데이터 지움
	fin.close();


	for (int i = 0; i<len; i++) {
		int ascii = (int)file_data[i];	// 각 문자의 아스키 코드 값
		prob[ascii]++;	// 아스키 코드 값에 해당하는 index에서의 값을 1 증가시켜 카운팅
	}
	prob[EOD_ascii]++;	// EOD를 한번 넣어줌 (EOD: '$')
	// prob 출력
	/*for (int j = 0; j < 128; j++) {
		if (prob[j] == 0) {	// 나오지 않은 문자면 건너뜀
			continue;
		}
		printf("'%c' : %d\n", j, prob[j]);
	}
	*/
	for (int i = 0; i < 128; i++) {
		prob_copy[i] = prob[i];	// prob의 값을 복사한 prob_copy
	}


	// Tree가 비어있는 초기 상태
	min_idx_prob = min_idx(prob_copy, 128);	// prob_copy에서 최소값 두개의 index 저장
	idx0 = min_idx_prob[0];
	idx1 = min_idx_prob[1];
	root_array[0] = new TreeNode(idx0, prob_copy[idx0], NULL, NULL, 0, '0', 0);	// prob: 최소값1, root0가 가리킴
	root_array[1] = new TreeNode(idx1, prob_copy[idx1], NULL, NULL, 1, '0', 0);	// prob: 최소값2, root1이 가리킴
	prob_copy[idx0] = prob_copy[idx1] = 0;		// TreeNode가 만들어졌으므로 해당 값 없앰

	root_array[0] = new TreeNode(0, root_array[0]->prob + root_array[1]->prob, root_array[0], root_array[1], 0, '0', 0);	// prob: 최소값1+최소값2, root0가 가리킴
	root_array[1] = 0;
	root_array[0]->leftChild->root_idx = -1;	// 자신을 가리키던 root가 다른걸 가리키므로 root_idx = -1로 설정
	root_array[0]->rightChild->root_idx = -1;
	root_array[0]->leftChild->parent = root_array[0]->rightChild->parent = root_array[0];	// 새로운 노드를 두 노드의 parent로 설정


	

	while(1) {
		// prob_copy에서 최소값 두개의 index 저장
		min_idx_prob = min_idx(prob_copy, 128);
		idx0 = min_idx_prob[0];
		idx1 = min_idx_prob[1];
		min_prob_root[0] = prob_copy[idx0];
		min_prob_root[1] = prob_copy[idx1];
		//printf("[%d]: %d [%d]: %d\n", idx0, prob_copy[idx0], idx1, prob_copy[idx1]);

		// prob_root에서 최소값 두개의 index 저장
		for (int i = 0; i < 128; i++) {	  // 임시 root들의 prob만을 저장
			prob_root[i] = 0;	// 0으로 초기화
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

		// min_prob_root에서 최소값 두개의 index 반환
		min_idx_prob_root = min_idx(min_prob_root, 4);
		idx4 = min_idx_prob_root[0];
		idx5 = min_idx_prob_root[1];
		if (min_prob_root[idx5] == 0) {// prob_copy의 모든 값이 비어있고 최종 root 값 하나만 남은 경우 반복문 탈출
			//printf("[%d]: %d\n", idx2, root_array[idx2]->prob);
			break;
		}
		//printf("[%d]: %d [%d]: %d\n", min_idx_prob_root[0], min_prob_root[idx4], min_idx_prob_root[1], min_prob_root[idx5]);


		int root_index[2] = { 0, };	// 덧셈할 두 노드를 가리키는 root_array의 index
		for (int i = 0; i < 2; i++) {
			int idx = (i == 0) ? idx4 : idx5;
	
			if (idx == 0 || idx == 1) {	// prob_copy의 값인 경우
				for (int j = 0; j < 128; j++) {
					if (root_array[j] == 0) // 비어있는 root의 index를 찾음
					{
						root_index[i] = j;		 // 새로 생성할 노드를 가리킬 root_array의 index
						
						int ascii = (idx == 0) ? idx0 : idx1;
						root_array[j] = new TreeNode(ascii, min_prob_root[idx], NULL, NULL, j, '0', 0);	// prob_copy의 값을 담은 TreeNode 생성
						//printf("root[%d] -> prob:%d!!\n", j, min_prob_root[idx]);
						//int idx1 = min_idx_prob[idx];	// prob_copy에서의 해당 값의 index를 저장
						idx = (idx == 0) ? idx0 : idx1;
						prob_copy[idx] = 0;			// TreeNode가 만들어졌으므로 해당 값 없앰
						break;
					}
				}
			}
			else {		// prob_root의 값인 경우
				root_index[i] = (idx == 2) ? idx2 : idx3;	// prob_root에서의 index = root_array에서의 index
				//int r = root_index[i];
				//printf("root[%d] -> prob:%d\n", r, root_array[r]->prob);
			}
		}

		// 두 노드를 더한 부모 노드 생성
		int root_index0 = root_index[0];
		int root_index1 = root_index[1];
		int root_idx = (root_index0 < root_index1) ? root_index0 : root_index1;	// 더 작은 root_index 값을 얻음, 상위 노드를 가리키기 위함
		root_array[root_idx] = new TreeNode(0, root_array[root_index0]->prob + root_array[root_index1]->prob, root_array[root_index0], root_array[root_index1], root_idx, '0', 0);	// 두 노드의 상위 노드 생성
		//printf("root[%d] -> prob:%d\n", root_idx, root_array[root_idx]->prob);
		root_array[root_idx]->leftChild->root_idx = -1;	// 자식노드를 가리키던 root가 부모노드를 가리키므로 root_idx = -1로 설정
		root_array[root_idx]->rightChild->root_idx = -1;
		root_array[root_idx]->leftChild->parent = root_array[root_idx]->rightChild->parent = root_array[root_idx];	// 새로운 노드를 두 노드의 parent로 설정
		root_idx = (root_index0 > root_index1) ? root_index0 : root_index1;	// 더 큰 root_index 값을 얻음, 더 이상 가리키지 않으므로 NULL로 할당하기 위함
		root_array[root_idx] = 0;

	}


	recursive_code(0, root_array[0], '2');	// root 노드를 넘겨줌, root의 code는 사용하지 않으므로 2로 설정

	for (int i = 1; i < 128; i++)
	{
		if (root_array[i] == 0)
			continue;
		get_codeword(root_array[i], codeword[i]);	// 해당 아스키 값을 갖는 Symbol의 codeword를 구함
	}
	//허프만 테이블 출력
	/*for (int j = 1; j < 128; j++) {
		if (*codeword[j] == 0)
			continue;
		printf("'%c'\t\t%d\t\t%s\n", j, prob[j], codeword[j]);	// Symbol, probability, codeword
	}
	*/


	// encoding
	for (int i = 0; i < len; i++) {
		int ascii = (int)file_data[i];
		strcat(encoding, codeword[ascii]);	// 해당 문자의 codeword를 붙여쓰기
	}

	strcpy(EOD, codeword[EOD_ascii]);	// EOD codeword 저장
	strcat(encoding, EOD);	// encoding 문자열 끝에 EOD 삽입

	int size = strlen(encoding);	// encoding 배열의 전체 데이터 수 
	int stuffing_num = 8 - (size % 8);	// 끝 부분을 8bit로 만들기 위해 0으로 채울 비트의 수 
	for (int i = 0; i < stuffing_num; i++) {
		if (stuffing_num == 8)		// 끝 8bit가 원래 다 채워져있으면 탈출
			break;
		encoding[size++] = '0';
	}
	encoding[size] = '\0';


	// Huffman_code.hbs 파일 생성
	ofstream fout_code("Huffman_code.hbs", ios::binary);
	char buffer[9] = { 0, };		// encoding 배열로부터 읽어온 8bit 문자열을 저장

	int i = 0, j = 0;
	while(encoding[j] != '\0') {
		int index = 0;
		for(j=8*i; j<8*i+8; j++) {	// 8bit씩 읽음
			buffer[index] = encoding[j];	// 8bit 문자열 저장
			index++;
		}
		buffer[8] = '\0';

		int decimal = strtol(buffer, NULL, 2);	// 문자열을 10진수로 변환
		char ch = (char)decimal;	// 10진수를 -127~128의 char형으로 변환

		fout_code << ch;	// -127~128의 char형을 파일에 write하면 16진수로 저장됨

		i++;
	}
	fout_code.close();

	
	// 허프만 테이블
	for (int i = 0; i < 128; i++) {
		if (prob[i] == 0)	// 빈도수가 0인 문자는 건너뜀
			continue;

		// ascii 코드 써넣기
		bitset<8> bs1(i);			// 10진수를 8bit binary로 변환
		string str = bs1.to_string();
		const char* ch = str.c_str();	// string to const char*
		strcat(encoding_table, ch);

		// codeword의 bit 수 써넣기
		int code_len = strlen(codeword[i]);
		bitset<8> bs2(code_len);	// 10진수를 binary로 변환
		str = bs2.to_string();
		ch = str.c_str();	// string to const char*
		strcat(encoding_table, ch);
		
		// codeword 써넣기
		strcat(encoding_table, codeword[i]);
	}

	strcat(encoding_table, EOD);	// encoding_table 끝에 EOD 삽입

	size = strlen(encoding_table);
	stuffing_num = 8 - (size % 8);	// 끝 부분을 8bit로 만들기 위해 0으로 채울 비트의 수 
	for (int i = 0; i < stuffing_num; i++) {
		if (stuffing_num == 8)		// 끝 8bit가 원래 다 채워져있으면 탈출
			break;
		encoding_table[size++] = '0';	// byte align
	}
	encoding_table[size] = '\0';


	// Huffman_table.hbs 파일 생성
	ofstream fout_table("Huffman_table.hbs", ios::out | ios::binary);
	memset(buffer, 0, sizeof(char)*9);	

	i = 0, j = 0;
	while (encoding_table[j] != '\0') {
		int index = 0;
		for (j = 8 * i; j < 8 * i + 8; j++) {	// 8bit씩 읽음
			buffer[index] = encoding_table[j];	// 8bit 문자열 저장
			index++;
		}
		buffer[index] = '\0';	// 마지막에 널 문자 추가

		int decimal = strtol(buffer, NULL, 2);	// 문자열을 10진수로 변환
		char ch = (char)decimal;	// 10진수를 -127~128의 char형으로 변환

		fout_table << ch;	// -127~128의 char형을 파일에 write하면 16진수로 저장됨

		i++;
	}
	fout_table.close();



	// 메모리 해제
	delete[] encoding;
	delete[] encoding_table;
	delete[] file_data;
	for (int i = 0; i < 128; i++)
		delete[] codeword[i];
	delete[] codeword;


	
	return 0;	
}


int* min_idx(int* arr, int size)	// arr에서 0보다 큰 최소값의 index를 반환하는 함수
{
	int min_2_index[2] = { 0, };	// 최소값 2개의 index (반환 값)
	int min1, min2;
	int min_index1, min_index2;

	for (int i = 0; i < 2; i++) {
		min1 = arr[0];
		min_index1 = 0;	// 최소값의 index
		for (int j = 1; j < size; j++)
		{
			if (min1 == 0) {		// min이 0이면 무조건 arr[j]로 할당
				min1 = arr[j];
				min_index1 = j;
			}
			else {
				if (min1 > arr[j] && arr[j] != 0) {	// arr[j]가 min보다 작은 경우
					min1 = arr[j];
					min_index1 = j;
				}
			}
		}

		if (i == 0) {	// 첫번째
			min_2_index[0] = min_index1;	// 첫번째 최소값의 index 저장
			min2 = min1;
			min_index2 = min_index1;
			arr[min_index1] = 0;	// 첫번째 최소값을 0으로 설정하여 두번째 최소값 계산시 제외시킴
		}
		else {	// 두번째
			min_2_index[1] = min_index1;	// 두번째 최소값의 index 저장
			arr[min_index2] = min2;		// 첫번째 최소값을 0으로 설정해둔 것을 복구
		}
	}

	return min_2_index;
}

void recursive_code(TreeNode* parent, TreeNode* node, char code)	// 허프만 트리의 각 노드에 0 또는 1을 할당
{
	node->code = code;		// 0 또는 1의 code 값 설정 (root인 경우만 2)
	node->parent = parent;	// 부모 노드 저장

	if (node->leftChild == 0 && node->rightChild == 0) {	// leaf node
		int idx = node->ascii;
		root_array[idx] = node;
		
		return;

	}
	recursive_code(node, node->leftChild, '0');	// 왼쪽 자식노드는 code가 0
	recursive_code(node, node->rightChild, '1');	// 오른쪽 자식노드는 code가 1
}

void get_codeword(TreeNode* node, char* codeword)
{
	char reverse_codeword[30] = { 0, };	// leaf에서 root 방향으로 저장하므로 역순
	int	len;	// codeword의 길이
	for (len = 0; node->code != '2'; len++) { // root에 도달하면 종료, root의 code 저장 X
		reverse_codeword[len] = node->code;
		node = node->parent;	// 부모 노드로 이동
	}
	for (int i = 0; i < len; i++)
		codeword[i] = reverse_codeword[len-1 - i];	// root에서 leaf 방향으로의 codeword
}
