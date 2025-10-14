#ifndef __avl_H__
#define __avl_H__

#include <stdint.h>
//* 헤더파일에는 무슨 함수가 쓰이는지 정보만 (인터페이스 느낌?)

struct node 
{
	// 왼쪽 자식 노드
	struct node* left;
	// 오른쪽 자식 노드 
	struct node* right;
	// 부모 노드
	struct node* parent;
	// 인덱스 검색 키 
	uint32_t key;
	// 레코드 바이트 오프셋
	uint64_t addr;
	// balance factor
	int bf; 
	// 노드의 높이 (필요한 경우 사용) 
	int height;
};

////////////////////////////////////////////////////////////////////////
//           아래 AVL 함수들은 반드시 구현되어야 합니다 
////////////////////////////////////////////////////////////////////////
/*
 * node를 할당
 * 구조체 멤버 초기화 
 * 키값은 key로 설정 
 * 필요에 따라 bf와 height 적절히 초기화 
 * 성공시 node 구조체가 할당된 주소 반환 
 * 실패시 NULL 반환 
 */
struct node* avl_create_node(int key, int addr);



/*
 * 파라미터로 주어진 node를 AVL 트리의 root 노드에 삽입
 * 성공시 0 반환 
 * 실패시 (키가 있는 경우) 0 이외의 값 반환
 */
int avl_insert_node(struct node** root, struct node* new_node);


/*
 * 파라미터로 주어진 node를 R 회전 
 */
void avl_rotate_right(struct node* x);

/*
 * 파라미터로 주어진 node를 L 회전
 */
void avl_rotate_left(struct node* x);

/*
 * 파라미터로 주어진 node를 LR 회전
 */
void avl_rotate_leftright(struct node* x);

/*
 * 파라미터로 주어진 node를 RL 회전
 */
void avl_rotate_rightleft(struct node* x);
/*
 * 파라미터로 주어진 고정길이 레코드 파일을 읽어 인덱스 된 AVL 트리 반환 
 * 성공시 AVL 트리의 루트 노드 반환 
 * 실패시 (파일 읽기에 문제가 있는 경우) NULL 반환 
 */
struct node* avl_create_tree(const char* filepath);

#endif 

