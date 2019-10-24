namespace clx {
namespace kernel {

static char adder2[] =
    "__kernel void \n"
    "vadd(__global int *a, __global int *b, __global int *c) \n"
    "{                                                      \n"
    "   size_t i = get_global_id(0);                        \n"
    "   c[i] = a[i] + b[i];                                 \n"
    "}                                                      \n";
}
} // namespace clx
