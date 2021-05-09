#include "pulsar.h"
static const svc_pulsar_frame_t seq_0[] = {
    {3, 0, 4},
    {3, 0, 4},
    {3, 0, 4},
    {3, 0, 4},

    {0, 0, 1},
    {0, 0, 0}
};

static const svc_pulsar_frame_t seq_1[] = {
    {0, 3, 4},
    {0, 3, 4},
    {0, 3, 4},
    {0, 3, 4},

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
    {"L1", 0.5*SVC_PULSAR_PHI_MAX/SVC_PULSAR_F_SAMPLE, seq_0},
    {"L2", 0.5*SVC_PULSAR_PHI_MAX/SVC_PULSAR_F_SAMPLE, seq_1},
    {"LB", 0.5*SVC_PULSAR_PHI_MAX/SVC_PULSAR_F_SAMPLE, seq_2},
    {"B4", 0.5*SVC_PULSAR_PHI_MAX/SVC_PULSAR_F_SAMPLE, seq_3},
};
const uint8_t svc_pulsar_seqs_n = 4;
