#if __CLC_FPSIZE == 32

_CLC_DEF_ldexp _CLC_OVERLOAD __CLC_GENTYPE __clc_ldexp(__CLC_GENTYPE x,
                                                       __CLC_INTN n) {
  if (!__clc_fp32_subnormals_supported()) {
    // This treats subnormals as zeros
    __CLC_INTN i = __CLC_AS_INTN(x);
    __CLC_INTN e = (i >> 23) & 0xff;
    __CLC_INTN m = i & 0x007fffff;
    __CLC_INTN s = i & (__CLC_INTN)0x80000000;
    __CLC_INTN v = __clc_add_sat(e, n);
    v = __clc_clamp(v, 0, 0xff);
    __CLC_INTN mr = __clc_select(m, 0, (e == 0 || v == 0 || v == 0xff));
    __CLC_INTN c = e == 0xff;
    mr = __clc_select(mr, m, c);
    __CLC_INTN er = __clc_select(v, e, c);
    er = __clc_select(e, er, e != 0);
    return __CLC_AS_GENTYPE(s | (er << 23) | mr);
  }

  // supports denormal values
  const __CLC_INTN multiplier = 24;
  __CLC_GENTYPE val_f;
  __CLC_UINTN val_ui;
  __CLC_UINTN sign;
  __CLC_INTN exponent;
  val_ui = __CLC_AS_UINTN(x);
  sign = val_ui & 0x80000000;
  // remove the sign bit
  val_ui = val_ui & 0x7fffffff;
  __CLC_INTN val_x = __CLC_AS_INTN(val_ui);
  // get the exponent
  exponent = __CLC_AS_INTN(val_ui >> 23);
  __CLC_INTN dexp = exponent;

  // denormal support
  __CLC_INTN fbh = __CLC_AS_INTN(
      (__CLC_UINTN)127 -
      (__CLC_AS_UINTN((__CLC_AS_GENTYPE(val_ui | 0x3f800000) - 1.0f)) >> 23));
  __CLC_INTN dexponent = 25 - fbh;
  __CLC_UINTN dval_ui =
      (((val_ui << fbh) & 0x007fffff) | (__CLC_AS_UINTN(dexponent) << 23));
  __CLC_INTN ex = dexponent + n - multiplier;
  dexponent = ex;
  __CLC_UINTN val = sign | (__CLC_AS_UINTN(ex) << 23) | (dval_ui & 0x007fffff);
  __CLC_INTN ex1 = dexponent + multiplier;
  ex1 = -ex1 + 25;
  dval_ui = (((dval_ui & 0x007fffff) | 0x800000) >> ex1);
  dval_ui = __clc_select(dval_ui, val, dexponent > (__CLC_INTN)0);
  // overflow
  dval_ui = __clc_select(dval_ui, (__CLC_UINTN)0x7f800000, dexponent > 254);
  // underflow
  dval_ui = __clc_select(dval_ui, (__CLC_UINTN)0, dexponent < -multiplier);
  dval_ui = dval_ui | sign;
  val_f = __CLC_AS_GENTYPE(dval_ui);

  exponent += n;

  val = sign | __CLC_AS_UINTN(exponent << 23) | (val_ui & 0x007fffff);
  ex1 = exponent + multiplier;
  ex1 = -ex1 + 25;
  val_ui = (((val_ui & 0x007fffff) | 0x800000) >> ex1);
  val_ui = __clc_select(val_ui, val, exponent > 0);
  // overflow
  val_ui = __clc_select(val_ui, (__CLC_UINTN)0x7f800000, exponent > 254);
  // underflow
  val_ui = __clc_select(val_ui, (__CLC_UINTN)0, exponent < -multiplier);
  val_ui = val_ui | sign;

  val_ui = __clc_select(val_ui, dval_ui, dexp == 0);
  val_f = __CLC_AS_GENTYPE(val_ui);

  val_f =
      __clc_select(val_f, x, (__clc_isnan(x) || __clc_isinf(x) || val_x == 0));
  return val_f;
}

#endif

#if __CLC_FPSIZE == 64

_CLC_DEF_ldexp _CLC_OVERLOAD __CLC_GENTYPE __clc_ldexp(__CLC_GENTYPE x,
                                                       __CLC_INTN n) {
  __CLC_LONGN l = __CLC_AS_LONGN(x);
  __CLC_INTN e = __CLC_CONVERT_INTN((l >> 52) & 0x7ff);
  __CLC_LONGN s = l & (__CLC_LONGN)0x8000000000000000;

  __CLC_ULONGN ux = __CLC_AS_ULONGN(x * 0x1.0p+53);
  __CLC_INTN de = __CLC_CONVERT_INTN((ux >> 52) & 0x7ff) - 53;
  __CLC_INTN c = e == 0;
  e = __clc_select(e, de, c);

  ux = __clc_select(__CLC_AS_ULONGN(l), ux, __CLC_CONVERT_LONGN(c));

  __CLC_INTN v = e + n;
  v = __clc_clamp(v, -0x7ff, 0x7ff);

  ux &= ~EXPBITS_DP64;

  __CLC_GENTYPE mr =
      __CLC_AS_GENTYPE(ux | (__CLC_CONVERT_ULONGN(v) + 53) << 52);
  mr = mr * 0x1.0p-53;

  mr = __clc_select(mr, __CLC_AS_GENTYPE(ux | (__CLC_CONVERT_ULONGN(v) << 52)),
                    __CLC_CONVERT_LONGN(v > 0));

  mr = __clc_select(mr, __CLC_AS_GENTYPE(s | PINFBITPATT_DP64),
                    __CLC_CONVERT_LONGN(v == 0x7ff));
  mr = __clc_select(mr, __CLC_AS_GENTYPE(s), __CLC_CONVERT_LONGN(v < -53));

  mr = __clc_select(mr, x,
                    __CLC_CONVERT_LONGN(__CLC_CONVERT_LONGN(n == 0) |
                                        __clc_isinf(x) |
                                        __CLC_CONVERT_LONGN(x == 0)));
  return mr;
}

#endif

#if __CLC_FPSIZE == 16

_CLC_DEF_ldexp _CLC_OVERLOAD __CLC_GENTYPE __clc_ldexp(__CLC_GENTYPE x,
                                                       __CLC_INTN n) {
  return __CLC_CONVERT_GENTYPE(__clc_ldexp(__CLC_CONVERT_FLOATN(x), n));
}

#endif
