#include "undup.h"
#include "utils.h"
#include <string.h>
#include <stdlib.h>

int hconvert_main() {
  exit(EXIT_FAILURE);
}


int main(int argc,char **argv) {
  if (argc == 0) fatal(EXIT_FAILURE,"Exec error");
  if (strcmp(argv[0],"hconvert") == 0) return hconvert_main(argc,argv);
  if (argc > 1 && strcmp(argv[1],"hconvert") == 0) {
    --argc, ++argv;
    return hconvert_main(argc,argv);
  }
  return undup_main(argc,argv);
}
