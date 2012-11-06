**
**requires:LUA
**requires:SQLITE3
**input
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<class name='indogermanic'>
	<class name='celtic' status='living'>
		<class name='gaulisch' status='dead'></class>
		<class name='goidelic' status='dead'>
			<class name='old irish' status='dead'></class>
			<class name='middle irish' status='dead'></class>
			<class name='manx' status='dead'></class>
			<class name='irish' status='living'></class>
			<class name='scotts gaelic' status='living'></class>
		</class>
		<class name='brythonic' status='dead'>
			<class name='comish' status='dead'></class>
			<class name='welsh' status='living'></class>
			<class name='breton' status='living'></class>
		</class>
	</class>
	<class name='germanic' status='living'>
		<class name='west germanic' status='living'>
			<class name='anglo-frisian' status='living'>
				<class name='old english' status='dead'>
					<class name='middle english' status='dead'>
						<class name='english' status='living'></class>
					</class>
				</class>
				<class name='old frisian' status='dead'>
					<class name='frisian' status='living'></class>
				</class>
			</class>
			<class name='german' status='living'>
				<class name='low german' status='living'>
					<class name='old saxon' status='dead'>
						<class name='plattdeutsch' status='dead'></class>
					</class>
					<class name='old low franconian' status='dead'>
						<class name='dutch' status='living'></class>
						<class name='flemish' status='living'></class>
						<class name='afrikaans' status='living'></class>
						<class name='south african dutch' status='dead'></class>
					</class>
				</class>
				<class name='high german' status='living'>
					<class name='alemannic' status='living'></class>
					<class name='alsatian' status='living'></class>
					<class name='bavarian' status='living'></class>
					<class name='franconian' status='living'></class>
					<class name='german' status='living'></class>
					<class name='pensilvania german' status='living'></class>
					<class name='swiss' status='living'></class>
					<class name='yiddish' status='living'></class>
				</class>
			</class>
		</class>
		<class name='east germanic' status='living'>
		</class>
		<class name='north germanic' status='living'>
			<class name='old west norse' status='living'>
				<class name='islandic' status='living'></class>
				<class name='faroese' status='living'></class>
			</class>
			<class name='old east norse' status='living'>
				<node>
					<class name='norwegian' status='living'></class>
				</node>
				<node>
					<class name='danish' status='living'></class>
					<class name='swedish' status='living'></class>
				</node>
			</class>
		</class>
	</class>
	<class name='italic' status='living'>
		<class name='oscan' status='dead'></class>
		<class name='umbrian' status='dead'></class>
		<class name='old latin' status='living'>
			<class name='catalan' status='living'></class>
			<class name='french' status='living'></class>
			<class name='galician' status='living'></class>
			<class name='portuguese' status='living'></class>
			<class name='italian' status='living'></class>
			<class name='provencal' status='living'></class>
			<class name='romansch' status='living'></class>
			<class name='romanian' status='living'></class>
			<class name='spanish' status='living'></class>
		</class>
	</class>
	<class name='slavonic' status='living'>
		<class name='west slavic' status='living'>
			<class name='chech' status='living'></class>
			<class name='polish' status='living'></class>
			<class name='slovak' status='living'></class>
			<class name='sorbian' status='living'></class>
		</class>
		<class name='east slavic' status='living'>
			<class name='belarussian' status='living'></class>
			<class name='russian' status='living'></class>
			<class name='ukrainian' status='living'></class>
		</class>
		<class name='south slavic' status='living'>
			<class name='bosnian' status='living'></class>
			<class name='bulgarian' status='living'></class>
			<class name='macedonian' status='living'></class>
			<class name='serbo-croatian' status='living'></class>
			<class name='slovene' status='living'></class>
		</class>
	</class>
	<class name='albanian' status='living'>
	</class>
	<class name='armenian' status='living'>
	</class>
	<class name='hellenic' status='living'>
		<class name='greek' status='living'></class>
	</class>
	<class name='baltic' status='living'>
		<node>
			<class name='lettish' status='living'></class>
		</node>
		<node>
			<class name='latvian' status='living'></class>
			<class name='lithuanian' status='living'></class>
		</node>
	</class>
	<class name='hittie' status='dead'>
	</class>
	<class name='indo iranian' status='living'>
		<class name='iranian' status='living'>
			<class name='avestan' status='dead'>
				<class name='pashto' status='living'></class>
			</class>
			<class name='old persian' status='dead'>
				<class name='balushti' status='living'></class>
				<class name='kurdish' status='living'></class>
				<class name='ossetic' status='living'></class>
				<class name='pashto' status='living'></class>
				<class name='persian' status='living'></class>
			</class>
			<class name='scythian' status='dead'></class>
		</class>
		<class name='indic' status='living'>
			<class name='sanskrit' status='dead'></class>
			<class name='prakrit' status='dead'></class>
			<class name='pali' status='dead'></class>
			<class name='bengali' status='living'></class>
			<class name='bihari' status='living'></class>
			<class name='bhili' status='living'></class>
			<class name='gujarati' status='living'></class>
			<class name='hindi' status='living'></class>
			<class name='hindustani' status='living'></class>
			<class name='marati' status='living'></class>
			<class name='nepali' status='living'></class>
			<class name='bahari' status='living'></class>
			<class name='punjabi' status='living'></class>
			<class name='rajasthani' status='living'></class>
			<class name='sindhi' status='living'></class>
			<class name='singhalese' status='living'></class>
			<class name='urdu' status='living'></class>
		</class>
	</class>
	<class name='tocharian' status='dead'>
	</class>
</class>**config
--input-filter xml:textwolf --output-filter xml:textwolf --module ../../src/modules/filter/textwolf/mod_filter_textwolf  --module ../../src/modules/cmdbind/lua/mod_command_lua --program=transaction_sqlite_demo_ig_lang_cat.lua --program simpleform.normalize --module ../../src/modules/normalize//number/mod_normalize_number --module ../../src/modules/cmdbind/directmap/mod_command_directmap --module ../wolfilter/modules/database/sqlite3/mod_db_sqlite3test --database 'identifier=testdb,file=test.db,dumpfile=DBDUMP,inputfile=DBDATA' --program=DBPRG.tdl run

**file:simpleform.normalize
int=number:integer;
uint=number:unsigned;
float=number:float;
**file: DBDATA
--
-- Generic tree implementation for SQL databases
-- (Modified Preorder Tree Traversal)
--
-- Note: the parent restriction assumes that references accept NULL
-- Note: if DEFERRED does not work on UNIQUE constraints then the
--       UNIQUE constraint must be dropped
-- Joe Celko example from http://www.ibase.ru/devinfo/DBMSTrees/sqltrees.html

CREATE TABLE tree (
 ID INTEGER PRIMARY KEY AUTOINCREMENT,
 parent INT REFERENCES tree( ID ),
 name TEXT,
 lft INT NOT NULL UNIQUE DEFERRABLE CHECK ( lft > 0 ),
 rgt INT NOT NULL UNIQUE DEFERRABLE CHECK ( rgt > 1 ),
 CONSTRAINT order_check CHECK ( rgt > lft )
);
**file:DBPRG.tdl
--
-- treeAddRoot
--
TRANSACTION treeAddRoot -- (/node/name)
BEGIN
	FOREACH /node DO INSERT INTO tree (parent, name, lft, rgt) VALUES (0, $(name), 1, 2);
END

--
-- treeAddNode
--
TRANSACTION treeAddNode -- (/node/parentid, /node/name)
BEGIN
	FOREACH /node DO NONEMPTY SELECT rgt FROM tree WHERE ID = $(parentid);
	DO UPDATE tree SET lft = lft + 2 WHERE lft > $1;
	DO UPDATE tree SET rgt = rgt + 2 WHERE rgt >= $1;
	FOREACH /node DO INSERT INTO tree (parent, name, lft, rgt) VALUES ($(parentid), $(name), $1, $1+1);
END

--
-- treeDeleteSubtree
--
TRANSACTION treeDeleteSubtree -- ( /node/id)
BEGIN
	FOREACH /node DO NONEMPTY SELECT lft,rgt,width=rgt-lft+1 FROM tree WHERE ID = $(id);
	DO DELETE FROM tree WHERE lft >= $1 AND lft <= $2;
	DO UPDATE tree SET lft = lft-$3 WHERE lft>$2;
	DO UPDATE tree SET rgt = rgt-$3 WHERE rgt>$2;
END


--
-- treeSelectNodeAndParents       :Get the node and its parents
-- treeSelectNodeAndParentsByName :Get the node and its parents by name
--
TRANSACTION treeSelectNodeAndParents -- (/node/id)
BEGIN
	FOREACH /node INTO /node DO SELECT P2.ID,P2.name FROM tree AS P1, tree AS P2 WHERE P1.lft BETWEEN P2.lft AND P2.rgt AND P1.ID = $(id);
END
TRANSACTION treeSelectNodeAndParentsByName -- (/node/name)
BEGIN
	FOREACH /node INTO /node DO SELECT P2.ID,P2.name FROM tree AS P1, tree AS P2 WHERE P1.lft BETWEEN P2.lft AND P2.rgt AND P1.name = $(name);
END

--
-- treeSelectParents       :Get the parents of a node
-- treeSelectParentsByName :Get the parents of a node by name
--
TRANSACTION treeSelectParents -- (/node/id)
BEGIN
	FOREACH /node INTO /node DO SELECT P2.ID,P2.name FROM tree AS P1, tree AS P2 WHERE P1.lft > P2.lft AND P1.lft < P2.rgt AND P1.ID = $(id);
END
TRANSACTION treeSelectParentsByName -- (/node/name)
BEGIN
	FOREACH /node INTO /node DO SELECT P2.ID,P2.name FROM tree AS P1, tree AS P2 WHERE P1.lft > P2.lft AND P1.lft < P2.rgt AND P1.name = $(name);
END

--
-- treeSelectNodeAndChildren       :Get the node and its children
-- treeSelectNodeAndChildrenByName :Get the node and its children by name
--
TRANSACTION treeSelectNodeAndChildren -- (/node/id)
BEGIN
	FOREACH /node INTO /node DO SELECT P2.ID,P2.name FROM tree AS P1, tree AS P2 WHERE P1.lft BETWEEN P2.lft AND P2.rgt AND P2.ID = $(id);
END
TRANSACTION treeSelectNodeAndChildrenByName -- (/node/name)
BEGIN
	FOREACH /node INTO /node DO SELECT P2.ID,P2.name FROM tree AS P1, tree AS P2 WHERE P1.lft BETWEEN P2.lft AND P2.rgt AND P2.name = $(name);
END

--
-- treeSelectChildren       :Get the children of a node
-- treeSelectChildrenByName :Get the children of a node by name
--
TRANSACTION treeSelectChildren -- (/node/id)
BEGIN
	FOREACH /node INTO /node DO SELECT P2.ID,P2.name FROM tree AS P1, tree AS P2 WHERE P1.lft > P2.lft AND P1.lft < P2.rgt AND P2.ID = $(id);
END
TRANSACTION treeSelectChildrenByName -- (/node/name)
BEGIN
	FOREACH /node INTO /node DO SELECT P2.ID,P2.name FROM tree AS P1, tree AS P2 WHERE P1.lft > P2.lft AND P1.lft < P2.rgt AND P2.name = $(name);
END

--
-- treeSelectNodeWithLevels :Get all the nodes with their level
--
TRANSACTION treeSelectNodeWithLevels
BEGIN
	INTO /node DO SELECT count(P2.*) AS level, P1.ID, P1.name FROM tree AS P1, tree AS P2 WHERE P1.lft BETWEEN P2.lft AND P2.rgt GROUP BY P1.name ORDER BY level ASC;
END
**outputfile:DBDUMP
**file: transaction_sqlite_demo_ig_lang_cat.lua
idcnt = 0

function insert_class( parentid, itr)
	local id = idcnt + 1
	for v,t in itr do
		if (t == "name") then
			local name = v
			if idcnt == 0 then
				local f = formfunction( "treeAddRoot")
				f( { node = { name=name } } )
			else
				local f = formfunction( "treeAddNode")
				f( { node = { name=name, parentid=parentid} } )
			end
			idcnt = idcnt + 1
		end
		if (t == "class") then
			insert_class( id, scope( itr))
		end
	end
end

function run()
	filter().empty = false
	output:opentag( "class")
	local itr = input:get()
	for v,t in itr do
		if t == "class" then
			insert_class( idcnt, scope( itr))
		end
	end
	output:closetag()
end

**output
<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<class/>
tree:
'1', '0', 'indogermanic', '1', '232'
'2', '1', 'celtic', '2', '25'
'3', '2', 'gaulisch', '3', '4'
'4', '2', 'goidelic', '5', '16'
'5', '4', 'old irish', '6', '7'
'6', '4', 'middle irish', '8', '9'
'7', '4', 'manx', '10', '11'
'8', '4', 'irish', '12', '13'
'9', '4', 'scotts gaelic', '14', '15'
'10', '2', 'brythonic', '17', '24'
'11', '10', 'comish', '18', '19'
'12', '10', 'welsh', '20', '21'
'13', '10', 'breton', '22', '23'
'14', '1', 'germanic', '26', '95'
'15', '14', 'west germanic', '27', '76'
'16', '15', 'anglo-frisian', '28', '39'
'17', '16', 'old english', '29', '34'
'18', '17', 'middle english', '30', '33'
'19', '18', 'english', '31', '32'
'20', '16', 'old frisian', '35', '38'
'21', '20', 'frisian', '36', '37'
'22', '15', 'german', '40', '75'
'23', '22', 'low german', '41', '56'
'24', '23', 'old saxon', '42', '45'
'25', '24', 'plattdeutsch', '43', '44'
'26', '23', 'old low franconian', '46', '55'
'27', '26', 'dutch', '47', '48'
'28', '26', 'flemish', '49', '50'
'29', '26', 'afrikaans', '51', '52'
'30', '26', 'south african dutch', '53', '54'
'31', '22', 'high german', '57', '74'
'32', '31', 'alemannic', '58', '59'
'33', '31', 'alsatian', '60', '61'
'34', '31', 'bavarian', '62', '63'
'35', '31', 'franconian', '64', '65'
'36', '31', 'german', '66', '67'
'37', '31', 'pensilvania german', '68', '69'
'38', '31', 'swiss', '70', '71'
'39', '31', 'yiddish', '72', '73'
'40', '14', 'east germanic', '77', '78'
'41', '14', 'north germanic', '79', '94'
'42', '41', 'old west norse', '80', '85'
'43', '42', 'islandic', '81', '82'
'44', '42', 'faroese', '83', '84'
'45', '41', 'old east norse', '86', '93'
'46', '45', 'norwegian', '87', '88'
'47', '45', 'danish', '89', '90'
'48', '45', 'swedish', '91', '92'
'49', '1', 'italic', '96', '121'
'50', '49', 'oscan', '97', '98'
'51', '49', 'umbrian', '99', '100'
'52', '49', 'old latin', '101', '120'
'53', '52', 'catalan', '102', '103'
'54', '52', 'french', '104', '105'
'55', '52', 'galician', '106', '107'
'56', '52', 'portuguese', '108', '109'
'57', '52', 'italian', '110', '111'
'58', '52', 'provencal', '112', '113'
'59', '52', 'romansch', '114', '115'
'60', '52', 'romanian', '116', '117'
'61', '52', 'spanish', '118', '119'
'62', '1', 'slavonic', '122', '153'
'63', '62', 'west slavic', '123', '132'
'64', '63', 'chech', '124', '125'
'65', '63', 'polish', '126', '127'
'66', '63', 'slovak', '128', '129'
'67', '63', 'sorbian', '130', '131'
'68', '62', 'east slavic', '133', '140'
'69', '68', 'belarussian', '134', '135'
'70', '68', 'russian', '136', '137'
'71', '68', 'ukrainian', '138', '139'
'72', '62', 'south slavic', '141', '152'
'73', '72', 'bosnian', '142', '143'
'74', '72', 'bulgarian', '144', '145'
'75', '72', 'macedonian', '146', '147'
'76', '72', 'serbo-croatian', '148', '149'
'77', '72', 'slovene', '150', '151'
'78', '1', 'albanian', '154', '155'
'79', '1', 'armenian', '156', '157'
'80', '1', 'hellenic', '158', '161'
'81', '80', 'greek', '159', '160'
'82', '1', 'baltic', '162', '169'
'83', '82', 'lettish', '163', '164'
'84', '82', 'latvian', '165', '166'
'85', '82', 'lithuanian', '167', '168'
'86', '1', 'hittie', '170', '171'
'87', '1', 'indo iranian', '172', '229'
'88', '87', 'iranian', '173', '192'
'89', '88', 'avestan', '174', '177'
'90', '89', 'pashto', '175', '176'
'91', '88', 'old persian', '178', '189'
'92', '91', 'balushti', '179', '180'
'93', '91', 'kurdish', '181', '182'
'94', '91', 'ossetic', '183', '184'
'95', '91', 'pashto', '185', '186'
'96', '91', 'persian', '187', '188'
'97', '88', 'scythian', '190', '191'
'98', '87', 'indic', '193', '228'
'99', '98', 'sanskrit', '194', '195'
'100', '98', 'prakrit', '196', '197'
'101', '98', 'pali', '198', '199'
'102', '98', 'bengali', '200', '201'
'103', '98', 'bihari', '202', '203'
'104', '98', 'bhili', '204', '205'
'105', '98', 'gujarati', '206', '207'
'106', '98', 'hindi', '208', '209'
'107', '98', 'hindustani', '210', '211'
'108', '98', 'marati', '212', '213'
'109', '98', 'nepali', '214', '215'
'110', '98', 'bahari', '216', '217'
'111', '98', 'punjabi', '218', '219'
'112', '98', 'rajasthani', '220', '221'
'113', '98', 'sindhi', '222', '223'
'114', '98', 'singhalese', '224', '225'
'115', '98', 'urdu', '226', '227'
'116', '1', 'tocharian', '230', '231'

sqlite_sequence:
'tree', '116'

**end
