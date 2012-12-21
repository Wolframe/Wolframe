#!/bin/sh
for dd in input output; do
echo "&#x59DA; &#x660E;" | recode XML-standalone..UTF-16BE > ../lua_script_1.$dd.xml
echo "yao ming"          | recode XML-standalone..UTF-16BE >> ../lua_script_1.$dd.xml
echo "&#x59DA; &#x660E;" | recode XML-standalone..UTF-8 > ../lua_script_2.$dd.xml
echo "yao ming"          | recode XML-standalone..UTF-8 >> ../lua_script_2.$dd.xml
done
./xmltestdoc.sh 'UTF-8' '<recipe><item amount="100g">chocolate</item><item amount="0.5l">milk</item></recipe>' > ../lua_script_3.input.xml
./xmltestdoc.sh 'UTF-8' '<recipe><item amount="100g">chocolate</item><item amount="0.5l">milk</item></recipe>' > ../lua_script_3.output.xml
echo "" >> ../lua_script_3.output.xml
./xmltestdoc.sh 'UCS-2LE' '<item><name>example</name><content><color>blue</color><material>metal</material></content><price>23.45</price></item>' > ../lua_script_4.input.xml
./xmltestdoc.sh 'UCS-2LE' '<item><name>example</name><content><color>blue</color><material>metal</material></content><price>23.45</price></item>' > ../lua_script_4.output.xml
./xmltestdoc.sh 'UTF-16BE' '<doc><name>mumbay</name><country>india</country><continent>asia</continent></doc>' > ../lua_script_5.input.xml
./xmltestdoc.sh 'UTF-16BE' '<doc><name>mumbay</name><country>india</country></doc>' > ../lua_script_5.output.xml
./xmltestdoc.sh 'UTF-16LE' '<customer><name>P&#xE4;rnu Linnavalitsus</name><address><street>Suur-Sepa 16</street><district>80098 Pärnu</district><country>372</country></address></customer>' > ../ddl_simpleform_1.input.xml
./xmltestdoc.sh 'UTF-16LE' '<customer id="1"><name>P&#xE4;rnu Linnavalitsus</name><address><street>Suur-Sepa 16</street><district>80098 Pärnu</district><country>372</country></address></customer>' > ../ddl_simpleform_1.output.xml
echo "" | recode UTF-8..UTF-16LE >> ../ddl_simpleform_1.output.xml
