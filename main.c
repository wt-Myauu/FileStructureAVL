#include <stdio.h>
#include "AVL.h"

#define NODE_WIDTH 6   // 숫자 자리수 고정

// 트리 구조를 재귀적으로 출력
void print_tree(struct node* root, int depth, char prefix) {
    if (!root) return;

    // 오른쪽 먼저 출력
    print_tree(root->right, depth + 1, '/');

    // 현재 노드 출력
    for (int i = 0; i < depth; i++)
        printf("      "); // 들여쓰기
    if (depth > 0)
        printf("%c-----", prefix); // 가지 표시
    printf("%*u\n", NODE_WIDTH, root->key);

    // 왼쪽 출력
    print_tree(root->left, depth + 1, '\\');
}

int main() {
    struct node* root = avl_create_tree("fixed_length_records.txt");
    if (!root) {
        printf("트리 생성 실패!\n");
        return 1;
    }

    printf("\n=== 트리 구조 그림 ===\n");
    print_tree(root, 0, ' '); // 루트에는 가지 없음

    return 0;
}
