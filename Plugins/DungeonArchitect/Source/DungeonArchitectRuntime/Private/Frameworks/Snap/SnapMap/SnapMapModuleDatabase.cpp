//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/Snap/SnapMap/SnapMapModuleDatabase.h"


uint32 GetTypeHash(const FSnapMapModuleDatabaseItem& A) {
    return HashCombine(GetTypeHash(A.Level), GetTypeHash(A.Category));
}

