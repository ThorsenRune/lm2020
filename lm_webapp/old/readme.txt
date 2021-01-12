
====INTERFACE.JS====

Client 		<->	Server						// though server Peek/Poke
AppClient 	<->	(APP <-> BT <->	Device)
			<->	Server


mExchange	where the communication happens
===	GLOBAL	====
[
oWatch object
Prot					//Exposed variables
Variables
	sName
	sUnit
	aRaw2UnitRange		//Conversion to units
	aValues				//Raw values
	
Controls
	sName
	aRange				//Display Range
	nIndex				//Index
]

3 Objects:
	data 	- memory content
	prot 	- protocol information, variable descriptors   calibration etc
	oWX		-	Widget information, what variable, index of data, range min & maximum visible scale
	
	oWX	{control id,   range, varname,dataindex, value(index,newvalue)->prot.oVar.value(*)}

App: if (poke) file exist then read it, send values to device and insert variablename in watch object


Math
	utils.js
	Number.prototype.norm			//Number <->  100% normalization given a range
	Number.prototype.unnorm