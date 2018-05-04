#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>
#include <time.h>


char *text;
long text_len = 0;


int file_exists(const char *filename) {
    struct stat buffer;
    int exist = stat(filename, &buffer);
    if (exist == 0)
        return 1;
    else
        return 0;
}


char *read_file(const char *filename) {
    char *buffer = NULL;
    long read_size;
    FILE *handler = fopen(filename, "r");
    if (handler) {
        fseek(handler, 0, SEEK_END);
        text_len = ftell(handler);
        rewind(handler);

        // allocate a string
        buffer = (char *) malloc(sizeof(char) * (text_len + 1));
        if (buffer == NULL) {
            printf("Out of memory");
            return NULL;
        }
        // read it all in one
        read_size = fread(buffer, sizeof(char), (size_t) text_len, handler);
        buffer[text_len] = '\0';
        if (text_len != read_size) {
            // Something went wrong
            free(buffer);
            buffer = NULL;
        }
        fclose(handler);
    }
    return buffer;
}


int bf_search(char *pattern, int i) {
    // Brute-Force
    int j;
    int index = -1;
    size_t pattern_len = strlen(pattern);
    clock_t startTime = clock();
    long end = text_len - pattern_len;
    while (i < end) {
        j = 0;
        while ((j < pattern_len) && (text[i + j] == pattern[j])) {
            j++;
        }
        if (j == pattern_len) {
            index = i;
            break;
        }
        i++;
    }

    if (index < 0) {
        printf("Pattern not found.");
    }
    clock_t endTime = clock();
    double duration = (endTime - startTime);
    printf("BF  Search found '%s' at position: %d  elapsed time: %f [ms]\n", pattern, index, duration);
    return index;
}


void get_fail_table(char *pattern, int *fail) {
    size_t pattern_len = strlen(pattern);
    int i = 1;
    int j = 0;
    fail[0] = 0;
    while (i < pattern_len) {
        if (pattern[i] == pattern[j]) {
            fail[i] = j + 1;
            i++;
            j++;
        } else if (j > 0) {
            j = fail[j - 1];
        } else {
            fail[i] = 0;
            i++;
        }
    }
}


int kmp_search(char *pattern, int i) {
    // Knuth–Morris–Pratt
    int j = 0;
    int index = -1;
    size_t pattern_len = strlen(pattern);
    int *fail;
    fail = (int *) malloc(sizeof(int) * pattern_len);
    get_fail_table(pattern, fail);

    clock_t startTime = clock();
    while (i < text_len) {
        while (j < pattern_len && text[i] == pattern[j]) {
            j++;
            i++;
        }
        if (j == pattern_len) {
            index = i - (int) pattern_len;
            break;
        }

        if (j > 0) {
            j = fail[j - 1];
        } else {
            i++;
        }
    }

    if (index < 0) {
        printf("Pattern not found.");
    }

    clock_t endTime = clock();
    double duration = (endTime - startTime);
    printf("KMP Search found '%s' at position: %d  elapsed time: %f [ms]\n", pattern, index, duration);
    free(fail);
    return index;
}


int max(int a, int b) { return (a > b) ? a : b; }


void get_last(char *str, int size, int badchar[256]) {
    int i;
    for (i = 0; i < 256; i++)
        badchar[i] = -1;

    for (i = 0; i < size; i++)
        badchar[(int) str[i]] = i;
}


int bm_search(char *pattern, int i) {
    int index = -1;
    int j = 0;
    int pattern_len = (int) strlen(pattern);
    long end = text_len - pattern_len;
    int badchar[256];
    get_last(pattern, pattern_len, badchar);

    clock_t startTime = clock();
    int s = 0;
    while (s <= end) {
        j = pattern_len - 1;
        while (j >= 0 && pattern[j] == text[s + j]) {
            j--;
        }
        if (j < 0) {
            index = s;
            break;
        } else {
            s += max(1, j - badchar[text[s + j]]);
        }
    }
    if (index < 0) {
        printf("Pattern not found.");
    }

    clock_t endTime = clock();
    double duration = (endTime - startTime);
    printf("BM  Search found '%s' at position: %d  elapsed time: %f [ms]\n", pattern, index, duration);
    return index;
}


int main() {

    char resolved_path[PATH_MAX];
    char *filename = "../big.txt";
    realpath(filename, resolved_path);

    int exist = file_exists(resolved_path);
    if (exist) {
        printf("File %s exist", resolved_path);
        text = read_file(resolved_path);
        printf("'\n");

        kmp_search("tradeMark1", 0);
        bf_search("tradeMark1", 0);
        kmp_search("tradeMark1", 0);
        bm_search("tradeMark1", 0);
        bf_search("tradeMark1", 0);
        bm_search("tradeMark1", 0);

        free(text);
    } else {
        printf("File %s does not exist", resolved_path);
    }

    return 0;
}