#include "stone/Driver/Job.h"

using namespace stone;
using namespace stone::driver;

// Some jobs only consume inputs -- For example, LinkJob
LinkJob::LinkJob(JobType jobType, bool isAsync, Compilation& compilation,
                 bool requiresLTO, LinkType linkType)
    : Job(jobType, isAsync, compilation),
      requiresLTO(requiresLTO),
      linkType(linkType) {}

// Some jobs only consume inputs -- For example, LinkJob
StaticLinkJob::StaticLinkJob(bool isAsync, Compilation& compilation,
                             bool requiresLTO, LinkType linkType)
    : LinkJob(JobType::StaticLink, isAsync, compilation, requiresLTO,
              linkType) {}

void StaticLinkJob::BuildOutputProfile() {}

// Some jobs only consume inputs -- For example, LinkJob
DynamicLinkJob::DynamicLinkJob(bool isAsync, Compilation& compilation,
                               bool requiresLTO, LinkType linkType)
    : LinkJob(JobType::DynamicLink, isAsync, compilation, requiresLTO,
              linkType) {}

void DynamicLinkJob::BuildOutputProfile() {}
