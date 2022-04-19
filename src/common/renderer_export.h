#pragma once

#ifdef RENDERER_EXPORT_DEFINE
    #define RENDERER_EXPORT __declspec(dllexport)
#else
    #define RENDERER_EXPORT __declspec(dllimport)
#endif