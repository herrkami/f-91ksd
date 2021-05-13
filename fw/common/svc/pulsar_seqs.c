#include "pulsar.h"
static const svc_pulsar_frame_t seq_0[] = {
    {0, 3, 4},
    {0, 3, 4},
    {0, 3, 4},
    {0, 3, 4},

    {0, 0, 1},
    {0, 0, 0}
};

static const svc_pulsar_frame_t seq_1[] = {
    {3, 0, 4},
    {3, 0, 4},
    {3, 0, 4},
    {3, 0, 4},

    {0, 0, 1},
    {0, 0, 0}
};

static const svc_pulsar_frame_t seq_2[] = {
    {3, 0, 4},
    {3, 0, 4},
    {3, 0, 4},
    {3, 0, 4},
    {0, 3, 4},
    {0, 3, 4},
    {0, 3, 4},
    {0, 3, 4},

    {0, 0, 1},

    {0, 0, 1},
    {0, 0, 0}
};

static const svc_pulsar_frame_t seq_3[] = {
    {1, 1, 2},
    {1, 0, 2},
    {1, 0, 2},
    {1, 0, 2},

    {0, 0, 0}
};

const svc_pulsar_sequence_t svc_pulsar_seqs[] = {
    {"L1", 120U*100U, seq_0},
    {"L2", 120U*100U, seq_1},
    {"LB", 120U*100U, seq_2},
    {"B4", 120U*100U, seq_3},
};
const uint8_t svc_pulsar_seqs_n = 4;
