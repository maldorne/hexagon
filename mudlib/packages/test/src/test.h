
#define ASSERT(x) if (!(x)) { mixed t;t=call_trace();write("%^BOLD%^"+file_name(this_object()) + ": Check failed on line "+t[sizeof(t)-1][3]+".%^RESET%^\n"); }
#define ASSERT2(x, r) if (!(x)) { write("%^BOLD%^"+file_name(this_object()) + ":" + r + ", Check failed.%^RESET%^\n"); }

#define REMOVE_LIVING handler("livings")->remove_living(this_object())
