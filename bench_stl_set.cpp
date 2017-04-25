#include <cassert>
#include <set>

#include "bench.h"
#include "bench_stl_set.h"

/******************************
 * Bench for C++ STL std::set *
 ******************************/

struct bench_stlset_compare {
    bool operator()(const benchpayload& a, const benchpayload& b) const {
        return a.a < b.a;
    }
};

struct bench_stlset_state {
    std::set<struct benchpayload, bench_stlset_compare> tree;
};

static void *bench_stlset_init(void)
{
    return new bench_stlset_state();
}

static void bench_stlset_exit(void *self)
{
    bench_stlset_state *state = reinterpret_cast<bench_stlset_state *>(self);
    delete state;
}

static void bench_stlset_insertbench(void *self, struct benchpayload *data, size_t n)
{
    bench_stlset_state *state = reinterpret_cast<bench_stlset_state *>(self);

    for (size_t i = 0; i < n; i++)
        state->tree.insert(data[i]);
}

static void bench_stlset_retrievebench(void *self, struct benchpayload *data, size_t n)
{
    bench_stlset_state *state = reinterpret_cast<bench_stlset_state *>(self);

    for (size_t i = 0; i < n; i++) {
        auto found = state->tree.find(data[i]);
        (void) found; assert(found != state->tree.end());
    }
}

static void bench_stlset_removebench(void *self, struct benchpayload *data, size_t n)
{
    bench_stlset_state *state = reinterpret_cast<bench_stlset_state *>(self);

    for (size_t i = 0; i < n; i++)
        state->tree.erase(data[i]);
}

static void bench_stlset_addelems(void *self, size_t *out_count, unsigned *out_sumofhashes)
{
    (void) self;

    *out_count = 42;
    *out_sumofhashes = 42;
}

struct treebenchfuncs bench_stlset_funcs = {
    "STL std::set",
    bench_stlset_init,
    bench_stlset_exit,
    bench_stlset_insertbench,
    bench_stlset_retrievebench,
    bench_stlset_removebench,
    bench_stlset_addelems,
};
