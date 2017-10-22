void assert(int assertion) {
    if (!assertion) {
        puts("Failed an assertion");
        exit(-1);
    }
}
