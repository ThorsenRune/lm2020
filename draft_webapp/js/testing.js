//testing.js		only for testing purpose

function testGetData(){
	var oElem=prot.ElementByName( display.idVarName1.value)
	if (oElem)		mTX_GetReq(oElem)
	mShowDropDownValue();
}
function testSetData(){
	var oElem=prot.ElementByName( display.idVarName1.value)
	oElem.Vector=utils.String2Array(idInput.value);
	mTX_SetReq(oElem);
	mShowDropDownValue();
}

function writeButton() {
	var a=idInput.value.split(',');
	for (var i=0;i<a.length;i++){
		var  senddata =parseInt(a[i])
		serial.send(senddata);
	}
}
function mFromDevice(aRXData){
	var elRes=document.getElementById("idData");
	elRes.innerHTML  = aRXData ;
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
var mShowDropDownValue=function(){	//Show data in input
	if (debug.ReceivedElement){
		if(debug.ReceivedElement.VarName==display.idVarName1.value){
			idInput.value=(''+debug.ReceivedElement.Vector).replaceAll(',',' , ')
		}
	}else {
		idInput.value='Select in dropdown'
	}
}

var mIsVisible=function(el){
	var vis=el.clientHeight>100
	return vis
}
