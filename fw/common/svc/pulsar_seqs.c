#include "pulsar.h"
static const svc_pulsar_frame_t seq_0[] = {
    {10, 10, 1},
    {0, 10, 1},
    {10, 0, 1},
    {0, 0, 0}
};

static const svc_pulsar_frame_t seq_1[] = {
    {10, 0, 4},
    {10, 0, 4},
    {10, 0, 4},
    {10, 0, 4},
    {0, 0, 1},
    {0, 0, 1},
    {0, 0, 1},
    {0, 0, 0}
};

static const svc_pulsar_frame_t seq_2[] = {
    {10, 10, 2},
    {0, 5, 2},
    {0, 10, 2},
    {0, 5, 2},

    {5, 10, 2},
    {0, 5, 2},
    {0, 10, 2},
    {0, 5, 2},

    {10, 10, 2},
    {0, 5, 2},
    {0, 10, 2},
    {0, 5, 2},

    {5, 10, 2},
    {0, 5, 2},
    {0, 10, 2},
    {0, 5, 2},

    {0, 0, 0}
};

const svc_pulsar_sequence_t svc_pulsar_seqs[] = {
    {"SI", 0.5*SVC_PULSAR_PHI_MAX/SVC_PULSAR_F_SAMPLE, seq_0},
    {"SN", 0.5*SVC_PULSAR_PHI_MAX/SVC_PULSAR_F_SAMPLE, seq_1},
    {"FT", 0.5*SVC_PULSAR_PHI_MAX/SVC_PULSAR_F_SAMPLE, seq_2},
};
const uint8_t svc_pulsar_seqs_n = 3;
