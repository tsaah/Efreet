#pragma once

#ifdef MODULE_EXPORT_DEFINE
    #define MODULE_EXPORT __declspec(dllexport)
#else
    #define MODULE_EXPORT __declspec(dllimport)
#endif