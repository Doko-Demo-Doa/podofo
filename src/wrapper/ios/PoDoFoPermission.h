#import <Foundation/Foundation.h>

/// Bitmask flags for -[PoDoFoDocument setEncryptedWithUserPassword:ownerPassword:permissions:],
/// mirroring PoDoFo's PdfPermissions enum values exactly (these are the
/// actual bit positions defined by the PDF spec, not an arbitrarily-assigned
/// numbering) so they pass straight through to native code unchanged.
typedef NS_OPTIONS(NSUInteger, PoDoFoPermission) {
    PoDoFoPermissionPrint       = 0x00000004,
    PoDoFoPermissionEdit        = 0x00000008,
    PoDoFoPermissionCopy        = 0x00000010,
    PoDoFoPermissionEditNotes   = 0x00000020,
    PoDoFoPermissionFillAndSign = 0x00000100,
    PoDoFoPermissionAccessible  = 0x00000200,
    PoDoFoPermissionDocAssembly = 0x00000400,
    PoDoFoPermissionHighPrint   = 0x00000800,
};

/// Every permission granted — matches PoDoFo's own PdfPermissions::Default.
static const PoDoFoPermission PoDoFoPermissionDefault =
    PoDoFoPermissionPrint | PoDoFoPermissionEdit | PoDoFoPermissionCopy | PoDoFoPermissionEditNotes
    | PoDoFoPermissionFillAndSign | PoDoFoPermissionAccessible | PoDoFoPermissionDocAssembly | PoDoFoPermissionHighPrint;
