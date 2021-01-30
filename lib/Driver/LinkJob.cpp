#include "stone/Driver/Job.h"

using namespace stone;
using namespace stone::driver;

// Some jobs only consume inputs -- For example, LinkJob
LinkJob::LinkJob(JobType jobType, Compilation& compilation, bool requiresLTO,
                 LinkType linkType)
    : Job(jobType, compilation), requiresLTO(requiresLTO), linkType(linkType) {}

// Some jobs only consume inputs -- For example, LinkJob
StaticLinkJob::StaticLinkJob(Compilation& compilation, bool requiresLTO,
                             LinkType linkType)
    : LinkJob(JobType::StaticLink, compilation, requiresLTO, linkType) {}

void StaticLinkJob::BuildCmdOutput() {}

// Some jobs only consume inputs -- For example, LinkJob
DynamicLinkJob::DynamicLinkJob(Compilation& compilation, bool requiresLTO,
                               LinkType linkType)
    : LinkJob(JobType::DynamicLink, compilation, requiresLTO, linkType) {}

void DynamicLinkJob::BuildCmdOutput() {}
