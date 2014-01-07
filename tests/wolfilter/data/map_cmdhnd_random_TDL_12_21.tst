**
**file:DBRES
**file:DBIN.tdl
SUBROUTINE proc_00(ARG1)
BEGIN
FOREACH tag_27 DO dbcall_77($(.),$(.));
KEEP AS res_00;
INTO out_31 DO dbcall_07($PARAM.1);
INTO out_63 DO proc_00();
KEEP AS res_01;
FOREACH tag_74 INTO out_30 DO dbcall_13();
KEEP AS res_02;
END


SUBROUTINE proc_01
BEGIN
INTO out_83 DO dbcall_49();
KEEP AS res_00;
FOREACH tag_88 INTO out_07 DO dbcall_71($RESULT.2,$RESULT.1);
KEEP AS res_01;
FOREACH tag_99 INTO out_72 DO dbcall_29($(.));
FOREACH RESULT INTO out_12 DO proc_00($3);
FOREACH RESULT INTO out_05 DO dbcall_94($2,$(.),$(.));
END


SUBROUTINE proc_02
BEGIN
DO dbcall_12();
KEEP AS res_00;
FOREACH tag_43 INTO out_57 DO proc_01($(.),$(.));
FOREACH tag_00 DO dbcall_13();
FOREACH tag_62 DO dbcall_36($(.));
FOREACH tag_07 INTO out_29 DO dbcall_56();
FOREACH tag_50 INTO out_28 DO dbcall_77($(.),$(.));
FOREACH tag_17 DO dbcall_36($(.),$(.),$(.));
FOREACH tag_09 INTO out_97 DO dbcall_64();
FOREACH tag_17 DO dbcall_46();
FOREACH tag_65 INTO out_55 DO dbcall_55($(.));
KEEP AS res_01;
END


SUBROUTINE proc_03(ARG1,ARG2,ARG3)
BEGIN
FOREACH tag_94 INTO out_01 DO dbcall_98($(.));
KEEP AS res_00;
FOREACH tag_38 DO dbcall_54($(.));
KEEP AS res_01;
END


SUBROUTINE proc_04(ARG1)
BEGIN
FOREACH tag_08 INTO out_22 DO dbcall_47($(.),$(.),$(.));
FOREACH tag_34 INTO out_92 DO dbcall_94($(.),$(.),$(.));
FOREACH tag_24 INTO out_03 DO dbcall_69($(.));
END


SUBROUTINE proc_05
BEGIN
FOREACH tag_76 INTO out_13 DO dbcall_68();
FOREACH tag_98 INTO out_83 DO proc_01($(.));
KEEP AS res_00;
FOREACH tag_71 INTO out_63 DO dbcall_15($RESULT.1,$RESULT.1);
END


SUBROUTINE proc_06
BEGIN
FOREACH tag_99 DO dbcall_23();
END


SUBROUTINE proc_07(ARG1)
BEGIN
FOREACH tag_35 INTO out_04 DO dbcall_44($(.),$(.));
FOREACH tag_14 INTO out_51 DO dbcall_50($(.),$(.));
FOREACH tag_82 INTO out_02 DO dbcall_31($(.));
KEEP AS res_00;
END


SUBROUTINE proc_08(ARG1,ARG2,ARG3)
BEGIN
FOREACH tag_04 INTO out_08 DO dbcall_27($(.),$(.),$(.));
KEEP AS res_00;
INTO out_49 DO dbcall_76($RESULT.1);
KEEP AS res_01;
INTO out_58 DO dbcall_51($(.));
FOREACH tag_01 INTO out_92 DO dbcall_81();
KEEP AS res_02;
FOREACH tag_51 INTO out_99 DO dbcall_61($(.),$(.),$(.));
KEEP AS res_03;
FOREACH tag_99 INTO out_57 DO proc_06();
KEEP AS res_04;
FOREACH tag_83 DO proc_02($(.),$PARAM.1,$1);
FOREACH RESULT INTO out_42 DO dbcall_20($res_01.2);
FOREACH res_02 DO dbcall_14($2,$PARAM.1,$PARAM.1);
END


SUBROUTINE proc_09(ARG1,ARG2)
BEGIN
FOREACH tag_73 DO dbcall_40();
KEEP AS res_00;
FOREACH tag_64 INTO out_67 DO dbcall_09($(.),$(.),$PARAM.1);
KEEP AS res_01;
FOREACH RESULT INTO out_77 DO dbcall_86();
FOREACH tag_97 DO dbcall_89($PARAM.2,$(.),$(.));
FOREACH tag_26 DO proc_06();
KEEP AS res_02;
END


SUBROUTINE proc_10
BEGIN
INTO out_06 DO dbcall_83($(.),$(.),$(.));
FOREACH tag_67 DO dbcall_79($(.),$(.),$(.));
DO dbcall_83($(.));
KEEP AS res_00;
INTO out_84 DO dbcall_51();
DO dbcall_62($(.),$(.),$(.));
FOREACH tag_57 INTO out_03 DO dbcall_39($(.));
KEEP AS res_01;
END


SUBROUTINE proc_11
BEGIN
FOREACH tag_70 INTO out_00 DO dbcall_14($(.),$(.));
KEEP AS res_00;
FOREACH tag_88 DO proc_02($1,$1);
KEEP AS res_01;
FOREACH tag_04 INTO out_27 DO dbcall_46($(.));
FOREACH RESULT DO dbcall_27($RESULT.1,$RESULT.1);
DO dbcall_31($RESULT.1);
KEEP AS res_02;
FOREACH RESULT DO dbcall_01();
FOREACH tag_09 INTO out_66 DO dbcall_13($(.),$(.),$(.));
FOREACH tag_08 INTO out_65 DO dbcall_33($res_00.2);
END


TRANSACTION test_transaction
BEGIN
DO proc_06($(.),$(.),$(.));
INTO out_78 DO dbcall_63($(.));
KEEP AS res_00;
FOREACH tag_94 DO dbcall_15();
KEEP AS res_01;
FOREACH tag_76 DO dbcall_17();
KEEP AS res_02;
FOREACH tag_51 INTO out_83 DO dbcall_93($(.));
FOREACH RESULT INTO out_82 DO dbcall_76($2);
KEEP AS res_03;
INTO out_74 DO dbcall_85($res_00.2,$1,$res_00.3);
KEEP AS res_04;
INTO out_89 DO dbcall_94($(.),$(.),$(.));
KEEP AS res_05;
FOREACH tag_71 INTO out_27 DO proc_03();
END


**file: test.dmap
COMMAND(run) SKIP CALL(test_transaction);
**config
--input-filter token --output-filter token --module ./../wolfilter/modules//filter/testtoken/mod_filter_testtoken --module ./../../src/modules/cmdbind/directmap/mod_command_directmap --module ./../wolfilter/modules//database/testtrace/mod_db_testtrace --database 'identifier=testdb,outfile=DBOUT,file=DBRES' --program=DBIN.tdl --cmdprogram=test.dmap run
**input
>doc
>tag_00
=X0X0X
<tag_00
>tag_01
=X1X0X
<tag_01
>tag_02
=X2X0X
<tag_02
>tag_03
=X3X0X
<tag_03
>tag_03
=X3X1X
<tag_03
>tag_04
=X4X0X
<tag_04
>tag_04
=X4X1X
<tag_04
>tag_04
=X4X2X
<tag_04
>tag_04
=X4X3X
<tag_04
>tag_05
=X5X0X
<tag_05
>tag_05
=X5X1X
<tag_05
>tag_06
=X6X0X
<tag_06
>tag_07
=X7X0X
<tag_07
>tag_08
=X8X0X
<tag_08
>tag_09
=X9X0X
<tag_09
>tag_09
=X9X1X
<tag_09
>tag_09
=X9X2X
<tag_09
>tag_09
=X9X3X
<tag_09
>tag_10
=X10X0X
<tag_10
>tag_11
=X11X0X
<tag_11
>tag_11
=X11X1X
<tag_11
>tag_11
=X11X2X
<tag_11
>tag_11
=X11X3X
<tag_11
>tag_12
=X12X0X
<tag_12
>tag_13
=X13X0X
<tag_13
>tag_13
=X13X1X
<tag_13
>tag_13
=X13X2X
<tag_13
>tag_14
=X14X0X
<tag_14
>tag_14
=X14X1X
<tag_14
>tag_14
=X14X2X
<tag_14
>tag_15
=X15X0X
<tag_15
>tag_15
=X15X1X
<tag_15
>tag_15
=X15X2X
<tag_15
>tag_16
=X16X0X
<tag_16
>tag_17
=X17X0X
<tag_17
>tag_17
=X17X1X
<tag_17
>tag_18
=X18X0X
<tag_18
>tag_18
=X18X1X
<tag_18
>tag_18
=X18X2X
<tag_18
>tag_19
=X19X0X
<tag_19
>tag_20
=X20X0X
<tag_20
>tag_21
=X21X0X
<tag_21
>tag_21
=X21X1X
<tag_21
>tag_21
=X21X2X
<tag_21
>tag_22
=X22X0X
<tag_22
>tag_22
=X22X1X
<tag_22
>tag_22
=X22X2X
<tag_22
>tag_23
=X23X0X
<tag_23
>tag_24
=X24X0X
<tag_24
>tag_25
=X25X0X
<tag_25
>tag_25
=X25X1X
<tag_25
>tag_26
=X26X0X
<tag_26
>tag_26
=X26X1X
<tag_26
>tag_26
=X26X2X
<tag_26
>tag_27
=X27X0X
<tag_27
>tag_27
=X27X1X
<tag_27
>tag_27
=X27X2X
<tag_27
>tag_28
=X28X0X
<tag_28
>tag_29
=X29X0X
<tag_29
>tag_29
=X29X1X
<tag_29
>tag_29
=X29X2X
<tag_29
>tag_29
=X29X3X
<tag_29
>tag_30
=X30X0X
<tag_30
>tag_30
=X30X1X
<tag_30
>tag_30
=X30X2X
<tag_30
>tag_30
=X30X3X
<tag_30
>tag_31
=X31X0X
<tag_31
>tag_31
=X31X1X
<tag_31
>tag_32
=X32X0X
<tag_32
>tag_32
=X32X1X
<tag_32
>tag_32
=X32X2X
<tag_32
>tag_33
=X33X0X
<tag_33
>tag_33
=X33X1X
<tag_33
>tag_33
=X33X2X
<tag_33
>tag_34
=X34X0X
<tag_34
>tag_34
=X34X1X
<tag_34
>tag_35
=X35X0X
<tag_35
>tag_36
=X36X0X
<tag_36
>tag_37
=X37X0X
<tag_37
>tag_37
=X37X1X
<tag_37
>tag_38
=X38X0X
<tag_38
>tag_38
=X38X1X
<tag_38
>tag_38
=X38X2X
<tag_38
>tag_38
=X38X3X
<tag_38
>tag_39
=X39X0X
<tag_39
>tag_39
=X39X1X
<tag_39
>tag_39
=X39X2X
<tag_39
>tag_39
=X39X3X
<tag_39
>tag_40
=X40X0X
<tag_40
>tag_40
=X40X1X
<tag_40
>tag_40
=X40X2X
<tag_40
>tag_41
=X41X0X
<tag_41
>tag_42
=X42X0X
<tag_42
>tag_43
=X43X0X
<tag_43
>tag_43
=X43X1X
<tag_43
>tag_44
=X44X0X
<tag_44
>tag_45
=X45X0X
<tag_45
>tag_45
=X45X1X
<tag_45
>tag_45
=X45X2X
<tag_45
>tag_46
=X46X0X
<tag_46
>tag_47
=X47X0X
<tag_47
>tag_48
=X48X0X
<tag_48
>tag_48
=X48X1X
<tag_48
>tag_48
=X48X2X
<tag_48
>tag_48
=X48X3X
<tag_48
>tag_49
=X49X0X
<tag_49
>tag_49
=X49X1X
<tag_49
>tag_49
=X49X2X
<tag_49
>tag_49
=X49X3X
<tag_49
>tag_50
=X50X0X
<tag_50
>tag_50
=X50X1X
<tag_50
>tag_50
=X50X2X
<tag_50
>tag_51
=X51X0X
<tag_51
>tag_51
=X51X1X
<tag_51
>tag_51
=X51X2X
<tag_51
>tag_52
=X52X0X
<tag_52
>tag_52
=X52X1X
<tag_52
>tag_53
=X53X0X
<tag_53
>tag_53
=X53X1X
<tag_53
>tag_53
=X53X2X
<tag_53
>tag_54
=X54X0X
<tag_54
>tag_54
=X54X1X
<tag_54
>tag_54
=X54X2X
<tag_54
>tag_55
=X55X0X
<tag_55
>tag_56
=X56X0X
<tag_56
>tag_56
=X56X1X
<tag_56
>tag_56
=X56X2X
<tag_56
>tag_56
=X56X3X
<tag_56
>tag_57
=X57X0X
<tag_57
>tag_57
=X57X1X
<tag_57
>tag_57
=X57X2X
<tag_57
>tag_57
=X57X3X
<tag_57
>tag_58
=X58X0X
<tag_58
>tag_59
=X59X0X
<tag_59
>tag_59
=X59X1X
<tag_59
>tag_59
=X59X2X
<tag_59
>tag_59
=X59X3X
<tag_59
>tag_60
=X60X0X
<tag_60
>tag_60
=X60X1X
<tag_60
>tag_61
=X61X0X
<tag_61
>tag_61
=X61X1X
<tag_61
>tag_61
=X61X2X
<tag_61
>tag_62
=X62X0X
<tag_62
>tag_62
=X62X1X
<tag_62
>tag_63
=X63X0X
<tag_63
>tag_63
=X63X1X
<tag_63
>tag_64
=X64X0X
<tag_64
>tag_64
=X64X1X
<tag_64
>tag_64
=X64X2X
<tag_64
>tag_65
=X65X0X
<tag_65
>tag_65
=X65X1X
<tag_65
>tag_65
=X65X2X
<tag_65
>tag_66
=X66X0X
<tag_66
>tag_66
=X66X1X
<tag_66
>tag_67
=X67X0X
<tag_67
>tag_68
=X68X0X
<tag_68
>tag_68
=X68X1X
<tag_68
>tag_68
=X68X2X
<tag_68
>tag_68
=X68X3X
<tag_68
>tag_69
=X69X0X
<tag_69
>tag_69
=X69X1X
<tag_69
>tag_69
=X69X2X
<tag_69
>tag_69
=X69X3X
<tag_69
>tag_70
=X70X0X
<tag_70
>tag_70
=X70X1X
<tag_70
>tag_71
=X71X0X
<tag_71
>tag_72
=X72X0X
<tag_72
>tag_73
=X73X0X
<tag_73
>tag_73
=X73X1X
<tag_73
>tag_73
=X73X2X
<tag_73
>tag_74
=X74X0X
<tag_74
>tag_75
=X75X0X
<tag_75
>tag_75
=X75X1X
<tag_75
>tag_75
=X75X2X
<tag_75
>tag_75
=X75X3X
<tag_75
>tag_76
=X76X0X
<tag_76
>tag_76
=X76X1X
<tag_76
>tag_76
=X76X2X
<tag_76
>tag_76
=X76X3X
<tag_76
>tag_77
=X77X0X
<tag_77
>tag_77
=X77X1X
<tag_77
>tag_77
=X77X2X
<tag_77
>tag_78
=X78X0X
<tag_78
>tag_78
=X78X1X
<tag_78
>tag_78
=X78X2X
<tag_78
>tag_79
=X79X0X
<tag_79
>tag_79
=X79X1X
<tag_79
>tag_79
=X79X2X
<tag_79
>tag_79
=X79X3X
<tag_79
>tag_80
=X80X0X
<tag_80
>tag_80
=X80X1X
<tag_80
>tag_80
=X80X2X
<tag_80
>tag_81
=X81X0X
<tag_81
>tag_81
=X81X1X
<tag_81
>tag_82
=X82X0X
<tag_82
>tag_82
=X82X1X
<tag_82
>tag_82
=X82X2X
<tag_82
>tag_83
=X83X0X
<tag_83
>tag_84
=X84X0X
<tag_84
>tag_84
=X84X1X
<tag_84
>tag_85
=X85X0X
<tag_85
>tag_85
=X85X1X
<tag_85
>tag_86
=X86X0X
<tag_86
>tag_86
=X86X1X
<tag_86
>tag_87
=X87X0X
<tag_87
>tag_87
=X87X1X
<tag_87
>tag_87
=X87X2X
<tag_87
>tag_87
=X87X3X
<tag_87
>tag_88
=X88X0X
<tag_88
>tag_89
=X89X0X
<tag_89
>tag_89
=X89X1X
<tag_89
>tag_89
=X89X2X
<tag_89
>tag_89
=X89X3X
<tag_89
>tag_90
=X90X0X
<tag_90
>tag_90
=X90X1X
<tag_90
>tag_90
=X90X2X
<tag_90
>tag_91
=X91X0X
<tag_91
>tag_91
=X91X1X
<tag_91
>tag_91
=X91X2X
<tag_91
>tag_92
=X92X0X
<tag_92
>tag_92
=X92X1X
<tag_92
>tag_92
=X92X2X
<tag_92
>tag_93
=X93X0X
<tag_93
>tag_93
=X93X1X
<tag_93
>tag_94
=X94X0X
<tag_94
>tag_94
=X94X1X
<tag_94
>tag_95
=X95X0X
<tag_95
>tag_96
=X96X0X
<tag_96
>tag_97
=X97X0X
<tag_97
>tag_97
=X97X1X
<tag_97
>tag_97
=X97X2X
<tag_97
>tag_98
=X98X0X
<tag_98
>tag_98
=X98X1X
<tag_98
>tag_99
=X99X0X
<tag_99
>tag_99
=X99X1X
<tag_99
>tag_99
=X99X2X
<tag_99
>tag_99
=X99X3X
<tag_99
<doc
**output
**end
