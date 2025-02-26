//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include <clc/integer/clc_add_sat.h>
#include <clc/internal/clc.h>
#include <clc/math/clc_subnormal_config.h>
#include <clc/math/math.h>
#include <clc/relational/clc_isinf.h>
#include <clc/relational/clc_isnan.h>
#include <clc/relational/clc_select.h>
#include <clc/shared/clc_clamp.h>

#define _CLC_DEF_ldexp _CLC_DEF __attribute__((weak))

#define __CLC_BODY <clc_ldexp.inc>
#include <clc/math/gentype.inc>
