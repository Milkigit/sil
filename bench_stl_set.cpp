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

static void bench_stlset_bench(void *self, struct benchpayload *data, struct action *action, size_t ndata, size_t naction, unsigned *result)
{
    bench_stlset_state *state = reinterpret_cast<bench_stlset_state *>(self);
    unsigned r;

    (void) ndata;

    for (size_t i = 0; i < naction; i++) {
        switch (action[i].action) {
        case BENCH_ACTION_ADD:
            r = state->tree.insert(data[action[i].index]).second ? 0 : 1;
            break;
        case BENCH_ACTION_FIND:
            {
                auto it = state->tree.find(data[action[i].index]);
                r = it != state->tree.end();
            }
            break;
        case BENCH_ACTION_REMOVE:
            {
                auto it = state->tree.find(data[action[i].index]);
                if (it == state->tree.end())
                    r = 0;
                else {
                    state->tree.erase(it);
                    r = 1;
                }
            }
            break;
        case BENCH_ACTION_HASHSUM:
            r = 0;
            for (benchpayload p: state->tree)
                r += hash_benchdata(&p);
            break;
        default:
            r = 0;
        }
        result[i] = r;
    }
}

struct treebenchfuncs bench_stlset_funcs = {
    "STL std::set",
    bench_stlset_init,
    bench_stlset_exit,
    bench_stlset_bench,
};
