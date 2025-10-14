#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "AVL.h"
#pragma warning(disable:4996) //strcpy_s
struct node* avl_create_node(int key, int addr)
{
	struct node *newNode = malloc(sizeof(struct node)); //노드를 할당
	if(newNode == NULL) return NULL; //malloc 실패시(NULL 반환시 NULL 리턴, 노드 생성 실패 의미)

	newNode -> left = NULL; //구조체 멤버 초기화
	newNode -> right = NULL;
	newNode -> parent = NULL;

	newNode -> key = (uint32_t)key; //인덱스 검색 키
	newNode -> addr = (uint64_t)addr; //레코드 바이트 오프셋(레코드 끝의 데이터 위치)
	//타입 안정성을 위해 캐스팅하여 구조체에 삽입.

	newNode -> bf = 0; //왼쪽과 오른쪽의 균형이 같아 balancing factor의 초기값은 0
	newNode -> height = 1; 

	return newNode; //node 구조체가 할당된 주소 반환
}

/*
  &(*root) 루트 포인터의 포인터
  root → 루트 포인터를 가리키는 포인터 (struct node**)
  *root → 루트 노드 포인터 (struct node*)
  (*root)->left → 루트 노드의 왼쪽 자식 (struct node*)
  &(*root)->left → 루트 노드의 왼쪽 자식 포인터를 가리키는 포인터 (struct node**)
*/

int avl_insert_node(struct node** root, struct node* new_node)
{
	if (*root == NULL) { //루트 노드가 NULL 이면 받아온 노드가 바로 루트노드
        *root = new_node;    
        return 0;
    }
	if(new_node->key == (*root)->key) return -1;
	if(new_node->key < (*root)->key){ //루트보다 작으면
		if((*root)->left == NULL) { // 왼쪽 자식이 없으면
			(*root)->left = new_node; //현재 노드가 왼쪽 노드이고
			new_node->parent = *root; //부모 노드는 root
		} else {
			avl_insert_node(&(*root)->left, new_node); //루트의 왼쪽 주소랑 현재 노드를 가지고 다시 함수 호출
		}

	}
	else { //반대의 경우 (루트보다 크면)
		if ((*root)->right == NULL) {
            (*root)->right = new_node;
            new_node->parent = *root;
        } else {
            avl_insert_node(&(*root)->right, new_node);
        }
	}
	return 0;
}

void avl_rotate_right(struct node* x)
{
	// TODO: 구현 
}

void avl_rotate_left(struct node* x)
{
	// TODO: 구현 
}

void avl_rotate_leftright(struct node* x)
{
	// TODO: 구현 
}

void avl_rotate_rightleft(struct node* x)
{
	// TODO: 구현 
}

typedef struct record{
	char id[8];
	char name[10];
	char gender[6];
	char age[3];
} RECORD;
//4 6 10 8 = 28

struct node* avl_create_tree(const char* filepath)
{	
	FILE *data;
	RECORD current_rec;
	struct node *root = NULL; 

	int record_byte_offset = 0; //파일 내 레코드 시작 위치
	
	data = fopen(filepath, "rb");  
	if(data == NULL) { 
		perror("파일 열기 실패!");
		return NULL;
	}

	while(fread(&current_rec, sizeof(RECORD), 1, data) == 1){
		int current_key = strtoul(current_rec.id, NULL, 10); //문자열을 정수형으로 변환
		int current_offset = record_byte_offset;

		struct node *new_node = avl_create_node(current_key,current_offset); //현재 키랑 현재 오프셋을 받아 노드 생성

		if(new_node == NULL){ //malloc 실패시
		perror("노드 생성 실패!");
		fclose(data);
		return NULL;
		}

		if(avl_insert_node(&root, new_node) != 0) { //중복 키의 경우
			free(new_node);
			printf("중복 키 %d 가 존재하므로 삽입 거부, 메모리가 해제되었습니다.\n", current_key);
		}

		else printf("%d 삽입 완료!" ,current_key);

		record_byte_offset += sizeof(RECORD); //바이트 오프셋 갱신
	}
	fclose(data);
	return root; //성공시 루트노드 반환
}

/*주어진 소스코드의 헤더파일에 선언된 함수 원형에 맞춰 avl.c에 AVL 관련 동작을 구현한다. 과
제 평가 시점에서 main.c와 avl.h 코드는 평가를 위한 소스코드로 대치된다. 즉, 채점을 위해
main.c와 avl.h 가 초기화 되므로, 여러분은 AVL 동작을 위한 소스코드는 avl.c 파일 내에 작성해
야 한다. 
AVL 트리가 잘 동작하는지 확인하기 위한 다양한 코드는 main.c와 avl.h에 작성이 가능
하다(채점시에는 초기화 됨). 
채점시 main.c 소스코드가 대치되어 avl.h의 함수 원형들을 호출하면서 다양한 테스트가 진행될 것이다. 
따라서 작성된 avl.c의 소스코드가 오류 없이 동작하는지 여부를 면밀히 파악해야 할 것이다. 
또한, 여러분은 이러한 고정길이 레코드 파일을 읽어 레코드의 ID를 검색 키로 하는 AVL 트리
를 구축하는 함수를 구현해야 한다.
AVL 트리의 노드 구조체는 키 값을 저장하는 key 멤버 변수
와, 각 레코드의 바이트 오프셋을 저장하는 addr 멤버 변수를 가진다. 따라서 각 레코드에 대한
인덱스 정보를 AVL 트리에 구축할 수 있어야 한다.
테스트 코드는 avl.h에 정의된 함수 원형을 호출하면서 AVL 트리를 생성하고, 루트 노드부터 노
드를 차례로 순회하며 각 레코드에 해당하는 노드가 존재하는지, 여러분의 코드에서 생성된 트리
가 AVL 성질을 만족하는지 여부를 평가할 것이다.*/