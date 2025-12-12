#include "prog.h"
#include "render.h"

int main(int argc, char *argv[]) {
    int res = 0;

    Type type = parse_args(argc, argv);

    Program prog;
    if (!program_init(&prog, type)) {
        return_defer(1);
    }

    setup(&prog);

    program_run(&prog);

defer:
    program_deinit(&prog);

    return res;
}
