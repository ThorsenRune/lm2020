//testing.js		only for testing purpose


//	todo:	Part of this is going to be a textinput widget
devpanel={}	;			//Panel for testing and Development
var nDbgLvl=0;
devpanel.redraw=function(){
	if (konst.kReady==prot.state()){
		lib.mFillDataList(idVarName2,Object.keys(prot.oProtElemVar));	// To gui
	}
	mShowRefreshRate();
}

function testGetData(){
	if (!mIsVisible(idInput)) return;
	if (document.activeElement==idInput) return; //	Dont overwrite if this widget has focus
	var oElem=prot.ElementByName( idVarName2.value)
	//if (oElem)		mTX_GetReq(oElem)  //This function is private
	if (oElem)	oElem.peekdata=true  //Correct way to request the variable
	mShowDropDownValue();
}
function testSetData(){nDbgLvlnDbgLvl
	var oElem=prot.ElementByName( idVarName2.value)
	var Vec=utils.String2Array(idInput.value);
	prot.setVector(oElem,Vec);
	mShowDropDownValue();
}

function writeButton() {
	var a=idInput.value.split(',');
	for (var i=0;i<a.length;i++){
		var  senddata =parseInt(a[i])
		serial.send(senddata);
	}
}

var mMessage=function(txt,clear){
	var el=idDebugText;
	if (clear) el.value='';
	el.value=el.value+txt+"\n";
}
var mDebugMsg=function(txt,clear){
	var el=idDebugText;
	if (clear) el.value='';
	el.value=el.value+txt+"\n";
	debugger
}
var mDebugMsg1=function(treshold,txt,clear){ //To replace mDebugMsg
	/*Display critical errors, stop if below a threshold
	Au:RT210128*/
	if (nDbgLvl&&(nDbgLvl<treshold)) debugger;		//Catch critical errors below given treshold
	var el=idDebugText;
	if (clear) el.value='';
	el.value=el.value+txt+"\n";
}
var mShowDropDownValue=function(){	//Show data in input
	if (debug.ReceivedElement){
		if(debug.ReceivedElement.VarName==idVarName2.value){
			idInput.value=(''+debug.ReceivedElement.Vector).replaceAll(',',' , ')

		}
	}else {
		idInput.value='Select in dropdown'
	}
}

var mIsVisible=function(el){		//Determines if the widget is visible
	var vis=el.clientHeight>10
	return vis
}
