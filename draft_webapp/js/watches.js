/*
	Watches are referring protocol eleemts and updating widgets
		the widget is the visual gadget
		A Watch point to
			|--->		Protocol Dataelement (prot.elem[])  //TODO: unify prot.elem as the datacarrying element


*/
"use strict";	//Require variable declarations
//Todo refactor oWatch with a more explanative name like 'protdata'
var oWatch={	//Object holding data for exchanging
	'Title':'Application name and version'	//Default string overwritten by JSON data in setup file
	,nPeriod:2000		//Refresh period in ms
	,sFileName:'data.txt'	//Filename on the server
	,oVarDesc:{}	//{varname: _VarDescObj }Containing variable names
	,oProtElemVar :{}	//{varname: _VarDataObj	} holding volatile data
	,wx:{}			//{window[id]:_WXObj	}Object of control settings wx.[id].[param]=
}

var _VarDescObj={	//Information about the dataobject protocol
	//held by oWatch.prot[VarName]
	VarName	:'FW varname'	//this object name
	,Alias	:'Desc name'
	,Factor	:1000	//raw 2 unit Conversion factor
	,Offset	:0		//subtracted before conversion
	,Unit	:'u'	//Name of the units mV,mA % etc
	,oData	:_VarDataObj	//access to the data
}
var _VarDataObj={

	//	held by oWatch.data[idWX]
	VarName	:'FW varname'	//this object name
	,nVarId: -1			//Valid range [64-128]Id  for cSerial protocol (reserved)
	,Size: 2
	,Vector:[1,2,3]		//Raw data
	,Poke:false			//Flag for sending data to device
	,Peek:true			//Flag for reading data from device
}
var _WXObj={			//Widget descriptor
	//	held by oWatch.wx[idWX]
	id		:' ID'
	,varname:'VarName'			//Identifier for the protocol object
	,Index	: 0				//Index for scalar in data vector
	,Range	: [0,100]		//Visible range
}
