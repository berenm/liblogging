//  operations.cpp  --------------------------------------------------------------------//

//  Copyright 2002-2009 Beman Dawes
//  Copyright 2001 Dietmar Kuehl

//  Distributed under the Boost Software License, Version 1.0.
//  See http://www.boost.org/LICENSE_1_0.txt

//  See library home page at http://www.boost.org/libs/filesystem

//--------------------------------------------------------------------------------------// 

//  define 64-bit offset macros BEFORE including boost/config.hpp (see ticket #5355) 
#if !(defined(__HP_aCC) && defined(_ILP32) && !defined(_STATVFS_ACPP_PROBLEMS_FIXED))
#define _FILE_OFFSET_BITS 64 // at worst, these defines may have no effect,
#endif
#if !defined(__PGI)
#define __USE_FILE_OFFSET64 // but that is harmless on Windows and on POSIX
      // 64-bit systems or on 32-bit systems which don't have files larger 
      // than can be represented by a traditional POSIX/UNIX off_t type. 
      // OTOH, defining them should kick in 64-bit off_t's (and thus 
      // st_size)on 32-bit systems that provide the Large File
      // Support (LFS)interface, such as Linux, Solaris, and IRIX.
      // The defines are given before any headers are included to
      // ensure that they are available to all included headers.
      // That is required at least on Solaris, and possibly on other
      // systems as well.
#else
#define _FILE_OFFSET_BITS 64
#endif

// define BOOST_FILESYSTEM_SOURCE so that <boost/filesystem/config.hpp> knows
// the library is being built (possibly exporting rather than importing code)
#define BOOST_FILESYSTEM_SOURCE 

#ifndef BOOST_SYSTEM_NO_DEPRECATED 
# define BOOST_SYSTEM_NO_DEPRECATED
#endif

#ifndef _POSIX_PTHREAD_SEMANTICS
# define _POSIX_PTHREAD_SEMANTICS  // Sun readdir_r()needs this
#endif

#include <boost/filesystem/operations.hpp>
#include <boost/scoped_array.hpp>
#include <boost/detail/workaround.hpp>
#include <vector> 
#include <cstdlib>     // for malloc, free
#include <cstring>
#include <cstdio>      // for remove, rename
#if defined(__QNXNTO__)  // see ticket #5355 
# include <stdio.h>
#endif
#include <cerrno>

#ifdef BOOST_FILEYSTEM_INCLUDE_IOSTREAM
# include <iostream>
#endif

namespace fs = boost::filesystem;
using boost::filesystem::path;
using boost::filesystem::filesystem_error;
using boost::filesystem::perms;
using boost::system::error_code;
using boost::system::error_category;
using boost::system::system_category;
using std::string;
using std::wstring;

# ifdef BOOST_POSIX_API

    const fs::path dot_path(".");
    const fs::path dot_dot_path("..");
#   include <sys/types.h>
#   include <sys/stat.h>
#   if !defined(__APPLE__) && !defined(__OpenBSD__)
#     include <sys/statvfs.h>
#     define BOOST_STATVFS statvfs
#     define BOOST_STATVFS_F_FRSIZE vfs.f_frsize
#   else
#     ifdef __OpenBSD__
#     include <sys/param.h>
#     endif
#     include <sys/mount.h>
#     define BOOST_STATVFS statfs
#     define BOOST_STATVFS_F_FRSIZE static_cast<boost::uintmax_t>(vfs.f_bsize)
#   endif
#   include <dirent.h>
#   include <unistd.h>
#   include <fcntl.h>
#   include <utime.h>
#   include "limits.h"

# else // BOOST_WINDOW_API

    const fs::path dot_path(L".");
    const fs::path dot_dot_path(L"..");
#   if (defined(__MINGW32__) || defined(__CYGWIN__)) && !defined(WINVER)
      // Versions of MinGW or Cygwin that support Filesystem V3 support at least WINVER 0x501.
      // See MinGW's windef.h
#     define WINVER 0x501
#   endif
#   include <io.h>
#   include <windows.h>
#   include <winnt.h>
#   if !defined(_WIN32_WINNT)
#     define  _WIN32_WINNT   0x0500
#   endif
#   if defined(__BORLANDC__) || defined(__MWERKS__)
#     if defined(__BORLANDC__)
        using std::time_t;
#     endif
#     include <utime.h>
#   else
#     include <sys/utime.h>
#   endif

//  REPARSE_DATA_BUFFER related definitions are found in ntifs.h, which is part of the 
//  Windows Device Driver Kit. Since that's inconvenient, the definitions are provided
//  here. See http://msdn.microsoft.com/en-us/library/ms791514.aspx

#if !defined(REPARSE_DATA_BUFFER_HEADER_SIZE)  // mingw winnt.h does provide the defs

#define SYMLINK_FLAG_RELATIVE 1

typedef struct _REPARSE_DATA_BUFFER {
  ULONG  ReparseTag;
  USHORT  ReparseDataLength;
  USHORT  Reserved;
  union {
    struct {
      USHORT  SubstituteNameOffset;
      USHORT  SubstituteNameLength;
      USHORT  PrintNameOffset;
      USHORT  PrintNameLength;
      ULONG  Flags;
      WCHAR  PathBuffer[1];
  /*  Example of distinction between substitute and print names:
        mklink /d ldrive c:\
        SubstituteName: c:\\??\
        PrintName: c:\
  */
     } SymbolicLinkReparseBuffer;
    struct {
      USHORT  SubstituteNameOffset;
      USHORT  SubstituteNameLength;
      USHORT  PrintNameOffset;
      USHORT  PrintNameLength;
      WCHAR  PathBuffer[1];
      } MountPointReparseBuffer;
    struct {
      UCHAR  DataBuffer[1];
    } GenericReparseBuffer;
  };
} REPARSE_DATA_BUFFER, *PREPARSE_DATA_BUFFER;

#define REPARSE_DATA_BUFFER_HEADER_SIZE \
  FIELD_OFFSET(REPARSE_DATA_BUFFER, GenericReparseBuffer)

#endif

#ifndef MAXIMUM_REPARSE_DATA_BUFFER_SIZE
#define MAXIMUM_REPARSE_DATA_BUFFER_SIZE  ( 16 * 1024 )
#endif

# ifndef FSCTL_GET_REPARSE_POINT
#   define FSCTL_GET_REPARSE_POINT 0x900a8
# endif

# ifndef IO_REPARSE_TAG_SYMLINK
#   define IO_REPARSE_TAG_SYMLINK (0xA000000CL)       
# endif

# endif  // BOOST_WINDOWS_API

//  BOOST_FILESYSTEM_STATUS_CACHE enables file_status cache in
//  dir_itr_increment. The config tests are placed here because some of the
//  macros being tested come from dirent.h.
//
// TODO: find out what macros indicate dirent::d_type present in more libraries
# if defined(BOOST_WINDOWS_API)\
  || defined(_DIRENT_HAVE_D_TYPE)// defined by GNU C library if d_type present
#   define BOOST_FILESYSTEM_STATUS_CACHE
# endif

//  POSIX/Windows macros  ----------------------------------------------------//

//  Portions of the POSIX and Windows API's are very similar, except for name,
//  order of arguments, and meaning of zero/non-zero returns. The macros below
//  abstract away those differences. They follow Windows naming and order of
//  arguments, and return true to indicate no error occurred. [POSIX naming,
//  order of arguments, and meaning of return were followed initially, but
//  found to be less clear and cause more coding errors.]

# if defined(BOOST_POSIX_API)

//  POSIX uses a 0 return to indicate success
#   define BOOST_ERRNO    errno 
#   define BOOST_SET_CURRENT_DIRECTORY(P)(::chdir(P)== 0)
#   define BOOST_CREATE_DIRECTORY(P)(::mkdir(P, S_IRWXU|S_IRWXG|S_IRWXO)== 0)
#   define BOOST_CREATE_HARD_LINK(F,T)(::link(T, F)== 0)
#   define BOOST_CREATE_SYMBOLIC_LINK(F,T,Flag)(::symlink(T, F)== 0)
#   define BOOST_REMOVE_DIRECTORY(P)(::rmdir(P)== 0)
#   define BOOST_DELETE_FILE(P)(::unlink(P)== 0)
#   define BOOST_COPY_DIRECTORY(F,T)(!(::stat(from.c_str(), &from_stat)!= 0\
         || ::mkdir(to.c_str(),from_stat.st_mode)!= 0))
#   define BOOST_COPY_FILE(F,T,FailIfExistsBool)copy_file_api(F, T, FailIfExistsBool)
#   define BOOST_MOVE_FILE(OLD,NEW)(::rename(OLD, NEW)== 0)
#   define BOOST_RESIZE_FILE(P,SZ)(::truncate(P, SZ)== 0)

#   define BOOST_ERROR_NOT_SUPPORTED ENOSYS
#   define BOOST_ERROR_ALREADY_EXISTS EEXIST

# else  // BOOST_WINDOWS_API

//  Windows uses a non-0 return to indicate success
#   define BOOST_ERRNO    ::GetLastError()
#   define BOOST_SET_CURRENT_DIRECTORY(P)(::SetCurrentDirectoryW(P)!= 0)
#   define BOOST_CREATE_DIRECTORY(P)(::CreateDirectoryW(P, 0)!= 0)
#   define BOOST_CREATE_HARD_LINK(F,T)(create_hard_link_api(F, T, 0)!= 0)
#   define BOOST_CREATE_SYMBOLIC_LINK(F,T,Flag)(create_symbolic_link_api(F, T, Flag)!= 0)
#   define BOOST_REMOVE_DIRECTORY(P)(::RemoveDirectoryW(P)!= 0)
#   define BOOST_DELETE_FILE(P)(::DeleteFileW(P)!= 0)
#   define BOOST_COPY_DIRECTORY(F,T)(::CreateDirectoryExW(F, T, 0)!= 0)
#   define BOOST_COPY_FILE(F,T,FailIfExistsBool)(::CopyFileW(F, T, FailIfExistsBool)!= 0)
#   define BOOST_MOVE_FILE(OLD,NEW)(::MoveFileExW(OLD, NEW, MOVEFILE_REPLACE_EXISTING|MOVEFILE_COPY_ALLOWED)!= 0)
#   define BOOST_RESIZE_FILE(P,SZ)(resize_file_api(P, SZ)!= 0)
#   define BOOST_READ_SYMLINK(P,T)

#   define BOOST_ERROR_ALREADY_EXISTS ERROR_ALREADY_EXISTS
#   define BOOST_ERROR_NOT_SUPPORTED ERROR_NOT_SUPPORTED

# endif

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                        helpers (all operating systems)                              //
//                                                                                      //
//--------------------------------------------------------------------------------------//

namespace
{

  fs::file_type query_file_type(const path& p, error_code* ec);

  boost::filesystem::directory_iterator end_dir_itr;

  const std::size_t buf_size(128);
  const error_code ok;

  bool error(bool was_error, error_code* ec, const string& message)
  {
    if (!was_error)
    {
      if (ec != 0) ec->clear();
    }
    else  
    { //  error
      if (ec == 0)
        BOOST_FILESYSTEM_THROW(filesystem_error(message,
          error_code(BOOST_ERRNO, system_category())));
      else
        ec->assign(BOOST_ERRNO, system_category());
    }
    return was_error;
  }

  bool error(bool was_error, const path& p, error_code* ec, const string& message)
  {
    if (!was_error)
    {
      if (ec != 0) ec->clear();
    }
    else  
    { //  error
      if (ec == 0)
        BOOST_FILESYSTEM_THROW(filesystem_error(message,
          p, error_code(BOOST_ERRNO, system_category())));
      else
        ec->assign(BOOST_ERRNO, system_category());
    }
    return was_error;
  }

  bool error(bool was_error, const path& p1, const path& p2, error_code* ec,
    const string& message)
  {
    if (!was_error)
    {
      if (ec != 0) ec->clear();
    }
    else  
    { //  error
      if (ec == 0)
        BOOST_FILESYSTEM_THROW(filesystem_error(message,
          p1, p2, error_code(BOOST_ERRNO, system_category())));
      else
        ec->assign(BOOST_ERRNO, system_category());
    }
    return was_error;
  }

  bool error(bool was_error, const error_code& result,
    const path& p, error_code* ec, const string& message)
    //  Overwrites ec if there has already been an error
  {
    if (!was_error)
    {
      if (ec != 0) ec->clear();
    }
    else  
    { //  error
      if (ec == 0)
        BOOST_FILESYSTEM_THROW(filesystem_error(message, p, result));
      else
        *ec = result;
    }
    return was_error;
  }

  bool error(bool was_error, const error_code& result,
    const path& p1, const path& p2, error_code* ec, const string& message)
    //  Overwrites ec if there has already been an error
  {
    if (!was_error)
    {
      if (ec != 0) ec->clear();
    }
    else  
    { //  error
      if (ec == 0)
        BOOST_FILESYSTEM_THROW(filesystem_error(message, p1, p2, result));
      else
        *ec = result;
    }
    return was_error;
  }

  bool is_empty_directory(const path& p)
  {
    return fs::directory_iterator(p)== end_dir_itr;
  }

  bool remove_directory(const path& p) // true if succeeds
    { return BOOST_REMOVE_DIRECTORY(p.c_str()); }
  
  bool remove_file(const path& p) // true if succeeds
    { return BOOST_DELETE_FILE(p.c_str()); }
  
  // called by remove and remove_all_aux
  bool remove_file_or_directory(const path& p, fs::file_type type, error_code* ec)
    // return true if file removed, false if not removed
  {
    if (type == fs::file_not_found)
    {
      if (ec != 0) ec->clear();
      return false;
    }

    if (type == fs::directory_file
#     ifdef BOOST_WINDOWS_API
        || type == fs::_detail_directory_symlink
#     endif
      )
    {
      if (error(!remove_directory(p), p, ec, "boost::filesystem::remove"))
        return false;
    }
    else
    {
      if (error(!remove_file(p), p, ec, "boost::filesystem::remove"))
        return false;
    }
    return true;
  }

  boost::uintmax_t remove_all_aux(const path& p, fs::file_type type,
    error_code* ec)
  {
    boost::uintmax_t count = 1;

    if (type == fs::directory_file)  // but not a directory symlink
    {
      for (fs::directory_iterator itr(p);
            itr != end_dir_itr; ++itr)
      {
        fs::file_type tmp_type = query_file_type(itr->path(), ec);
        if (ec != 0 && *ec)
          return count;
        count += remove_all_aux(itr->path(), tmp_type, ec);
      }
    }
    remove_file_or_directory(p, type, ec);
    return count;
  }

#ifdef BOOST_POSIX_API

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                            POSIX-specific helpers                                    //
//                                                                                      //
//--------------------------------------------------------------------------------------//

  const char dot = '.';

  bool not_found_error(int errval)
  {
    return errno == ENOENT || errno == ENOTDIR;
  }

  bool // true if ok
  copy_file_api(const std::string& from_p,
    const std::string& to_p, bool fail_if_exists)
  {
    const std::size_t buf_sz = 32768;
    boost::scoped_array<char> buf(new char [buf_sz]);
    int infile=-1, outfile=-1;  // -1 means not open

    // bug fixed: code previously did a stat()on the from_file first, but that
    // introduced a gratuitous race condition; the stat()is now done after the open()

    if ((infile = ::open(from_p.c_str(), O_RDONLY))< 0)
      { return false; }

    struct stat from_stat;
    if (::stat(from_p.c_str(), &from_stat)!= 0)
    { 
      ::close(infile);
      return false;
    }

    int oflag = O_CREAT | O_WRONLY | O_TRUNC;
    if (fail_if_exists)
      oflag |= O_EXCL;
    if ((outfile = ::open(to_p.c_str(), oflag, from_stat.st_mode))< 0)
    {
      int open_errno = errno;
      BOOST_ASSERT(infile >= 0);
      ::close(infile);
      errno = open_errno;
      return false;
    }

    ssize_t sz, sz_read=1, sz_write;
    while (sz_read > 0
      && (sz_read = ::read(infile, buf.get(), buf_sz))> 0)
    {
      // Allow for partial writes - see Advanced Unix Programming (2nd Ed.),
      // Marc Rochkind, Addison-Wesley, 2004, page 94
      sz_write = 0;
      do
      {
        if ((sz = ::write(outfile, buf.get() + sz_write,
          sz_read - sz_write))< 0)
        { 
          sz_read = sz; // cause read loop termination
          break;        //  and error to be thrown after closes
        }
        sz_write += sz;
      } while (sz_write < sz_read);
    }

    if (::close(infile)< 0)sz_read = -1;
    if (::close(outfile)< 0)sz_read = -1;

    return sz_read >= 0;
  }

  inline fs::file_type query_file_type(const path& p, error_code* ec)
  {
    return fs::symlink_status(p, *ec).type();
  }

# else

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                            Windows-specific helpers                                  //
//                                                                                      //
//--------------------------------------------------------------------------------------//

  const wchar_t dot = L'.';

  bool not_found_error(int errval)
  {
    return errval == ERROR_FILE_NOT_FOUND
      || errval == ERROR_PATH_NOT_FOUND
      || errval == ERROR_INVALID_NAME  // "tools/jam/src/:sys:stat.h", "//foo"
      || errval == ERROR_INVALID_DRIVE  // USB card reader with no card inserted
      || errval == ERROR_NOT_READY  // CD/DVD drive with no disc inserted
      || errval == ERROR_INVALID_PARAMETER  // ":sys:stat.h"
      || errval == ERROR_BAD_PATHNAME  // "//nosuch" on Win64
      || errval == ERROR_BAD_NETPATH;  // "//nosuch" on Win32
  }

// some distributions of mingw as early as GLIBCXX__ 20110325 have _stricmp, but the
// offical 4.6.2 release with __GLIBCXX__ 20111026  doesn't. Play it safe for now, and
// only use _stricmp if _MSC_VER is defined
#if defined(_MSC_VER) // || (defined(__GLIBCXX__) && __GLIBCXX__ >= 20110325)
#  define BOOST_FILESYSTEM_STRICMP _stricmp
#else
#  define BOOST_FILESYSTEM_STRICMP strcmp
#endif

  perms make_permissions(const path& p, DWORD attr)
  {
    perms prms = fs::owner_read | fs::group_read | fs::others_read;
    if  ((attr & FILE_ATTRIBUTE_READONLY) == 0)
      prms |= fs::owner_write | fs::group_write | fs::others_write;
    if (BOOST_FILESYSTEM_STRICMP(p.extension().string().c_str(), ".exe") == 0
      || BOOST_FILESYSTEM_STRICMP(p.extension().string().c_str(), ".com") == 0
      || BOOST_FILESYSTEM_STRICMP(p.extension().string().c_str(), ".bat") == 0
      || BOOST_FILESYSTEM_STRICMP(p.extension().string().c_str(), ".cmd") == 0)
      prms |= fs::owner_exe | fs::group_exe | fs::others_exe;
    return prms;
  }

  // these constants come from inspecting some Microsoft sample code
  std::time_t to_time_t(const FILETIME & ft)
  {
    __int64 t = (static_cast<__int64>(ft.dwHighDateTime)<< 32)
      + ft.dwLowDateTime;
#   if !defined(BOOST_MSVC) || BOOST_MSVC > 1300 // > VC++ 7.0
    t -= 116444736000000000LL;
#   else
    t -= 116444736000000000;
#   endif
    t /= 10000000;
    return static_cast<std::time_t>(t);
  }

  void to_FILETIME(std::time_t t, FILETIME & ft)
  {
    __int64 temp = t;
    temp *= 10000000;
#   if !defined(BOOST_MSVC) || BOOST_MSVC > 1300 // > VC++ 7.0
    temp += 116444736000000000LL;
#   else
    temp += 116444736000000000;
#   endif
    ft.dwLowDateTime = static_cast<DWORD>(temp);
    ft.dwHighDateTime = static_cast<DWORD>(temp >> 32);
  }

  // Thanks to Jeremy Maitin-Shepard for much help and for permission to
  // base the equivalent()implementation on portions of his 
  // file-equivalence-win32.cpp experimental code.

  struct handle_wrapper
  {
    HANDLE handle;
    handle_wrapper(HANDLE h)
      : handle(h){}
    ~handle_wrapper()
    {
      if (handle != INVALID_HANDLE_VALUE)
        ::CloseHandle(handle);
    }
  };

  HANDLE create_file_handle(const path& p, DWORD dwDesiredAccess,
    DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile)
  {
    return ::CreateFileW(p.c_str(), dwDesiredAccess, dwShareMode,
      lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes,
      hTemplateFile);
  }

  bool is_reparse_point_a_symlink(const path& p)
  {
    handle_wrapper h(create_file_handle(p, FILE_READ_EA,
      FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING,
      FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, NULL));
    if (h.handle == INVALID_HANDLE_VALUE)
      return false;

    boost::scoped_array<char> buf(new char [MAXIMUM_REPARSE_DATA_BUFFER_SIZE]);    
 
    // Query the reparse data
    DWORD dwRetLen;
    BOOL result = ::DeviceIoControl(h.handle, FSCTL_GET_REPARSE_POINT, NULL, 0, buf.get(),
      MAXIMUM_REPARSE_DATA_BUFFER_SIZE, &dwRetLen, NULL);
    if (!result) return false;
 
    return reinterpret_cast<const REPARSE_DATA_BUFFER*>(buf.get())
      ->ReparseTag == IO_REPARSE_TAG_SYMLINK;
  }

  inline std::size_t get_full_path_name(
    const path& src, std::size_t len, wchar_t* buf, wchar_t** p)
  {
    return static_cast<std::size_t>(
      ::GetFullPathNameW(src.c_str(), static_cast<DWORD>(len), buf, p));
  }

  fs::file_status process_status_failure(const path& p, error_code* ec)
  {
    int errval(::GetLastError());
    if (ec != 0)                             // always report errval, even though some
      ec->assign(errval, system_category());   // errval values are not status_errors

    if (not_found_error(errval))
    {
      return fs::file_status(fs::file_not_found, fs::no_perms);
    }
    else if ((errval == ERROR_SHARING_VIOLATION))
    {
      return fs::file_status(fs::type_unknown);
    }
    if (ec == 0)
      BOOST_FILESYSTEM_THROW(filesystem_error("boost::filesystem::status",
        p, error_code(errval, system_category())));
    return fs::file_status(fs::status_error);
  }

  //  differs from symlink_status() in that directory symlinks are reported as
  //  _detail_directory_symlink, as required on Windows by remove() and its helpers.
  fs::file_type query_file_type(const path& p, error_code* ec)
  {
    DWORD attr(::GetFileAttributesW(p.c_str()));
    if (attr == 0xFFFFFFFF)
    {
      return process_status_failure(p, ec).type();
    }

    if (ec != 0) ec->clear();

    if (attr & FILE_ATTRIBUTE_REPARSE_POINT)
    {
      if (is_reparse_point_a_symlink(p))
        return (attr & FILE_ATTRIBUTE_DIRECTORY)
          ? fs::_detail_directory_symlink
          : fs::symlink_file;
      return fs::reparse_file;
    }

    return (attr & FILE_ATTRIBUTE_DIRECTORY)
      ? fs::directory_file
      : fs::regular_file;
  }

  BOOL resize_file_api(const wchar_t* p, boost::uintmax_t size)
  {
    HANDLE handle = CreateFileW(p, GENERIC_WRITE, 0, 0, OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL, 0);
    LARGE_INTEGER sz;
    sz.QuadPart = size;
    return handle != INVALID_HANDLE_VALUE
      && ::SetFilePointerEx(handle, sz, 0, FILE_BEGIN)
      && ::SetEndOfFile(handle)
      && ::CloseHandle(handle);
  }

  //  Windows kernel32.dll functions that may or may not be present
  //  must be accessed through pointers

  typedef BOOL (WINAPI *PtrCreateHardLinkW)(
    /*__in*/       LPCWSTR lpFileName,
    /*__in*/       LPCWSTR lpExistingFileName,
    /*__reserved*/ LPSECURITY_ATTRIBUTES lpSecurityAttributes
   );

  PtrCreateHardLinkW create_hard_link_api = PtrCreateHardLinkW(
    ::GetProcAddress(
      ::GetModuleHandle(TEXT("kernel32.dll")), "CreateHardLinkW"));

  typedef BOOLEAN (WINAPI *PtrCreateSymbolicLinkW)(
    /*__in*/ LPCWSTR lpSymlinkFileName,
    /*__in*/ LPCWSTR lpTargetFileName,
    /*__in*/ DWORD dwFlags
   );

  PtrCreateSymbolicLinkW create_symbolic_link_api = PtrCreateSymbolicLinkW(
    ::GetProcAddress(
      ::GetModuleHandle(TEXT("kernel32.dll")), "CreateSymbolicLinkW"));

#endif

//#ifdef BOOST_WINDOWS_API
//
//
//  inline bool get_free_disk_space(const std::wstring& ph,
//    PULARGE_INTEGER avail, PULARGE_INTEGER total, PULARGE_INTEGER free)
//    { return ::GetDiskFreeSpaceExW(ph.c_str(), avail, total, free)!= 0; }
//
//#endif

} // unnamed namespace

//--------------------------------------------------------------------------------------//
//                                                                                      //
//                operations functions declared in operations.hpp                       //
//                            in alphabetic order                                       //
//                                                                                      //
//--------------------------------------------------------------------------------------//

namespace boost
{
namespace filesystem
{

namespace detail
{

  BOOST_FILESYSTEM_DECL
  void copy_file(const path& from, const path& to,
                  BOOST_SCOPED_ENUM(copy_option)option,
                  error_code* ec)
  {
    error(!BOOST_COPY_FILE(from.c_str(), to.c_str(),
      option == copy_option::fail_if_exists),
        from, to, ec, "boost::filesystem::copy_file");
  }

}  // namespace detail
} // namespace filesystem
} // namespace boost

namespace boost
{
namespace filesystem3
{

namespace detail
{

  BOOST_FILESYSTEM_DECL
  void copy_file(const path& from, const path& to,
                  BOOST_SCOPED_ENUM(copy_option)option,
                  error_code* ec)
  {
    error(!BOOST_COPY_FILE(from.c_str(), to.c_str(),
      option == copy_option::fail_if_exists),
        from, to, ec, "boost::filesystem::copy_file");
  }

}  // namespace detail
} // namespace filesystem
} // namespace boost
