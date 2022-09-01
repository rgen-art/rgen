//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_FS_FILE_H
#define EMP_FS_FILE_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace fs {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name file_e
#define enum_values\
    enum_value( Text   )\
    enum_value( Binary )
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define enum_name create_e
#define enum_values\
    enum_value( New)\
    enum_value( Trunc)\
    enum_value( Append)\
    enum_value( Read)
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool create_directories(c_astring& a_rsPath);
EMP_RETURN bool delete_file(c_astring& a_rsFile);
EMP_RETURN bool copy_file(c_astring& a_rsFrom, c_astring& a_rsTo);
EMP_RETURN bool move_file(c_astring& a_rsFrom, c_astring& a_rsTo);
EMP_RETURN bool compare_file(c_astring& a_rsFrom, c_astring& a_rsTo);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, file_c)
private:
    int m_siDescriptor = -1;
    FILE* m_pFile = nullptr;
    astring m_sName{};

public:
    file_c(void) = default;
    ~file_c(void);

public:
    EMP_RETURN bool create(c_astring& a_sName, c_file_e a_eMod, c_create_e a_eCreate);
    void destroy(void);
    
    EMP_RETURN bool write(c_astring& a_sBuffer);

    template <size_t t_stCount>
    EMP_RETURN bool read(array<ui8, t_stCount, true> a_aui8Dest)
    { return read(reinterpret_cast<char*>(a_aui8Dest.first_ptr()), t_stCount); }

    EMP_RETURN bool read(char* const a_pDest, c_size a_stCount);

    EMP_RETURN bool read_all(astring& a_rsBuffer) const;
    EMP_RETURN bool read_chunk(astring& a_sChunk, c_size a_stCount);

    EMP_RETURN bool read_char(char& a_rValue);
    EMP_RETURN bool read_sint16(si16& a_rtValue, c_size a_stCount);
    EMP_RETURN bool read_uint16(ui16& a_rtValue);
    EMP_RETURN bool read_uint32(ui32& a_rtValue);
    c_astring& name(void) const { return m_sName; }
    EMP_RETURN FILE* pointer(void) const { return m_pFile; }
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool list(c_astring& a_sDirectory, c_astring& a_sPattern, vector<astring>& a_rvFilenames, c_bool a_bAppendPath);
EMP_RETURN bool path_exists(c_astring& a_sPath, bool& a_rbExists);
EMP_RETURN bool remove(c_astring& a_sPath);
EMP_RETURN bool rename(c_astring& a_sFrom, c_astring& a_sTo);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if defined EMP_XX_OS_WIN
typedef unsigned long       DWORD;
namespace emp { namespace win {
void GetLastErrorString(DWORD const a_dwError, astring& a_rsError);
astring GetLastErrorString(DWORD const a_dwError);
astring GetLastErrorString(void);
#if defined EMP_XX_COMPILER_MSC
EMP_RETURN bool CharToWChar(cpc_char a_szFrom, vector<wchar_t>& a_rTo);
#endif
} }
#endif

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

