#pragma once

#include "static_assert.h"
#include "basic_types.h"
#include "Vector2D.h"
#include "Vector3D.h"
#include "Vector4D.h"
#include "LoggerProvider.h"
#include "asserts.h"

#ifdef _DEBUG
#define DEBUG_OP(x) x
#else
#define DEBUG_OP(x) (void(0))
#endif