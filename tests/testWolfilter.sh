#!/bin/sh
export LD_LIBRARY_PATH=../src/libwolframe
echo executing test lua_peerformfunc_table UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_table.lua run > temp/lua_peerformfunc_table.UTF-8.xml
echo executing test lua_peerformfunc_table UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_table.lua run > temp/lua_peerformfunc_table.UTF-16LE.xml
echo executing test lua_peerformfunc_table UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_table.lua run > temp/lua_peerformfunc_table.UTF-16BE.xml
echo executing test lua_peerformfunc_table UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_table.lua run > temp/lua_peerformfunc_table.UCS-2LE.xml
echo executing test lua_peerformfunc_table UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_table.lua run > temp/lua_peerformfunc_table.UCS-2BE.xml
echo executing test lua_peerformfunc_table UCS-4LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4LE/' | recode UTF-8..UCS-4LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_table.lua run > temp/lua_peerformfunc_table.UCS-4LE.xml
echo executing test lua_peerformfunc_table UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_table.lua run > temp/lua_peerformfunc_table.UCS-4BE.xml
echo executing test lua_peerformfunc_table_libxml2 UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_table.lua run > temp/lua_peerformfunc_table.UTF-8.xml
echo executing test lua_peerformfunc_table_libxml2 UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_table.lua run > temp/lua_peerformfunc_table.UTF-16LE.xml
echo executing test lua_peerformfunc_table_libxml2 UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_table.lua run > temp/lua_peerformfunc_table.UTF-16BE.xml
echo executing test lua_peerformfunc_table_libxml2 UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_table.lua run > temp/lua_peerformfunc_table.UCS-2LE.xml
echo executing test lua_peerformfunc_table_libxml2 UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_table.lua run > temp/lua_peerformfunc_table.UCS-2BE.xml
echo executing test lua_peerformfunc_table_libxml2 UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_table.lua run > temp/lua_peerformfunc_table.UCS-4BE.xml
echo executing test lua_echo_input_doctype UTF-8
cat wolfilter/template/doc/employee_assignment_doctype.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/echo_input_doctype.lua run > temp/employee_assignment_doctype.UTF-8.xml
echo executing test lua_echo_input_doctype UTF-16LE
cat wolfilter/template/doc/employee_assignment_doctype.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/echo_input_doctype.lua run > temp/employee_assignment_doctype.UTF-16LE.xml
echo executing test lua_echo_input_doctype UTF-16BE
cat wolfilter/template/doc/employee_assignment_doctype.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/echo_input_doctype.lua run > temp/employee_assignment_doctype.UTF-16BE.xml
echo executing test lua_echo_input_doctype UCS-2LE
cat wolfilter/template/doc/employee_assignment_doctype.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/echo_input_doctype.lua run > temp/employee_assignment_doctype.UCS-2LE.xml
echo executing test lua_echo_input_doctype UCS-2BE
cat wolfilter/template/doc/employee_assignment_doctype.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/echo_input_doctype.lua run > temp/employee_assignment_doctype.UCS-2BE.xml
echo executing test lua_echo_input_doctype UCS-4LE
cat wolfilter/template/doc/employee_assignment_doctype.UTF-8.xml | sed 's/UTF-8/UCS-4LE/' | recode UTF-8..UCS-4LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/echo_input_doctype.lua run > temp/employee_assignment_doctype.UCS-4LE.xml
echo executing test lua_echo_input_doctype UCS-4BE
cat wolfilter/template/doc/employee_assignment_doctype.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/echo_input_doctype.lua run > temp/employee_assignment_doctype.UCS-4BE.xml
echo executing test lua_echo_input_doctype_libxml2 UTF-8
cat wolfilter/template/doc/employee_assignment_doctype.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/echo_input_doctype.lua run > temp/employee_assignment_doctype.UTF-8.xml
echo executing test lua_echo_input_doctype_libxml2 UTF-16LE
cat wolfilter/template/doc/employee_assignment_doctype.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/echo_input_doctype.lua run > temp/employee_assignment_doctype.UTF-16LE.xml
echo executing test lua_echo_input_doctype_libxml2 UTF-16BE
cat wolfilter/template/doc/employee_assignment_doctype.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/echo_input_doctype.lua run > temp/employee_assignment_doctype.UTF-16BE.xml
echo executing test lua_echo_input_doctype_libxml2 UCS-2LE
cat wolfilter/template/doc/employee_assignment_doctype.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/echo_input_doctype.lua run > temp/employee_assignment_doctype.UCS-2LE.xml
echo executing test lua_echo_input_doctype_libxml2 UCS-2BE
cat wolfilter/template/doc/employee_assignment_doctype.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/echo_input_doctype.lua run > temp/employee_assignment_doctype.UCS-2BE.xml
echo executing test lua_echo_input_doctype_libxml2 UCS-4BE
cat wolfilter/template/doc/employee_assignment_doctype.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/echo_input_doctype.lua run > temp/employee_assignment_doctype.UCS-4BE.xml
echo executing test lua_ddlform_iterator_scope UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_iterator_scope.lua run > temp/lua_ddlform_iterator_scope.UTF-8.xml
echo executing test lua_ddlform_iterator_scope UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_iterator_scope.lua run > temp/lua_ddlform_iterator_scope.UTF-16LE.xml
echo executing test lua_ddlform_iterator_scope UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_iterator_scope.lua run > temp/lua_ddlform_iterator_scope.UTF-16BE.xml
echo executing test lua_ddlform_iterator_scope UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_iterator_scope.lua run > temp/lua_ddlform_iterator_scope.UCS-2LE.xml
echo executing test lua_ddlform_iterator_scope UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_iterator_scope.lua run > temp/lua_ddlform_iterator_scope.UCS-2BE.xml
echo executing test lua_ddlform_iterator_scope UCS-4LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4LE/' | recode UTF-8..UCS-4LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_iterator_scope.lua run > temp/lua_ddlform_iterator_scope.UCS-4LE.xml
echo executing test lua_ddlform_iterator_scope UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_iterator_scope.lua run > temp/lua_ddlform_iterator_scope.UCS-4BE.xml
echo executing test lua_ddlform_iterator_scope_libxml2 UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_iterator_scope.lua run > temp/lua_ddlform_iterator_scope.UTF-8.xml
echo executing test lua_ddlform_iterator_scope_libxml2 UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_iterator_scope.lua run > temp/lua_ddlform_iterator_scope.UTF-16LE.xml
echo executing test lua_ddlform_iterator_scope_libxml2 UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_iterator_scope.lua run > temp/lua_ddlform_iterator_scope.UTF-16BE.xml
echo executing test lua_ddlform_iterator_scope_libxml2 UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_iterator_scope.lua run > temp/lua_ddlform_iterator_scope.UCS-2LE.xml
echo executing test lua_ddlform_iterator_scope_libxml2 UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_iterator_scope.lua run > temp/lua_ddlform_iterator_scope.UCS-2BE.xml
echo executing test lua_ddlform_iterator_scope_libxml2 UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_iterator_scope.lua run > temp/lua_ddlform_iterator_scope.UCS-4BE.xml
echo executing test map_echo_input_generator UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf - > temp/employee_assignment_print.UTF-8.xml
echo executing test map_echo_input_generator UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf - > temp/employee_assignment_print.UTF-16LE.xml
echo executing test map_echo_input_generator UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf - > temp/employee_assignment_print.UTF-16BE.xml
echo executing test map_echo_input_generator UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf - > temp/employee_assignment_print.UCS-2LE.xml
echo executing test map_echo_input_generator UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf - > temp/employee_assignment_print.UCS-2BE.xml
echo executing test map_echo_input_generator UCS-4LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4LE/' | recode UTF-8..UCS-4LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf - > temp/employee_assignment_print.UCS-4LE.xml
echo executing test map_echo_input_generator UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf - > temp/employee_assignment_print.UCS-4BE.xml
echo executing test map_echo_input_generator_libxml2 UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 - > temp/employee_assignment_print.UTF-8.xml
echo executing test map_echo_input_generator_libxml2 UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 - > temp/employee_assignment_print.UTF-16LE.xml
echo executing test map_echo_input_generator_libxml2 UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 - > temp/employee_assignment_print.UTF-16BE.xml
echo executing test map_echo_input_generator_libxml2 UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 - > temp/employee_assignment_print.UCS-2LE.xml
echo executing test map_echo_input_generator_libxml2 UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 - > temp/employee_assignment_print.UCS-2BE.xml
echo executing test map_echo_input_generator_libxml2 UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 - > temp/employee_assignment_print.UCS-4BE.xml
echo executing test filter_multilingual UTF-8
cat wolfilter/template/doc/multilingual.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf - > temp/multilingual.UTF-8.xml
echo executing test filter_multilingual UTF-16LE
cat wolfilter/template/doc/multilingual.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf - > temp/multilingual.UTF-16LE.xml
echo executing test filter_multilingual UTF-16BE
cat wolfilter/template/doc/multilingual.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf - > temp/multilingual.UTF-16BE.xml
echo executing test filter_multilingual UCS-2LE
cat wolfilter/template/doc/multilingual.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf - > temp/multilingual.UCS-2LE.xml
echo executing test filter_multilingual UCS-2BE
cat wolfilter/template/doc/multilingual.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf - > temp/multilingual.UCS-2BE.xml
echo executing test filter_multilingual UCS-4LE
cat wolfilter/template/doc/multilingual.UTF-8.xml | sed 's/UTF-8/UCS-4LE/' | recode UTF-8..UCS-4LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf - > temp/multilingual.UCS-4LE.xml
echo executing test filter_multilingual UCS-4BE
cat wolfilter/template/doc/multilingual.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf - > temp/multilingual.UCS-4BE.xml
echo executing test filter_multilingual_libxml2 UTF-8
cat wolfilter/template/doc/multilingual.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 - > temp/multilingual.UTF-8.xml
echo executing test filter_multilingual_libxml2 UTF-16LE
cat wolfilter/template/doc/multilingual.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 - > temp/multilingual.UTF-16LE.xml
echo executing test filter_multilingual_libxml2 UTF-16BE
cat wolfilter/template/doc/multilingual.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 - > temp/multilingual.UTF-16BE.xml
echo executing test filter_multilingual_libxml2 UCS-2LE
cat wolfilter/template/doc/multilingual.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 - > temp/multilingual.UCS-2LE.xml
echo executing test filter_multilingual_libxml2 UCS-2BE
cat wolfilter/template/doc/multilingual.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 - > temp/multilingual.UCS-2BE.xml
echo executing test filter_multilingual_libxml2 UCS-4BE
cat wolfilter/template/doc/multilingual.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 - > temp/multilingual.UCS-4BE.xml
echo executing test lua_peerformfunc_input_generator UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_input_generator.lua run > temp/lua_ddlform_input_generator.UTF-8.xml
echo executing test lua_peerformfunc_input_generator UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_input_generator.lua run > temp/lua_ddlform_input_generator.UTF-16LE.xml
echo executing test lua_peerformfunc_input_generator UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_input_generator.lua run > temp/lua_ddlform_input_generator.UTF-16BE.xml
echo executing test lua_peerformfunc_input_generator UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_input_generator.lua run > temp/lua_ddlform_input_generator.UCS-2LE.xml
echo executing test lua_peerformfunc_input_generator UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_input_generator.lua run > temp/lua_ddlform_input_generator.UCS-2BE.xml
echo executing test lua_peerformfunc_input_generator UCS-4LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4LE/' | recode UTF-8..UCS-4LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_input_generator.lua run > temp/lua_ddlform_input_generator.UCS-4LE.xml
echo executing test lua_peerformfunc_input_generator UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_input_generator.lua run > temp/lua_ddlform_input_generator.UCS-4BE.xml
echo executing test lua_peerformfunc_input_generator_libxml2 UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_input_generator.lua run > temp/lua_ddlform_input_generator.UTF-8.xml
echo executing test lua_peerformfunc_input_generator_libxml2 UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_input_generator.lua run > temp/lua_ddlform_input_generator.UTF-16LE.xml
echo executing test lua_peerformfunc_input_generator_libxml2 UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_input_generator.lua run > temp/lua_ddlform_input_generator.UTF-16BE.xml
echo executing test lua_peerformfunc_input_generator_libxml2 UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_input_generator.lua run > temp/lua_ddlform_input_generator.UCS-2LE.xml
echo executing test lua_peerformfunc_input_generator_libxml2 UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_input_generator.lua run > temp/lua_ddlform_input_generator.UCS-2BE.xml
echo executing test lua_peerformfunc_input_generator_libxml2 UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_input_generator.lua run > temp/lua_ddlform_input_generator.UCS-4BE.xml
echo executing test lua_ddlform_input_generator UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_input_generator.lua run > temp/lua_ddlform_input_generator.UTF-8.xml
echo executing test lua_ddlform_input_generator UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_input_generator.lua run > temp/lua_ddlform_input_generator.UTF-16LE.xml
echo executing test lua_ddlform_input_generator UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_input_generator.lua run > temp/lua_ddlform_input_generator.UTF-16BE.xml
echo executing test lua_ddlform_input_generator UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_input_generator.lua run > temp/lua_ddlform_input_generator.UCS-2LE.xml
echo executing test lua_ddlform_input_generator UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_input_generator.lua run > temp/lua_ddlform_input_generator.UCS-2BE.xml
echo executing test lua_ddlform_input_generator UCS-4LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4LE/' | recode UTF-8..UCS-4LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_input_generator.lua run > temp/lua_ddlform_input_generator.UCS-4LE.xml
echo executing test lua_ddlform_input_generator UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_input_generator.lua run > temp/lua_ddlform_input_generator.UCS-4BE.xml
echo executing test lua_ddlform_input_generator_libxml2 UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_input_generator.lua run > temp/lua_ddlform_input_generator.UTF-8.xml
echo executing test lua_ddlform_input_generator_libxml2 UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_input_generator.lua run > temp/lua_ddlform_input_generator.UTF-16LE.xml
echo executing test lua_ddlform_input_generator_libxml2 UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_input_generator.lua run > temp/lua_ddlform_input_generator.UTF-16BE.xml
echo executing test lua_ddlform_input_generator_libxml2 UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_input_generator.lua run > temp/lua_ddlform_input_generator.UCS-2LE.xml
echo executing test lua_ddlform_input_generator_libxml2 UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_input_generator.lua run > temp/lua_ddlform_input_generator.UCS-2BE.xml
echo executing test lua_ddlform_input_generator_libxml2 UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_input_generator.lua run > temp/lua_ddlform_input_generator.UCS-4BE.xml
echo executing test lua_echo_input_table UTF-8
cat wolfilter/template/doc/employee_assignment_print_2.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/echo_input_table.lua run > temp/lua_echo_input_table.UTF-8.xml
echo executing test lua_echo_input_table UTF-16LE
cat wolfilter/template/doc/employee_assignment_print_2.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/echo_input_table.lua run > temp/lua_echo_input_table.UTF-16LE.xml
echo executing test lua_echo_input_table UTF-16BE
cat wolfilter/template/doc/employee_assignment_print_2.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/echo_input_table.lua run > temp/lua_echo_input_table.UTF-16BE.xml
echo executing test lua_echo_input_table UCS-2LE
cat wolfilter/template/doc/employee_assignment_print_2.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/echo_input_table.lua run > temp/lua_echo_input_table.UCS-2LE.xml
echo executing test lua_echo_input_table UCS-2BE
cat wolfilter/template/doc/employee_assignment_print_2.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/echo_input_table.lua run > temp/lua_echo_input_table.UCS-2BE.xml
echo executing test lua_echo_input_table UCS-4LE
cat wolfilter/template/doc/employee_assignment_print_2.UTF-8.xml | sed 's/UTF-8/UCS-4LE/' | recode UTF-8..UCS-4LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/echo_input_table.lua run > temp/lua_echo_input_table.UCS-4LE.xml
echo executing test lua_echo_input_table UCS-4BE
cat wolfilter/template/doc/employee_assignment_print_2.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/echo_input_table.lua run > temp/lua_echo_input_table.UCS-4BE.xml
echo executing test lua_echo_input_table_libxml2 UTF-8
cat wolfilter/template/doc/employee_assignment_print_2.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/echo_input_table.lua run > temp/lua_echo_input_table.UTF-8.xml
echo executing test lua_echo_input_table_libxml2 UTF-16LE
cat wolfilter/template/doc/employee_assignment_print_2.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/echo_input_table.lua run > temp/lua_echo_input_table.UTF-16LE.xml
echo executing test lua_echo_input_table_libxml2 UTF-16BE
cat wolfilter/template/doc/employee_assignment_print_2.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/echo_input_table.lua run > temp/lua_echo_input_table.UTF-16BE.xml
echo executing test lua_echo_input_table_libxml2 UCS-2LE
cat wolfilter/template/doc/employee_assignment_print_2.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/echo_input_table.lua run > temp/lua_echo_input_table.UCS-2LE.xml
echo executing test lua_echo_input_table_libxml2 UCS-2BE
cat wolfilter/template/doc/employee_assignment_print_2.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/echo_input_table.lua run > temp/lua_echo_input_table.UCS-2BE.xml
echo executing test lua_echo_input_table_libxml2 UCS-4BE
cat wolfilter/template/doc/employee_assignment_print_2.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/echo_input_table.lua run > temp/lua_echo_input_table.UCS-4BE.xml
echo executing test lua_peerformfunc_iterator UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_iterator.lua run > temp/lua_ddlform_iterator.UTF-8.xml
echo executing test lua_peerformfunc_iterator UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_iterator.lua run > temp/lua_ddlform_iterator.UTF-16LE.xml
echo executing test lua_peerformfunc_iterator UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_iterator.lua run > temp/lua_ddlform_iterator.UTF-16BE.xml
echo executing test lua_peerformfunc_iterator UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_iterator.lua run > temp/lua_ddlform_iterator.UCS-2LE.xml
echo executing test lua_peerformfunc_iterator UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_iterator.lua run > temp/lua_ddlform_iterator.UCS-2BE.xml
echo executing test lua_peerformfunc_iterator UCS-4LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4LE/' | recode UTF-8..UCS-4LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_iterator.lua run > temp/lua_ddlform_iterator.UCS-4LE.xml
echo executing test lua_peerformfunc_iterator UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_iterator.lua run > temp/lua_ddlform_iterator.UCS-4BE.xml
echo executing test lua_peerformfunc_iterator_libxml2 UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_iterator.lua run > temp/lua_ddlform_iterator.UTF-8.xml
echo executing test lua_peerformfunc_iterator_libxml2 UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_iterator.lua run > temp/lua_ddlform_iterator.UTF-16LE.xml
echo executing test lua_peerformfunc_iterator_libxml2 UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_iterator.lua run > temp/lua_ddlform_iterator.UTF-16BE.xml
echo executing test lua_peerformfunc_iterator_libxml2 UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_iterator.lua run > temp/lua_ddlform_iterator.UCS-2LE.xml
echo executing test lua_peerformfunc_iterator_libxml2 UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_iterator.lua run > temp/lua_ddlform_iterator.UCS-2BE.xml
echo executing test lua_peerformfunc_iterator_libxml2 UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --function transaction:echo_peer:employee_assignment_print:employee_assignment_print --script wolfilter/scripts/peerformfunc_iterator.lua run > temp/lua_ddlform_iterator.UCS-4BE.xml
echo executing test lua_echo_input_doctype_form UTF-8
cat wolfilter/template/doc/employee_assignment_doctype.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/echo_input_doctype_form.lua run > temp/lua_echo_input_doctype_form.UTF-8.xml
echo executing test lua_echo_input_doctype_form UTF-16LE
cat wolfilter/template/doc/employee_assignment_doctype.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/echo_input_doctype_form.lua run > temp/lua_echo_input_doctype_form.UTF-16LE.xml
echo executing test lua_echo_input_doctype_form UTF-16BE
cat wolfilter/template/doc/employee_assignment_doctype.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/echo_input_doctype_form.lua run > temp/lua_echo_input_doctype_form.UTF-16BE.xml
echo executing test lua_echo_input_doctype_form UCS-2LE
cat wolfilter/template/doc/employee_assignment_doctype.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/echo_input_doctype_form.lua run > temp/lua_echo_input_doctype_form.UCS-2LE.xml
echo executing test lua_echo_input_doctype_form UCS-2BE
cat wolfilter/template/doc/employee_assignment_doctype.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/echo_input_doctype_form.lua run > temp/lua_echo_input_doctype_form.UCS-2BE.xml
echo executing test lua_echo_input_doctype_form UCS-4LE
cat wolfilter/template/doc/employee_assignment_doctype.UTF-8.xml | sed 's/UTF-8/UCS-4LE/' | recode UTF-8..UCS-4LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/echo_input_doctype_form.lua run > temp/lua_echo_input_doctype_form.UCS-4LE.xml
echo executing test lua_echo_input_doctype_form UCS-4BE
cat wolfilter/template/doc/employee_assignment_doctype.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/echo_input_doctype_form.lua run > temp/lua_echo_input_doctype_form.UCS-4BE.xml
echo executing test lua_echo_input_doctype_form_libxml2 UTF-8
cat wolfilter/template/doc/employee_assignment_doctype.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/echo_input_doctype_form.lua run > temp/lua_echo_input_doctype_form.UTF-8.xml
echo executing test lua_echo_input_doctype_form_libxml2 UTF-16LE
cat wolfilter/template/doc/employee_assignment_doctype.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/echo_input_doctype_form.lua run > temp/lua_echo_input_doctype_form.UTF-16LE.xml
echo executing test lua_echo_input_doctype_form_libxml2 UTF-16BE
cat wolfilter/template/doc/employee_assignment_doctype.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/echo_input_doctype_form.lua run > temp/lua_echo_input_doctype_form.UTF-16BE.xml
echo executing test lua_echo_input_doctype_form_libxml2 UCS-2LE
cat wolfilter/template/doc/employee_assignment_doctype.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/echo_input_doctype_form.lua run > temp/lua_echo_input_doctype_form.UCS-2LE.xml
echo executing test lua_echo_input_doctype_form_libxml2 UCS-2BE
cat wolfilter/template/doc/employee_assignment_doctype.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/echo_input_doctype_form.lua run > temp/lua_echo_input_doctype_form.UCS-2BE.xml
echo executing test lua_echo_input_doctype_form_libxml2 UCS-4BE
cat wolfilter/template/doc/employee_assignment_doctype.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/echo_input_doctype_form.lua run > temp/lua_echo_input_doctype_form.UCS-4BE.xml
echo executing test map_formfunc UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf employee_assignment_convert > temp/map_formfunc.UTF-8.xml
echo executing test map_formfunc UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf employee_assignment_convert > temp/map_formfunc.UTF-16LE.xml
echo executing test map_formfunc UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf employee_assignment_convert > temp/map_formfunc.UTF-16BE.xml
echo executing test map_formfunc UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf employee_assignment_convert > temp/map_formfunc.UCS-2LE.xml
echo executing test map_formfunc UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf employee_assignment_convert > temp/map_formfunc.UCS-2BE.xml
echo executing test map_formfunc UCS-4LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4LE/' | recode UTF-8..UCS-4LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf employee_assignment_convert > temp/map_formfunc.UCS-4LE.xml
echo executing test map_formfunc UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf employee_assignment_convert > temp/map_formfunc.UCS-4BE.xml
echo executing test map_formfunc_libxml2 UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 employee_assignment_convert > temp/map_formfunc.UTF-8.xml
echo executing test map_formfunc_libxml2 UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 employee_assignment_convert > temp/map_formfunc.UTF-16LE.xml
echo executing test map_formfunc_libxml2 UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 employee_assignment_convert > temp/map_formfunc.UTF-16BE.xml
echo executing test map_formfunc_libxml2 UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 employee_assignment_convert > temp/map_formfunc.UCS-2LE.xml
echo executing test map_formfunc_libxml2 UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 employee_assignment_convert > temp/map_formfunc.UCS-2BE.xml
echo executing test map_formfunc_libxml2 UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 employee_assignment_convert > temp/map_formfunc.UCS-4BE.xml
echo executing test map_peerformfunc UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf echo_transaction > temp/map_ddlform.UTF-8.xml
echo executing test map_peerformfunc UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf echo_transaction > temp/map_ddlform.UTF-16LE.xml
echo executing test map_peerformfunc UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf echo_transaction > temp/map_ddlform.UTF-16BE.xml
echo executing test map_peerformfunc UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf echo_transaction > temp/map_ddlform.UCS-2LE.xml
echo executing test map_peerformfunc UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf echo_transaction > temp/map_ddlform.UCS-2BE.xml
echo executing test map_peerformfunc UCS-4LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4LE/' | recode UTF-8..UCS-4LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf echo_transaction > temp/map_ddlform.UCS-4LE.xml
echo executing test map_peerformfunc UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf echo_transaction > temp/map_ddlform.UCS-4BE.xml
echo executing test map_peerformfunc_libxml2 UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 echo_transaction > temp/map_ddlform.UTF-8.xml
echo executing test map_peerformfunc_libxml2 UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 echo_transaction > temp/map_ddlform.UTF-16LE.xml
echo executing test map_peerformfunc_libxml2 UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 echo_transaction > temp/map_ddlform.UTF-16BE.xml
echo executing test map_peerformfunc_libxml2 UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 echo_transaction > temp/map_ddlform.UCS-2LE.xml
echo executing test map_peerformfunc_libxml2 UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 echo_transaction > temp/map_ddlform.UCS-2BE.xml
echo executing test map_peerformfunc_libxml2 UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 echo_transaction > temp/map_ddlform.UCS-4BE.xml
echo executing test lua_formfunc_table UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/formfunc_table.lua run > temp/lua_formfunc_table.UTF-8.xml
echo executing test lua_formfunc_table UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/formfunc_table.lua run > temp/lua_formfunc_table.UTF-16LE.xml
echo executing test lua_formfunc_table UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/formfunc_table.lua run > temp/lua_formfunc_table.UTF-16BE.xml
echo executing test lua_formfunc_table UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/formfunc_table.lua run > temp/lua_formfunc_table.UCS-2LE.xml
echo executing test lua_formfunc_table UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/formfunc_table.lua run > temp/lua_formfunc_table.UCS-2BE.xml
echo executing test lua_formfunc_table UCS-4LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4LE/' | recode UTF-8..UCS-4LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/formfunc_table.lua run > temp/lua_formfunc_table.UCS-4LE.xml
echo executing test lua_formfunc_table UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/formfunc_table.lua run > temp/lua_formfunc_table.UCS-4BE.xml
echo executing test lua_formfunc_table_libxml2 UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/formfunc_table.lua run > temp/lua_formfunc_table.UTF-8.xml
echo executing test lua_formfunc_table_libxml2 UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/formfunc_table.lua run > temp/lua_formfunc_table.UTF-16LE.xml
echo executing test lua_formfunc_table_libxml2 UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/formfunc_table.lua run > temp/lua_formfunc_table.UTF-16BE.xml
echo executing test lua_formfunc_table_libxml2 UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/formfunc_table.lua run > temp/lua_formfunc_table.UCS-2LE.xml
echo executing test lua_formfunc_table_libxml2 UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/formfunc_table.lua run > temp/lua_formfunc_table.UCS-2BE.xml
echo executing test lua_formfunc_table_libxml2 UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/formfunc_table.lua run > temp/lua_formfunc_table.UCS-4BE.xml
echo executing test lua_formfunc_input_generator UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/formfunc_input_generator.lua run > temp/lua_formfunc_input_generator.UTF-8.xml
echo executing test lua_formfunc_input_generator UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/formfunc_input_generator.lua run > temp/lua_formfunc_input_generator.UTF-16LE.xml
echo executing test lua_formfunc_input_generator UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/formfunc_input_generator.lua run > temp/lua_formfunc_input_generator.UTF-16BE.xml
echo executing test lua_formfunc_input_generator UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/formfunc_input_generator.lua run > temp/lua_formfunc_input_generator.UCS-2LE.xml
echo executing test lua_formfunc_input_generator UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/formfunc_input_generator.lua run > temp/lua_formfunc_input_generator.UCS-2BE.xml
echo executing test lua_formfunc_input_generator UCS-4LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4LE/' | recode UTF-8..UCS-4LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/formfunc_input_generator.lua run > temp/lua_formfunc_input_generator.UCS-4LE.xml
echo executing test lua_formfunc_input_generator UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/formfunc_input_generator.lua run > temp/lua_formfunc_input_generator.UCS-4BE.xml
echo executing test lua_formfunc_input_generator_libxml2 UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/formfunc_input_generator.lua run > temp/lua_formfunc_input_generator.UTF-8.xml
echo executing test lua_formfunc_input_generator_libxml2 UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/formfunc_input_generator.lua run > temp/lua_formfunc_input_generator.UTF-16LE.xml
echo executing test lua_formfunc_input_generator_libxml2 UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/formfunc_input_generator.lua run > temp/lua_formfunc_input_generator.UTF-16BE.xml
echo executing test lua_formfunc_input_generator_libxml2 UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/formfunc_input_generator.lua run > temp/lua_formfunc_input_generator.UCS-2LE.xml
echo executing test lua_formfunc_input_generator_libxml2 UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/formfunc_input_generator.lua run > temp/lua_formfunc_input_generator.UCS-2BE.xml
echo executing test lua_formfunc_input_generator_libxml2 UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/formfunc_input_generator.lua run > temp/lua_formfunc_input_generator.UCS-4BE.xml
echo executing test map_ddlform UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform employee_assignment_print > temp/map_ddlform.UTF-8.xml
echo executing test map_ddlform UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform employee_assignment_print > temp/map_ddlform.UTF-16LE.xml
echo executing test map_ddlform UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform employee_assignment_print > temp/map_ddlform.UTF-16BE.xml
echo executing test map_ddlform UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform employee_assignment_print > temp/map_ddlform.UCS-2LE.xml
echo executing test map_ddlform UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform employee_assignment_print > temp/map_ddlform.UCS-2BE.xml
echo executing test map_ddlform UCS-4LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4LE/' | recode UTF-8..UCS-4LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform employee_assignment_print > temp/map_ddlform.UCS-4LE.xml
echo executing test map_ddlform UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform employee_assignment_print > temp/map_ddlform.UCS-4BE.xml
echo executing test map_ddlform_libxml2 UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform employee_assignment_print > temp/map_ddlform.UTF-8.xml
echo executing test map_ddlform_libxml2 UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform employee_assignment_print > temp/map_ddlform.UTF-16LE.xml
echo executing test map_ddlform_libxml2 UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform employee_assignment_print > temp/map_ddlform.UTF-16BE.xml
echo executing test map_ddlform_libxml2 UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform employee_assignment_print > temp/map_ddlform.UCS-2LE.xml
echo executing test map_ddlform_libxml2 UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform employee_assignment_print > temp/map_ddlform.UCS-2BE.xml
echo executing test map_ddlform_libxml2 UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform employee_assignment_print > temp/map_ddlform.UCS-4BE.xml
echo executing test lua_echo_input_generator UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/echo_input_generator.lua run > temp/employee_assignment_print.UTF-8.xml
echo executing test lua_echo_input_generator UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/echo_input_generator.lua run > temp/employee_assignment_print.UTF-16LE.xml
echo executing test lua_echo_input_generator UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/echo_input_generator.lua run > temp/employee_assignment_print.UTF-16BE.xml
echo executing test lua_echo_input_generator UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/echo_input_generator.lua run > temp/employee_assignment_print.UCS-2LE.xml
echo executing test lua_echo_input_generator UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/echo_input_generator.lua run > temp/employee_assignment_print.UCS-2BE.xml
echo executing test lua_echo_input_generator UCS-4LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4LE/' | recode UTF-8..UCS-4LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/echo_input_generator.lua run > temp/employee_assignment_print.UCS-4LE.xml
echo executing test lua_echo_input_generator UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/echo_input_generator.lua run > temp/employee_assignment_print.UCS-4BE.xml
echo executing test lua_echo_input_generator_libxml2 UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/echo_input_generator.lua run > temp/employee_assignment_print.UTF-8.xml
echo executing test lua_echo_input_generator_libxml2 UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/echo_input_generator.lua run > temp/employee_assignment_print.UTF-16LE.xml
echo executing test lua_echo_input_generator_libxml2 UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/echo_input_generator.lua run > temp/employee_assignment_print.UTF-16BE.xml
echo executing test lua_echo_input_generator_libxml2 UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/echo_input_generator.lua run > temp/employee_assignment_print.UCS-2LE.xml
echo executing test lua_echo_input_generator_libxml2 UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/echo_input_generator.lua run > temp/employee_assignment_print.UCS-2BE.xml
echo executing test lua_echo_input_generator_libxml2 UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/echo_input_generator.lua run > temp/employee_assignment_print.UCS-4BE.xml
echo executing test lua_formfunc_iterator UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/formfunc_iterator.lua run > temp/lua_formfunc_iterator.UTF-8.xml
echo executing test lua_formfunc_iterator UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/formfunc_iterator.lua run > temp/lua_formfunc_iterator.UTF-16LE.xml
echo executing test lua_formfunc_iterator UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/formfunc_iterator.lua run > temp/lua_formfunc_iterator.UTF-16BE.xml
echo executing test lua_formfunc_iterator UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/formfunc_iterator.lua run > temp/lua_formfunc_iterator.UCS-2LE.xml
echo executing test lua_formfunc_iterator UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/formfunc_iterator.lua run > temp/lua_formfunc_iterator.UCS-2BE.xml
echo executing test lua_formfunc_iterator UCS-4LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4LE/' | recode UTF-8..UCS-4LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/formfunc_iterator.lua run > temp/lua_formfunc_iterator.UCS-4LE.xml
echo executing test lua_formfunc_iterator UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --script wolfilter/scripts/formfunc_iterator.lua run > temp/lua_formfunc_iterator.UCS-4BE.xml
echo executing test lua_formfunc_iterator_libxml2 UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/formfunc_iterator.lua run > temp/lua_formfunc_iterator.UTF-8.xml
echo executing test lua_formfunc_iterator_libxml2 UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/formfunc_iterator.lua run > temp/lua_formfunc_iterator.UTF-16LE.xml
echo executing test lua_formfunc_iterator_libxml2 UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/formfunc_iterator.lua run > temp/lua_formfunc_iterator.UTF-16BE.xml
echo executing test lua_formfunc_iterator_libxml2 UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/formfunc_iterator.lua run > temp/lua_formfunc_iterator.UCS-2LE.xml
echo executing test lua_formfunc_iterator_libxml2 UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/formfunc_iterator.lua run > temp/lua_formfunc_iterator.UCS-2BE.xml
echo executing test lua_formfunc_iterator_libxml2 UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --script wolfilter/scripts/formfunc_iterator.lua run > temp/lua_formfunc_iterator.UCS-4BE.xml
echo executing test filter_employee_assignment_print UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf - > temp/employee_assignment_print.UTF-8.xml
echo executing test filter_employee_assignment_print UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf - > temp/employee_assignment_print.UTF-16LE.xml
echo executing test filter_employee_assignment_print UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf - > temp/employee_assignment_print.UTF-16BE.xml
echo executing test filter_employee_assignment_print UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf - > temp/employee_assignment_print.UCS-2LE.xml
echo executing test filter_employee_assignment_print UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf - > temp/employee_assignment_print.UCS-2BE.xml
echo executing test filter_employee_assignment_print UCS-4LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4LE/' | recode UTF-8..UCS-4LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf - > temp/employee_assignment_print.UCS-4LE.xml
echo executing test filter_employee_assignment_print UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf - > temp/employee_assignment_print.UCS-4BE.xml
echo executing test filter_employee_assignment_print_libxml2 UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 - > temp/employee_assignment_print.UTF-8.xml
echo executing test filter_employee_assignment_print_libxml2 UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 - > temp/employee_assignment_print.UTF-16LE.xml
echo executing test filter_employee_assignment_print_libxml2 UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 - > temp/employee_assignment_print.UTF-16BE.xml
echo executing test filter_employee_assignment_print_libxml2 UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 - > temp/employee_assignment_print.UCS-2LE.xml
echo executing test filter_employee_assignment_print_libxml2 UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 - > temp/employee_assignment_print.UCS-2BE.xml
echo executing test filter_employee_assignment_print_libxml2 UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 - > temp/employee_assignment_print.UCS-4BE.xml
echo executing test lua_ddlform_iterator UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_iterator.lua run > temp/lua_ddlform_iterator.UTF-8.xml
echo executing test lua_ddlform_iterator UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_iterator.lua run > temp/lua_ddlform_iterator.UTF-16LE.xml
echo executing test lua_ddlform_iterator UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_iterator.lua run > temp/lua_ddlform_iterator.UTF-16BE.xml
echo executing test lua_ddlform_iterator UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_iterator.lua run > temp/lua_ddlform_iterator.UCS-2LE.xml
echo executing test lua_ddlform_iterator UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_iterator.lua run > temp/lua_ddlform_iterator.UCS-2BE.xml
echo executing test lua_ddlform_iterator UCS-4LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4LE/' | recode UTF-8..UCS-4LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_iterator.lua run > temp/lua_ddlform_iterator.UCS-4LE.xml
echo executing test lua_ddlform_iterator UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_iterator.lua run > temp/lua_ddlform_iterator.UCS-4BE.xml
echo executing test lua_ddlform_iterator_libxml2 UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_iterator.lua run > temp/lua_ddlform_iterator.UTF-8.xml
echo executing test lua_ddlform_iterator_libxml2 UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_iterator.lua run > temp/lua_ddlform_iterator.UTF-16LE.xml
echo executing test lua_ddlform_iterator_libxml2 UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_iterator.lua run > temp/lua_ddlform_iterator.UTF-16BE.xml
echo executing test lua_ddlform_iterator_libxml2 UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_iterator.lua run > temp/lua_ddlform_iterator.UCS-2LE.xml
echo executing test lua_ddlform_iterator_libxml2 UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_iterator.lua run > temp/lua_ddlform_iterator.UCS-2BE.xml
echo executing test lua_ddlform_iterator_libxml2 UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_iterator.lua run > temp/lua_ddlform_iterator.UCS-4BE.xml
echo executing test lua_echo_input_doctype_table UTF-8
cat wolfilter/template/doc/employee_assignment_doctype_2.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/echo_input_doctype_table.lua run > temp/lua_echo_input_doctype_table.UTF-8.xml
echo executing test lua_echo_input_doctype_table UTF-16LE
cat wolfilter/template/doc/employee_assignment_doctype_2.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/echo_input_doctype_table.lua run > temp/lua_echo_input_doctype_table.UTF-16LE.xml
echo executing test lua_echo_input_doctype_table UTF-16BE
cat wolfilter/template/doc/employee_assignment_doctype_2.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/echo_input_doctype_table.lua run > temp/lua_echo_input_doctype_table.UTF-16BE.xml
echo executing test lua_echo_input_doctype_table UCS-2LE
cat wolfilter/template/doc/employee_assignment_doctype_2.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/echo_input_doctype_table.lua run > temp/lua_echo_input_doctype_table.UCS-2LE.xml
echo executing test lua_echo_input_doctype_table UCS-2BE
cat wolfilter/template/doc/employee_assignment_doctype_2.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/echo_input_doctype_table.lua run > temp/lua_echo_input_doctype_table.UCS-2BE.xml
echo executing test lua_echo_input_doctype_table UCS-4LE
cat wolfilter/template/doc/employee_assignment_doctype_2.UTF-8.xml | sed 's/UTF-8/UCS-4LE/' | recode UTF-8..UCS-4LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/echo_input_doctype_table.lua run > temp/lua_echo_input_doctype_table.UCS-4LE.xml
echo executing test lua_echo_input_doctype_table UCS-4BE
cat wolfilter/template/doc/employee_assignment_doctype_2.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/echo_input_doctype_table.lua run > temp/lua_echo_input_doctype_table.UCS-4BE.xml
echo executing test lua_echo_input_doctype_table_libxml2 UTF-8
cat wolfilter/template/doc/employee_assignment_doctype_2.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/echo_input_doctype_table.lua run > temp/lua_echo_input_doctype_table.UTF-8.xml
echo executing test lua_echo_input_doctype_table_libxml2 UTF-16LE
cat wolfilter/template/doc/employee_assignment_doctype_2.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/echo_input_doctype_table.lua run > temp/lua_echo_input_doctype_table.UTF-16LE.xml
echo executing test lua_echo_input_doctype_table_libxml2 UTF-16BE
cat wolfilter/template/doc/employee_assignment_doctype_2.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/echo_input_doctype_table.lua run > temp/lua_echo_input_doctype_table.UTF-16BE.xml
echo executing test lua_echo_input_doctype_table_libxml2 UCS-2LE
cat wolfilter/template/doc/employee_assignment_doctype_2.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/echo_input_doctype_table.lua run > temp/lua_echo_input_doctype_table.UCS-2LE.xml
echo executing test lua_echo_input_doctype_table_libxml2 UCS-2BE
cat wolfilter/template/doc/employee_assignment_doctype_2.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/echo_input_doctype_table.lua run > temp/lua_echo_input_doctype_table.UCS-2BE.xml
echo executing test lua_echo_input_doctype_table_libxml2 UCS-4BE
cat wolfilter/template/doc/employee_assignment_doctype_2.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/echo_input_doctype_table.lua run > temp/lua_echo_input_doctype_table.UCS-4BE.xml
echo executing test lua_ddlform_table UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_table.lua run > temp/lua_ddlform_table.UTF-8.xml
echo executing test lua_ddlform_table UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_table.lua run > temp/lua_ddlform_table.UTF-16LE.xml
echo executing test lua_ddlform_table UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_table.lua run > temp/lua_ddlform_table.UTF-16BE.xml
echo executing test lua_ddlform_table UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_table.lua run > temp/lua_ddlform_table.UCS-2LE.xml
echo executing test lua_ddlform_table UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_table.lua run > temp/lua_ddlform_table.UCS-2BE.xml
echo executing test lua_ddlform_table UCS-4LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4LE/' | recode UTF-8..UCS-4LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_table.lua run > temp/lua_ddlform_table.UCS-4LE.xml
echo executing test lua_ddlform_table UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:textwolf --output-filter xml:textwolf --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_table.lua run > temp/lua_ddlform_table.UCS-4BE.xml
echo executing test lua_ddlform_table_libxml2 UTF-8
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-8/' | recode UTF-8..UTF-8 | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_table.lua run > temp/lua_ddlform_table.UTF-8.xml
echo executing test lua_ddlform_table_libxml2 UTF-16LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16LE/' | recode UTF-8..UTF-16LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_table.lua run > temp/lua_ddlform_table.UTF-16LE.xml
echo executing test lua_ddlform_table_libxml2 UTF-16BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UTF-16BE/' | recode UTF-8..UTF-16BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_table.lua run > temp/lua_ddlform_table.UTF-16BE.xml
echo executing test lua_ddlform_table_libxml2 UCS-2LE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2LE/' | recode UTF-8..UCS-2LE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_table.lua run > temp/lua_ddlform_table.UCS-2LE.xml
echo executing test lua_ddlform_table_libxml2 UCS-2BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-2BE/' | recode UTF-8..UCS-2BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_table.lua run > temp/lua_ddlform_table.UCS-2BE.xml
echo executing test lua_ddlform_table_libxml2 UCS-4BE
cat wolfilter/template/doc/employee_assignment_print.UTF-8.xml | sed 's/UTF-8/UCS-4BE/' | recode UTF-8..UCS-4BE | ../wtest/cleanInput BOM EOLN | ../src/wolfilter --input-filter xml:libxml2 --output-filter xml:libxml2 --form wolfilter/scripts/employee_assignment_print.simpleform --script wolfilter/scripts/ddlform_table.lua run > temp/lua_ddlform_table.UCS-4BE.xml
