//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#ifndef EMP_RFX_TYPE_I_H
#define EMP_RFX_TYPE_I_H

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace fct { template <typename> class function_t; } }
namespace emp { namespace loc { struct string_s; } }
namespace emp { namespace str { class formater_i; } }
namespace emp { namespace xml { class reader_c; } }
namespace emp { namespace xml { class writer_c; } }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace rfx {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
using namespace emp::fct;
using namespace emp::loc;
using namespace emp::str;
using namespace emp::tim;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct field_s;
class type_i;
class class_c;
class factory_c;
enum class type_e : ui32;
template <typename> class store_t;
struct type_s;
class store_i;

template <typename U>
using is_data = emp::tt::integral<bool, type_t<U>::pod>;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN factory_c& factory(void);
EMP_RETURN bool reflect(type_i& a_rType);
EMP_RETURN store_i* store(c_astring& a_sType);
template <typename T> EMP_RETURN store_t<T>* store(void);
template <typename T> EMP_RETURN type_s const* id(void);
EMP_RETURN size_t next_object_id(void);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
typedef fct::function_t<bool(*)(type_i* const, c_size, field_s const&)> reflect_f;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool read_token(c_astring& a_sToken, xml::reader_c& a_rReader, c_astring& a_sValue, c_bool a_bLog);
EMP_RETURN bool read_type(xml::reader_c& a_rReader, c_astring& a_sType, c_bool a_bLog);
EMP_RETURN bool read_name(xml::reader_c& a_rReader, c_astring& a_sName, c_bool a_bLog);
EMP_RETURN bool read_count(xml::reader_c& a_rReader, size_t& a_rstCount, c_bool a_bLog);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool read_file(xml::reader_c& a_rReader, c_size a_stDepth, c_astring& a_sDirectory, class_c& a_rClass);
EMP_RETURN bool read_object(xml::reader_c& a_rReader, c_size a_stDepth, c_astring& a_sDirectory, class_c& a_rClass);
EMP_RETURN bool read_value(xml::reader_c& a_rReader, c_size a_stDepth, c_astring& a_sDirectory, class_c& a_rClass);
EMP_RETURN bool read_ctor(c_astring& a_sDirectory, class_c& a_rClass);

EMP_RETURN bool eat_object(astring& a_rReader, class_c& a_rClass);
EMP_RETURN bool eat_value(astring& a_rReader, class_c& a_rClass);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// @@0 remove EMP_RETURN bool write_object(xml::writer_c& a_rWriter, type_i& a_rType);
EMP_RETURN bool write_attribute(xml::writer_c& a_rWriter, class_c& a_rClass, c_astring& a_sName);
EMP_RETURN bool write_object(xml::writer_c& a_rWriter, class_c& a_rClass);
EMP_RETURN bool write_element(xml::writer_c& a_rWriter, class_c& a_rClass);

EMP_RETURN bool write_count(xml::writer_c& a_rWriter, c_size a_stCount);
EMP_RETURN bool write_type(xml::writer_c& a_rWriter, c_astring& a_sType);

EMP_RETURN bool write_name(xml::writer_c& a_rWriter, c_astring& a_sName);

// @@0 remove EMP_RETURN bool write(xml::writer_c& a_rWriter, class_c& a_rClass);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void compute_xml_type(c_astring& a_sType, astring& a_rOut);
void compute_ctor_type(c_astring& a_sType, astring& a_rOut);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#define EMP_RFX_TYPE_E_NAME type_e
#define EMP_RFX_TYPE_E_VALUES\
    enum_value_ex( none        , ( undefined , undefined , false , false ) )\
    enum_value_ex( eBool       , ( bool      , bool      , true  , true  ) )\
    enum_value_ex( Char        , ( char      , char      , true  , true  ) )\
    enum_value_ex( Float32     , ( f32       , f32       , true  , true  ) )\
    enum_value_ex( SInt32      , ( si32      , si32      , true  , true  ) )\
    enum_value_ex( SInt64      , ( si64      , si64      , true  , true  ) )\
    enum_value_ex( UInt8       , ( ui8       , ui8       , true  , true  ) )\
    enum_value_ex( UInt16      , ( ui16      , ui16      , true  , true  ) )\
    enum_value_ex( UInt32      , ( ui32      , ui32      , true  , true  ) )\
    enum_value_ex( UInt64      , ( ui64      , ui64      , true  , true  ) )\
    enum_value_ex( AString     , ( astring   , astring   , true  , false ) )\
    enum_value_ex( U8String    , ( u8string  , u8string  , true  , false ) )\
    enum_value_ex( Time        , ( time_c    , time_c    , true  , true  ) )\
    enum_value_ex( Enum        , ( undefined , undefined , false , true  ) )\
    enum_value_ex( Class       , ( undefined , undefined , false , false ) )\
    enum_value_ex( Vector      , ( undefined , undefined , false , false ) )\
    enum_value_ex( Pointer     , ( undefined , undefined , false , false ) )\
    enum_value_ex( Function    , ( undefined , undefined , false , false ) )

#define enum_name EMP_RFX_TYPE_E_NAME
#define enum_values EMP_RFX_TYPE_E_VALUES
#include "emp_tt_enum.def.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
struct type_s
{
    type_e m_eType;
    astring m_sName;
    astring m_sSerializableName;
    astring m_sCtorFilename;
    bool m_bInterface;
    bool m_bPointer;

    /* @@0 remove type_s(void):
        m_eType(),
        m_sName(),
        m_sSerializableName(),
        m_sCtorFilename(),
        m_bInterface(),
        m_bPointer()
    {}*/

    type_s(c_astring& a_sName, c_type_e a_eType, c_bool a_bInterface):
        m_eType(a_eType),
        m_sName(a_sName),
        m_sSerializableName(),
        m_sCtorFilename(),
        m_bInterface(a_bInterface),
        m_bPointer(false)
    {
    }

    /* @@0 remove ~type_s(void)
    {}*/
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template <typename T>
EMP_NOINSTANCE(class, typeid_t)
private:
    friend class factory_c;
    static type_s* s_pInstance;
};
template <typename T> type_s* typeid_t<T>::s_pInstance = nullptr;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*EMP_NOCOPY(class, link_i)
public:
    link_i(void) {}
    virtual ~link_i(void) = 0;

public:
    // >> link_i
    //virtual void* object(void) = 0;
    // << link_i
};
inline link_i::~link_i(void) {}*/

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, type_i)
public:
    type_i(void) {}
    virtual ~type_i(void) = 0;

    // >> type_i
    virtual EMP_RETURN type_s const* id(void) const = 0;
    virtual EMP_RETURN bool bind(void) = 0;
    virtual EMP_RETURN bool read_object(xml::reader_c& a_rReader, c_size a_stDepth, c_astring& a_sDirectory) = 0;
    virtual EMP_RETURN bool read_attribute(xml::reader_c& a_rReader) = 0;
    virtual EMP_RETURN bool eat_object(astring& a_rReader) = 0;
    virtual EMP_RETURN bool post_load(void) = 0;
    virtual EMP_RETURN bool from_string(c_astring& a_sValue) = 0;
    virtual EMP_RETURN bool from_string(c_u8string& a_sValue) = 0;
    virtual EMP_RETURN bool to_string(str::formater_i* const a_pFormater, u8string& a_rsOut) const = 0;
    virtual EMP_RETURN size_t field_count(void) const = 0;
    virtual EMP_RETURN bool settable(void) const = 0;
    virtual EMP_RETURN bool bindable(void) const = 0;
    virtual EMP_RETURN bool iterate(c_size a_stDepth, reflect_f const& a_rCallback) = 0;
    virtual EMP_RETURN bool reflect(factory_c& a_rFactory) = 0;

    virtual EMP_RETURN bool write(xml::writer_c& a_rWriter) = 0;
    virtual EMP_RETURN bool write_object(xml::writer_c& a_rWriter) = 0;
    virtual EMP_RETURN bool write_attribute(xml::writer_c& a_rWriter, c_astring& a_sName) = 0;
    virtual EMP_RETURN bool write_element(xml::writer_c& a_rWriter) = 0;

    virtual EMP_RETURN bool is_attribute(void) const = 0;
    virtual EMP_RETURN bool is_writable(void) const = 0;

    // << type_i
};
inline type_i::~type_i(void) {}
EMP_FORWARD(class, type_i)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class,type_c, public, type_i)
private:
    type_s m_sId;
public:

    // >> type_i
    
    // << type_i
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class store_i;
template <typename> class store_t;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_STRUCT(object_s)
    astring name{};
    store_i* concrete = nullptr;
    void* pointer = nullptr;
    bool changed = false;
    bool file = false;
    bool variable = false;
    bool declared = false;
    bool written = false;

    object_s(c_astring& a_sName, store_i* const a_pConcrete, void* const a_pPointer, c_bool a_bFile, c_bool a_bVariable):
        name(a_sName), concrete(a_pConcrete), pointer(a_pPointer), changed(false), file(a_bFile), variable(a_bVariable)
    {}
};

EMP_PRAGMA_PUSH_IGNORE(EMP_W_EFFCPP)
template <typename T>
struct object_t
{
    store_t<T>* store = nullptr;
    T* pointer = nullptr;
    object_s* m_pInterface = nullptr;

    object_t(void)
    {}

    object_t(store_t<T>* const a_pStore, T* const a_pPointer, object_s* const a_pInterface):
        store(a_pStore), pointer(a_pPointer), m_pInterface(a_pInterface)
    {}
};
EMP_PRAGMA_POP_IGNORE(EMP_W_EFFCPP)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY(class, store_i)
public:
    store_i(void) {}
    virtual ~store_i(void) = 0;

public:


    // >> type_i
    virtual EMP_RETURN type_s const* id(void) const = 0;
    virtual EMP_RETURN bool is_child_of(type_s const* const a_pBase) const = 0;
    virtual EMP_RETURN bool read(c_astring& a_sPath) = 0;
    virtual EMP_RETURN void* read_void(xml::reader_c& a_rReader, c_size a_stDepth, c_astring& a_sDirectory, c_astring& a_sVariable) = 0;
    virtual EMP_RETURN bool instanciate(vector<astring>& a_rvsFiles, vector<astring>& a_rvsNames) = 0;
    virtual EMP_RETURN bool post_load(void) = 0;

    virtual EMP_RETURN bool write(c_astring& a_sPath, c_bool a_bAll) = 0;
    virtual EMP_RETURN bool write_void(xml::writer_c& a_rWriter, void* a_pObject) = 0;
    virtual EMP_RETURN bool write_attribute(c_astring& a_sName, xml::writer_c& a_rWriter) = 0;

    virtual void destroy(void) = 0;
    //virtual void destroy_concrete(void* a_pObject) = 0;
    //virtual EMP_RETURN size_t recursive_object_count(void) const = 0;
    //virtual EMP_RETURN object_s recursive_get_object(void* a_pObject) = 0;
    // << type_i

    // >> store_i
    //virtual EMP_RETURN bool create(factory_c* const a_fFactory) = 0;
    //virtual void destroy(void) = 0;

    //virtual EMP_RETURN bool write(c_astring& a_sPath, c_bool a_bAll) = 0;

    //virtual EMP_RETURN bool pre_load(void) = 0;

    //virtual EMP_RETURN bool instanciate(vector<astring>& a_rvsFiles, vector<astring>& a_rvsNames) = 0;

    //virtual EMP_RETURN c_astring& s_type(void) const = 0;
    //virtual EMP_RETURN c_astring& xml_type(void) const = 0;


    //virtual EMP_RETURN size_t get_all_object_count(void) const = 0;
    //virtual EMP_RETURN void* get_all_object(c_size a_stIndex) = 0;
    // << store_i

    //>> type_i
    /*
    virtual EMP_RETURN c_astring& xml_type(void) const = 0;

    virtual EMP_RETURN bool reflect(factory_c* const a_pFactory) = 0;
    virtual EMP_RETURN bool iterate(c_size a_stDepth, reflect_f const& a_rCallback) = 0;

    virtual EMP_RETURN bool read(reader_c& a_rReader, c_size a_stDepth, c_astring& a_sDirectory) = 0;
    virtual EMP_RETURN bool read_attribute(reader_c& EMP_SILENT(a_rReader)) { return false;}

    virtual EMP_RETURN bool write(writer_c& a_rWriter) = 0;
    virtual EMP_RETURN bool write_object(writer_c& a_rWriter) = 0;
    virtual EMP_RETURN bool write_element(writer_c& EMP_SILENT(a_rWriter)) { return false; }

    virtual EMP_RETURN size_t field_count(void) const = 0;

    virtual EMP_RETURN bool post_load(void) { return true; }*/
    //<< type_i
};
inline store_i::~store_i(void) {}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_NOCOPY_BASE(class, enum_i, public, type_i)
public:
    enum_i(void) {}
    virtual ~enum_i(void) = 0;

public:
    //>> enum_i
    virtual EMP_RETURN size_t enum_value_count(void) const = 0;
    virtual EMP_RETURN emp::loc::string_s const& enum_to_string(c_size a_stValue) const = 0;
    virtual void enum_from_index(c_size a_stVlalue) = 0;
    virtual EMP_RETURN size_t enum_current_index(void) const = 0;
    //<< enum_i
};
inline enum_i::~enum_i(void) {}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

