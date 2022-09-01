//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_fs_file.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_hh_stdio.h"

#include "emp_ast_errno.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if defined EMP_XX_COMPILER_MSC
#else
#include <dirent.h>
#include <sys/types.h>
#include "emp_ast_errno.h"
#endif

#if defined EMP_XX_OS_WIN
#include "emp_hh_windows.h"
#include <sys/stat.h>
#include <share.h>

#include <io.h>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace win {

#if defined EMP_XX_COMPILER_MSC
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool CharToWChar(char const* const a_szFrom, vector<wchar_t>& a_rTo)
{
    int siSize =  MultiByteToWideChar(CP_UTF8, 0, a_szFrom, -1, NULL, 0);
    if (siSize <= 0)
    { return false; }

    a_rTo.resize(emp::ast::cast<size_t>(siSize));

    return MultiByteToWideChar(CP_UTF8, 0, a_szFrom, -1, a_rTo.ptr(0), siSize) != 0;
}
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void GetLastErrorString(DWORD const a_dwError, astring& a_rsError)
{
    char* lpMsgBuf = nullptr; // @@0 alloc
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        a_dwError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&lpMsgBuf,
        0, NULL);

    a_rsError.append_cstr(lpMsgBuf);
    LocalFree(lpMsgBuf);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
astring GetLastErrorString(DWORD const a_dwError)
{
    astring sError;
    GetLastErrorString(a_dwError, sError);
    return sError;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
astring GetLastErrorString(void)
{
    astring sError;
    GetLastErrorString(GetLastError(), sError);
    return sError;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace fs {

EMP_RETURN bool create_directories(c_astring& EMP_SILENT(a_rsPath))
{
    return false;
}
EMP_RETURN bool delete_file(c_astring& EMP_SILENT(a_rsFile))
{
    return false;
}
EMP_RETURN bool copy_file(c_astring& EMP_SILENT(a_rsFrom), c_astring& EMP_SILENT(a_rsTo))
{
    return false;
}
EMP_RETURN bool move_file(c_astring& EMP_SILENT(a_rsFrom), c_astring& EMP_SILENT(a_rsTo))
{
    return false;
}
// EMP_RETURN bool compare_file(c_astring& EMP_SILENT(a_rsFrom), c_astring& EMP_SILENT(a_rsTo))
// {
//     return false;
// }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::str;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool compare_file(c_astring& a_rsFrom, c_astring& a_rsTo)
{
    emp::fs::file_c oFrom;
    EMP_RET_FALSE_IF_NOT(oFrom.create(a_rsFrom, file_e::Binary, create_e::Read), "failed to open file '#0'", a_rsFrom);

    astring sFrom{};
    EMP_RET_FALSE_IF_NOT(oFrom.read_all(sFrom), "failed to read file '#0'", a_rsFrom);
    EMP_RET_FALSE_IF(sFrom.is_empty(), "empty file '#0'", a_rsFrom);

    file_c oTo;
    EMP_RET_FALSE_IF_NOT(oTo.create(a_rsTo, file_e::Binary, create_e::Read), "failed to open file '#0'", a_rsTo);

    astring sTo{};
    EMP_RET_FALSE_IF_NOT(oTo.read_all(sTo), "failed to read file '#0'", a_rsTo);
    EMP_RET_FALSE_IF(sTo.is_empty(), "empty file '#0'", a_rsTo);

    EMP_RET_FALSE_IF(sFrom.len() != sTo.len());

    c_size stCount = sFrom.len();
    for (size_t stIndex = 0; stIndex < stCount; ++stIndex)
    {
        if (sFrom.get_char(stIndex) != sTo.get_char(stIndex))
        { return false; }
    }

    return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if defined (EMP_XX_COMPILER_MSC)
EMP_RETURN bool delete_file(c_astring& EMP_SILENT(a_rsFile))
{ EMP_RET_FALSE; }
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool path_exists(c_astring& a_sPath, bool& a_rbExists)
{
    a_rbExists = false;
    astring sPath(a_sPath);
#if defined (EMP_XX_COMPILER_MSC)
    WIN32_FIND_DATAA oFindData;
    // @@0 remove EMP_RET_FALSE_IF_NOT(a_sPath.len() + 3 < MAX_PATH);

    HANDLE hFind = FindFirstFileA(sPath.cstr(), &oFindData); // @@0 unicode
    if (hFind == INVALID_HANDLE_VALUE)
    {
        DWORD const dwError = GetLastError();
        EMP_RET_FALSE_IF(dwError != ERROR_FILE_NOT_FOUND, "GetLastError #0", emp::win::GetLastErrorString(dwError));
        return true;
    }
    else
    {
        // @@0 check file type if ((oFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
        EMP_RET_FALSE_IF_NOT(FindClose(hFind), "GetLastError #0", emp::win::GetLastErrorString());
        a_rbExists = true;
        return true;
    }
#else
    c_int siResult = access(sPath.cstr(), F_OK);
    if (siResult == 0)
    {
        a_rbExists = true;
        return true;
    }

    c_int siErrno = emp::ast::get_errno();
    EMP_RET_FALSE_IF(siErrno != ENOENT, "access(#0), errno : #1, #2", sPath, siErrno,
        astring::ref(emp::ast::errno_to_cstr(siErrno)));
    return true;
#endif
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
file_c::~file_c(void)
{
    destroy();
    EMP_ASSERT(-1 == m_siDescriptor);
    EMP_ASSERT(nullptr == m_pFile);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_INTERN EMP_RETURN bool is_little_endian(void);
EMP_INTERN EMP_RETURN bool is_little_endian(void)
{
    short int number = 0x1;
    char *numPtr = (char*)&number;
    return (numPtr[0] == 1);
}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool file_c::create(c_astring& a_sName, c_file_e a_eMod, c_create_e a_eCreate) //@@0 string
{
    EMP_RET_FALSE_IF(a_sName.is_empty(), "empty filename");

    EMP_ASSERT(-1 == m_siDescriptor);
    EMP_ASSERT(nullptr == m_pFile);

    m_sName = a_sName;

    cpc_char szName = m_sName.cstr();

    emp::ast::set_errno(0); // @@0 only on non msvc ??? fdopen ??

#if defined EMP_XX_COMPILER_MSC
    int ioFlag = 0;
    if (a_eMod == file_e::Binary)
    { ioFlag |= _O_BINARY; }
    if (a_bCreate)
    { ioFlag |= _O_CREAT | _O_RDWR | _O_EXCL; }
    else
    { ioFlag |= _O_RDONLY; }
    EMP_RET_FALSE_IF(_sopen_s(&m_siDescriptor, szName, ioFlag, _SH_DENYWR, _S_IREAD|_S_IWRITE) != 0, "failed to open #0", m_sName);
#else
    int ioFlag = 0; // @@0 check flags on nix.
    if (a_eMod == file_e::Binary)
    { ioFlag |= O_BINARY; }
    else if (a_eMod == file_e::Text)
    { ioFlag |= O_TEXT; }
    if (a_eCreate == create_e::New)
    { ioFlag |= O_CREAT | O_RDWR | O_EXCL; }
    else if (a_eCreate == create_e::Trunc)
    { ioFlag |= O_CREAT | O_RDWR | O_TRUNC; }
    else if (a_eCreate == create_e::Append)
    { ioFlag |= O_CREAT | O_RDWR; }
    else
    { ioFlag |= O_RDONLY; }
    m_siDescriptor = open(szName, ioFlag, 0744);
#endif
    EMP_RET_FALSE_IF(-1 == m_siDescriptor, "Failed to open #0, #1", a_sName, astring::ref(emp::ast::errno_to_cstr()));

    // @@0  : fdopen alternative, not available if __STRICT_ANSI__
#if defined EMP_XX_OS_WIN
#define XTMP_FDOPEN _fdopen
#else
#define XTMP_FDOPEN fdopen
#endif

    if (a_eMod == file_e::Binary)
    { m_pFile = XTMP_FDOPEN(m_siDescriptor, a_eCreate != create_e::Read ? "wb+" : "rb"); }
    else if (a_eMod == file_e::Text)
    { m_pFile = XTMP_FDOPEN(m_siDescriptor, a_eCreate != create_e::Read ? "w+" : "r"); }

#undef XTMP_FDOPEN
    EMP_RET_FALSE_IF(nullptr == m_pFile, "Failed to open #0, #1", a_sName, astring::ref(emp::ast::errno_to_cstr()));
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void file_c::destroy(void)
{
    if (m_pFile != nullptr)
    { fclose(m_pFile); }

    if (m_siDescriptor != -1)
    {
        //close(m_siDescriptor); // @@0 remove ?
    }

    m_siDescriptor = -1;
    m_pFile = nullptr;

    // @@0 :clear
    m_sName = astring();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool file_c::write(c_astring& a_sBuffer)
{
    EMP_RET_FALSE_IF(m_siDescriptor == -1);
    EMP_RET_FALSE_IF(m_pFile == nullptr);

    astring sBuffer(a_sBuffer);

    EMP_RET_FALSE_IF(fwrite(sBuffer.cstr(), sizeof(char), sBuffer.len(), m_pFile) != sBuffer.len()); // @@0 ferrro
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool file_c::read_all(astring& a_sBuffer) const //@@0 string
{
    EMP_RET_FALSE_IF(m_siDescriptor == -1);
    EMP_RET_FALSE_IF(m_pFile == nullptr);

    // struct stat oStats;
    emp::ast::set_errno(0);

    // EMP_RET_FALSE_IF(fstat(m_siDescriptor, &oStats) == -1,
    //     "failed to fstat #0, #1", m_siDescriptor, astring::ref(emp::ast::errno_to_cstr()));

    // size_t stLen = 0;
    // EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(oStats.st_size, stLen),
    //     "cannot convert #0 to size_t", oStats.st_size);

    EMP_RET_FALSE_IF(fseek(m_pFile, 0, SEEK_END) == -1,
        "Failed to fseek #0, #1", m_siDescriptor, astring::ref(emp::ast::errno_to_cstr()));

    long lLen = ftell(m_pFile);
    EMP_RET_FALSE_IF(lLen < 0, "Failed to ftell #0, #1", m_siDescriptor, astring::ref(ast::errno_to_cstr()));

    c_size stLen = static_cast<size_t>(lLen);

    emp::ast::set_errno(0);
    EMP_RET_FALSE_IF(fseek(m_pFile, 0, SEEK_SET) == -1,
        "Failed to fseek #0, #1", m_siDescriptor, astring::ref(emp::ast::errno_to_cstr()));

    c_size stSize = stLen + 1;
    a_sBuffer.resize(stSize);

    emp::ast::set_errno(0);
    c_size stCount = fread(a_sBuffer.get_first_char_ptr(), 1, stLen, m_pFile);

    EMP_RET_FALSE_IF(stCount > stLen);

    if (stCount != stLen)
    {
        if (ferror(m_pFile))
        {
            EMP_LOG_ERROR( "Failed to read file #0, #1", m_siDescriptor, astring::ref(emp::ast::errno_to_cstr()));
            clearerr(m_pFile);
            return false;
        }
    }

    a_sBuffer.set_char(stLen, 0);
    a_sBuffer.resize(stSize);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool file_c::read_chunk(astring& EMP_SILENT(a_sChunk), c_size a_stCount) //@@0 string
{
    if ((a_stCount > 0) == false)
    {
        EMP_LOG_INFO("Invalid chunk size #0", a_stCount);
        return false;
    }

    /* @@0 string
    a_sChunk.grow(a_stCount + 1);

    emp::ast::set_errno(0);
    c_size stCount = fread(a_sChunk.ptr(), 1, a_stCount, m_pFile);
    //EMP_LOG_DEBUG("Read count : #0", stCount);
    EMP_ASSERT(stCount <= a_stCount);

    if (stCount != a_stCount)
    {
        if (ferror(m_pFile))
        {
            EMP_LOG_ERROR("Failed to read file #0, #1", m_siDescriptor, astring(emp::ast::errno_to_cstr(), false));
            clearerr(m_pFile);
            return false;
        }
    }

    a_sChunk.set(stCount, 0);*/
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@0 factor
EMP_RETURN bool file_c::read(char* const a_pDest, c_size a_stCount)
{
    emp::ast::set_errno(0);
    c_size stCount = fread(a_pDest, 1, a_stCount, m_pFile);
    if (stCount != a_stCount)
    {
        if (ferror(m_pFile))
        {
            EMP_LOG_ERROR("Failed to read char from file '#0', '#1', '#2'",
                m_sName, m_siDescriptor, astring::ref(emp::ast::errno_to_cstr()));
            clearerr(m_pFile);
            return false;
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@0 factor
EMP_RETURN bool file_c::read_char(char& a_rValue)
{
    emp::ast::set_errno(0);
    c_size stCount = fread(&a_rValue, 1, 1, m_pFile);
    if (stCount != 1)
    {
        if (ferror(m_pFile))
        {
            EMP_LOG_ERROR("Failed to read char from file '#0', '#1', '#2'",
                m_sName, m_siDescriptor, astring::ref(emp::ast::errno_to_cstr()));
            clearerr(m_pFile);
            return false;
        }
    }
    return true;
}



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@0 factor
EMP_RETURN bool file_c::read_sint16(si16& a_rsi16Value, c_size a_stCount)
{
    emp::ast::set_errno(0);
    c_size stCount = fread(&a_rsi16Value, 1, a_stCount, m_pFile);
    if (stCount != a_stCount)
    {
        if (ferror(m_pFile))
        {
            EMP_LOG_ERROR("Failed to read char from file '#0', '#1', '#2'",
                m_sName, m_siDescriptor, astring::ref(emp::ast::errno_to_cstr()));
            clearerr(m_pFile);
            return false;
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@0 factor
EMP_RETURN bool file_c::read_uint16(ui16& a_rui16Value)
{
    emp::ast::set_errno(0);
    c_size stCount = fread(&a_rui16Value, 1, 2, m_pFile);
    if (stCount != 2)
    {
        if (ferror(m_pFile))
        {
            EMP_LOG_ERROR("Failed to read char from file '#0', '#1', '#2'",
                m_sName, m_siDescriptor, astring::ref(emp::ast::errno_to_cstr()));
            clearerr(m_pFile);
            return false;
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@0 factor
EMP_RETURN bool file_c::read_uint32(ui32& a_rui32Value)
{
    emp::ast::set_errno(0);
    c_size stCount = fread(&a_rui32Value, 1, 4, m_pFile);
    if (stCount != 4)
    {
        if (ferror(m_pFile))
        {
            EMP_LOG_ERROR("Failed to read char from file '#0', '#1', '#2'",
                m_sName, m_siDescriptor, astring::ref(emp::ast::errno_to_cstr()));
            clearerr(m_pFile);
            return false;
        }
    }
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_INTERN EMP_RETURN bool match(c_astring& a_sFilename, c_astring& a_sPattern)
{
    astring sFilename = a_sFilename;
    astring sPattern = a_sPattern;
    while (sFilename.len() && sPattern.len())
    {
        c_char cFilename = sFilename.get_first_char();
        c_char cPattern = sPattern.get_first_char();
//        EMP_LOG_DEBUG("CMP : #0, #1", sFilename, sPattern);
        if (emp::str::is_mul(cPattern))
        {
            astring sNewFilename(sFilename);
            astring sNewPattern(sPattern);
            sNewPattern.trim_left();
            while(sNewFilename.len())
            {
                if (match(sNewFilename, sNewPattern))
                { return true; }
                sNewFilename.trim_left();
            }
        }
        else
        {
            if (cFilename != cPattern)
            { return false; }
        }
        sFilename.trim_left();
        sPattern.trim_left();
    }
    return sFilename.len() == sPattern.len();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool list(c_astring& a_sDirectory, c_astring& EMP_SILENT(a_sPattern), vector<astring>& a_rvFilenames, c_bool EMP_SILENT(a_bAppendPath))
{
#if defined EMP_XX_COMPILER_MSC

    WIN32_FIND_DATAA oFindData;
    // @@0 remove LARGE_INTEGER liFileSize;

    EMP_RET_FALSE_IF_NOT(a_sDirectory.len() + 3 < MAX_PATH);

    astring sDirectory(a_sDirectory);
    sDirectory.append_string(ALITERAL("*"));

    HANDLE hFind = FindFirstFileA(sDirectory.cstr(), &oFindData); // @@0 unicode
    EMP_RET_FALSE_IF(hFind == INVALID_HANDLE_VALUE, "GetLastError #0", emp::win::GetLastErrorString()); //-V773

    bool bResult = true;
    auto const& oFinalize = emp::tt::finally([hFind, &bResult, &sDirectory]
    {
        if (FindClose(hFind) == false)
        {
            EMP_LOG_ERROR("Failed to close handle '#0'", sDirectory);
            bResult = false;
        }
    });
    emp::tt::silent(oFinalize);

    do
    {
        if ((oFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
        {
            a_rvFilenames.add(astring::dup(oFindData.cFileName));
        }
    }
    while (FindNextFileA(hFind, &oFindData) != 0);

    DWORD const dwError = GetLastError();
    EMP_RET_FALSE_IF(dwError != ERROR_NO_MORE_FILES, "GetLastError #0", emp::win::GetLastErrorString(dwError));

    return bResult; // @@0 check raii order
#else
    //@@0 ascii
    /*if (a_sDirectory.is_empty())
    {
        EMP_LOG_ERROR("Libfs does not handle empty directory name");
        return false;
    }

    if (a_sDirectory.is_ascii() == false)
    {
       EMP_LOG_ERROR("Libfs does not handle non ascii directory names : #0", a_sDirectory);
       return false;
    }*/

    EMP_RET_FALSE_IF(a_sPattern.is_empty(), "empty list pattern");
//        a_sPattern.is_ascii() == false)
//    {
  //      EMP_LOG_ERROR("does not handle non ascii search patterns : '#0'", a_sPattern);
    //    return false;
    //}

    //@@0 ascii u8string sDirectory = u8string::make(a_sDirectory); // @@0 const c_str
    // @@0 ascii EMP_ASSERT(sDirectory.is_empty() == false && sDirectory.is_ascii());

    emp::ast::set_errno(0);
    astring sDirectory(a_sDirectory);
    DIR* const pDirectory = opendir(sDirectory.cstr());
    if (pDirectory == nullptr)
    {
        EMP_LOG_ERROR("failed to opendir '#0' : '#1'", a_sDirectory, astring::ref(emp::ast::errno_to_cstr()));
        return false;
    }

    bool bReturn = true;

    while (true)
    {
        emp::ast::set_errno(0);
        dirent* const pEntry = readdir(pDirectory);
        if (pEntry)
        {
            c_astring sFilename(astring::ref(pEntry->d_name));
           // static_assert(aliteral(pEntry->d_name).len() > 0, "");
            // @@0 ascii if (sFilename.is_ascii())
            {
                if (match(sFilename, a_sPattern))
                {
                    if (a_bAppendPath)
                    {
                        astring sPath(a_sDirectory);
                        sPath.append_string(astring::dup(pEntry->d_name));
                        a_rvFilenames.add(sPath);
                    }
                    else
                    {
                        astring sPath;
                        a_rvFilenames.add(astring::dup(pEntry->d_name));
                    }
                }
            }
            //else
            //@@0 ascii { EMP_LOG_WARNING("Discarding non ascii filename : #0", sFilename); }
        }
        else
        {
            if (emp::ast::get_errno() != 0)
            {
                EMP_LOG_ERROR("Failed to readdir #0 : #1", a_sDirectory, astring::ref(emp::ast::errno_to_cstr()));
                bReturn = false;
            }
            break;
        }
    }

    emp::ast::set_errno(0);
    if (closedir(pDirectory) != 0)
    {
        EMP_LOG_ERROR("Failed to closedir #0 : #1", a_sDirectory, astring::ref(emp::ast::errno_to_cstr()));
        return false;
    }

    return bReturn;
#endif
}

EMP_RETURN bool remove(c_astring& a_sPath)
{
    emp::ast::set_errno(0);
    ::remove(astring(a_sPath).cstr());
    if (emp::ast::get_errno() != 0)
    {
        EMP_LOG_ERROR("Failed to remove file #0 : #1", a_sPath, astring::ref(emp::ast::errno_to_cstr()));
        return false;
    }
    return true;
}
EMP_RETURN bool rename(c_astring& a_sFrom, c_astring& a_sTo)
{
    emp::ast::set_errno(0);
    ::rename(astring(a_sFrom).cstr(), astring(a_sTo).cstr());
    if (emp::ast::get_errno() != 0)
    {
        EMP_LOG_ERROR("Failed to rename files #0 to #1 : #2", a_sFrom, a_sTo, astring::ref(emp::ast::errno_to_cstr()));
        return false;
    }
    return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

