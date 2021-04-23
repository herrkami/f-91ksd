#include "pulsar.h"
static const svc_pulsar_frame_t seq_0[] = {
    {3, 3, 1},
    {0, 3, 1},
    {3, 0, 1},
    {0, 0, 0}
};

static const svc_pulsar_frame_t seq_1[] = {
    {3, 0, 4},
    {3, 0, 4},
    {3, 0, 4},
    {3, 0, 4},
    {0, 0, 1},
    {0, 0, 1},
    {0, 0, 1},
    {0, 0, 0}
};

static const svc_pulsar_frame_t seq_2[] = {
    {3, 3, 2},
    {0, 1, 2},
    {0, 3, 2},
    {0, 1, 2},

    {1, 3, 2},
    {0, 1, 2},
    {0, 3, 2},
    {0, 1, 2},

    {3, 3, 2},
    {0, 1, 2},
    {0, 3, 2},
    {0, 1, 2},

    {1, 3, 2},
    {0, 1, 2},
    {0, 3, 2},
    {0, 1, 2},

    {0, 0, 0}
};

const svc_pulsar_sequence_t svc_pulsar_seqs[] = {
    {"SI", 0.5*SVC_PULSAR_PHI_MAX/SVC_PULSAR_F_SAMPLE, seq_0},
    {"SN", 0.5*SVC_PULSAR_PHI_MAX/SVC_PULSAR_F_SAMPLE, seq_1},
    {"FT", 0.5*SVC_PULSAR_PHI_MAX/SVC_PULSAR_F_SAMPLE, seq_2},
};
const uint8_t svc_pulsar_seqs_n = 3;
