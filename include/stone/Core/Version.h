#ifndef STONE_CORE_VERSION_H
#define STONE_CORE_VERSION_H
#include <string>

namespace stone {
/// Retrieves the repository path (e.g., Subversion path) that
/// identifies the particular stone branch, tag, or trunk from which this
/// stone was built.
std::string GetRepoPath();

/// Retrieves the repository revision number (or identifier) from which
/// this stone was built.
std::string GetRevision();

/// Retrieves the full repository version that is an amalgamation of
/// the information in GetRepPath() and GetRevision().
std::string GetFullRepoVersion();

/// Retrieves a string representing the complete stone version,
/// which includes the stone version number, the repository version,
/// and the vendor tag.
std::string GetFullVersion();

}  // namespace stone

#endif
