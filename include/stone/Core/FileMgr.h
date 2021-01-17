#ifndef STONE_CORE_FILEMANAGER_H
#define STONE_CORE_FILEMANAGER_H

#include <ctime>
#include <map>
#include <memory>
#include <string>

#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/ErrorOr.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/VirtualFileSystem.h"
#include "stone/Core/FileSystemOptions.h"
#include "stone/Core/LLVM.h"

namespace llvm {

class MemoryBuffer;

}  // end namespace llvm

namespace stone {

class FileSystemStatCache;

/// Cached information about one directory (either on disk or in
/// the virtual file system).
class SrcDir {
  friend class FileMgr;
  llvm::StringRef Name;  // Name of the directory.

 public:
  StringRef getName() const { return Name; }
};

/// Cached information about one file (either on disk
/// or in the virtual file system).
///
/// If the 'File' member is valid, then this SrcFile has an open file
/// descriptor for the file.
class SrcFile {
  friend class FileMgr;

  StringRef Name;            // Name of the file.
  std::string RealPathName;  // Real path to the file; could be empty.
  off_t Size;                // File size in bytes.
  time_t ModTime;            // Modification time of file.
  const SrcDir *Dir;         // Directory file lives in.
  unsigned UID;              // A unique (small) ID for the file.
  llvm::sys::fs::UniqueID UniqueID;
  bool IsNamedPipe;
  bool IsValid;  // Is this \c SrcFile initialized and valid?

  /// The open file, if it is owned by the \p SrcFile.
  mutable std::unique_ptr<llvm::vfs::File> File;

 public:
  SrcFile() : UniqueID(0, 0), IsNamedPipe(false), IsValid(false) {}

  SrcFile(const SrcFile &) = delete;
  SrcFile &operator=(const SrcFile &) = delete;

  StringRef getName() const { return Name; }
  StringRef tryGetRealPathName() const { return RealPathName; }
  bool isValid() const { return IsValid; }
  off_t getSize() const { return Size; }
  unsigned getUID() const { return UID; }
  const llvm::sys::fs::UniqueID &getUniqueID() const { return UniqueID; }
  time_t getModificationTime() const { return ModTime; }

  /// Return the directory the file lives in.
  const SrcDir *getDir() const { return Dir; }

  bool operator<(const SrcFile &RHS) const { return UniqueID < RHS.UniqueID; }

  /// Check whether the file is a named pipe (and thus can't be opened by
  /// the native FileMgr methods).
  bool isNamedPipe() const { return IsNamedPipe; }

  void closeFile() const {
    File.reset();  // rely on destructor to close File
  }

  // Only for use in tests to see if deferred opens are happening, rather than
  // relying on RealPathName being empty.
  bool isOpenForTests() const { return File != nullptr; }
};

/// Implements support for file system lookup, file system caching,
/// and directory search management.
///
/// This also handles more advanced properties, such as uniquing files based
/// on "inode", so that a file with two names (e.g. symlinked) will be treated
/// as a single file.
///
class FileMgr : public RefCountedBase<FileMgr> {
  IntrusiveRefCntPtr<llvm::vfs::FileSystem> FS;
  FileSystemOptions FileSystemOpts;

  /// Cache for existing real directories.
  std::map<llvm::sys::fs::UniqueID, SrcDir> UniqueRealDirs;

  /// Cache for existing real files.
  std::map<llvm::sys::fs::UniqueID, SrcFile> UniqueRealFiles;

  /// The virtual directories that we have allocated.
  ///
  /// For each virtual file (e.g. foo/bar/baz.cpp), we add all of its parent
  /// directories (foo/ and foo/bar/) here.
  SmallVector<std::unique_ptr<SrcDir>, 4> VirtualDirectoryEntries;
  /// The virtual files that we have allocated.
  SmallVector<std::unique_ptr<SrcFile>, 4> VirtualFileEntries;

  /// A cache that maps paths to directory entries (either real or
  /// virtual) we have looked up
  ///
  /// The actual Entries for real directories/files are
  /// owned by UniqueRealDirs/UniqueRealFiles above, while the Entries
  /// for virtual directories/files are owned by
  /// VirtualDirectoryEntries/VirtualFileEntries above.
  ///
  llvm::StringMap<SrcDir *, llvm::BumpPtrAllocator> SeenDirEntries;

  /// A cache that maps paths to file entries (either real or
  /// virtual) we have looked up.
  ///
  /// \see SeenDirEntries
  llvm::StringMap<SrcFile *, llvm::BumpPtrAllocator> SeenFileEntries;

  /// The canonical names of directories.
  llvm::DenseMap<const SrcDir *, llvm::StringRef> CanonicalDirNames;

  /// Storage for canonical names that we have computed.
  llvm::BumpPtrAllocator CanonicalNameStorage;

  /// Each SrcFile we create is assigned a unique ID #.
  ///
  unsigned NextFileUID;

  // Statistics.
  unsigned NumDirLookups, NumFileLookups;
  unsigned NumDirCacheMisses, NumFileCacheMisses;

  // Caching.
  std::unique_ptr<FileSystemStatCache> StatCache;

  bool getStatValue(StringRef Path, llvm::vfs::Status &Status, bool isFile,
                    std::unique_ptr<llvm::vfs::File> *F);

  /// Add all ancestors of the given path (pointing to either a file
  /// or a directory) as virtual directories.
  void addAncestorsAsVirtualDirs(StringRef Path);

  /// Fills the RealPathName in file entry.
  void fillRealPathName(SrcFile *UFE, llvm::StringRef FileName);

 public:
  /// Construct a file manager, optionally with a custom VFS.
  ///
  /// \param FS if non-null, the VFS to use.  Otherwise uses
  /// llvm::vfs::getRealFileSystem().
  FileMgr(const FileSystemOptions &FileSystemOpts,
          IntrusiveRefCntPtr<llvm::vfs::FileSystem> FS = nullptr);
  ~FileMgr();

  /// Installs the provided FileSystemStatCache object within
  /// the FileMgr.
  ///
  /// Ownership of this object is transferred to the FileMgr.
  ///
  /// \param statCache the new stat cache to install. Ownership of this
  /// object is transferred to the FileMgr.
  void setStatCache(std::unique_ptr<FileSystemStatCache> statCache);

  /// Removes the FileSystemStatCache object from the manager.
  void clearStatCache();

  /// Lookup, cache, and verify the specified directory (real or
  /// virtual).
  ///
  /// This returns NULL if the directory doesn't exist.
  ///
  /// \param CacheFailure If true and the file does not exist, we'll cache
  /// the failure to find this file.
  const SrcDir *getDirectory(StringRef DirName, bool CacheFailure = true);

  /// Lookup, cache, and verify the specified file (real or
  /// virtual).
  ///
  /// This returns NULL if the file doesn't exist.
  ///
  /// \param OpenFile if true and the file exists, it will be opened.
  ///
  /// \param CacheFailure If true and the file does not exist, we'll cache
  /// the failure to find this file.
  const SrcFile *getFile(StringRef Filename, bool OpenFile = false,
                         bool CacheFailure = true);

  /// Returns the current file system options
  FileSystemOptions &getFileSystemOpts() { return FileSystemOpts; }
  const FileSystemOptions &getFileSystemOpts() const { return FileSystemOpts; }

  llvm::vfs::FileSystem &getVirtualFileSystem() const { return *FS; }

  /// Retrieve a file entry for a "virtual" file that acts as
  /// if there were a file with the given name on disk.
  ///
  /// The file itself is not accessed.
  const SrcFile *getVirtualFile(StringRef Filename, off_t Size,
                                time_t ModificationTime);

  /// Open the specified file as a MemoryBuffer, returning a new
  /// MemoryBuffer if successful, otherwise returning null.
  llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> getBufferForFile(
      const SrcFile *Entry, bool isVolatile = false,
      bool ShouldCloseOpenFile = true);
  llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> getBufferForFile(
      StringRef Filename, bool isVolatile = false);

  /// Get the 'stat' information for the given \p Path.
  ///
  /// If the path is relative, it will be resolved against the WorkingDir of the
  /// FileMgr's FileSystemOptions.
  ///
  /// \returns false on success, true on error.
  bool getNoncachedStatValue(StringRef Path, llvm::vfs::Status &Result);

  /// Remove the real file \p Entry from the cache.
  void invalidateCache(const SrcFile *Entry);

  /// If path is not absolute and FileSystemOptions set the working
  /// directory, the path is modified to be relative to the given
  /// working directory.
  /// \returns true if \c path changed.
  bool FixupRelativePath(SmallVectorImpl<char> &path) const;

  /// Makes \c Path absolute taking into account FileSystemOptions and the
  /// working directory option.
  /// \returns true if \c Path changed to absolute.
  bool makeAbsolutePath(SmallVectorImpl<char> &Path) const;

  /// Produce an array mapping from the unique IDs assigned to each
  /// file to the corresponding SrcFile pointer.
  void GetUniqueIDMapping(SmallVectorImpl<const SrcFile *> &UIDToFiles) const;

  /// Modifies the size and modification time of a previously created
  /// SrcFile. Use with caution.
  static void modifySrcFile(SrcFile *File, off_t Size, time_t ModificationTime);

  /// Retrieve the canonical name for a given directory.
  ///
  /// This is a very expensive operation, despite its results being cached,
  /// and should only be used when the physical layout of the file system is
  /// required, which is (almost) never.
  StringRef getCanonicalName(const SrcDir *Dir);

  void PrintStats() const;
};

}  // end namespace stone

#endif  // LLVM_CLANG_BASIC_FILEMANAGER_H
