//-----------------------------------------------------------------------------
// rgen.art
//-----------------------------------------------------------------------------
#include "emp_fs_file.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#import <Foundation/Foundation.h>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
namespace emp { namespace fs {

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool create_directories(c_astring& a_rsPath)
{
    astring sPath(a_rsPath);
    NSString* nssPath = [[NSString alloc] initWithCString:sPath.cstr() encoding:NSUTF8StringEncoding];

    NSError *error;

    c_bool bError =
        [[NSFileManager defaultManager]
            createDirectoryAtPath:nssPath
            withIntermediateDirectories:YES
            attributes:nil
            error:&error];
    if (error)
    {
        u8string s8Error;
        EMP_RET_FALSE_IF_NOT(u8string::dup([[error localizedDescription] UTF8String], s8Error));

        astring sError;
        EMP_RET_FALSE_IF_NOT(astring::from_u8string(sError, s8Error));

        EMP_LOG_ERROR("Failed to create directories #0, error : #1", a_rsPath, sError);
        return false;
    }

    return bError;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool delete_file(c_astring& a_rsPath)
{
    NSFileManager *fileManager = [NSFileManager defaultManager];

    astring sPath(a_rsPath);
    NSString* nssPath = [[NSString alloc] initWithCString:sPath.cstr() encoding:NSUTF8StringEncoding];

    NSError *error;
    c_bool bError = [fileManager removeItemAtPath:nssPath error:&error];
    if (error)
    {
        u8string s8Error;
        EMP_RET_FALSE_IF_NOT(u8string::dup([[error localizedDescription] UTF8String], s8Error));

        astring sError;
        EMP_RET_FALSE_IF_NOT(astring::from_u8string(sError, s8Error));

        EMP_LOG_ERROR("Failed to delete file #0, error : #1", a_rsPath, sError);
        return false;
    }

    return bError;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool copy_file(c_astring& a_rsFrom, c_astring& a_rsTo)
{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSError *error = nil;

    astring sPathFrom(a_rsFrom);
    NSString* nssPathFrom = [[NSString alloc] initWithCString:sPathFrom.cstr() encoding:NSUTF8StringEncoding];

    astring sPathTo(a_rsTo);
    NSString* nssPathTo = [[NSString alloc] initWithCString:sPathTo.cstr() encoding:NSUTF8StringEncoding];

    c_bool bError = [fileManager copyItemAtPath:nssPathFrom toPath:nssPathTo error:&error];
    if (error)
    {
        u8string s8Error;
        EMP_RET_FALSE_IF_NOT(u8string::dup([[error localizedDescription] UTF8String], s8Error));

        astring sError;
        EMP_RET_FALSE_IF_NOT(astring::from_u8string(sError, s8Error));

        EMP_LOG_ERROR("Failed to copy from #0, to #1, error : #2", a_rsFrom, a_rsTo, sError);
        return false;
    }

    return bError;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EMP_RETURN bool move_file(c_astring& a_rsFrom, c_astring& a_rsTo)
{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSError *error = nil;

    astring sPathFrom(a_rsFrom);
    NSString* nssPathFrom = [[NSString alloc] initWithCString:sPathFrom.cstr() encoding:NSUTF8StringEncoding];

    astring sPathTo(a_rsTo);
    NSString* nssPathTo = [[NSString alloc] initWithCString:sPathTo.cstr() encoding:NSUTF8StringEncoding];

    c_bool bError = [fileManager moveItemAtPath:nssPathFrom toPath:nssPathTo error:&error];
    if (error)
    {
        u8string s8Error;
        EMP_RET_FALSE_IF_NOT(u8string::dup([[error localizedDescription] UTF8String], s8Error));

        astring sError;
        EMP_RET_FALSE_IF_NOT(astring::from_u8string(sError, s8Error));

        EMP_LOG_ERROR("Failed to copy from #0, to #1, error : #2", a_rsFrom, a_rsTo, sError);
        return false;
    }

    return bError;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} }

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

