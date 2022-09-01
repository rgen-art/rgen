//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_png_util.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include "emp_fs_file.h"

#include "emp_mem_allocator.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#if defined EMP_XX_OS_WIN
#include <libpng16/png.h>
#else
#include <libpng16/png.h>
#endif

#include <stdio.h>
#include <jpeglib.h>
#include <setjmp.h>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace png {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void free_func(png_structp, void* a_pPointer);
void free_func(png_structp, void* a_pPointer)
{ EMP_DEALLOCATE_ARRAY(reinterpret_cast<char*>(a_pPointer)); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN void* malloc_func(png_structp, c_size a_stSize);
EMP_RETURN void* malloc_func(png_structp, c_size a_stSize)
{ return EMP_ALLOCATE_ARRAY(char, a_stSize); }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool load(c_astring& a_sPath, vector<ui8>& a_rvPixels, vector<ui8*>& a_rvpRows, ui32& a_rui32Width, ui32& a_rui32Height, ui32& a_rui32Depth)
{
    a_rvPixels.resize(0);
    a_rui32Width = 0;
    a_rui32Height = 0;
    a_rui32Depth = 0;

    emp::fs::file_c oFile;
    EMP_RET_FALSE_IF_NOT(oFile.create(a_sPath, emp::fs::file_e::Binary, fs::create_e::Read));

    cnt::array<unsigned char, 8, true> cHeader;
    EMP_RET_FALSE_IF_NOT(oFile.read(cHeader));
    EMP_RET_FALSE_IF_NOT(png_sig_cmp(cHeader.first_ptr(), 0, 8));

    png_structp pPngStruct = nullptr;
    png_infop pPngInfo = nullptr;

    pPngStruct = png_create_read_struct_2(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr, nullptr,
        &emp::png::malloc_func, &emp::png::free_func); // @@0 use allocater
    EMP_RET_FALSE_IF(pPngStruct == nullptr);

    pPngInfo = png_create_info_struct(pPngStruct);
    EMP_RET_FALSE_IF(pPngInfo == nullptr);

    auto oFinally = emp::tt::finally([&pPngStruct, &pPngInfo]
    {
		// TODO:RG leak ?
        //if (pPngInfo != nullptr)
        //{ png_free_data(pPngStruct, pPngInfo, PNG_FREE_ALL, -1); }
        if (pPngStruct != nullptr)
        { png_destroy_read_struct(&pPngStruct, &pPngInfo, nullptr); }
    });

    EMP_PRAGMA_PUSH_IGNORE(EMP_W_SETJMP_CPP)
    if (setjmp(png_jmpbuf(pPngStruct)))
    { return false; }
    EMP_PRAGMA_POP_IGNORE(EMP_W_SETJMP_CPP)

    png_init_io(pPngStruct, oFile.pointer());
    png_set_sig_bytes(pPngStruct, 8);

    png_read_info(pPngStruct, pPngInfo);

    a_rui32Width = png_get_image_width(pPngStruct, pPngInfo);
    a_rui32Height = png_get_image_height(pPngStruct, pPngInfo);
    auto bytes = png_get_rowbytes(pPngStruct, pPngInfo);
    auto color_type = png_get_color_type(pPngStruct, pPngInfo);
    if (color_type == PNG_COLOR_TYPE_RGB)
    {
        EMP_RET_FALSE_IF((bytes % 3) > 0);
        a_rui32Depth = 3;
    }
    else if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
    {
        EMP_RET_FALSE_IF((bytes % 4) > 0);
        a_rui32Depth = 4;
    }
    else
    { return false; }

    auto bit_depth = png_get_bit_depth(pPngStruct, pPngInfo);
    EMP_RET_FALSE_IF(bit_depth != 8);

    emp::tt::silent(color_type);
    emp::tt::silent(bit_depth);

    auto number_of_passes = png_set_interlace_handling(pPngStruct);
    emp::tt::silent(number_of_passes);

   //if (png_get_valid(pPngStruct, pPngInfo, PNG_INFO_tRNS))
   { png_set_tRNS_to_alpha(pPngStruct); }

    png_read_update_info(pPngStruct, pPngInfo);


    //bits per CHANNEL! note: not per pixel!

   // png_uint_32 newBitdepth   = png_get_bit_depth(pPngStruct, pPngInfo);
   // png_uint_32 newChannels   = png_get_channels(pPngStruct, pPngInfo);

    /*auto stride = newBitdepth * a_rui32Width * newChannels / 8;
    if (stride != bytes)
    {
        int kk = 0;
        ++kk;
    }*/
    EMP_RET_FALSE_IF_NOT((a_rui32Width > 0) && (a_rui32Height > 0));

    /* read file
     if (setjmp(png_jmpbuf(png_ptr)))
     abort_("[read_png_file] Error during read_image");*/
    a_rvpRows.resize(a_rui32Height);

    bytes = bytes + 3 - ((bytes-1) % 4);

    a_rvPixels.resize(bytes * a_rui32Height);
    for (size_t stIndex = 0; stIndex < a_rui32Height; ++stIndex)
    { a_rvpRows.set(stIndex, a_rvPixels.ptr((a_rui32Height - stIndex - 1) * bytes)); }

    png_read_image(pPngStruct, a_rvpRows.first_ptr());

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool save(c_astring& a_sPath, vector<ui32> const& a_rvPixels, vector<ui8>& EMP_SILENT(a_rvScratchPad), c_ui32 a_ui32Width, c_ui32 a_ui32Height, c_bool EMP_SILENT(a_bCreateIntermediateDirectories), fs::c_create_e a_eCreate, c_bool a_bAlpha) // @@0 implement
{
    // astring sKey(ALITERAL("Title"));
    // astring sText(ALITERAL("rgen"));

    png_structp pPngStruct = nullptr;
    png_infop pPngInfo = nullptr;

    // pPngStruct = png_create_write_struct_2(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr, nullptr,
    //     &emp::png::malloc_func, &emp::png::free_func); // @@0 use allocater
    pPngStruct = png_create_write_struct(PNG_LIBPNG_VER_STRING,
        nullptr, nullptr, nullptr);// , nullptr);// , &emp::png::malloc_func, &emp::png::free_func);

    EMP_RET_FALSE_IF(pPngStruct == nullptr);


    pPngInfo = png_create_info_struct(pPngStruct);
    EMP_RET_FALSE_IF(pPngInfo == nullptr);

    auto oFinally = emp::tt::finally([&pPngStruct, pPngInfo]
    {
        if (pPngInfo != nullptr)
        {
            png_free_data(pPngStruct, pPngInfo, PNG_FREE_ALL, -1);
        }
        if (pPngStruct != nullptr)
        {
            png_destroy_write_struct(&pPngStruct, (png_infopp)NULL);
        }
    });

    emp::fs::file_c oFile;
    EMP_RET_FALSE_IF_NOT(oFile.create(a_sPath, emp::fs::file_e::Binary, a_eCreate));

    EMP_PRAGMA_PUSH_IGNORE(EMP_W_SETJMP_CPP)
    if (setjmp(png_jmpbuf(pPngStruct)))
    {
        return false;
    }
    EMP_PRAGMA_POP_IGNORE(EMP_W_SETJMP_CPP)

    png_init_io(pPngStruct, oFile.pointer());
    png_set_IHDR(pPngStruct, pPngInfo, a_ui32Width, a_ui32Height, 8, a_bAlpha ? PNG_COLOR_TYPE_RGB_ALPHA : PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    // png_text oPngText;
    // oPngText.compression = PNG_TEXT_COMPRESSION_NONE;
    // oPngText.key = sKey.get_first_char_ptr();
    // oPngText.text = sText.get_first_char_ptr();
    // png_set_text(pPngStruct, pPngInfo, &oPngText, 1);

    png_write_info(pPngStruct, pPngInfo);

    c_ui32 components = a_bAlpha ? 4 : 3;
    c_ui32 stride = a_ui32Width * components;

    for (ui32 y = 0; y < a_ui32Height; ++y)
    {
        c_ui32 offset = (a_ui32Height - y - 1) * stride;
        c_ui8* pPixel = reinterpret_cast<unsigned char*>(const_cast<unsigned int*>(a_rvPixels.ptr(0))) + offset;
        png_write_row(pPngStruct, pPixel);
    }

    png_write_end(pPngStruct, NULL);

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp::jpg {
    
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool save(c_astring& a_sPath, vector<ui32> const& a_rvPixels, vector<ui8>& EMP_SILENT(a_rvScratchPad), c_ui32 a_ui32Width, c_ui32 a_ui32Height, c_bool EMP_SILENT(a_bCreateIntermediateDirectories), fs::c_create_e a_eCreate, c_bool a_bAlpha, int a_iQuality)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    astring filename(a_sPath);

    emp::fs::file_c oFile;
    EMP_RET_FALSE_IF_NOT(oFile.create(a_sPath, emp::fs::file_e::Binary, a_eCreate));

    jpeg_stdio_dest(&cinfo, oFile.pointer());

    cinfo.image_width = a_ui32Width;
    cinfo.image_height = a_ui32Height;
    cinfo.input_components = a_bAlpha ? 4 : 3;
    cinfo.in_color_space = a_bAlpha ? JCS_EXT_RGBA : JCS_RGB;
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, a_iQuality, TRUE);

    jpeg_start_compress(&cinfo, TRUE);

    JDIMENSION const stride = static_cast<ui32>(cinfo.input_components) * a_ui32Width;

    for (ui32 y = 0; y < a_ui32Height; ++y)
    {
        c_ui32 offset = (a_ui32Height - y - 1) * stride;
        row_pointer[0] = reinterpret_cast<unsigned char*>(const_cast<unsigned int*>(a_rvPixels.ptr(0))) + offset;
        (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

