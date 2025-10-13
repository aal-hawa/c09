#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static void trim_inplace(char *s)
{
    // trim leading
    char *start = s;
    while (*start && isspace((unsigned char)*start)) start++;
    if (start != s) memmove(s, start, strlen(start) + 1);

    // trim trailing
    size_t len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) { s[len - 1] = '\0'; len--; }
}

static char *read_file_whole(const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f) return NULL;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return NULL; }
    long size = ftell(f);
    if (size < 0) { fclose(f); return NULL; }
    rewind(f);
    char *buf = (char*)malloc((size + 1) + 1); // +1 for safety
    if (!buf) { fclose(f); return NULL; }
    size_t r = fread(buf, 1, size, f);
    buf[r] = '\0';
    fclose(f);
    return buf;
}

int main(void)
{
    int rc;

    // 1) Generate input: 3000 random ints 1..100000 written as space-separated to input.txt
    //    You can change the shuf parameters if you want different sizes.
    rc = system("shuf -i 1-100000 -n 5 | tr '\\n' ' ' > input.txt");
    if (rc != 0) {
        fprintf(stderr, "Error: failed to generate input with shuf (exit %d)\n", rc);
        return 2;
    }

    // 2) Run ./PmergeMe with the numbers from input.txt and save stdout to out.txt
    //    Use shell to expand the arguments via `cat input.txt` substitution.
    rc = system("./PmergeMe `cat input.txt` > out.txt 2> pmerge_stderr.txt");
    if (rc != 0) {
        fprintf(stderr, "Warning: ./PmergeMe exited with status %d (check pmerge_stderr.txt)\n", rc);
        // continue: maybe program printed output anyway or printed error lines.
    }

    // 3) Build canonical correct sorted result using sort
    rc = system("cat input.txt | tr ' ' '\\n' | grep -v '^$' | sort -n | tr '\\n' ' ' > correct.txt");
    if (rc != 0) {
        fprintf(stderr, "Error: failed to produce correct.txt (sort step) (exit %d)\n", rc);
        return 3;
    }

    // 4) Read out.txt and extract the 'After:' line
    char *out_text = read_file_whole("out.txt");
    if (!out_text) {
        fprintf(stderr, "Error: cannot open out.txt (did PmergeMe run?)\n");
        return 4;
    }

    char *after_line = NULL;
    // find line that begins with "After"
    char *p = out_text;
    while (*p) {
        char *line_start = p;
        char *newline = strchr(p, '\n');
        if (!newline) newline = p + strlen(p);
        size_t len = (size_t)(newline - line_start);
        if (len >= 5 && strncmp(line_start, "After", 5) == 0) {
            // copy this line
            after_line = (char*)malloc(len + 1);
            if (!after_line) { free(out_text); return 5; }
            strncpy(after_line, line_start, len);
            after_line[len] = '\0';
            break;
        }
        if (*newline == '\0') break;
        p = newline + 1;
    }
    if (!after_line) {
        fprintf(stderr, "Error: no line starting with 'After' found in out.txt\n");
        free(out_text);
        return 6;
    }

    // After line may be like: "After: 1 2 3 4 5"
    // remove the "After:" prefix
    char *after_vals = strchr(after_line, ':');
    if (!after_vals) {
        fprintf(stderr, "Error: malformed 'After' line: %s\n", after_line);
        free(after_line); free(out_text);
        return 7;
    }
    after_vals++; // move past ':'
    trim_inplace(after_vals);

    // 5) Read correct.txt
    char *correct_text = read_file_whole("correct.txt");
    if (!correct_text) {
        fprintf(stderr, "Error: cannot open correct.txt\n");
        free(after_line); free(out_text);
        return 8;
    }
    trim_inplace(correct_text);

    // 6) Compare: normalize spaces (collapse multiple spaces into single for comparison)
    //    We'll produce normalized strings for both.
    // normalize function:
    char *normalize(const char*);
    // implement locally
    char *norm_after = NULL;
    char *norm_correct = NULL;

    // simple normalize - collapse whitespace sequences to single spaces and trim ends
    {
        // normalize after_vals
        const char *src = after_vals;
        size_t bufsz = strlen(src) + 1;
        norm_after = (char*)malloc(bufsz);
        if (!norm_after) { free(correct_text); free(after_line); free(out_text); return 9; }
        char *dst = norm_after;
        int in_space = 0;
        for (; *src; ++src) {
            if (isspace((unsigned char)*src)) {
                if (!in_space) { *dst++ = ' '; in_space = 1; }
            } else {
                *dst++ = *src; in_space = 0;
            }
        }
        // remove trailing space
        if (dst > norm_after && *(dst - 1) == ' ') dst--;
        *dst = '\0';
        trim_inplace(norm_after);
    }
    {
        const char *src = correct_text;
        size_t bufsz = strlen(src) + 1;
        norm_correct = (char*)malloc(bufsz);
        if (!norm_correct) { free(norm_after); free(correct_text); free(after_line); free(out_text); return 10; }
        char *dst = norm_correct;
        int in_space = 0;
        for (; *src; ++src) {
            if (isspace((unsigned char)*src)) {
                if (!in_space) { *dst++ = ' '; in_space = 1; }
            } else {
                *dst++ = *src; in_space = 0;
            }
        }
        if (dst > norm_correct && *(dst - 1) == ' ') dst--;
        *dst = '\0';
        trim_inplace(norm_correct);
    }

    // 7) Final compare
    if (strcmp(norm_after, norm_correct) == 0) {
        printf("OK: sorted correctly\n");
        // cleanup
        free(norm_after); free(norm_correct);
        free(correct_text); free(after_line); free(out_text);
        return 0;
    } else {
        printf("FAIL: results differ\n");
        printf("---- Program output (After:) ----\n%s\n", norm_after);
        printf("---- Expected (sorted) ---------\n%s\n", norm_correct);
        // optional: write both to files for visual diff
        FILE *fa = fopen("got.txt", "w");
        if (fa) { fprintf(fa, "%s\n", norm_after); fclose(fa); }
        FILE *fb = fopen("expect.txt", "w");
        if (fb) { fprintf(fb, "%s\n", norm_correct); fclose(fb); }

        free(norm_after); free(norm_correct);
        free(correct_text); free(after_line); free(out_text);
        return 1;
    }
}
