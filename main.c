// 5번째 레코드 age 정확 추출 (BOM 제거 + 27B 고정폭)
#include <stdio.h>
#include <string.h>

static void skip_utf8_bom(FILE* fp) {
    int b1 = fgetc(fp);
    int b2 = fgetc(fp);
    int b3 = fgetc(fp);
    if (!(b1 == 0xEF && b2 == 0xBB && b3 == 0xBF)) {
        if (b3 != EOF) ungetc(b3, fp);
        if (b2 != EOF) ungetc(b2, fp);
        if (b1 != EOF) ungetc(b1, fp);
    }
}

static int read_fixed_record(FILE* fp, char rec[27]) {
    size_t n = fread(rec, 1, 27, fp);
    if (n < 27) return 0;
    // 개행 소비: \r\n, \n, \r 허용
    int c = fgetc(fp);
    if (c == '\r') {
        int c2 = fgetc(fp);
        if (c2 != '\n' && c2 != EOF) ungetc(c2, fp);
    }
    else if (c != '\n' && c != EOF) {
        ungetc(c, fp);
    }
    return 1;
}

int main(void) {
    const char* path = "fixed_length_records";
    FILE* fp = fopen(path, "rb");
    if (!fp) { perror("open"); return 1; }

    skip_utf8_bom(fp);            // ★ BOM이 있으면 건너뜀

    char rec[27];
    int idx = 0;
    while (read_fixed_record(fp, rec)) {
        ++idx;
        if (idx == 10) {
            char age[4];
            memcpy(age, rec + 24, 3); age[3] = '\0';
            printf("5번째 레코드 age = %s\n", age);
            break;
        }
    }
    fclose(fp);
    return 0;
}
