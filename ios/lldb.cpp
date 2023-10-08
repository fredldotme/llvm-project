extern int lldb_main(int argc, char const **argv);

extern "C" {
__attribute__ ((visibility("default"))) int lldb_hook(int argc, char **argv) {
    return lldb_main(argc, (char const**) argv);
}
}
