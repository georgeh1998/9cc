#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }


  printf("	.arch armv8-a\n");
  printf("	.text\n");
  printf("	.align	2\n");
  printf("	.global	main\n");
  printf("  .type	main, %%function\n");
  printf("main:\n");
  printf(".LFB0:\n");
  printf("	.cfi_startproc\n");
  printf("	mov	w0, %d\n", atoi(argv[1]));
  printf("	ret\n");
  printf("	.cfi_endproc\n");
  printf(".LFE0:\n");
  printf("	.size	main, .-main\n");
  printf("	.ident	\"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0\"\n");
  printf("  .section	.note.GNU-stack,\"\",@progbits\n");
  return 0;
}