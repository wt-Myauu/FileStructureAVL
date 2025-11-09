#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <stdint.h>

#include "AVL.h"
#pragma warning(disable:4996) //strcpy_s

//검색키, 오프셋만 스왑
static inline void swap_payload(struct node* a, struct node* b) { 
    uint32_t k = a->key;   a->key = b->key;   b->key = k;
    uint64_t d = a->addr;  a->addr = b->addr; b->addr = d;
}

static int node_height(struct node* n) {
    return n ? n->height : 0;
}

static void update_node(struct node* n) {
    if (!n) return;
    int lh = node_height(n->left);
    int rh = node_height(n->right);
    n->height = (lh > rh ? lh : rh) + 1;
    n->bf = lh - rh; 
}

static struct node* find_root(struct node* any)
{
    if (!any) return NULL;
    while (any->parent) any = any->parent;
    return any;
}

static void avl_update_tree(struct node** root, struct node* start)
{
    struct node* p = start;
    while (p) {
        update_node(p);
        int bf = p->bf;
        if (bf > 1) {
            int l_lh = node_height(p->left ? p->left->left : NULL);
            int l_rh = node_height(p->left ? p->left->right : NULL);
            if (l_lh >= l_rh) {
                avl_rotate_right(p);
            }
            else {
                avl_rotate_leftright(p);
            }
        }
        else if (bf < -1) {

            int r_rh = node_height(p->right ? p->right->right : NULL);
            int r_lh = node_height(p->right ? p->right->left : NULL);
            if (r_rh >= r_lh) {
                avl_rotate_left(p);
            }
            else {
                avl_rotate_rightleft(p);
            }
        }
        p = p->parent;
    }
    if (root) {
        struct node* anchor = (start ? start : *root);
        *root = find_root(anchor);
    }
}
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

int avl_insert_node(struct node** root, struct node* new_node)
{
    if (*root == NULL) {
        *root = new_node;
        return 0;
    }

    struct node* cur = *root;
    struct node* parent = NULL;
    while (cur) {
        parent = cur;
        if (new_node->key == cur->key) {
            return -1;  //만약 중복키 인경우 -1
        }
        else if (new_node->key < cur->key) {
            cur = cur->left;
        }
        else {
            cur = cur->right;
        }
    }
    //부모의 왼쪽
    if (new_node->key < parent->key) {
        parent->left = new_node;
        new_node->parent = parent;
    }
    //부모의 오른쪽
    else {
        parent->right = new_node;
        new_node->parent = parent;
    }
    //트리를 업데이트
    avl_update_tree(root, parent);
    *root = find_root(new_node);
    return 0;
}
//우회전
void avl_rotate_right(struct node* x)
{
    if (!x || !x->left) return;

    struct node* y = x->left;
    struct node* A = y->left;
    struct node* B = y->right;
    struct node* C = x->right;

    swap_payload(x, y);

    x->left = A;      
    if (A) A->parent = x;
    x->right = y;     
    y->parent = x;
    y->left = B;     
    if (B) B->parent = y;
    y->right = C;    
    if (C) C->parent = y;

    update_node(y);
    update_node(x);
}
//좌회전
void avl_rotate_left(struct node* x)
{
    if (!x || !x->right) return;

    struct node* y = x->right;
    struct node* A = x->left;
    struct node* B = y->left;
    struct node* C = y->right;

    swap_payload(x, y);

    x->left = y;      
    y->parent = x;
    x->right = C;       
    if (C) C->parent = x;
    y->left = A;      
    if (A) A->parent = y;
    y->right = B;     
    if (B) B->parent = y;

    update_node(y);
    update_node(x);
}

 void avl_rotate_leftright(struct node* x)
    {
        if (!x || !x->left) return;
        avl_rotate_left(x->left);
        avl_rotate_right(x);
    }

 void avl_rotate_rightleft(struct node* x)
    {
        if (!x || !x->right) return;
        avl_rotate_right(x->right);
        avl_rotate_left(x);
    }

typedef struct record{
	char id[8];
	char name[10];
	char gender[6];
	char age[3];
	char n;
} RECORD;
//4 6 10 8 = 28

struct node* avl_create_tree(const char* filepath)
{
    int i = 1;
    FILE* data = fopen(filepath, "rb");
    if (!data) {
        perror("파일 열기 실패!");
        return NULL;
    }

    struct node* root = NULL;
    char buf[64];  // 한 레코드(27) + 
       while (1) {
        long start_offset = ftell(data);      // 이 라인의 시작 바이트 오프셋
        if (!fgets(buf, sizeof buf, data))    // 한 줄 읽기(개행 포함)
            break;

        size_t len = strlen(buf);
        if (len < 27) {
            //비정상 라인은 스킵
            continue;
        }

        // 원본 바이트를 그대로 채우고
        RECORD current_rec;
        memcpy(current_rec.id, buf + 0, 8);
        memcpy(current_rec.name, buf + 8, 10);
        memcpy(current_rec.gender, buf + 18, 6);
        memcpy(current_rec.age, buf + 24, 3);

        // 널 종료를 보장 후 카피
        char idbuf[9], namebuf[11], genderbuf[7], agebuf[4];
        memcpy(idbuf, buf + 0, 8); idbuf[8] = '\0';
        memcpy(namebuf, buf + 8, 10); namebuf[10] = '\0';
        memcpy(genderbuf, buf + 18, 6); genderbuf[6] = '\0';
        memcpy(agebuf, buf + 24, 3); agebuf[3] = '\0';

        unsigned key = (unsigned)strtoul(idbuf, NULL, 10);
        int current_offset = (int)start_offset; // 이 레코드의 파일 내 시작 위치

        struct node* new_node = avl_create_node(key, current_offset);
        if (!new_node) {
            perror("노드 생성 실패!");
            fclose(data);
            return NULL; //파일 읽기에 문제가 있는경우 NULL
        }
        if (avl_insert_node(&root, new_node) != 0) {
            free(new_node);
            printf("중복 키 %u 가 존재하므로 삽입 거부, 메모리가 해제되었습니다.\n", key);
        }
        else {
            printf("%d %u 삽입 완료!\n",i, key); 
           
        }
        i++;
    }
    fclose(data);
    return root; // 성공시 루트 노드 반환
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