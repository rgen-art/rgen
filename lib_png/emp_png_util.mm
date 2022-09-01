//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_png_util.h"

#include "emp_fs_file.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <ImageIO/ImageIO.h>
#import <MobileCoreServices/MobileCoreServices.h>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace png {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool load(c_astring& a_sPath, vector<ui8>& a_rvPixels, vector<ui8*>&, ui32& a_rui32Width, ui32& a_rui32Height, ui32& a_rui32Depth)
{
    a_rvPixels.resize(0);
    a_rui32Width = 0;
    a_rui32Height = 0;
    a_rui32Depth = 0;

    astring sPath(a_sPath);

    NSString* nssPath = [[NSString alloc] initWithCString:sPath.cstr() encoding:NSUTF8StringEncoding];

    //UIImage *puiImage = [UIImage imageWithContentsOfFile:nssPath];
    //EMP_RET_FALSE_IF(puiImage == nullptr, "Failed to load '#0'", sPath);
    //CGImageRef cgImageRef = [puiImage CGImage];

    CGDataProviderRef imgDataProvider = CGDataProviderCreateWithCFData((CFDataRef)[NSData dataWithContentsOfFile:nssPath]);
    CGImageRef cgImageRef = CGImageCreateWithPNGDataProvider(imgDataProvider, NULL, true, kCGRenderingIntentDefault);

    EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(CGImageGetWidth(cgImageRef), a_rui32Width));
    EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(CGImageGetHeight(cgImageRef), a_rui32Height));

    ui32 uiBitsPerPixel = 0;
    EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(CGImageGetBitsPerPixel(cgImageRef), uiBitsPerPixel));

    ui32 uiBitsPerComponent = 0;
    EMP_RET_FALSE_IF_NOT(emp::ast::try_cast(CGImageGetBitsPerComponent(cgImageRef), uiBitsPerComponent));

    a_rui32Depth = uiBitsPerPixel / uiBitsPerComponent;

    CGColorSpaceRef cgColorSpace = CGColorSpaceCreateDeviceRGB();

    a_rvPixels.resize(a_rui32Width * a_rui32Height * a_rui32Depth);
    EMP_RET_FALSE_IF_NOT(a_rvPixels.count() > 0);

    CGContextRef cgContext = CGBitmapContextCreate(a_rvPixels.first_ptr(), a_rui32Width, a_rui32Height,
                    uiBitsPerComponent, a_rui32Depth * a_rui32Width, cgColorSpace,
                    kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
    CGColorSpaceRelease(cgColorSpace);

    CGContextDrawImage(cgContext, CGRectMake(0, 0, a_rui32Width, a_rui32Height), cgImageRef);
    CGContextRelease(cgContext);

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool save(c_astring& a_sPath EMP_UNUSED, vector<ui32> const& a_rvPixels, vector<ui8>&, c_ui32 a_ui32Width, c_ui32 a_ui32Height, c_bool a_bCreateIntermediateDirectories, c_bool)
{
    c_size stByteWidth = a_ui32Width * 4;
    c_size stSize = stByteWidth * a_ui32Height;
    EMP_ASSERT(stSize == (a_rvPixels.count() * 4));

    EMP_RET_FALSE_IF_NOT(stSize == (a_rvPixels.count() * 4));

    CGDataProviderRef rDataProvider = CGDataProviderCreateWithData(NULL, a_rvPixels.first_const_ptr(), stSize, NULL);
    CGColorSpaceRef rColorSpace = CGColorSpaceCreateDeviceRGB();
    CGImageRef rImage = CGImageCreate(a_ui32Width, a_ui32Height, 8, 32, stByteWidth, rColorSpace, kCGBitmapByteOrderDefault | kCGImageAlphaLast, rDataProvider, NULL, NO, kCGRenderingIntentDefault);

    // Set layer contents???
    //UIImage *puiImage = [UIImage imageWithCGImage:rImage];


    astring sPath(a_sPath);
    astring sDirectory;
    astring sFilename;
    if (a_bCreateIntermediateDirectories)
    {
        extract_PF(sPath, sDirectory, sFilename);
        EMP_RET_FALSE_IF_NOT(emp::fs::create_directories(sDirectory));
    }

    bool bExists = false;
    EMP_RET_FALSE_IF_NOT(emp::fs::path_exists(a_sPath, bExists));
    if (bExists)
    { EMP_RET_FALSE_IF_NOT(emp::fs::delete_file(a_sPath)); }

    NSString* nssPath = [[NSString alloc] initWithCString:sPath.cstr() encoding:NSUTF8StringEncoding];
    CFURLRef rUrl = (__bridge CFURLRef)[NSURL fileURLWithPath:nssPath];

    CGImageDestinationRef rDestination = CGImageDestinationCreateWithURL(rUrl, kUTTypePNG, 1, NULL);
    EMP_RET_FALSE_IF(rDestination == nil);
    CGImageDestinationAddImage(rDestination, rImage, nil);

    EMP_RET_FALSE_IF_NOT(CGImageDestinationFinalize(rDestination));

    CFRelease(rDestination);

    CGDataProviderRelease(rDataProvider);
    CGColorSpaceRelease(rColorSpace);
    CGImageRelease(rImage);

    return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

