
/*
 *  This file is part of the lgmp package for Lua 5.2.
 *
 *  Author: Wim Couwenberg 
 *  Date  : 2012/1/15
 *
 *  The lgmp package is distributed under the MIT license.  See the file
 *  "COPYRIGHT" that came with the lgmp package distribution for license
 *  details.
 */

#include <stdlib.h>
#include <string.h>

#include "lua.h"
#include "lauxlib.h"
#include "gmp.h"

#ifndef LGMP_API
#	if defined _WIN32
#		define LGMP_API __declspec(dllexport)
#	else
#		define LGMP_API extern
#	endif
#endif

static const char lgmp_metarand[] = "randmeta";
static const char lgmp_metaz[] = "zmeta";
static const char lgmp_metaf[] = "fmeta";

static void lgmp_free_str(char *data)
{
	void (*current_free)(void *data, size_t size);

	mp_get_memory_functions(NULL, NULL, &current_free);

	if (current_free == NULL)
		free(data);
	else
		(*current_free)(data, strlen(data) + 1);
}

static gmp_randstate_t *lgmp_rawrand(lua_State *L)
{
	gmp_randstate_t *pr = (gmp_randstate_t *)lua_newuserdata(L, sizeof(gmp_randstate_t));
	lua_getfield(L, lua_upvalueindex(1), lgmp_metarand);
	lua_setmetatable(L, -2);

	return pr;
}

static gmp_randstate_t *lgmp_torand(lua_State *L, int idx)
{
	return (gmp_randstate_t *)lua_touserdata(L, idx);
}

static mpz_t *lgmp_toz(lua_State *L, int idx)
{
	return (mpz_t *)lua_touserdata(L, idx);
}

static mpz_t *lgmp_rawz(lua_State *L)
{
	mpz_t *pz = (mpz_t *)lua_newuserdata(L, sizeof(mpz_t));
	lua_getfield(L, lua_upvalueindex(1), lgmp_metaz);
	lua_setmetatable(L, -2);

	return pz;
}

static mpz_t *lgmp_optz(lua_State *L, int idx, int top)
{
	mpz_t *pz;

	if (top > 0)
		lua_settop(L, top);

	if (!lua_isnil(L, idx))
		pz = lgmp_toz(L, idx);
	else
	{
		pz = lgmp_rawz(L);
		mpz_init(*pz);
		lua_replace(L, idx);
	}

	return pz;
}

static mpf_t *lgmp_tof(lua_State *L, int idx)
{
	return (mpf_t *)lua_touserdata(L, idx);
}

static mpf_t *lgmp_rawf(lua_State *L)
{
	mpf_t *pf = (mpf_t *)lua_newuserdata(L, sizeof(mpf_t));
	lua_getfield(L, lua_upvalueindex(1), lgmp_metaf);
	lua_setmetatable(L, -2);

	return pf;
}

static mpf_t *lgmp_optf(lua_State *L, int idx, int top)
{
	mpf_t *pf;

	if (top > 0)
		lua_settop(L, top);

	if (!lua_isnil(L, idx))
		pf = lgmp_tof(L, idx);
	else
	{
		pf = lgmp_rawf(L);
		mpf_init(*pf);
		lua_replace(L, idx);
	}

	return pf;
}

static int lmpz_asprintf(lua_State *L)
{
	char *str;
	const char *a1 = lua_tostring(L, 1);
	mpz_t *a2 = lgmp_toz(L, 2);

	if (lua_isnil(L, 3))
		gmp_asprintf(&str, a1, *a2);
	else
	{
		int a3 = lua_tointeger(L, 3);
		gmp_asprintf(&str, a1, a3, *a2);
	}

	lua_pushstring(L, str);
	lgmp_free_str(str);

	return 1;
}

static int lmpf_asprintf(lua_State *L)
{
	char *str;
	const char *a1 = lua_tostring(L, 1);
	mpf_t *a2 = lgmp_tof(L, 2);

	if (lua_isnil(L, 3))
		gmp_asprintf(&str, a1, *a2);
	else
	{
		int a3 = lua_tointeger(L, 3);
		gmp_asprintf(&str, a1, a3, *a2);
	}

	lua_pushstring(L, str);
	lgmp_free_str(str);

	return 1;
}

static int lgmp_randinit_default(lua_State *L)
{
	gmp_randstate_t *res = lgmp_rawrand(L);
	
	gmp_randinit_default(*res);

	return 1;
}

static int lgmp_randinit_lc_2exp(lua_State *L)
{
	gmp_randstate_t *res = lgmp_rawrand(L);
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);
	unsigned long arg3 = (unsigned long)lua_tonumber(L, 3);

	gmp_randinit_lc_2exp(*res, *arg1, arg2, arg3);

	return 1;
}

static int lgmp_randinit_lc_2exp_size(lua_State *L)
{
	gmp_randstate_t *res = lgmp_rawrand(L);
	unsigned long arg = (unsigned long)lua_tonumber(L, 1);

	lua_pushinteger(L, gmp_randinit_lc_2exp_size(*res, arg));

	return 2;
}

static int lgmp_randinit_mt(lua_State *L)
{
	gmp_randstate_t *res = lgmp_rawrand(L);

	gmp_randinit_mt(*res);

	return 1;
}

static int lgmp_randinit_set(lua_State *L)
{
	gmp_randstate_t *res = lgmp_rawrand(L);
	gmp_randstate_t *arg = lgmp_torand(L, 1);

	gmp_randinit_set(*res, *arg);

	return 1;
}

static int lgmp_randseed(lua_State *L)
{
	gmp_randstate_t *arg1 = lgmp_torand(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	gmp_randseed(*arg1, *arg2);

	return 0;
}

static int lgmp_randseed_ui(lua_State *L)
{
	gmp_randstate_t *arg1 = lgmp_torand(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	gmp_randseed_ui(*arg1, arg2);

	return 0;
}

static int lgmp_randclear(lua_State *L)
{
	gmp_randstate_t *arg = lgmp_torand(L, 1);

	gmp_randclear(*arg);

	return 0;
}

static int lmpz_abs(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 2, 2);
	mpz_t *src = lgmp_toz(L, 1);

	mpz_abs(*res, *src);

	return 1;
}

static int lmpz_add(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	mpz_add(*res, *arg1, *arg2);

	return 1;
}

static int lmpz_add_ui(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpz_add_ui(*res, *arg1, arg2);

	return 1;
}

static int lmpz_addmul(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);
	mpz_t *arg3 = lgmp_toz(L, 3);

	mpz_addmul(*arg1, *arg2, *arg3);

	return 0;
}

static int lmpz_addmul_ui(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);
	unsigned long arg3 = (unsigned long)lua_tonumber(L, 3);

	mpz_addmul_ui(*arg1, *arg2, arg3);

	return 0;
}

static int lmpz_and(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	mpz_and(*res, *arg1, *arg2);

	return 1;
}

#if 0
#define mpz_array_init __gmpz_array_init
__GMP_DECLSPEC void mpz_array_init __GMP_PROTO ((mpz_ptr, mp_size_t, mp_size_t));
#endif

static int lmpz_bin_ui(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpz_bin_ui(*res, *arg1, arg2);

	return 1;
}

static int lmpz_bin_uiui(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	unsigned long arg1 = (unsigned long)lua_tonumber(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpz_bin_uiui(*res, arg1, arg2);

	return 1;
}

static int lmpz_cdiv_q(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	mpz_cdiv_q(*res, *arg1, *arg2);

	return 1;
}

static int lmpz_cdiv_q_2exp(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpz_cdiv_q_2exp(*res, *arg1, arg2);

	return 1;
}

static int lmpz_cdiv_q_ui(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	lua_pushnumber(L, mpz_cdiv_q_ui(*res, *arg1, arg2));

	return 2;
}

static int lmpz_cdiv_qr(lua_State *L)
{
	mpz_t *resq = lgmp_optz(L, 3, 4);
	mpz_t *resr = lgmp_optz(L, 4, 0);
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	mpz_cdiv_qr(*resq, *resr, *arg1, *arg2);

	return 2;
}

static int lmpz_cdiv_qr_ui(lua_State *L)
{
	mpz_t *resq = lgmp_optz(L, 3, 4);
	mpz_t *resr = lgmp_optz(L, 4, 0);
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	lua_pushnumber(L, mpz_cdiv_qr_ui(*resq, *resr, *arg1, arg2));

	return 3;
}

static int lmpz_cdiv_r(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	mpz_cdiv_r(*res, *arg1, *arg2);

	return 1;
}

static int lmpz_cdiv_r_2exp(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpz_cdiv_r_2exp(*res, *arg1, arg2);

	return 1;
}

static int lmpz_cdiv_r_ui(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	lua_pushnumber(L, mpz_cdiv_r_ui(*res, *arg1, arg2));

	return 2;
}

static int lmpz_cdiv_ui(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	lua_pushnumber(L, mpz_cdiv_ui(*arg1, arg2));

	return 1;
}

static int lmpz_clear(lua_State *L)
{
	mpz_t *arg = lgmp_toz(L, 1);

	mpz_clear(*arg);

	return 0;
}

static int lmpz_clrbit(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpz_clrbit(*arg1, arg2);

	return 0;
}

static int lmpz_cmp(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	lua_pushinteger(L, mpz_cmp(*arg1, *arg2));

	return 1;
}

static int lmpz_cmp_d(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	double arg2 = lua_tonumber(L, 2);

	lua_pushinteger(L, mpz_cmp_d(*arg1, arg2));

	return 1;
}

static int lmpz_cmp_si(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	signed long arg2 = (signed long)lua_tonumber(L, 2);

	lua_pushinteger(L, mpz_cmp_si(*arg1, arg2));

	return 1;
}

static int lmpz_cmp_ui(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	lua_pushinteger(L, mpz_cmp_ui(*arg1, arg2));

	return 1;
}

static int lmpz_cmpabs(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	lua_pushinteger(L, mpz_cmpabs(*arg1, *arg2));

	return 1;
}

static int lmpz_cmpabs_d(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	double arg2 = lua_tonumber(L, 2);

	lua_pushinteger(L, mpz_cmpabs_d(*arg1, arg2));

	return 1;
}

static int lmpz_cmpabs_ui(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	lua_pushinteger(L, mpz_cmpabs_ui(*arg1, arg2));

	return 1;
}

static int lmpz_com(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 2, 2);
	mpz_t *arg = lgmp_toz(L, 1);

	mpz_com(*res, *arg);

	return 1;
}

static int lmpz_combit(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpz_combit(*arg1, arg2);

	return 0;
}

static int lmpz_congruent_p(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);
	mpz_t *arg3 = lgmp_toz(L, 3);

	lua_pushinteger(L, mpz_congruent_p(*arg1, *arg2, *arg3));

	return 1;
}

static int lmpz_congruent_2exp_p(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);
	unsigned long arg3 = (unsigned long)lua_tonumber(L, 3);

	lua_pushinteger(L, mpz_congruent_2exp_p(*arg1, *arg2, arg3));

	return 1;
}

static int lmpz_congruent_ui_p(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);
	unsigned long arg3 = (unsigned long)lua_tonumber(L, 3);

	lua_pushinteger(L, mpz_congruent_ui_p(*arg1, arg2, arg3));

	return 1;
}

static int lmpz_divexact(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	mpz_divexact(*res, *arg1, *arg2);

	return 1;
}

static int lmpz_divexact_ui(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpz_divexact_ui(*res, *arg1, arg2);

	return 1;
}

static int lmpz_divisible_p(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	lua_pushinteger(L, mpz_divisible_p(*arg1, *arg2));

	return 1;
}

static int lmpz_divisible_ui_p(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	lua_pushinteger(L, mpz_divisible_ui_p(*arg1, arg2));

	return 1;
}

static int lmpz_divisible_2exp_p(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	lua_pushinteger(L, mpz_divisible_2exp_p(*arg1, arg2));

	return 1;
}

#if 0
#define mpz_dump __gmpz_dump
__GMP_DECLSPEC void mpz_dump __GMP_PROTO ((mpz_srcptr));

#define mpz_export __gmpz_export
__GMP_DECLSPEC void *mpz_export __GMP_PROTO ((void *, size_t *, int, size_t, int, size_t, mpz_srcptr));
#endif

static int lmpz_fac_ui(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 2, 2);
	unsigned long arg = (unsigned long)lua_tonumber(L, 1);

	mpz_fac_ui(*res, arg);

	return 1;
}

static int lmpz_fdiv_q(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	mpz_fdiv_q(*res, *arg1, *arg2);

	return 1;
}

static int lmpz_fdiv_q_2exp(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpz_fdiv_q_2exp(*res, *arg1, arg2);

	return 1;
}

static int lmpz_fdiv_q_ui(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	lua_pushnumber(L, mpz_fdiv_q_ui(*res, *arg1, arg2));

	return 2;
}

static int lmpz_fdiv_qr(lua_State *L)
{
	mpz_t *resq = lgmp_optz(L, 3, 4);
	mpz_t *resr = lgmp_optz(L, 4, 0);
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	mpz_fdiv_qr(*resq, *resr, *arg1, *arg2);

	return 2;
}

static int lmpz_fdiv_qr_ui(lua_State *L)
{
	mpz_t *resq = lgmp_optz(L, 3, 4);
	mpz_t *resr = lgmp_optz(L, 4, 0);
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	lua_pushnumber(L, mpz_fdiv_qr_ui(*resq, *resr, *arg1, arg2));

	return 3;
}

static int lmpz_fdiv_r(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	mpz_fdiv_r(*res, *arg1, *arg2);

	return 1;
}

static int lmpz_fdiv_r_2exp(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpz_fdiv_r_2exp(*res, *arg1, arg2);

	return 1;
}

static int lmpz_fdiv_r_ui(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	lua_pushnumber(L, mpz_fdiv_r_ui(*res, *arg1, arg2));

	return 2;
}

static int lmpz_fdiv_ui(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	lua_pushnumber(L, mpz_fdiv_ui(*arg1, arg2));

	return 1;
}

static int lmpz_fib_ui(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 2, 2);
	unsigned long arg = (unsigned long)lua_tonumber(L, 1);

	mpz_fib_ui(*res, arg);

	return 1;
}

static int lmpz_fib2_ui(lua_State *L)
{
	mpz_t *res1 = lgmp_optz(L, 2, 3);
	mpz_t *res2 = lgmp_optz(L, 3, 0);
	unsigned long arg = (unsigned long)lua_tonumber(L, 1);

	mpz_fib2_ui(*res1, *res2, arg);

	return 2;
}

static int lmpz_fits_sint_p(lua_State *L)
{
	mpz_t *arg = lgmp_toz(L, 1);

	lua_pushinteger(L, mpz_fits_sint_p(*arg));

	return 1;
}

static int lmpz_fits_slong_p(lua_State *L)
{
	mpz_t *arg = lgmp_toz(L, 1);

	lua_pushinteger(L, mpz_fits_slong_p(*arg));

	return 1;
}

static int lmpz_fits_sshort_p(lua_State *L)
{
	mpz_t *arg = lgmp_toz(L, 1);

	lua_pushinteger(L, mpz_fits_sshort_p(*arg));

	return 1;
}

static int lmpz_fits_uint_p(lua_State *L)
{
	mpz_t *arg = lgmp_toz(L, 1);

	lua_pushinteger(L, mpz_fits_uint_p(*arg));

	return 1;
}

static int lmpz_fits_ulong_p(lua_State *L)
{
	mpz_t *arg = lgmp_toz(L, 1);

	lua_pushinteger(L, mpz_fits_ulong_p(*arg));

	return 1;
}

static int lmpz_fits_ushort_p(lua_State *L)
{
	mpz_t *arg = lgmp_toz(L, 1);

	lua_pushinteger(L, mpz_fits_ushort_p(*arg));

	return 1;
}

static int lmpz_gcd(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	mpz_gcd(*res, *arg1, *arg2);

	return 1;
}

static int lmpz_gcd_ui(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	lua_pushnumber(L, mpz_gcd_ui(*res, *arg1, arg2));

	return 2;
}

static int lmpz_gcdext(lua_State *L)
{
	mpz_t *resg = lgmp_optz(L, 3, 5);
	mpz_t *resp = lgmp_optz(L, 4, 0);
	mpz_t *resq = lgmp_optz(L, 5, 0);
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	mpz_gcdext(*resg, *resp, *resq, *arg1, *arg2);

	return 3;
}

static int lmpz_get_d(lua_State *L)
{
	mpz_t *arg = lgmp_toz(L, 1);

	lua_pushnumber(L, mpz_get_d(*arg));

	return 1;
}

static int lmpz_get_d_2exp(lua_State *L)
{
	signed long resexp;
	mpz_t *arg = lgmp_toz(L, 1);

	lua_pushnumber(L, mpz_get_d_2exp(&resexp, *arg));
	lua_pushinteger(L, resexp);

	return 2;
}

#if 0
#define mpz_get_si __gmpz_get_si
__GMP_DECLSPEC /* signed */ long int mpz_get_si __GMP_PROTO ((mpz_srcptr)) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;
#endif

static int lmpz_get_str(lua_State *L)
{
	int arg1 = lua_tointeger(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	char *res = mpz_get_str(NULL, arg1, *arg2);
	lua_pushstring(L, res);
	lgmp_free_str(res);

	return 1;
}

#if 0
#define mpz_get_ui __gmpz_get_ui
#if __GMP_INLINE_PROTOTYPES || defined (__GMP_FORCE_mpz_get_ui)
__GMP_DECLSPEC unsigned long int mpz_get_ui __GMP_PROTO ((mpz_srcptr)) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;
#endif

#define mpz_getlimbn __gmpz_getlimbn
#if __GMP_INLINE_PROTOTYPES || defined (__GMP_FORCE_mpz_getlimbn)
__GMP_DECLSPEC mp_limb_t mpz_getlimbn __GMP_PROTO ((mpz_srcptr, mp_size_t)) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;
#endif
#endif

static int lmpz_hamdist(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	lua_pushnumber(L, mpz_hamdist(*arg1, *arg2));

	return 1;
}

#if 0
#define mpz_import __gmpz_import
__GMP_DECLSPEC void mpz_import __GMP_PROTO ((mpz_ptr, size_t, int, size_t, int, size_t, __gmp_const void *));
#endif

static int lmpz_init(lua_State *L)
{
	mpz_t *res = lgmp_rawz(L);
	mpz_init(*res);

	return 1;
}

#if 0
#define mpz_init2 __gmpz_init2
__GMP_DECLSPEC void mpz_init2 __GMP_PROTO ((mpz_ptr, unsigned long));
#endif

static int lmpz_init_set(lua_State *L)
{
	mpz_t *res = lgmp_rawz(L);
	mpz_t *arg = lgmp_toz(L, 1);

	mpz_init_set(*res, *arg);

	return 1;
}

static int lmpz_init_set_d(lua_State *L)
{
	mpz_t *res = lgmp_rawz(L);
	double arg = lua_tonumber(L, 1);

	mpz_init_set_d(*res, arg);

	return 1;
}

#if 0
#define mpz_init_set_si __gmpz_init_set_si
__GMP_DECLSPEC void mpz_init_set_si __GMP_PROTO ((mpz_ptr, signed long int));
#endif

static int lmpz_init_set_str(lua_State *L)
{
	mpz_t *res = lgmp_rawz(L);
	const char *arg1 = lua_tostring(L, 1);
	int arg2 = lua_tointeger(L, 2);

	if (mpz_init_set_str(*res, arg1, arg2) != 0)
		lua_pushnil(L);

	return 1;
}

#if 0
#define mpz_init_set_ui __gmpz_init_set_ui
__GMP_DECLSPEC void mpz_init_set_ui __GMP_PROTO ((mpz_ptr, unsigned long int));

#define mpz_inp_raw __gmpz_inp_raw
#ifdef _GMP_H_HAVE_FILE
__GMP_DECLSPEC size_t mpz_inp_raw __GMP_PROTO ((mpz_ptr, FILE *));
#endif

#define mpz_inp_str __gmpz_inp_str
#ifdef _GMP_H_HAVE_FILE
__GMP_DECLSPEC size_t mpz_inp_str __GMP_PROTO ((mpz_ptr, FILE *, int));
#endif
#endif

static int lmpz_invert(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	lua_pushinteger(L, mpz_invert(*res, *arg1, *arg2));

	return 2;
}

static int lmpz_ior(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	mpz_ior(*res, *arg1, *arg2);

	return 1;
}

static int lmpz_kronecker(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	lua_pushinteger(L, mpz_kronecker(*arg1, *arg2));

	return 1;
}

static int lmpz_kronecker_si(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	long arg2 = (long)lua_tonumber(L, 2);

	lua_pushinteger(L, mpz_kronecker_si(*arg1, arg2));

	return 1;
}

static int lmpz_kronecker_ui(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	lua_pushinteger(L, mpz_kronecker_ui(*arg1, arg2));

	return 1;
}

static int lmpz_si_kronecker(lua_State *L)
{
	long arg1 = (long)lua_tonumber(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	lua_pushinteger(L, mpz_si_kronecker(arg1, *arg2));

	return 1;
}

static int lmpz_ui_kronecker(lua_State *L)
{
	unsigned long arg1 = (unsigned long)lua_tonumber(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	lua_pushinteger(L, mpz_ui_kronecker(arg1, *arg2));

	return 1;
}

static int lmpz_lcm(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	mpz_lcm(*res, *arg1, *arg2);

	return 1;
}

static int lmpz_lcm_ui(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpz_lcm_ui(*res, *arg1, arg2);

	return 1;
}

static int lmpz_lucnum_ui(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 2, 2);
	unsigned long arg = (unsigned long)lua_tonumber(L, 1);

	mpz_lucnum_ui(*res, arg);

	return 1;
}

static int lmpz_lucnum2_ui(lua_State *L)
{
	mpz_t *res1 = lgmp_optz(L, 2, 3);
	mpz_t *res2 = lgmp_optz(L, 3, 0);
	unsigned long arg = (unsigned long)lua_tonumber(L, 1);

	mpz_lucnum2_ui(*res1, *res2, arg);

	return 2;
}

static int lmpz_millerrabin(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	int arg2 = lua_tointeger(L, 2);

	lua_pushinteger(L, mpz_millerrabin(*arg1, arg2));

	return 1;
}

static int lmpz_mod(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	mpz_mod(*res, *arg1, *arg2);

	return 1;
}

#if 0
#define mpz_mod_ui mpz_fdiv_r_ui /* same as fdiv_r because divisor unsigned */
#endif

static int lmpz_mul(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	mpz_mul(*res, *arg1, *arg2);

	return 1;
}

static int lmpz_mul_2exp(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpz_mul_2exp(*res, *arg1, arg2);

	return 1;
}

static int lmpz_mul_si(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	long arg2 = (long)lua_tonumber(L, 2);

	mpz_mul_si(*res, *arg1, arg2);

	return 1;
}

static int lmpz_mul_ui(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpz_mul_ui(*res, *arg1, arg2);

	return 1;
}

static int lmpz_neg(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 2, 2);
	mpz_t *arg = lgmp_toz(L, 1);

	mpz_neg(*res, *arg);

	return 1;
}

static int lmpz_nextprime(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 2, 2);
	mpz_t *arg = lgmp_toz(L, 1);

	mpz_nextprime(*res, *arg);

	return 1;
}

#if 0
#define mpz_out_raw __gmpz_out_raw
#ifdef _GMP_H_HAVE_FILE
__GMP_DECLSPEC size_t mpz_out_raw __GMP_PROTO ((FILE *, mpz_srcptr));
#endif

#define mpz_out_str __gmpz_out_str
#ifdef _GMP_H_HAVE_FILE
__GMP_DECLSPEC size_t mpz_out_str __GMP_PROTO ((FILE *, int, mpz_srcptr));
#endif
#endif

static int lmpz_perfect_power_p(lua_State *L)
{
	mpz_t *arg = lgmp_toz(L, 1);

	lua_pushinteger(L, mpz_perfect_power_p(*arg));

	return 1;
}

static int lmpz_perfect_square_p(lua_State *L)
{
	mpz_t *arg = lgmp_toz(L, 1);

	lua_pushinteger(L, mpz_perfect_square_p(*arg));

	return 1;
}

static int lmpz_popcount(lua_State *L)
{
	mpz_t *arg = lgmp_toz(L, 1);

	lua_pushnumber(L, mpz_popcount(*arg));

	return 1;
}

static int lmpz_pow_ui(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpz_pow_ui(*res, *arg1, arg2);

	return 1;
}

static int lmpz_powm(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 4, 4);
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);
	mpz_t *arg3 = lgmp_toz(L, 3);

	mpz_powm(*res, *arg1, *arg2, *arg3);

	return 1;
}

static int lmpz_powm_ui(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 4, 4);
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);
	mpz_t *arg3 = lgmp_toz(L, 3);

	mpz_powm_ui(*res, *arg1, arg2, *arg3);

	return 1;
}

static int lmpz_probab_prime_p(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	int arg2 = lua_tointeger(L, 2);

	lua_pushinteger(L, mpz_probab_prime_p(*arg1, arg2));

	return 1;
}

#if 0
#define mpz_random __gmpz_random
__GMP_DECLSPEC void mpz_random __GMP_PROTO ((mpz_ptr, mp_size_t));

#define mpz_random2 __gmpz_random2
__GMP_DECLSPEC void mpz_random2 __GMP_PROTO ((mpz_ptr, mp_size_t));

#define mpz_realloc2 __gmpz_realloc2
__GMP_DECLSPEC void mpz_realloc2 __GMP_PROTO ((mpz_ptr, unsigned long));
#endif

static int lmpz_remove(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	mpz_remove(*res, *arg1, *arg2);

	return 1;
}

static int lmpz_root(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	lua_pushinteger(L, mpz_root(*res, *arg1, arg2));

	return 2;
}

static int lmpz_rootrem(lua_State *L)
{
	mpz_t *resro = lgmp_optz(L, 3, 4);
	mpz_t *resre = lgmp_optz(L, 4, 0);
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpz_rootrem(*resro, *resre, *arg1, arg2);

	return 2;
}

static int lmpz_rrandomb(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	gmp_randstate_t *arg1 = lgmp_torand(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpz_rrandomb(*res, *arg1, arg2);

	return 1;
}

static int lmpz_scan0(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	lua_pushnumber(L, mpz_scan0(*arg1, arg2));

	return 1;
}

static int lmpz_scan1(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	lua_pushnumber(L, mpz_scan1(*arg1, arg2));

	return 1;
}

static int lmpz_set(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	mpz_set(*arg1, *arg2);

	return 0;
}

static int lmpz_set_d(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	double arg2 = lua_tonumber(L, 2);

	mpz_set_d(*arg1, arg2);

	return 0;
}

static int lmpz_set_f(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpf_t *arg2 = lgmp_tof(L, 2);

	mpz_set_f(*arg1, *arg2);

	return 0;
}

#if 0
#define mpz_set_q __gmpz_set_q
#if __GMP_INLINE_PROTOTYPES || defined (__GMP_FORCE_mpz_set_q)
__GMP_DECLSPEC void mpz_set_q __GMP_PROTO ((mpz_ptr, mpq_srcptr));
#endif

#define mpz_set_si __gmpz_set_si
__GMP_DECLSPEC void mpz_set_si __GMP_PROTO ((mpz_ptr, signed long int));
#endif

static int lmpz_set_str(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	const char *arg2 = lua_tostring(L, 2);
	int arg3 = lua_tointeger(L, 3);

	lua_pushinteger(L, mpz_set_str(*arg1, arg2, arg3));

	return 1;
}

#if 0
#define mpz_set_ui __gmpz_set_ui
__GMP_DECLSPEC void mpz_set_ui __GMP_PROTO ((mpz_ptr, unsigned long int));
#endif

static int lmpz_setbit(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpz_setbit(*arg1, arg2);

	return 0;
}

static int lmpz_sgn(lua_State *L)
{
	mpz_t *arg = lgmp_toz(L, 1);

	lua_pushinteger(L, mpz_sgn(*arg));

	return 1;
}

#if 0
#define mpz_size __gmpz_size
#if __GMP_INLINE_PROTOTYPES || defined (__GMP_FORCE_mpz_size)
__GMP_DECLSPEC size_t mpz_size __GMP_PROTO ((mpz_srcptr)) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;
#endif
#endif

static int lmpz_sizeinbase(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	int arg2 = lua_tointeger(L, 2);

	lua_pushinteger(L, mpz_sizeinbase(*arg1, arg2));

	return 1;
}

static int lmpz_sqrt(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 2, 2);
	mpz_t *arg = lgmp_toz(L, 1);

	mpz_sqrt(*res, *arg);

	return 1;
}

static int lmpz_sqrtrem(lua_State *L)
{
	mpz_t *ress = lgmp_optz(L, 2, 3);
	mpz_t *resr = lgmp_optz(L, 3, 0);
	mpz_t *arg = lgmp_toz(L, 1);

	mpz_sqrtrem(*ress, *resr, *arg);

	return 2;
}

static int lmpz_sub(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	mpz_sub(*res, *arg1, *arg2);

	return 1;
}

static int lmpz_sub_ui(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpz_sub_ui(*res, *arg1, arg2);

	return 1;
}

static int lmpz_ui_sub(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	unsigned long arg1 = (unsigned long)lua_tonumber(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	mpz_ui_sub(*res, arg1, *arg2);

	return 1;
}

static int lmpz_submul(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);
	mpz_t *arg3 = lgmp_toz(L, 3);

	mpz_submul(*arg1, *arg2, *arg3);

	return 0;
}

static int lmpz_submul_ui(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);
	unsigned long arg3 = (unsigned long)lua_tonumber(L, 3);

	mpz_submul_ui(*arg1, *arg2, arg3);

	return 0;
}

#if 0
#define mpz_swap __gmpz_swap
__GMP_DECLSPEC void mpz_swap __GMP_PROTO ((mpz_ptr, mpz_ptr)) __GMP_NOTHROW;
#endif

static int lmpz_tdiv_q(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	mpz_tdiv_q(*res, *arg1, *arg2);

	return 1;
}

static int lmpz_tdiv_q_2exp(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpz_tdiv_q_2exp(*res, *arg1, arg2);

	return 1;
}

static int lmpz_tdiv_q_ui(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	lua_pushnumber(L, mpz_tdiv_q_ui(*res, *arg1, arg2));

	return 2;
}

static int lmpz_tdiv_qr(lua_State *L)
{
	mpz_t *resq = lgmp_optz(L, 3, 4);
	mpz_t *resr = lgmp_optz(L, 4, 0);
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	mpz_tdiv_qr(*resq, *resr, *arg1, *arg2);

	return 2;
}

static int lmpz_tdiv_qr_ui(lua_State *L)
{
	mpz_t *resq = lgmp_optz(L, 3, 4);
	mpz_t *resr = lgmp_optz(L, 4, 0);
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	lua_pushnumber(L, mpz_tdiv_qr_ui(*resq, *resr, *arg1, arg2));

	return 3;
}

static int lmpz_tdiv_r(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	mpz_tdiv_r(*res, *arg1, *arg2);

	return 1;
}

static int lmpz_tdiv_r_2exp(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpz_tdiv_r_2exp(*res, *arg1, arg2);

	return 1;
}

static int lmpz_tdiv_r_ui(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	lua_pushnumber(L, mpz_tdiv_r_ui(*res, *arg1, arg2));

	return 2;
}

static int lmpz_tdiv_ui(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	lua_pushnumber(L, mpz_tdiv_ui(*arg1, arg2));

	return 1;
}

static int lmpz_tstbit(lua_State *L)
{
	mpz_t *arg1 = lgmp_toz(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	lua_pushinteger(L, mpz_tstbit(*arg1, arg2));

	return 1;
}

static int lmpz_ui_pow_ui(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	unsigned long arg1 = (unsigned long)lua_tonumber(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpz_ui_pow_ui(*res, arg1, arg2);

	return 1;
}

static int lmpz_urandomb(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	gmp_randstate_t *arg1 = lgmp_torand(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpz_urandomb(*res, *arg1, arg2);

	return 1;
}

static int lmpz_urandomm(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	gmp_randstate_t *arg1 = lgmp_torand(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	mpz_urandomm(*res, *arg1, *arg2);

	return 1;
}

static int lmpz_xor(lua_State *L)
{
	mpz_t *res = lgmp_optz(L, 3, 3);
	mpz_t *arg1 = lgmp_toz(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	mpz_xor(*res, *arg1, *arg2);

	return 1;
}

static int lmpf_abs(lua_State *L)
{
	mpf_t *res = lgmp_optf(L, 2, 2);
	mpf_t *arg = lgmp_tof(L, 1);

	mpf_abs(*res, *arg);

	return 1;
}

static int lmpf_add(lua_State *L)
{
	mpf_t *res = lgmp_optf(L, 3, 3);
	mpf_t *arg1 = lgmp_tof(L, 1);
	mpf_t *arg2 = lgmp_tof(L, 2);

	mpf_add(*res, *arg1, *arg2);

	return 1;
}

static int lmpf_add_ui(lua_State *L)
{
	mpf_t *res = lgmp_optf(L, 3, 3);
	mpf_t *arg1 = lgmp_tof(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpf_add_ui(*res, *arg1, arg2);

	return 1;
}

static int lmpf_ceil(lua_State *L)
{
	mpf_t *res = lgmp_optf(L, 2, 2);
	mpf_t *arg = lgmp_tof(L, 1);

	mpf_ceil(*res, *arg);

	return 1;
}

static int lmpf_clear(lua_State *L)
{
	mpf_t *arg = lgmp_tof(L, 1);

	mpf_clear(*arg);

	return 0;
}

static int lmpf_cmp(lua_State *L)
{
	mpf_t *arg1 = lgmp_tof(L, 1);
	mpf_t *arg2 = lgmp_tof(L, 2);

	lua_pushinteger(L, mpf_cmp(*arg1, *arg2));

	return 1;
}

static int lmpf_cmp_d(lua_State *L)
{
	mpf_t *arg1 = lgmp_tof(L, 1);
	double arg2 = lua_tonumber(L, 2);

	lua_pushinteger(L, mpf_cmp_d(*arg1, arg2));

	return 1;
}

static int lmpf_cmp_si(lua_State *L)
{
	mpf_t *arg1 = lgmp_tof(L, 1);
	signed long arg2 = (signed long)lua_tonumber(L, 2);

	lua_pushinteger(L, mpf_cmp_si(*arg1, arg2));

	return 1;
}

static int lmpf_cmp_ui(lua_State *L)
{
	mpf_t *arg1 = lgmp_tof(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	lua_pushinteger(L, mpf_cmp_ui(*arg1, arg2));

	return 1;
}

static int lmpf_div(lua_State *L)
{
	mpf_t *res = lgmp_optf(L, 3, 3);
	mpf_t *arg1 = lgmp_tof(L, 1);
	mpf_t *arg2 = lgmp_tof(L, 2);

	mpf_div(*res, *arg1, *arg2);

	return 1;
}

static int lmpf_div_2exp(lua_State *L)
{
	mpf_t *res = lgmp_optf(L, 3, 3);
	mpf_t *arg1 = lgmp_tof(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpf_div_2exp(*res, *arg1, arg2);

	return 1;
}

static int lmpf_div_ui(lua_State *L)
{
	mpf_t *res = lgmp_optf(L, 3, 3);
	mpf_t *arg1 = lgmp_tof(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpf_div_ui(*res, *arg1, arg2);

	return 1;
}

#if 0
#define mpf_dump __gmpf_dump
__GMP_DECLSPEC void mpf_dump __GMP_PROTO ((mpf_srcptr));
#endif

static int lmpf_eq(lua_State *L)
{
	mpf_t *arg1 = lgmp_tof(L, 1);
	mpf_t *arg2 = lgmp_tof(L, 2);
	unsigned long arg3 = (unsigned long)lua_tonumber(L, 3);

	lua_pushinteger(L, mpf_eq(*arg1, *arg2, arg3));

	return 1;
}

static int lmpf_fits_sint_p(lua_State *L)
{
	mpf_t *arg = lgmp_tof(L, 1);

	lua_pushinteger(L, mpf_fits_sint_p(*arg));

	return 1;
}

static int lmpf_fits_slong_p(lua_State *L)
{
	mpf_t *arg = lgmp_tof(L, 1);

	lua_pushinteger(L, mpf_fits_slong_p(*arg));

	return 1;
}

static int lmpf_fits_sshort_p(lua_State *L)
{
	mpf_t *arg = lgmp_tof(L, 1);

	lua_pushinteger(L, mpf_fits_sshort_p(*arg));

	return 1;
}

static int lmpf_fits_uint_p(lua_State *L)
{
	mpf_t *arg = lgmp_tof(L, 1);

	lua_pushinteger(L, mpf_fits_uint_p(*arg));

	return 1;
}

static int lmpf_fits_ulong_p(lua_State *L)
{
	mpf_t *arg = lgmp_tof(L, 1);

	lua_pushinteger(L, mpf_fits_ulong_p(*arg));

	return 1;
}

static int lmpf_fits_ushort_p(lua_State *L)
{
	mpf_t *arg = lgmp_tof(L, 1);

	lua_pushinteger(L, mpf_fits_ushort_p(*arg));

	return 1;
}

static int lmpf_floor(lua_State *L)
{
	mpf_t *res = lgmp_optf(L, 2, 2);
	mpf_t *arg = lgmp_tof(L, 1);

	mpf_floor(*res, *arg);

	return 1;
}

static int lmpf_get_d(lua_State *L)
{
	mpf_t *arg = lgmp_tof(L, 1);

	lua_pushnumber(L, mpf_get_d(*arg));

	return 1;
}

static int lmpf_get_d_2exp(lua_State *L)
{
	signed long exp;
	mpf_t *arg = lgmp_tof(L, 1);

	lua_pushnumber(L, mpf_get_d_2exp(&exp, *arg));
	lua_pushnumber(L, exp);

	return 2;
}

static int lmpf_get_default_prec(lua_State *L)
{
	lua_pushnumber(L, mpf_get_default_prec());

	return 1;
}

static int lmpf_get_prec(lua_State *L)
{
	mpf_t *arg = lgmp_tof(L, 1);

	lua_pushnumber(L, mpf_get_prec(*arg));

	return 1;
}

#if 0
#define mpf_get_si __gmpf_get_si
__GMP_DECLSPEC long mpf_get_si __GMP_PROTO ((mpf_srcptr)) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;
#endif

static int lmpf_get_str(lua_State *L)
{
	mp_exp_t exp;
	int arg1 = lua_tointeger(L, 1);
	size_t arg2 = lua_tointeger(L, 2);
	mpf_t *arg3 = lgmp_tof(L, 3);
	
	char *res = mpf_get_str(NULL, &exp, arg1, arg2, *arg3);
	lua_pushstring(L, res);
	lua_pushnumber(L, exp);
	lgmp_free_str(res);

	return 2;
}

#if 0
#define mpf_get_ui __gmpf_get_ui
__GMP_DECLSPEC unsigned long mpf_get_ui __GMP_PROTO ((mpf_srcptr)) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;
#endif

static int lmpf_init(lua_State *L)
{
	mpf_t *res = lgmp_rawf(L);

	mpf_init(*res);

	return 1;
}

#if 0
#define mpf_init2 __gmpf_init2
__GMP_DECLSPEC void mpf_init2 __GMP_PROTO ((mpf_ptr, unsigned long int));
#endif

static int lmpf_init_set(lua_State *L)
{
	mpf_t *res = lgmp_rawf(L);
	mpf_t *arg = lgmp_tof(L, 1);

	mpf_init_set(*res, *arg);

	return 1;
}

static int lmpf_init_set_d(lua_State *L)
{
	mpf_t *res = lgmp_rawf(L);
	double arg = lua_tonumber(L, 1);

	mpf_init_set_d(*res, arg);

	return 1;
}

#if 0
#define mpf_init_set_si __gmpf_init_set_si
__GMP_DECLSPEC void mpf_init_set_si __GMP_PROTO ((mpf_ptr, signed long int));
#endif

static int lmpf_init_set_str(lua_State *L)
{
	mpf_t *res = lgmp_rawf(L);
	const char *arg1 = lua_tostring(L, 1);
	int arg2 = lua_tonumber(L, 2);

	if (mpf_init_set_str(*res, arg1, arg2) != 0)
	{
		lua_pushnil(L);
		lua_replace(L, -2);
	}

	return 1;
}

#if 0
#define mpf_init_set_ui __gmpf_init_set_ui
__GMP_DECLSPEC void mpf_init_set_ui __GMP_PROTO ((mpf_ptr, unsigned long int));

#define mpf_inp_str __gmpf_inp_str
#ifdef _GMP_H_HAVE_FILE
__GMP_DECLSPEC size_t mpf_inp_str __GMP_PROTO ((mpf_ptr, FILE *, int));
#endif
#endif

static int lmpf_integer_p(lua_State *L)
{
	mpf_t *arg = lgmp_tof(L, 1);

	lua_pushinteger(L, mpf_integer_p(*arg));

	return 1;
}

static int lmpf_mul(lua_State *L)
{
	mpf_t *res = lgmp_optf(L, 3, 3);
	mpf_t *arg1 = lgmp_tof(L, 1);
	mpf_t *arg2 = lgmp_tof(L, 2);

	mpf_mul(*res, *arg1, *arg2);

	return 1;
}

static int lmpf_mul_2exp(lua_State *L)
{
	mpf_t *res = lgmp_optf(L, 3, 3);
	mpf_t *arg1 = lgmp_tof(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpf_mul_2exp(*res, *arg1, arg2);

	return 1;
}

static int lmpf_mul_ui(lua_State *L)
{
	mpf_t *res = lgmp_optf(L, 3, 3);
	mpf_t *arg1 = lgmp_tof(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpf_mul_ui(*res, *arg1, arg2);

	return 1;
}

static int lmpf_neg(lua_State *L)
{
	mpf_t *res = lgmp_optf(L, 2, 2);
	mpf_t *arg = lgmp_tof(L, 1);

	mpf_neg(*res, *arg);

	return 1;
}

#if 0
#define mpf_out_str __gmpf_out_str
#ifdef _GMP_H_HAVE_FILE
__GMP_DECLSPEC size_t mpf_out_str __GMP_PROTO ((FILE *, int, size_t, mpf_srcptr));
#endif
#endif

static int lmpf_pow_ui(lua_State *L)
{
	mpf_t *res = lgmp_optf(L, 3, 3);
	mpf_t *arg1 = lgmp_tof(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpf_pow_ui(*res, *arg1, arg2);

	return 1;
}

static int lmpf_random2(lua_State *L)
{
	mpf_t *res = lgmp_optf(L, 3, 3);
	mp_size_t arg1 = (mp_size_t)lua_tonumber(L, 1);
	mp_exp_t arg2 = (mp_exp_t)lua_tonumber(L, 2);

	mpf_random2(*res, arg1, arg2);

	return 1;
}

static int lmpf_reldiff(lua_State *L)
{
	mpf_t *res = lgmp_optf(L, 3, 3);
	mpf_t *arg1 = lgmp_tof(L, 1);
	mpf_t *arg2 = lgmp_tof(L, 2);

	mpf_reldiff(*res, *arg1, *arg2);

	return 1;
}

static int lmpf_set(lua_State *L)
{
	mpf_t *arg1 = lgmp_tof(L, 1);
	mpf_t *arg2 = lgmp_tof(L, 2);

	mpf_set(*arg1, *arg2);

	return 0;
}

static int lmpf_set_d(lua_State *L)
{
	mpf_t *arg1 = lgmp_tof(L, 1);
	double arg2 = lua_tonumber(L, 2);

	mpf_set_d(*arg1, arg2);

	return 0;
}

static int lmpf_set_default_prec(lua_State *L)
{
	unsigned int arg = (unsigned int)lua_tonumber(L, 1);

	mpf_set_default_prec(arg);

	return 0;
}

static int lmpf_set_prec(lua_State *L)
{
	mpf_t *arg1 = lgmp_tof(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpf_set_prec(*arg1, arg2);

	return 0;
}

#if 0
#define mpf_set_prec_raw __gmpf_set_prec_raw
__GMP_DECLSPEC void mpf_set_prec_raw __GMP_PROTO ((mpf_ptr, unsigned long int)) __GMP_NOTHROW;

#define mpf_set_q __gmpf_set_q
__GMP_DECLSPEC void mpf_set_q __GMP_PROTO ((mpf_ptr, mpq_srcptr));

#define mpf_set_si __gmpf_set_si
__GMP_DECLSPEC void mpf_set_si __GMP_PROTO ((mpf_ptr, signed long int));
#endif

static int lmpf_set_str(lua_State *L)
{
	mpf_t *arg1 = lgmp_tof(L, 1);
	const char *arg2 = lua_tostring(L, 2);
	int arg3 = lua_tointeger(L, 3);

	lua_pushinteger(L, mpf_set_str(*arg1, arg2, arg3));

	return 1;
}

#if 0
#define mpf_set_ui __gmpf_set_ui
__GMP_DECLSPEC void mpf_set_ui __GMP_PROTO ((mpf_ptr, unsigned long int));
#endif

static int lmpf_set_z(lua_State *L)
{
	mpf_t *arg1 = lgmp_tof(L, 1);
	mpz_t *arg2 = lgmp_toz(L, 2);

	mpf_set_z(*arg1, *arg2);

	return 0;
}

static int lmpf_sgn(lua_State *L)
{
	mpf_t *arg = lgmp_tof(L, 1);

	lua_pushinteger(L, mpf_sgn(*arg));

	return 1;
}

#if 0
#define mpf_size __gmpf_size
__GMP_DECLSPEC size_t mpf_size __GMP_PROTO ((mpf_srcptr)) __GMP_NOTHROW __GMP_ATTRIBUTE_PURE;
#endif

static int lmpf_sqrt(lua_State *L)
{
	mpf_t *res = lgmp_optf(L, 2, 2);
	mpf_t *arg = lgmp_tof(L, 1);

	mpf_sqrt(*res, *arg);

	return 1;
}

static int lmpf_sqrt_ui(lua_State *L)
{
	mpf_t *res = lgmp_optf(L, 2, 2);
	unsigned int arg = (unsigned int)lua_tonumber(L, 1);

	mpf_sqrt_ui(*res, arg);

	return 1;
}

static int lmpf_sub(lua_State *L)
{
	mpf_t *res = lgmp_optf(L, 3, 3);
	mpf_t *arg1 = lgmp_tof(L, 1);
	mpf_t *arg2 = lgmp_tof(L, 2);

	mpf_sub(*res, *arg1, *arg2);

	return 1;
}

static int lmpf_sub_ui(lua_State *L)
{
	mpf_t *res = lgmp_optf(L, 3, 3);
	mpf_t *arg1 = lgmp_tof(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpf_sub_ui(*res, *arg1, arg2);

	return 1;
}

#if 0
#define mpf_swap __gmpf_swap
__GMP_DECLSPEC void mpf_swap __GMP_PROTO ((mpf_ptr, mpf_ptr)) __GMP_NOTHROW;
#endif

static int lmpf_trunc(lua_State *L)
{
	mpf_t *res = lgmp_optf(L, 2, 2);
	mpf_t *arg = lgmp_tof(L, 1);

	mpf_trunc(*res, *arg);

	return 1;
}

static int lmpf_ui_div(lua_State *L)
{
	mpf_t *res = lgmp_optf(L, 3, 3);
	unsigned long arg1 = (unsigned long)lua_tonumber(L, 1);
	mpf_t *arg2 = lgmp_tof(L, 2);

	mpf_ui_div(*res, arg1, *arg2);

	return 1;
}

static int lmpf_ui_sub(lua_State *L)
{
	mpf_t *res = lgmp_optf(L, 3, 3);
	unsigned long arg1 = (unsigned long)lua_tonumber(L, 1);
	mpf_t *arg2 = lgmp_tof(L, 2);

	mpf_ui_sub(*res, arg1, *arg2);

	return 1;
}

static int lmpf_urandomb(lua_State *L)
{
	mpf_t *res = lgmp_optf(L, 3, 3);
	gmp_randstate_t *arg1 = lgmp_torand(L, 1);
	unsigned long arg2 = (unsigned long)lua_tonumber(L, 2);

	mpf_urandomb(*res, *arg1, arg2);

	return 1;
}

static const luaL_Reg lgmp_prv[] =
{
	{"gmp_randinit_default", lgmp_randinit_default},
	{"gmp_randinit_lc_2exp", lgmp_randinit_lc_2exp},
	{"gmp_randinit_lc_2exp_size", lgmp_randinit_lc_2exp_size},
	{"gmp_randinit_mt", lgmp_randinit_mt},
	{"gmp_randinit_set", lgmp_randinit_set},
	{"gmp_randseed", lgmp_randseed},
	{"gmp_randseed_ui", lgmp_randseed_ui},
	{"gmp_randclear", lgmp_randclear},
	{"mpz_abs", lmpz_abs},
	{"mpz_add", lmpz_add},
	{"mpz_add_ui", lmpz_add_ui},
	{"mpz_addmul", lmpz_addmul},
	{"mpz_addmul_ui", lmpz_addmul_ui},
	{"mpz_and", lmpz_and},
	{"mpz_asprintf", lmpz_asprintf},
	{"mpz_bin_ui", lmpz_bin_ui},
	{"mpz_bin_uiui", lmpz_bin_uiui},
	{"mpz_cdiv_q", lmpz_cdiv_q},
	{"mpz_cdiv_q_2exp", lmpz_cdiv_q_2exp},
	{"mpz_cdiv_q_ui", lmpz_cdiv_q_ui},
	{"mpz_cdiv_qr", lmpz_cdiv_qr},
	{"mpz_cdiv_qr_ui", lmpz_cdiv_qr_ui},
	{"mpz_cdiv_r", lmpz_cdiv_r},
	{"mpz_cdiv_r_2exp", lmpz_cdiv_r_2exp},
	{"mpz_cdiv_r_ui", lmpz_cdiv_r_ui},
	{"mpz_cdiv_ui", lmpz_cdiv_ui},
	{"mpz_clear", lmpz_clear},
	{"mpz_clrbit", lmpz_clrbit},
	{"mpz_cmp", lmpz_cmp},
	{"mpz_cmp_d", lmpz_cmp_d},
	{"mpz_cmp_si", lmpz_cmp_si},
	{"mpz_cmp_ui", lmpz_cmp_ui},
	{"mpz_cmpabs", lmpz_cmpabs},
	{"mpz_cmpabs_d", lmpz_cmpabs_d},
	{"mpz_cmpabs_ui", lmpz_cmpabs_ui},
	{"mpz_com", lmpz_com},
	{"mpz_combit", lmpz_combit},
	{"mpz_congruent_p", lmpz_congruent_p},
	{"mpz_congruent_2exp_p", lmpz_congruent_2exp_p},
	{"mpz_congruent_ui_p", lmpz_congruent_ui_p},
	{"mpz_divexact", lmpz_divexact},
	{"mpz_divexact_ui", lmpz_divexact_ui},
	{"mpz_divisible_p", lmpz_divisible_p},
	{"mpz_divisible_ui_p", lmpz_divisible_ui_p},
	{"mpz_divisible_2exp_p", lmpz_divisible_2exp_p},
	{"mpz_fac_ui", lmpz_fac_ui},
	{"mpz_fdiv_q", lmpz_fdiv_q},
	{"mpz_fdiv_q_2exp", lmpz_fdiv_q_2exp},
	{"mpz_fdiv_q_ui", lmpz_fdiv_q_ui},
	{"mpz_fdiv_qr", lmpz_fdiv_qr},
	{"mpz_fdiv_qr_ui", lmpz_fdiv_qr_ui},
	{"mpz_fdiv_r", lmpz_fdiv_r},
	{"mpz_fdiv_r_2exp", lmpz_fdiv_r_2exp},
	{"mpz_fdiv_r_ui", lmpz_fdiv_r_ui},
	{"mpz_fdiv_ui", lmpz_fdiv_ui},
	{"mpz_fib_ui", lmpz_fib_ui},
	{"mpz_fib2_ui", lmpz_fib2_ui},
	{"mpz_fits_sint_p", lmpz_fits_sint_p},
	{"mpz_fits_slong_p", lmpz_fits_slong_p},
	{"mpz_fits_sshort_p", lmpz_fits_sshort_p},
	{"mpz_fits_uint_p", lmpz_fits_uint_p},
	{"mpz_fits_ulong_p", lmpz_fits_ulong_p},
	{"mpz_fits_ushort_p", lmpz_fits_ushort_p},
	{"mpz_gcd", lmpz_gcd},
	{"mpz_gcd_ui", lmpz_gcd_ui},
	{"mpz_gcdext", lmpz_gcdext},
	{"mpz_get_d", lmpz_get_d},
	{"mpz_get_d_2exp", lmpz_get_d_2exp},
	{"mpz_get_str", lmpz_get_str},
	{"mpz_hamdist", lmpz_hamdist},
	{"mpz_init", lmpz_init},
	{"mpz_init_set", lmpz_init_set},
	{"mpz_init_set_d", lmpz_init_set_d},
	{"mpz_init_set_str", lmpz_init_set_str},
	{"mpz_invert", lmpz_invert},
	{"mpz_ior", lmpz_ior},
	{"mpz_kronecker", lmpz_kronecker},
	{"mpz_kronecker_si", lmpz_kronecker_si},
	{"mpz_kronecker_ui", lmpz_kronecker_ui},
	{"mpz_si_kronecker", lmpz_si_kronecker},
	{"mpz_ui_kronecker", lmpz_ui_kronecker},
	{"mpz_lcm", lmpz_lcm},
	{"mpz_lcm_ui", lmpz_lcm_ui},
	{"mpz_lucnum_ui", lmpz_lucnum_ui},
	{"mpz_lucnum2_ui", lmpz_lucnum2_ui},
	{"mpz_millerrabin", lmpz_millerrabin},
	{"mpz_mod", lmpz_mod},
	{"mpz_mul", lmpz_mul},
	{"mpz_mul_2exp", lmpz_mul_2exp},
	{"mpz_mul_si", lmpz_mul_si},
	{"mpz_mul_ui", lmpz_mul_ui},
	{"mpz_neg", lmpz_neg},
	{"mpz_nextprime", lmpz_nextprime},
	{"mpz_perfect_power_p", lmpz_perfect_power_p},
	{"mpz_perfect_square_p", lmpz_perfect_square_p},
	{"mpz_popcount", lmpz_popcount},
	{"mpz_pow_ui", lmpz_pow_ui},
	{"mpz_powm", lmpz_powm},
	{"mpz_powm_ui", lmpz_powm_ui},
	{"mpz_probab_prime_p", lmpz_probab_prime_p},
	{"mpz_remove", lmpz_remove},
	{"mpz_root", lmpz_root},
	{"mpz_rootrem", lmpz_rootrem},
	{"mpz_rrandomb", lmpz_rrandomb},
	{"mpz_scan0", lmpz_scan0},
	{"mpz_scan1", lmpz_scan1},
	{"mpz_set", lmpz_set},
	{"mpz_set_d", lmpz_set_d},
	{"mpz_set_f", lmpz_set_f},
	{"mpz_set_str", lmpz_set_str},
	{"mpz_setbit", lmpz_setbit},
	{"mpz_sgn", lmpz_sgn},
	{"mpz_sizeinbase", lmpz_sizeinbase},
	{"mpz_sqrt", lmpz_sqrt},
	{"mpz_sqrtrem", lmpz_sqrtrem},
	{"mpz_sub", lmpz_sub},
	{"mpz_sub_ui", lmpz_sub_ui},
	{"mpz_ui_sub", lmpz_ui_sub},
	{"mpz_submul", lmpz_submul},
	{"mpz_submul_ui", lmpz_submul_ui},
	{"mpz_tdiv_q", lmpz_tdiv_q},
	{"mpz_tdiv_q_2exp", lmpz_tdiv_q_2exp},
	{"mpz_tdiv_q_ui", lmpz_tdiv_q_ui},
	{"mpz_tdiv_qr", lmpz_tdiv_qr},
	{"mpz_tdiv_qr_ui", lmpz_tdiv_qr_ui},
	{"mpz_tdiv_r", lmpz_tdiv_r},
	{"mpz_tdiv_r_2exp", lmpz_tdiv_r_2exp},
	{"mpz_tdiv_r_ui", lmpz_tdiv_r_ui},
	{"mpz_tdiv_ui", lmpz_tdiv_ui},
	{"mpz_tstbit", lmpz_tstbit},
	{"mpz_ui_pow_ui", lmpz_ui_pow_ui},
	{"mpz_urandomb", lmpz_urandomb},
	{"mpz_urandomm", lmpz_urandomm},
	{"mpz_xor", lmpz_xor},
	{"mpf_abs", lmpf_abs},
	{"mpf_add", lmpf_add},
	{"mpf_add_ui", lmpf_add_ui},
	{"mpf_asprintf", lmpf_asprintf},
	{"mpf_ceil", lmpf_ceil},
	{"mpf_clear", lmpf_clear},
	{"mpf_cmp", lmpf_cmp},
	{"mpf_cmp_d", lmpf_cmp_d},
	{"mpf_cmp_si", lmpf_cmp_si},
	{"mpf_cmp_ui", lmpf_cmp_ui},
	{"mpf_div", lmpf_div},
	{"mpf_div_2exp", lmpf_div_2exp},
	{"mpf_div_ui", lmpf_div_ui},
	{"mpf_eq", lmpf_eq},
	{"mpf_fits_sint_p", lmpf_fits_sint_p},
	{"mpf_fits_slong_p", lmpf_fits_slong_p},
	{"mpf_fits_sshort_p", lmpf_fits_sshort_p},
	{"mpf_fits_uint_p", lmpf_fits_uint_p},
	{"mpf_fits_ulong_p", lmpf_fits_ulong_p},
	{"mpf_fits_ushort_p", lmpf_fits_ushort_p},
	{"mpf_floor", lmpf_floor},
	{"mpf_get_d", lmpf_get_d},
	{"mpf_get_d_2exp", lmpf_get_d_2exp},
	{"mpf_get_default_prec", lmpf_get_default_prec},
	{"mpf_get_prec", lmpf_get_prec},
	{"mpf_get_str", lmpf_get_str},
	{"mpf_init", lmpf_init},
	{"mpf_init_set", lmpf_init_set},
	{"mpf_init_set_d", lmpf_init_set_d},
	{"mpf_init_set_str", lmpf_init_set_str},
	{"mpf_integer_p", lmpf_integer_p},
	{"mpf_mul", lmpf_mul},
	{"mpf_mul_2exp", lmpf_mul_2exp},
	{"mpf_mul_ui", lmpf_mul_ui},
	{"mpf_neg", lmpf_neg},
	{"mpf_pow_ui", lmpf_pow_ui},
	{"mpf_random2", lmpf_random2},
	{"mpf_reldiff", lmpf_reldiff},
	{"mpf_set", lmpf_set},
	{"mpf_set_d", lmpf_set_d},
	{"mpf_set_default_prec", lmpf_set_default_prec},
	{"mpf_set_prec", lmpf_set_prec},
	{"mpf_set_str", lmpf_set_str},
	{"mpf_set_z", lmpf_set_z},
	{"mpf_sgn", lmpf_sgn},
	{"mpf_sqrt", lmpf_sqrt},
	{"mpf_sqrt_ui", lmpf_sqrt_ui},
	{"mpf_sub", lmpf_sub},
	{"mpf_sub_ui", lmpf_sub_ui},
	{"mpf_trunc", lmpf_trunc},
	{"mpf_ui_div", lmpf_ui_div},
	{"mpf_ui_sub", lmpf_ui_sub},
	{"mpf_urandomb", lmpf_urandomb},
	{NULL, NULL}
};


static int lgmp_initialize(lua_State *L)
{
	lua_pushnumber(L, ULONG_MAX);
	lua_setfield(L, 1, "ULONG_MAX");

	lua_pushnumber(L, LONG_MIN);
	lua_setfield(L, 1, "LONG_MIN");

	lua_pushnumber(L, LONG_MAX);
	lua_setfield(L, 1, "LONG_MAX");

	lua_pushstring(L, gmp_version);
	lua_setfield(L, 1, "version");

	lua_pushvalue(L, 1);
	lua_pushvalue(L, 2);

	luaL_setfuncs(L, lgmp_prv, 1);

	return 0;
}

LGMP_API int luaopen_gmp(lua_State *L)
{
	lua_pushcfunction(L, lgmp_initialize);
	return 1;
}
