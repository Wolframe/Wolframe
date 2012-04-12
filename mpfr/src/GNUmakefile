TOPDIR = ../..

SUBDIRS =

-include $(TOPDIR)/makefiles/gmake/platform.mk

INCLUDE_DIRS =

INCLUDE_LDFLAGS =

INCLUDE_LIBS =

LOCAL_STATIC_LIB_OBJS = \

LOCAL_STATIC_LIB = \
	libmpfr.a

LOCAL_STATIC_LIB_OBJS = \
	abort_prec_max.o \
	acos.o \
	acosh.o \
	add.o \
	add_d.o \
	add_ui.o \
	add1.o \
	add1sp.o \
	agm.o \
	ai.o \
	asin.o \
	asinh.o \
	atan.o \
	atan2.o \
	atanh.o \
	bernoulli.o \
	buildopt.o \
	cache.o \
	cbrt.o \
	check.o \
	clear.o \
	clears.o \
	cmp.o \
	cmp_abs.o \
	cmp_d.o \
	cmp_ld.o \
	cmp_si.o \
	cmp_ui.o \
	cmp2.o \
	comparisons.o \
	const_catalan.o \
	const_euler.o \
	const_log2.o \
	const_pi.o \
	constant.o \
	copysign.o \
	cos.o \
	cosh.o \
	cot.o \
	coth.o \
	csc.o \
	csch.o \
	d_div.o \
	d_sub.o \
	digamma.o \
	dim.o \
	div.o \
	div_2exp.o \
	div_2si.o \
	div_2ui.o \
	div_d.o \
	div_ui.o \
	dump.o \
	eint.o \
	eq.o \
	erf.o \
	erfc.o \
	exceptions.o \
	exp.o \
	exp_2.o \
	exp10.o \
	exp2.o \
	exp3.o \
	expm1.o \
	extract.o \
	factorial.o \
	fits_intmax.o \
	fits_sint.o \
	fits_slong.o \
	fits_sshort.o \
	fits_uint.o \
	fits_uintmax.o \
	fits_ulong.o \
	fits_ushort.o \
	fma.o \
	fms.o \
	frac.o \
	free_cache.o \
	frexp.o \
	gamma.o \
	gammaonethird.o \
	get_d.o \
	get_d64.o \
	get_exp.o \
	get_f.o \
	get_flt.o \
	get_ld.o \
	get_patches.o \
	get_si.o \
	get_sj.o \
	get_str.o \
	get_ui.o \
	get_uj.o \
	get_z.o \
	get_z_exp.o \
	gmp_op.o \
	grandom.o \
	hypot.o \
	init.o \
	init2.o \
	inits.o \
	inits2.o \
	inp_str.o \
	int_ceil_log2.o \
	isinf.o \
	isinteger.o \
	isnan.o \
	isnum.o \
	isqrt.o \
	isregular.o \
	iszero.o \
	jn.o \
	li2.o \
	lngamma.o \
	log.o \
	log10.o \
	log1p.o \
	log2.o \
	logging.o \
	min_prec.o \
	minmax.o \
	modf.o \
	mp_clz_tab.o \
	mpfr-gmp.o \
	mpn_exp.o \
	mul.o \
	mul_2exp.o \
	mul_2si.o \
	mul_2ui.o \
	mul_d.o \
	mul_ui.o \
	mulders.o \
	neg.o \
	next.o \
	out_str.o \
	pow.o \
	pow_si.o \
	pow_ui.o \
	pow_z.o \
	powerof2.o \
	print_raw.o \
	print_rnd_mode.o \
	printf.o \
	rec_sqrt.o \
	reldiff.o \
	rem1.o \
	rint.o \
	root.o \
	round_near_x.o \
	round_p.o \
	round_prec.o \
	scale2.o \
	sec.o \
	sech.o \
	set.o \
	set_d.o \
	set_d64.o \
	set_dfl_prec.o \
	set_exp.o \
	set_f.o \
	set_flt.o \
	set_inf.o \
	set_ld.o \
	set_nan.o \
	set_prc_raw.o \
	set_prec.o \
	set_q.o \
	set_rnd.o \
	set_si.o \
	set_si_2exp.o \
	set_sj.o \
	set_str.o \
	set_str_raw.o \
	set_ui.o \
	set_ui_2exp.o \
	set_uj.o \
	set_z.o \
	set_z_exp.o \
	set_zero.o \
	setmax.o \
	setmin.o \
	setsign.o \
	sgn.o \
	si_op.o \
	signbit.o \
	sin.o \
	sin_cos.o \
	sinh.o \
	sinh_cosh.o \
	sqr.o \
	sqrt.o \
	sqrt_ui.o \
	stack_interface.o \
	strtofr.o \
	sub.o \
	sub_d.o \
	sub_ui.o \
	sub1.o \
	sub1sp.o \
	subnormal.o \
	sum.o \
	swap.o \
	tan.o \
	tanh.o \
	uceil_exp2.o \
	uceil_log2.o \
	ufloor_log2.o \
	ui_div.o \
	ui_pow.o \
	ui_pow_ui.o \
	ui_sub.o \
	urandom.o \
	urandomb.o \
	vasprintf.o \
	version.o \
	volatile.o \
	yn.o \
	zeta.o \
	zeta_ui.o

-include $(TOPDIR)/makefiles/gmake/sub.mk

# overwrite compilation flags, hard code configure
# stuff for the platforms which don't package MPFR,
# currently this is Centos 5 and Redhat 5/6
ALL_CFLAGS = \
	-O2 -Wall -I. \
	$(INCLUDE_DIRS) $(INCLUDE_CFLAGS) \
	-DHAVE_STDARG=1 -DHAVE_INTMAX_T \
	-DHAVE_LOCAL_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 \
	-DHAVE_ATTRIBUTE_CONST=1 -DHAVE_VA_COPY=1 \
	-DHAVE_WCHAR_H=1 -DHAVE_LONG_LONG=1

$(LOCAL_STATIC_LIB): $(LOCAL_STATIC_LIB_OBJS)
	ar rcs $(LOCAL_STATIC_LIB) $(LOCAL_STATIC_LIB_OBJS)

local_all: $(LOCAL_STATIC_LIB)

local_clean:
	@rm -f $(LOCAL_STATIC_LIB) $(LOCAL_STATIC_LIB_OBJS)

local_distclean:

local_install:

local_test:

local_longtest:
