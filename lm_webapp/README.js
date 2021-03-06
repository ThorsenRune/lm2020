//README.JS  			-				//DESCRIPTION TEXTFILE
// This describes calls and inner workings of the WebApp.
/*Its a JS file to allow for codefolding in ATOM, but has not executable code in it
*/
TODOS: see TODO.JS



-----------------DESCRIPTION of objects -----------------------
	prot.oProtElemVar 	: the protocol elements
					{VarName: "aIAmp", nIdx: 7, Size: 2, Vector: Array(2), Peek: true, …}
	prot.oWidgets	: controls and widgets showing/manipulating protocol elements
					{id: "wss1", varname: "xData", Index: 10, Range: Array(2)}
/***************************************************************************/
Program outline{
	Main_Init()
	|-->	Main_Loop()
	|			v
	^-----------+

	Main_Loop(){
		nMainState=prot.reset xor running
		on(nMainState)
			+-->	mCommunication Reset
			|
			<Xor>
			|
			+-->	dataexchange on the protocol
	}

	Widgets are registered in an array wx[]
	A timer calls a refresh of the display
	{
		each widget points to elements {
			is the value new then redraw the widge
			it will set a read flag for the next iteration
		}
		loop elements
			if they are read then query the lm for data

	}
}


/*		STATEMACHINE FOR ASYNC CALLS in the Mainloop
	note: it has been chosen not to use the async/promise facilities in javascript
			to keep coding style with c


	States of protocol	'prot.State'
	Then 	only if serial.isReady()
		|	prot.kCommInit	:	prot.State=prot.kCommInit	, do handshake and initialize protocol
		|	prot.kReady		:	prot.State= prot.kReady running

*/

******************Call graph******************
Structure{
	index.php // a php file so php server is required
		this assembles prototype.html with submodules signal,sliders etc
}
{


main.js:Main_Init // Entry point in main.js
	|-->	mWebSocket_InitAsync();		//Setting up commuication with the LM [WIFI & BLUETOOTH (TODO:BT) ]
	|-->	init_SetupFromServer();		//Get settings from server
	|-->	initAndSetupTheSliders();	//GUI setup
	|-->	signal.init				//Initialize signal widget


}

OBJECTS{
	prot		//Class with protocol methods and data elements - tied to a specific device
				//	in case of more devices the  will be an array of protocols
	prot.oWX[]	//Array of elements
		|-->protocol.js
		|-->display.js
	prot.oData		//Dataelements being exchanged over the protocol
		|-->	oProtElemVar {		}
		|-->	oVarDescr
		|-->	wx{}		Array pointers to  widgets
}

oWX		//

====INTERFACE.JS====
{
Client 		<->	Server						// though server Peek/Poke
AppClient 	<->	(APP <-> BT <->	Device)
			<->	Server


mExchange	where the communication happens
}

/*
	THE SETUP FILE
	defaults to "data/data.txt"
	Contains descriptions and relations between Protocol elements and widgets
*/
	wx:		relation between hardcoded widgets and their FW variable names
   "wx": {
        "wvs1": {
            "id": "wvs1",
            "varname": "IMin",
            "Index": "0",
            "Range": [
                "0",
                "50"
            ]
        },


communication protocol see uml.txt
===	GLOBAL	====
[
prot.oData object
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
